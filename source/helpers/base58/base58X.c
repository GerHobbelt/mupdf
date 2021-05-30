
#include "base58X.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


// Derived from bitcoin code.
// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
//
// see also:
// - https://en.wikipedia.org/wiki/Binary-to-text_encoding
// - https://github.com/bitcoin/bitcoin/blob/master/src/base58.cpp
// - https://crypto.bi/base58-source/
//
// We have also looked at other encodings (Base91, Ascii85, etc.) as we want
// the BLAKE3-based fingerprint to be represented in as few characters as possible
// for most fingerprint lookups will be in a database, but we also need
// the resulting fingerprint string to be useful as:
// - a filename on ANY filesystem. Hence Ascii85+ encodings are OUT,
// - safe in code (strings)
// - safe in URLs as path or query particle, etc. (hence similar requirements to RFC1924: https://tools.ietf.org/html/rfc1924)
//
// I think ultimately the author of bitcoin, Satoshi Nakamoto, phrased it most succinctly:
//
// > Why base-58 instead of standard base-64 encoding?
// > - Don't want 0OIl characters that look the same in some fonts and
// >      could be used to create visually identical looking data.
// > - A string with non-alphanumeric characters is not as easily accepted as input.
// > - E-mail usually won't line-break if there's no punctuation to break at.
// > - Double-clicking selects the whole string as one word if it's all alphanumeric.
// >
//
// IMPORTANT DIFFERENCE IN ENCODING VS. THE BITCOIN VERSION:
//
// We treat the input as LITTLE ENDIAN. Hence the result will be INCOMPATIBLE with
// any bitcoin base58 code you'll find out there.
//
// WE DO NOT USE THIS FOR BITCOIN WORK. This implementation is used in conjunction
// with BLAKE3-based fingerprinting of documents, where the generated fingerprint should
// be *short*(ish) to speed string comparisons anywhere, while the underlying hash
// should *virtually guarantee* uniqueness of hash for any content/file so file content
// can be uniquely identified using only the fingerprint.
//
// Therefor we also do not apply any checksums or other extra 'validation mechanisms'
// to the generated fingerprint STRING.
//
// Also note that the length is not encoded either as the width of the raw fingerprint
// is known to both encoder and decoder and *predetermined/fixed*.
//
// Also note that we DO NOT encode zeroes in a special way: we expect to encode random
// data (the bytes of a cryptographic hash) so no byte values should 'predominate' in
// the raw hash, thus making special handling of any of those a moot point.

/*
> Some quick JS testcode to see which base58 power is close to a power-of-2 (and therefor optimal
> for converting binary numbers of that 2^N base to a 58^M number base)
>
> for (let i = 1; i < 16; i++) { console.log({i, p: Math.log(Math.pow(58,i))/Math.log(2) }); }
{ i: 1, p: 5.857980995127572 }
{ i: 2, p: 11.715961990255144 }
{ i: 3, p: 17.57394298538272 }
{ i: 4, p: 23.431923980510287 }
{ i: 5, p: 29.289904975637864 }
{ i: 6, p: 35.14788597076544 }
{ i: 7, p: 41.005866965893006 }    <-- 41 bits of binary data is VERY close (AND *smaller* still!) to 7 Base58 characters. Nice!
{ i: 8, p: 46.863847961020575 }
{ i: 9, p: 52.72182895614815 }
{ i: 10, p: 58.57980995127573 }
{ i: 11, p: 64.4377909464033 }
{ i: 12, p: 70.29577194153087 }
{ i: 13, p: 76.15375293665845 }
{ i: 14, p: 82.01173393178601 }
{ i: 15, p: 87.86971492691359 }
>
> The list above tells us that the optimal conversion (for up to 2^87) from binary (base2) to base58
> is by chopping the binary data into 41-bit sized chunks (p = 41), each of which fit snugly in 7 base58
> 'digits' (i = 7).
>
> For the eternally curious, here's the extended analysis run for up to 2^1024:
>
> { let optimal = -1; let opti_ppm = 1E9; for (let i = 1; Math.log(Math.pow(58,i))/Math.log(2) <= 1025; i++) { let p = Math.log(Math.pow(58,i))/Math.log(2); let
 d = p - Math.round(p); let ppm = Math.round(1E6 * d / p); let usable = (ppm >= 0 ? "USABLE" : ""); if (opti_ppm > ppm && ppm >= 0) { optimal = i; opti_ppm = pp
m; } console.log({ i, p, ppm, v: usable + (optimal == i ? ",OPTIMAL" : "") }); } }
{ i: 1, p: 5.857980995127572, ppm: -24244, v: '' }
{ i: 2, p: 11.715961990255144, ppm: -24244, v: '' }
{ i: 3, p: 17.57394298538272, ppm: -24244, v: '' }
{ i: 4, p: 23.431923980510287, ppm: 18433, v: 'USABLE,OPTIMAL' }
{ i: 5, p: 29.289904975637864, ppm: 9898, v: 'USABLE,OPTIMAL' }
{ i: 6, p: 35.14788597076544, ppm: 4208, v: 'USABLE,OPTIMAL' }
{ i: 7, p: 41.005866965893006, ppm: 143, v: 'USABLE,OPTIMAL' }
{ i: 8, p: 46.863847961020575, ppm: -2905, v: '' }
{ i: 9, p: 52.72182895614815, ppm: -5276, v: '' }
{ i: 10, p: 58.57980995127573, ppm: -7173, v: '' }
{ i: 11, p: 64.4377909464033, ppm: 6794, v: 'USABLE' }
{ i: 12, p: 70.29577194153087, ppm: 4208, v: 'USABLE' }
{ i: 13, p: 76.15375293665845, ppm: 2019, v: 'USABLE' }
{ i: 14, p: 82.01173393178601, ppm: 143, v: 'USABLE' }
{ i: 15, p: 87.86971492691359, ppm: -1483, v: '' }
{ i: 16, p: 93.72769592204115, ppm: -2905, v: '' }
{ i: 17, p: 99.58567691716873, ppm: -4160, v: '' }
{ i: 18, p: 105.4436579122963, ppm: 4208, v: 'USABLE' }
{ i: 19, p: 111.30163890742388, ppm: 2710, v: 'USABLE' }
{ i: 20, p: 117.15961990255146, ppm: 1362, v: 'USABLE' }
{ i: 21, p: 123.01760089767902, ppm: 143, v: 'USABLE' }
{ i: 22, p: 128.8755818928066, ppm: -965, v: '' }
{ i: 23, p: 134.73356288793417, ppm: -1978, v: '' }
{ i: 24, p: 140.59154388306175, ppm: -2905, v: '' }
{ i: 25, p: 146.44952487818932, ppm: 3069, v: 'USABLE' }
{ i: 26, p: 152.3075058733169, ppm: 2019, v: 'USABLE' }
{ i: 27, p: 158.16548686844445, ppm: 1046, v: 'USABLE' }
{ i: 28, p: 164.02346786357202, ppm: 143, v: 'USABLE' }
{ i: 29, p: 169.8814488586996, ppm: -698, v: '' }
{ i: 30, p: 175.73942985382718, ppm: -1483, v: '' }
{ i: 31, p: 181.59741084895475, ppm: -2217, v: '' }
{ i: 32, p: 187.4553918440823, ppm: 2429, v: 'USABLE' }
{ i: 33, p: 193.3133728392099, ppm: 1621, v: 'USABLE' }
{ i: 34, p: 199.17135383433745, ppm: 860, v: 'USABLE' }
{ i: 35, p: 205.02933482946506, ppm: 143, v: 'USABLE' }
{ i: 36, p: 210.8873158245926, ppm: -534, v: '' }
{ i: 37, p: 216.74529681972015, ppm: -1175, v: '' }
{ i: 38, p: 222.60327781484776, ppm: -1782, v: '' }
{ i: 39, p: 228.4612588099753, ppm: 2019, v: 'USABLE' }
{ i: 40, p: 234.3192398051029, ppm: 1362, v: 'USABLE' }
{ i: 41, p: 240.17722080023046, ppm: 738, v: 'USABLE' }
{ i: 42, p: 246.03520179535803, ppm: 143, v: 'USABLE' }
{ i: 43, p: 251.8931827904856, ppm: -424, v: '' }
{ i: 44, p: 257.7511637856132, ppm: -965, v: '' }
{ i: 45, p: 263.6091447807408, ppm: -1483, v: '' }
{ i: 46, p: 269.46712577586834, ppm: 1734, v: 'USABLE' }
{ i: 47, p: 275.3251067709959, ppm: 1181, v: 'USABLE' }
{ i: 48, p: 281.1830877661235, ppm: 651, v: 'USABLE' }
{ i: 49, p: 287.04106876125104, ppm: 143, v: 'USABLE' }
{ i: 50, p: 292.89904975637864, ppm: -345, v: '' }
{ i: 51, p: 298.7570307515062, ppm: -813, v: '' }
{ i: 52, p: 304.6150117466338, ppm: -1264, v: '' }
{ i: 53, p: 310.47299274176135, ppm: 1523, v: 'USABLE' }
{ i: 54, p: 316.3309737368889, ppm: 1046, v: 'USABLE' }
{ i: 55, p: 322.1889547320165, ppm: 586, v: 'USABLE' }
{ i: 56, p: 328.04693572714405, ppm: 143, v: 'USABLE' }
{ i: 57, p: 333.90491672227165, ppm: -285, v: '' }
{ i: 58, p: 339.7628977173992, ppm: -698, v: '' }
{ i: 59, p: 345.62087871252675, ppm: -1097, v: '' }
{ i: 60, p: 351.47885970765435, ppm: 1362, v: 'USABLE' }
{ i: 61, p: 357.3368407027819, ppm: 943, v: 'USABLE' }
{ i: 62, p: 363.1948216979095, ppm: 536, v: 'USABLE' }
{ i: 63, p: 369.05280269303705, ppm: 143, v: 'USABLE' }
{ i: 64, p: 374.9107836881646, ppm: -238, v: '' }
{ i: 65, p: 380.76876468329215, ppm: -607, v: '' }
{ i: 66, p: 386.6267456784198, ppm: -965, v: '' }
{ i: 67, p: 392.48472667354736, ppm: 1235, v: 'USABLE' }
{ i: 68, p: 398.3427076686749, ppm: 860, v: 'USABLE' }
{ i: 69, p: 404.20068866380245, ppm: 497, v: 'USABLE' }
{ i: 70, p: 410.0586696589301, ppm: 143, v: 'USABLE' }
{ i: 71, p: 415.91665065405766, ppm: -200, v: '' }
{ i: 72, p: 421.7746316491852, ppm: -534, v: '' }
{ i: 73, p: 427.63261264431276, ppm: -859, v: '' }
{ i: 74, p: 433.4905936394403, ppm: 1132, v: 'USABLE' }
{ i: 75, p: 439.34857463456797, ppm: 793, v: 'USABLE' }
{ i: 76, p: 445.2065556296955, ppm: 464, v: 'USABLE' }
{ i: 77, p: 451.06453662482306, ppm: 143, v: 'USABLE' }
{ i: 78, p: 456.9225176199506, ppm: -170, v: '' }
{ i: 79, p: 462.78049861507816, ppm: -474, v: '' }
{ i: 80, p: 468.6384796102058, ppm: -771, v: '' }
{ i: 81, p: 474.49646060533337, ppm: 1046, v: 'USABLE' }
{ i: 82, p: 480.3544416004609, ppm: 738, v: 'USABLE' }
{ i: 83, p: 486.21242259558846, ppm: 437, v: 'USABLE' }
{ i: 84, p: 492.07040359071607, ppm: 143, v: 'USABLE' }
{ i: 85, p: 497.9283845858437, ppm: -144, v: '' }
{ i: 86, p: 503.7863655809712, ppm: -424, v: '' }
{ i: 87, p: 509.64434657609877, ppm: -698, v: '' }
{ i: 88, p: 515.5023275712264, ppm: -965, v: '' }
{ i: 89, p: 521.3603085663539, ppm: 691, v: 'USABLE' }
{ i: 90, p: 527.2182895614816, ppm: 414, v: 'USABLE' }
{ i: 91, p: 533.0762705566091, ppm: 143, v: 'USABLE' }
{ i: 92, p: 538.9342515517367, ppm: -122, v: '' }
{ i: 93, p: 544.7922325468642, ppm: -381, v: '' }
{ i: 94, p: 550.6502135419918, ppm: -635, v: '' }
{ i: 95, p: 556.5081945371194, ppm: -884, v: '' }
{ i: 96, p: 562.366175532247, ppm: 651, v: 'USABLE' }
{ i: 97, p: 568.2241565273745, ppm: 394, v: 'USABLE' }
{ i: 98, p: 574.0821375225021, ppm: 143, v: 'USABLE' }
{ i: 99, p: 579.9401185176297, ppm: -103, v: '' }
{ i: 100, p: 585.7980995127573, ppm: -345, v: '' }
{ i: 101, p: 591.6560805078848, ppm: -581, v: '' }
{ i: 102, p: 597.5140615030124, ppm: -813, v: '' }
{ i: 103, p: 603.3720424981399, ppm: 617, v: 'USABLE' }
{ i: 104, p: 609.2300234932676, ppm: 378, v: 'USABLE' }
{ i: 105, p: 615.0880044883951, ppm: 143, v: 'USABLE' }
{ i: 106, p: 620.9459854835227, ppm: -87, v: '' }
{ i: 107, p: 626.8039664786502, ppm: -313, v: '' }
{ i: 108, p: 632.6619474737778, ppm: -534, v: '' }
{ i: 109, p: 638.5199284689054, ppm: -752, v: '' }
{ i: 110, p: 644.377909464033, ppm: 586, v: 'USABLE' }
{ i: 111, p: 650.2358904591605, ppm: 363, v: 'USABLE' }
{ i: 112, p: 656.0938714542881, ppm: 143, v: 'USABLE' }
{ i: 113, p: 661.9518524494156, ppm: -73, v: '' }
{ i: 114, p: 667.8098334445433, ppm: -285, v: '' }
{ i: 115, p: 673.6678144396708, ppm: -493, v: '' }
{ i: 116, p: 679.5257954347984, ppm: -698, v: '' }
{ i: 117, p: 685.3837764299259, ppm: 560, v: 'USABLE' }
{ i: 118, p: 691.2417574250535, ppm: 350, v: 'USABLE' }
{ i: 119, p: 697.0997384201811, ppm: 143, v: 'USABLE' }
{ i: 120, p: 702.9577194153087, ppm: -60, v: '' }
{ i: 121, p: 708.8157004104362, ppm: -260, v: '' }
{ i: 122, p: 714.6736814055638, ppm: -457, v: '' }
{ i: 123, p: 720.5316624006914, ppm: -650, v: '' }
{ i: 124, p: 726.389643395819, ppm: 536, v: 'USABLE' }
{ i: 125, p: 732.2476243909465, ppm: 338, v: 'USABLE' }
{ i: 126, p: 738.1056053860741, ppm: 143, v: 'USABLE' }
{ i: 127, p: 743.9635863812017, ppm: -49, v: '' }
{ i: 128, p: 749.8215673763292, ppm: -238, v: '' }
{ i: 129, p: 755.6795483714568, ppm: -424, v: '' }
{ i: 130, p: 761.5375293665843, ppm: -607, v: '' }
{ i: 131, p: 767.395510361712, ppm: 515, v: 'USABLE' }
{ i: 132, p: 773.2534913568396, ppm: 328, v: 'USABLE' }
{ i: 133, p: 779.1114723519671, ppm: 143, v: 'USABLE' }
{ i: 134, p: 784.9694533470947, ppm: -39, v: '' }
{ i: 135, p: 790.8274343422222, ppm: -218, v: '' }
{ i: 136, p: 796.6854153373498, ppm: -395, v: '' }
{ i: 137, p: 802.5433963324774, ppm: -569, v: '' }
{ i: 138, p: 808.4013773276049, ppm: 497, v: 'USABLE' }
{ i: 139, p: 814.2593583227325, ppm: 319, v: 'USABLE' }
{ i: 140, p: 820.1173393178602, ppm: 143, v: 'USABLE' }
{ i: 141, p: 825.9753203129877, ppm: -30, v: '' }
{ i: 142, p: 831.8333013081153, ppm: -200, v: '' }
{ i: 143, p: 837.6912823032428, ppm: -369, v: '' }
{ i: 144, p: 843.5492632983704, ppm: -534, v: '' }
{ i: 145, p: 849.407244293498, ppm: 479, v: 'USABLE' }
{ i: 146, p: 855.2652252886255, ppm: 310, v: 'USABLE' }
{ i: 147, p: 861.1232062837531, ppm: 143, v: 'USABLE' }
{ i: 148, p: 866.9811872788806, ppm: -22, v: '' }
{ i: 149, p: 872.8391682740083, ppm: -184, v: '' }
{ i: 150, p: 878.6971492691359, ppm: -345, v: '' }
{ i: 151, p: 884.5551302642634, ppm: -503, v: '' }
{ i: 152, p: 890.413111259391, ppm: 464, v: 'USABLE' }
{ i: 153, p: 896.2710922545185, ppm: 302, v: 'USABLE' }
{ i: 154, p: 902.1290732496461, ppm: 143, v: 'USABLE' }
{ i: 155, p: 907.9870542447737, ppm: -14, v: '' }
{ i: 156, p: 913.8450352399012, ppm: -170, v: '' }
{ i: 157, p: 919.7030162350289, ppm: -323, v: '' }
{ i: 158, p: 925.5609972301563, ppm: -474, v: '' }
{ i: 159, p: 931.418978225284, ppm: 450, v: 'USABLE' }
{ i: 160, p: 937.2769592204116, ppm: 295, v: 'USABLE' }
{ i: 161, p: 943.1349402155391, ppm: 143, v: 'USABLE' }
{ i: 162, p: 948.9929212106667, ppm: -7, v: '' }
{ i: 163, p: 954.8509022057942, ppm: -156, v: '' }
{ i: 164, p: 960.7088832009218, ppm: -303, v: '' }
{ i: 165, p: 966.5668641960496, ppm: -448, v: '' }
{ i: 166, p: 972.4248451911769, ppm: 437, v: 'USABLE' }
{ i: 167, p: 978.2828261863046, ppm: 289, v: 'USABLE' }
{ i: 168, p: 984.1408071814321, ppm: 143, v: 'USABLE' }
{ i: 169, p: 989.9987881765597, ppm: -1, v: '' }
{ i: 170, p: 995.8567691716873, ppm: -144, v: '' }
{ i: 171, p: 1001.7147501668148, ppm: -285, v: '' }
{ i: 172, p: 1007.5727311619424, ppm: -424, v: '' }
{ i: 173, p: 1013.4307121570699, ppm: 425, v: 'USABLE' }
{ i: 174, p: 1019.2886931521975, ppm: 283, v: 'USABLE' }
>
> which tells us that 7-digit base58 is the closest we can get to *any* binary power AND contain
> ALL the values representable in that base2 number range: NEGATIVE ppm (parts-per-million) means
> that particular base58 number is quite close to a power-of-2 number, BUT cannot represent
> EVERY possible value in that power-of-2 range, unfortunately.
> For instance, a 169-digit base58 number comes VERY close to the value represented by 2^990 (989.998788... rounded UP)
> BUT cannot represent all the values 0..2^990. It CAN represent all binary values 0..2^989 but then
> the ppm figure is much worse: 1010, hence less efficient than simply using 2^41 ~ 7-digit base58 number sequences.
>
> Here's the augmented test code to observe the 'corrected' ppm for those negative-ppm cases:
>
> { let optimal = -1; let opti_ppm = 1E9; for (let i = 1; Math.log(Math.pow(58,i))/Math.log(2) <= 1025; i++) { let p = Math.log(Math.pow(58,i))/Math.log(2); let
 d = p - Math.round(p); let ppm = Math.round(1E6 * d / p); if (d < 0) { d = p - Math.round(p - 1); p -= 1; ppm = Math.round(1E6 * d / p); } let usable = (ppm >=
 0 ? "USABLE" : ""); if (opti_ppm > ppm && ppm >= 0) { optimal = i; opti_ppm = ppm; } console.log({ i, p, ppm, v: usable + (optimal == i ? ",OPTIMAL" : "") });
} }
>
> All in all, the story is quite simple: when we chop the large BLAKE3 hashes into chunks of 41 bits each,
> we can encode those nicely as base58 number sequences, thus producing shorter string representations when
> compared to basic HEX: 32 bytes of hash --> 64 nibbles --> string length = 64 when encoded in HEX.
> Meanwhile, 32 bytes is 2^8^32 = 2^256. This 'number' fits in 256/41 ~= 6.24 groups of 2^41, hence takes
> 7 groups to encode. Since each group costs 7 characters, the total string size is 7*7 = 49 characters when
> encoding that same BLAKE3 2^256 hash in base58 instead of HEX.
>
> ---
>
> Incidentally, we picked base58 instead of base64 to ensure that all 'digits' are legible in almost all fonts out there:
> no 'l' or 'i' while we use '1'. HOWEVER, we do accept O and o while using 0.
>
> Also note that 2^41 nicely fits within regular (non-bignum) integer arithmatic, also in JavaScript which can only
> do precise integer work up to 2^53 due to JS 'numbers' always being floating point (ES3/ES5).
>
> ---
>
> If we also wish to discard O/o vs. 0, we would arrive at a base56 (10+23+23=56) system instead, which has a different
> optimality point:
>
>  { let optimal = -1; let opti_ppm = 1E9; for (let i = 1; Math.log(Math.pow(56,i))/Math.log(2) <= 1025; i++) { let p = Math.log(Math.pow(56,i))/Math.log(2); let
 d = p - Math.round(p); let ppm = Math.round(1E6 * d / p); if (d < 0) { d = p - Math.round(p - 1); p -= 1; ppm = Math.round(1E6 * d / p); } let usable = (ppm >=
 0 ? "USABLE" : ""); if (opti_ppm > ppm && ppm >= 0) { optimal = i; opti_ppm = ppm; } console.log({ i, p, ppm, v: usable + (optimal == i ? ",OPTIMAL" : "") });
}}
{ i: 1, p: 4.807354922057605, ppm: 167942, v: 'USABLE,OPTIMAL' }
{ i: 2, p: 10.61470984411521, ppm: 57911, v: 'USABLE,OPTIMAL' }
{ i: 3, p: 17.422064766172813, ppm: 24226, v: 'USABLE,OPTIMAL' }
{ i: 4, p: 23.22941968823042, ppm: 9876, v: 'USABLE,OPTIMAL' }
{ i: 5, p: 29.03677461028802, ppm: 1266, v: 'USABLE,OPTIMAL' }
{ i: 6, p: 33.844129532345626, ppm: 24942, v: 'USABLE' }
...
{ i: 30, p: 174.22064766172812, ppm: 1266, v: 'USABLE' }
{ i: 31, p: 180.02800258378574, ppm: 156, v: 'USABLE,OPTIMAL' }
{ i: 32, p: 184.83535750584335, ppm: 4519, v: 'USABLE' }
...
{ i: 56, p: 325.21187563522585, ppm: 652, v: 'USABLE' }
{ i: 57, p: 331.01923055728344, ppm: 58, v: 'USABLE,OPTIMAL' }
{ i: 58, p: 335.826585479341, ppm: 2461, v: 'USABLE' }
...
{ i: 82, p: 476.20310360872355, ppm: 427, v: 'USABLE' }
{ i: 83, p: 482.01045853078114, ppm: 22, v: 'USABLE,OPTIMAL' }
{ i: 84, p: 486.8178134528387, ppm: 1680, v: 'USABLE' }
...
{ i: 108, p: 627.1943315822213, ppm: 310, v: 'USABLE' }
{ i: 109, p: 633.0016865042788, ppm: 3, v: 'USABLE,OPTIMAL' }
{ i: 110, p: 637.8090414263365, ppm: 1268, v: 'USABLE' }
...
{ i: 176, p: 1022.0944662821383, ppm: 92, v: 'USABLE' }
>
> which tells us that 2^29 fits nicely in a 5-digit base56 number.
> Then, a 2^256 hash would cost 256/29 ~= 8.827 -> 9 numbers, at 5 characters each:
> 9*5 = 45 characters.
>
> Which is 2 characters LESS than the Base58 scheme we started with, due to the last
> Base56 number not being filled entirely.
>
> However, do we want to employ a base56 encoding rather than a base58 system?
> Since we have dropped the most relevant confusing characters in the fonts out there by
> dropping 0/O and 1/l (as 1/i is not confusing and neither is 1/L but l/i can be), there's
> no *human visual* need to drop more.
> So we consider what happens in the base58 case with that last number:
>
> BLAKE3, at 256 bits, consumes 256/41 numbers: 6 full 7-digit base58 numbers plus a
> 'remainder' of 0.2439: that's 10 binary bits!
> Now that 'corrected for negative ppm' table comes in handy: p=10.716 can be (optimally!)
> encoded in a 2-digit base58 number! (i=2)
> Which would then have us produce a hash string length of 6*7+2=44 characters, which is yet
> one character better than the (untrimmed) base56 variant we considered shortly above.
>
> Here's the first bit of that table for reference:
>
> { let optimal = -1; let opti_ppm = 1E9; for (let i = 1; Math.log(Math.pow(58,i))/Math.log(2) <= 1025; i++) { let p = Math.log(Math.pow(58,i))/Math.log(2); let
 d = p - Math.round(p); let ppm = Math.round(1E6 * d / p); if (d < 0) { d = p - Math.round(p - 1); p -= 1; ppm = Math.round(1E6 * d / p); } let usable = (ppm >=
 0 ? "USABLE" : ""); if (opti_ppm > ppm && ppm >= 0) { optimal = i; opti_ppm = ppm; } console.log({ i, p, ppm, v: usable + (optimal == i ? ",OPTIMAL" : "") });
} }
{ i: 1, p: 4.857980995127572, ppm: 176613, v: 'USABLE,OPTIMAL' }
{ i: 2, p: 10.715961990255144, ppm: 66813, v: 'USABLE,OPTIMAL' }
{ i: 3, p: 16.57394298538272, ppm: 34629, v: 'USABLE,OPTIMAL' }
{ i: 4, p: 23.431923980510287, ppm: 18433, v: 'USABLE,OPTIMAL' }
{ i: 5, p: 29.289904975637864, ppm: 9898, v: 'USABLE,OPTIMAL' }
{ i: 6, p: 35.14788597076544, ppm: 4208, v: 'USABLE,OPTIMAL' }
{ i: 7, p: 41.005866965893006, ppm: 143, v: 'USABLE,OPTIMAL' }
{ i: 8, p: 45.863847961020575, ppm: 18835, v: 'USABLE' }
{ i: 9, p: 51.72182895614815, ppm: 13956, v: 'USABLE' }
{ i: 10, p: 57.57980995127573, ppm: 10070, v: 'USABLE' }
{ i: 11, p: 64.4377909464033, ppm: 6794, v: 'USABLE' }
...
{ i: 43, p: 250.8931827904856, ppm: 3560, v: 'USABLE' }
{ i: 44, p: 256.7511637856132, ppm: 2926, v: 'USABLE' }
{ i: 45, p: 262.6091447807408, ppm: 2320, v: 'USABLE' }
...
{ i: 174, p: 1019.2886931521975, ppm: 283, v: 'USABLE' }
>
> which ALSO tells us that the 'optimal' encoding of a 256-bit bignum would take i=44 base58 'digits',
> hence we also now know for sure that our 'chop that 256 bit number into 41-bit chunks' approach is
> AS GOOD as treating the entire number as a BigNum: both ways we can encode this bit series as a
> 44 character long string! Without us having to resort to costly BigNum arithmetic because we can
> safely work with 64-bit chunks as we chop the byte sequence into 41 bit segments!
*/

/** All alphanumeric characters except for "0", "I", "O", and "l" */
static const char* srcBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const int8_t mapBase58[256] = {
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
	-1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
	22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
	-1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
	47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
};

static inline uint64_t mk_b256_digit(uint8_t d, int pos)
{
	uint64_t rv = d;
	return rv << (pos << 3);
}

#define N41_MASK     ((((uint64_t)1) << 41) - 1)

// Encode the `src` byte sequence into a Base58X string value stored in the `dst` buffer.
// Return a reference to the NUL-terminated string buffer when done or NULL on error.
const char* EncodeBase58X(char* dst, size_t dstsize, uint8_t* src, size_t srcsize)
{
	if (!src || !dst || srcsize == 0 || dstsize < 8)
		return NULL;

	// Consume `src` in 41-bit chunks; pad last incomplete chunk with zero bits in MSBs.
	// Treat every 41-bit chunk as a little endian binary number and decode that into 7 Base48X digits.
	char* rv = dst;
	uint64_t prevbitsbuf = 0;
	int bitpos = 0;

	while (srcsize > 8)
	{
		uint64_t v;

		if (bitpos < 41)
		{
			// get 64 new bits.
			// little endian, arbitrary memory alignment, so decode the bytes into a binary 64-bit value here:
			uint64_t word =
				mk_b256_digit(src[0], 0) |
				mk_b256_digit(src[1], 1) |
				mk_b256_digit(src[2], 2) |
				mk_b256_digit(src[3], 3) |
				mk_b256_digit(src[4], 4) |
				mk_b256_digit(src[5], 5) |
				mk_b256_digit(src[6], 6) |
				mk_b256_digit(src[7], 7);
			src += 8;
			srcsize -= 8;

			// merge with the bits we still have from the previous round.
			// Those will be the least significant bits as we're working little endian here in 41-bit number land.
			v = prevbitsbuf | (word << bitpos);
			// mask at 41 bits.
			v &= N41_MASK;
			// and store the remaining bits in the tempory buffer;
			prevbitsbuf = word >> (41 - bitpos);
			bitpos += 64 - 41;
		}
		else
		{
			// tempory buffer is large enough to hold an entire 41-bit number:
			v = prevbitsbuf;
			// mask at 41 bits.
			v &= N41_MASK;
			// and keep the remaining bits in the tempory buffer;
			prevbitsbuf >>= 41;
			bitpos -= 41;
		}

		// now translate this 41-bit number `v` to a 7-digit base58X string segment:
		if (dstsize < 7)
			return NULL; // dst buffer overflow
		dst[6] = srcBase58[v % 58];
		v /= 58;
		dst[5] = srcBase58[v % 58];
		v /= 58;
		dst[4] = srcBase58[v % 58];
		v /= 58;
		dst[3] = srcBase58[v % 58];
		v /= 58;
		dst[2] = srcBase58[v % 58];
		v /= 58;
		dst[1] = srcBase58[v % 58];
		v /= 58;
		dst[0] = srcBase58[v % 58];
		dstsize -= 7;
		dst += 7;
	}

	// process the remainder of `src':
	uint64_t last_legs_value = 0;
	uint64_t last_legs_bitcount = 0;

	while (srcsize > 0 || bitpos > 0)
	{
		uint64_t v;

		if (bitpos < 41 && srcsize > 0)
		{
			// get 64 new bits: that the bits we've got plus some zero padding.
			// little endian, arbitrary memory alignment, so decode the bytes into a binary 64-bit value here:
			uint8_t src8[8];

			memset(src8, 0, sizeof(src8));
			memcpy(src8, src, srcsize);
			uint64_t word =
				mk_b256_digit(src8[0], 0) |
				mk_b256_digit(src8[1], 1) |
				mk_b256_digit(src8[2], 2) |
				mk_b256_digit(src8[3], 3) |
				mk_b256_digit(src8[4], 4) |
				mk_b256_digit(src8[5], 5) |
				mk_b256_digit(src8[6], 6) |
				mk_b256_digit(src8[7], 7);
			size_t feedbits = srcsize * 8;
			srcsize = 0;

			// merge with the bits we still have from the previous round.
			// Those will be the least significant bits as we're working little endian here in 41-bit number land.
			v = prevbitsbuf | (word << bitpos);
			// mask at 41 bits.
			v &= N41_MASK;
			// and store the remaining bits in the tempory buffer;
			prevbitsbuf = word >> (41 - bitpos);
			bitpos += feedbits - 41;
		}
		else
		{
			// tempory buffer is large enough to hold an entire 41-bit number
			// (or there's no more bits left to feed us at all)
			v = prevbitsbuf;
			// mask at 41 bits.
			v &= N41_MASK;
			// and keep the remaining bits in the tempory buffer;
			// but before we do that, we quickly check if we're on our last legs,
			// i.e. if we've reached the end and have only a few more bits to encode.
			// We'll do that *outside* the loop as we won't need to print all those 7
			// base58 digits, won't we?
			if (bitpos < 41)
			{
				last_legs_value = v;
				last_legs_bitcount = bitpos;
				break;
			}
			prevbitsbuf >>= 41;
			bitpos -= 41;
		}

		// now translate this 41-bit number `v` to a 7-digit base58X BIG ENDIAN string segment:
		// (In our case, using BIG ENDIAN notation here makes the decoding logic logic simpler
		// and faster as we can then go through the string via `*src++ + boundary check`
		// rather than having to first check if there's still 7 digits worth of string left
		// before we decode those 7 digits in reverse order as we must do (old * 58 + digit) there.
		if (dstsize < 7)
			return NULL; // dst buffer overflow
		dst[6] = srcBase58[v % 58];
		v /= 58;
		dst[5] = srcBase58[v % 58];
		v /= 58;
		dst[4] = srcBase58[v % 58];
		v /= 58;
		dst[3] = srcBase58[v % 58];
		v /= 58;
		dst[2] = srcBase58[v % 58];
		v /= 58;
		dst[1] = srcBase58[v % 58];
		v /= 58;
		dst[0] = srcBase58[v % 58];
		dstsize -= 7;
		dst += 7;
	}

	// We do not print all 7 base58 digits for the last few bits: this would otherwise produce a tail
	// of useless, space-consuming '1' chracters we can do without.
	// Of course, this makes the decoder a tad more complicated too as now we must accept that
	// the decoder MAY run into a truncated/partial base58 number at the end of its input.   :'-(
	// The things we do for speed and space savings some times...   ;-D
	if (last_legs_bitcount > 0)
	{
		// determine how many base58 digits we actually need to encode those remaining `last_legs_bitccount` bits:
		uint64_t number_mask = ((uint64_t)1) << last_legs_bitcount;
		char dst58buf[7];
		int di = 0;

		while (number_mask)
		{
			dst58buf[di++] = srcBase58[last_legs_value % 58];
			last_legs_value /= 58;
			number_mask /= 58;
		}

		// we now have the base58 number in little endian form in the intermediate buffer.
		// reverse print those significant base58 digits to `dst` and we're done!
		if (dstsize < di)
			return NULL; // dst buffer overflow
		while (di > 0)
		{
			*dst++ = dst58buf[--di];
		}
	}

	if (dstsize < 1)
		return NULL; // dst buffer overflow
	*dst = 0;

	return rv;
}


// Decode the `src` Base58X string to a byte sequence stored in the `dst` buffer.
// Return a reference to the `dst` buffer when done or NULL on error.
//
// NOTE: `*targetsize` is set to the *decoded* number of bytes produced by this decode.
//
// NOTE: The buffer MUST be 8 bytes larger to account for boundary effects during decoding.
const uint8_t* DecodeBase58X(uint8_t* dst, size_t dstsize, size_t* targetsize_ref, const char* src)
{
	if (!src || !dst || dstsize == 0)
		return NULL;

	// Consume `src` in 7-digit chunks; when the `src` is not sufficiently large
	// for such a "41-bit encoded number", that's an error.
	// Treat every 7-digit Base58X chunk as a little endian number and decode that into a 41-bit binary number.
	uint8_t* rv = dst;
	uint64_t prevbitsbuf = 0;
	int bitpos = 0;
	const char* last_legs_src;

	for (;;)
	{
		// remember where we are in the source data at this time: this will become
		// handy when we run into the edge boundary in those lovely DECODE_MACRO()
		// macros below.
		last_legs_src = src;

		// get 7 new digits.
		// little endian, arbitrary memory alignment
		uint64_t word = 0;

#define DECODE_MACRO(pos)					\
		{									\
			uint8_t c = *src++;				\
			if (!c)   						\
				break;	         			\
			int d = mapBase58[c];			\
			if (d < 0)						\
				return NULL;				\
			word *= 58;						\
			word += d;						\
		}

		DECODE_MACRO();
		DECODE_MACRO();
		DECODE_MACRO();
		DECODE_MACRO();
		DECODE_MACRO();
		DECODE_MACRO();
		DECODE_MACRO();

		if (bitpos >= 64 - 41)  // ~ if bitpos + 41 >= 64, i.e. do we have enough for a 64bit word now?
		{
			// merge with the bits we still have from the previous round.
			// Those will be the least significant bits as we're working little endian here in 64-bit number land.
			uint64_t v = prevbitsbuf | (word << bitpos);
			// and store the remaining bits in the tempory buffer;
			prevbitsbuf = word >> (64 - bitpos);
			bitpos += 41 - 64;

			// now translate this 64-bit number `v` to an 8-byte buffer segment:

#define PUT_MACRO()											\
		{													\
			uint8_t c = (uint8_t)v;							\
			if (!dstsize)									\
				return NULL; /* dst buffer overflow */		\
			*dst++ = c;										\
			v >>= 8;										\
		}

			PUT_MACRO();
			PUT_MACRO();
			PUT_MACRO();
			PUT_MACRO();
			PUT_MACRO();
			PUT_MACRO();
			PUT_MACRO();
			PUT_MACRO();
			dstsize -= 8;
		}
		else
		{
			// tempory buffer is not yet large enough to help deliver an entire 64-bit number on the next round:
			prevbitsbuf |= (word << bitpos);
			bitpos += 41;
		}
	}

	// When you get here, you've hit the end.
	//
	// Now we must make sure there's no dregs (a truncated base58 number!)
	// still waiting to be transformed at the end there.
	// Hence we first recover the position we were in before we hit that
	// possibly truncated last base58 number (or was it the NUL sentinel
	// immediately anyway?)
	src = last_legs_src;
	if (*src)
	{
		// whoops! still a truncated base58 number to do then!
		//
		// get less than 7 new digits.
		// They're little endian, arbitrary memory alignment
		uint64_t word = 0;
		uint64_t number_mask = 0;

		while (*src)
		{
			uint8_t c = *src++;
			if (!c)
				break;	         	// EOF!
			int d = mapBase58[c];
			if (d < 0)
				return NULL;
			word *= 58;
			word += d;
			number_mask *= 58;
			number_mask += 57;   // this is kinda like saying `~0` in base 58!   :-)
		}

		// We *know* we won't have enough bits for a full 64-bit word, but that's okay now.
		//
		// merge with the bits we still have from the previous round.
		// Those will be the least significant bits as we're working little endian here in 64-bit number land.
		uint64_t v = prevbitsbuf | (word << bitpos);
		// and make sure we account for those bits in our mask value too:
		// (we CAN be a bit sneaky about it though: a LEFT-SHIFT suffices for this to work out fine)
		number_mask <<= bitpos;

		// now translate this 1..64-bit number `v` to an 8-byte buffer segment:

		while (number_mask)
		{
			uint8_t c = (uint8_t)v;
			if (!dstsize)
				return NULL; /* dst buffer overflow */
			*dst++ = c;
			dstsize--;
			v >>= 8;
			number_mask >>= 8;
		}
	}

	*targetsize_ref = dst - rv;
	return rv;
}

