
#include "base58X.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mupdf/assertions.h"


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
> Also note that 2^41 nicely fits within regular (non-bignum) integer arithmetic, also in JavaScript which can only
> do precise integer work up to 2^53 due to JS 'numbers' always being floating point (ES3/ES5).
>
> ---
>
> If we also wish to discard O/o vs. 0, we would arrive at a base56 (10+23+23=56) system instead, which has a different
> optimality point:
>
>  { let base = 10+26+26; let optimal = -1; let opti_ppm = 1E9; for (let i = 1; Math.log(Math.pow(base, i)) / Math.log(2) <= 1025; i++) { let p = Math.log(Math.pow(base,i))/Math.log(2); let d = p - Math.round(p); let ppm = Math.round(1E6 * d / p); if (d < 0) { d = p - Math.round(p - 1); p -= 1; ppm = Math.round(1E6 * d / p); } let usable = (ppm >= 0 ? "USABLE" : ""); if (opti_ppm > ppm && ppm >= 0) { optimal = i; opti_ppm = ppm; } console.log({ i, base, p, ppm, v: usable + (optimal == i ? ",OPTIMAL" : "") }); }}
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

---

## Revisiting BASE58X and reviewing the reasons why in light of a new potential optimum.

Going even further, we investigate the optimal encoding output size (in characters) for any base
from 10+23+23 (upper & lower, leaving out all o, i and l) to 10+26+26 (leaving out nothing,
assuming the font used to display these hashes will make o, i and l easy to read vs. 1 and 0).
We use this code:

> { let optc = 1E6; for (let base = 10+23+23; base <= 10+27+27; base++) { let optimal = -1; let opti_ppm = 1E9; for (let i = 1; Math.log(Math.pow(base,i))/Math.log(2) <= 1025; i++) { let p = Math.log(Math.pow(base, i)) / Math.log(2); let d = p - Math.round(p); let ppm = Math.round(1E6 * d / p); if (d < 0) { d = p - Math.round(p - 1); p -= 1; ppm = Math.round(1E6 * d / p); } let usable = (ppm >= 0 ? "USABLE" : ""); if (opti_ppm > ppm && ppm >= 0) { optimal = i; opti_ppm = ppm; } let b = Math.floor(p); let w = Math.floor(256 / b); let r = 256 - w * b; let rc = Math.ceil(Math.log(Math.pow(2, r)) / Math.log(base)); let cc = w * i + rc; let show = (cc < optc) || (optimal == i); if (cc < optc) { optc = cc; } if (show) { console.log({ i, base, p, ppm, v: usable + (optimal == i ? ",OPTIMAL" : ""), b, w, r, rc, cc }); } }}}

> (Note: we stripped out the records that are only potentially relevant if we were looking at encoding 
> numbers with *more* than 256 binary digits, i.e. we show the *first* record with w=0 (NumberOfWords is zero) 
> and remove the rest.)

> b = number of bits fitting in this baseN numeric word (which has i characters in the encoded base)
> w = number of full words used, *excluding* any short tail a.k.a. 'remainder'.
> r = the remainder: the number of bits in the tail which still need to be encoded.
> rc = number of base characters required to encode this tail/remainder.
> cc = character count: the total number of base characters required to successfully encode a 256 bit number.
>
{ i: 1, base: 56, p: 4.807354922057605, ppm: 167942, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 56, p: 10.61470984411521, ppm: 57911, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 56, p: 17.422064766172813, ppm: 24226, v: 'USABLE,OPTIMAL', b: 17, w: 15, r: 1, rc: 1, cc: 46 }
{ i: 4, base: 56, p: 23.22941968823042, ppm: 9876, v: 'USABLE,OPTIMAL', b: 23, w: 11, r: 3, rc: 1, cc: 45 }
{ i: 5, base: 56, p: 29.03677461028802, ppm: 1266, v: 'USABLE,OPTIMAL', b: 29, w: 8, r: 24, rc: 5, cc: 45 }
{ i: 31, base: 56, p: 180.02800258378574, ppm: 156, v: 'USABLE,OPTIMAL', b: 180, w: 1, r: 76, rc: 14, cc: 45 }
{ i: 57, base: 56, p: 331.01923055728344, ppm: 58, v: 'USABLE,OPTIMAL', b: 331, w: 0, r: 256, rc: 45, cc: 45 }
{ i: 1, base: 57, p: 4.832890014164742, ppm: 172338, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 57, p: 10.665780028329484, ppm: 62422, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 57, p: 17.498670042494226, ppm: 28498, v: 'USABLE,OPTIMAL', b: 17, w: 15, r: 1, rc: 1, cc: 46 }
{ i: 4, base: 57, p: 23.33156005665897, ppm: 14211, v: 'USABLE,OPTIMAL', b: 23, w: 11, r: 3, rc: 1, cc: 45 }
{ i: 5, base: 57, p: 29.164450070823708, ppm: 5639, v: 'USABLE,OPTIMAL', b: 29, w: 8, r: 24, rc: 5, cc: 45 }
{ i: 11, base: 57, p: 64.16179015581216, ppm: 2522, v: 'USABLE,OPTIMAL', b: 64, w: 4, r: 0, rc: 0, cc: 44 }
{ i: 17, base: 57, p: 99.15913024080061, ppm: 1605, v: 'USABLE,OPTIMAL', b: 99, w: 2, r: 58, rc: 10, cc: 44 }
{ i: 23, base: 57, p: 134.15647032578906, ppm: 1166, v: 'USABLE,OPTIMAL', b: 134, w: 1, r: 122, rc: 21, cc: 44 }
{ i: 29, base: 57, p: 169.1538104107775, ppm: 909, v: 'USABLE,OPTIMAL', b: 169, w: 1, r: 87, rc: 15, cc: 44 }
{ i: 35, base: 57, p: 204.15115049576596, ppm: 740, v: 'USABLE,OPTIMAL', b: 204, w: 1, r: 52, rc: 9, cc: 44 }
{ i: 41, base: 57, p: 239.1484905807544, ppm: 621, v: 'USABLE,OPTIMAL', b: 239, w: 1, r: 17, rc: 3, cc: 44 }
{ i: 47, base: 57, p: 274.1458306657429, ppm: 532, v: 'USABLE,OPTIMAL', b: 274, w: 0, r: 256, rc: 44, cc: 44 }
{ i: 1, base: 58, p: 4.857980995127572, ppm: 176613, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 58, p: 10.715961990255144, ppm: 66813, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 58, p: 16.57394298538272, ppm: 34629, v: 'USABLE,OPTIMAL', b: 16, w: 16, r: 0, rc: 0, cc: 48 }
{ i: 4, base: 58, p: 23.431923980510287, ppm: 18433, v: 'USABLE,OPTIMAL', b: 23, w: 11, r: 3, rc: 1, cc: 45 }
{ i: 5, base: 58, p: 29.289904975637864, ppm: 9898, v: 'USABLE,OPTIMAL', b: 29, w: 8, r: 24, rc: 5, cc: 45 }
{ i: 6, base: 58, p: 35.14788597076544, ppm: 4208, v: 'USABLE,OPTIMAL', b: 35, w: 7, r: 11, rc: 2, cc: 44 }
{ i: 7, base: 58, p: 41.005866965893006, ppm: 143, v: 'USABLE,OPTIMAL', b: 41, w: 6, r: 10, rc: 2, cc: 44 }
{ i: 1, base: 59, p: 4.882643049361842, ppm: 180772, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 59, p: 10.765286098723683, ppm: 71088, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 59, p: 16.647929148085524, ppm: 38920, v: 'USABLE,OPTIMAL', b: 16, w: 16, r: 0, rc: 0, cc: 48 }
{ i: 4, base: 59, p: 22.530572197447366, ppm: 23549, v: 'USABLE,OPTIMAL', b: 22, w: 11, r: 14, rc: 3, cc: 47 }
{ i: 5, base: 59, p: 29.41321524680921, ppm: 14049, v: 'USABLE,OPTIMAL', b: 29, w: 8, r: 24, rc: 5, cc: 45 }
{ i: 6, base: 59, p: 35.29585829617105, ppm: 8382, v: 'USABLE,OPTIMAL', b: 35, w: 7, r: 11, rc: 2, cc: 44 }
{ i: 7, base: 59, p: 41.17850134553289, ppm: 4335, v: 'USABLE,OPTIMAL', b: 41, w: 6, r: 10, rc: 2, cc: 44 }
{ i: 8, base: 59, p: 47.06114439489473, ppm: 1299, v: 'USABLE,OPTIMAL', b: 47, w: 5, r: 21, rc: 4, cc: 44 }
{ i: 17, base: 59, p: 100.0049318391513, ppm: 49, v: 'USABLE,OPTIMAL', b: 100, w: 2, r: 56, rc: 10, cc: 44 }
{ i: 1, base: 60, p: 4.906890595608519, ppm: 184820, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 60, p: 10.813781191217037, ppm: 75254, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 60, p: 16.720671786825555, ppm: 43101, v: 'USABLE,OPTIMAL', b: 16, w: 16, r: 0, rc: 0, cc: 48 }
{ i: 4, base: 60, p: 22.627562382434075, ppm: 27734, v: 'USABLE,OPTIMAL', b: 22, w: 11, r: 14, rc: 3, cc: 47 }
{ i: 5, base: 60, p: 28.534452978042594, ppm: 18730, v: 'USABLE,OPTIMAL', b: 28, w: 9, r: 4, rc: 1, cc: 46 }
{ i: 6, base: 60, p: 35.44134357365111, ppm: 12453, v: 'USABLE,OPTIMAL', b: 35, w: 7, r: 11, rc: 2, cc: 44 }
{ i: 7, base: 60, p: 41.348234169259634, ppm: 8422, v: 'USABLE,OPTIMAL', b: 41, w: 6, r: 10, rc: 2, cc: 44 }
{ i: 8, base: 60, p: 47.25512476486815, ppm: 5399, v: 'USABLE,OPTIMAL', b: 47, w: 5, r: 21, rc: 4, cc: 44 }
{ i: 9, base: 60, p: 53.16201536047667, ppm: 3048, v: 'USABLE,OPTIMAL', b: 53, w: 4, r: 44, rc: 8, cc: 44 }
{ i: 10, base: 60, p: 59.06890595608519, ppm: 1167, v: 'USABLE,OPTIMAL', b: 59, w: 4, r: 20, rc: 4, cc: 44 }
{ i: 21, base: 60, p: 124.0447025077789, ppm: 360, v: 'USABLE,OPTIMAL', b: 124, w: 2, r: 8, rc: 2, cc: 44 }
{ i: 32, base: 60, p: 189.0204990594726, ppm: 108, v: 'USABLE,OPTIMAL', b: 189, w: 1, r: 67, rc: 12, cc: 44 }
{ i: 75, base: 60, p: 443.0167946706389, ppm: 38, v: 'USABLE,OPTIMAL', b: 443, w: 0, r: 256, rc: 44, cc: 44 }
{ i: 1, base: 61, p: 4.930737337562887, ppm: 188762, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 61, p: 10.861474675125773, ppm: 79315, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 61, p: 16.79221201268866, ppm: 47177, v: 'USABLE,OPTIMAL', b: 16, w: 16, r: 0, rc: 0, cc: 48 }
{ i: 4, base: 61, p: 22.722949350251547, ppm: 31816, v: 'USABLE,OPTIMAL', b: 22, w: 11, r: 14, rc: 3, cc: 47 }
{ i: 5, base: 61, p: 28.65368668781443, ppm: 22813, v: 'USABLE,OPTIMAL', b: 28, w: 9, r: 4, rc: 1, cc: 46 }
{ i: 6, base: 61, p: 34.58442402537732, ppm: 16898, v: 'USABLE,OPTIMAL', b: 34, w: 7, r: 18, rc: 4, cc: 46 }
{ i: 7, base: 61, p: 40.515161362940205, ppm: 12715, v: 'USABLE,OPTIMAL', b: 40, w: 6, r: 16, rc: 3, cc: 45 }
{ i: 8, base: 61, p: 47.44589870050309, ppm: 9398, v: 'USABLE,OPTIMAL', b: 47, w: 5, r: 21, rc: 4, cc: 44 }
{ i: 9, base: 61, p: 53.376636038065975, ppm: 7056, v: 'USABLE,OPTIMAL', b: 53, w: 4, r: 44, rc: 8, cc: 44 }
{ i: 10, base: 61, p: 59.30737337562886, ppm: 5183, v: 'USABLE,OPTIMAL', b: 59, w: 4, r: 20, rc: 4, cc: 44 }
{ i: 11, base: 61, p: 65.23811071319176, ppm: 3650, v: 'USABLE,OPTIMAL', b: 65, w: 3, r: 61, rc: 11, cc: 44 }
{ i: 12, base: 61, p: 71.16884805075463, ppm: 2372, v: 'USABLE,OPTIMAL', b: 71, w: 3, r: 43, rc: 8, cc: 44 }
{ i: 13, base: 61, p: 77.09958538831752, ppm: 1292, v: 'USABLE,OPTIMAL', b: 77, w: 3, r: 25, rc: 5, cc: 44 }
{ i: 14, base: 61, p: 83.03032272588041, ppm: 365, v: 'USABLE,OPTIMAL', b: 83, w: 3, r: 7, rc: 2, cc: 44 }
{ i: 43, base: 61, p: 255.0217055152041, ppm: 85, v: 'USABLE,OPTIMAL', b: 255, w: 1, r: 1, rc: 1, cc: 44 }
{ i: 72, base: 61, p: 427.0130883045278, ppm: 31, v: 'USABLE,OPTIMAL', b: 427, w: 0, r: 256, rc: 44, cc: 44 }
{ i: 1, base: 62, p: 4.954196310386876, ppm: 192604, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 62, p: 10.908392620773752, ppm: 83275, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 62, p: 16.862588931160627, ppm: 51154, v: 'USABLE,OPTIMAL', b: 16, w: 16, r: 0, rc: 0, cc: 48 }
{ i: 4, base: 62, p: 22.816785241547503, ppm: 35798, v: 'USABLE,OPTIMAL', b: 22, w: 11, r: 14, rc: 3, cc: 47 }
{ i: 5, base: 62, p: 28.77098155193438, ppm: 26797, v: 'USABLE,OPTIMAL', b: 28, w: 9, r: 4, rc: 1, cc: 46 }
{ i: 6, base: 62, p: 34.72517786232125, ppm: 20883, v: 'USABLE,OPTIMAL', b: 34, w: 7, r: 18, rc: 4, cc: 46 }
{ i: 7, base: 62, p: 40.67937417270813, ppm: 16701, v: 'USABLE,OPTIMAL', b: 40, w: 6, r: 16, rc: 3, cc: 45 }
{ i: 8, base: 62, p: 46.633570483095006, ppm: 13586, v: 'USABLE,OPTIMAL', b: 46, w: 5, r: 26, rc: 5, cc: 45 }
{ i: 9, base: 62, p: 52.587766793481876, ppm: 11177, v: 'USABLE,OPTIMAL', b: 52, w: 4, r: 48, rc: 9, cc: 45 }
{ i: 10, base: 62, p: 58.54196310386876, ppm: 9258, v: 'USABLE,OPTIMAL', b: 58, w: 4, r: 24, rc: 5, cc: 45 }
{ i: 11, base: 62, p: 65.49615941425563, ppm: 7575, v: 'USABLE,OPTIMAL', b: 65, w: 3, r: 61, rc: 11, cc: 44 }
{ i: 12, base: 62, p: 71.4503557246425, ppm: 6303, v: 'USABLE,OPTIMAL', b: 71, w: 3, r: 43, rc: 8, cc: 44 }
{ i: 13, base: 62, p: 77.40455203502938, ppm: 5226, v: 'USABLE,OPTIMAL', b: 77, w: 3, r: 25, rc: 5, cc: 44 }
{ i: 14, base: 62, p: 83.35874834541626, ppm: 4304, v: 'USABLE,OPTIMAL', b: 83, w: 3, r: 7, rc: 2, cc: 44 }
{ i: 15, base: 62, p: 89.31294465580312, ppm: 3504, v: 'USABLE,OPTIMAL', b: 89, w: 2, r: 78, rc: 14, cc: 44 }
{ i: 16, base: 62, p: 95.26714096619001, ppm: 2804, v: 'USABLE,OPTIMAL', b: 95, w: 2, r: 66, rc: 12, cc: 44 }
{ i: 17, base: 62, p: 101.22133727657689, ppm: 2187, v: 'USABLE,OPTIMAL', b: 101, w: 2, r: 54, rc: 10, cc: 44 }
{ i: 18, base: 62, p: 107.17553358696375, ppm: 1638, v: 'USABLE,OPTIMAL', b: 107, w: 2, r: 42, rc: 8, cc: 44 }
{ i: 19, base: 62, p: 113.12972989735063, ppm: 1147, v: 'USABLE,OPTIMAL', b: 113, w: 2, r: 30, rc: 6, cc: 44 }
{ i: 20, base: 62, p: 119.08392620773752, ppm: 705, v: 'USABLE,OPTIMAL', b: 119, w: 2, r: 18, rc: 4, cc: 44 }
{ i: 21, base: 62, p: 125.03812251812438, ppm: 305, v: 'USABLE,OPTIMAL', b: 125, w: 2, r: 6, rc: 2, cc: 44 }
{ i: 43, base: 62, p: 256.03044134663566, ppm: 119, v: 'USABLE,OPTIMAL', b: 256, w: 1, r: 0, rc: 0, cc: 43 }
{ i: 65, base: 62, p: 387.0227601751469, ppm: 59, v: 'USABLE,OPTIMAL', b: 387, w: 0, r: 256, rc: 43, cc: 43 }
{ i: 1, base: 63, p: 4.977279923499917, ppm: 196348, v: 'USABLE,OPTIMAL', b: 4, w: 64, r: 0, rc: 0, cc: 64 }
{ i: 2, base: 63, p: 10.954559846999834, ppm: 87138, v: 'USABLE,OPTIMAL', b: 10, w: 25, r: 6, rc: 2, cc: 52 }
{ i: 3, base: 63, p: 16.93183977049975, ppm: 55035, v: 'USABLE,OPTIMAL', b: 16, w: 16, r: 0, rc: 0, cc: 48 }
{ i: 4, base: 63, p: 22.909119693999667, ppm: 39684, v: 'USABLE,OPTIMAL', b: 22, w: 11, r: 14, rc: 3, cc: 47 }
{ i: 5, base: 63, p: 28.886399617499585, ppm: 30686, v: 'USABLE,OPTIMAL', b: 28, w: 9, r: 4, rc: 1, cc: 46 }
{ i: 6, base: 63, p: 34.8636795409995, ppm: 24773, v: 'USABLE,OPTIMAL', b: 34, w: 7, r: 18, rc: 4, cc: 46 }
{ i: 7, base: 63, p: 40.84095946449941, ppm: 20591, v: 'USABLE,OPTIMAL', b: 40, w: 6, r: 16, rc: 3, cc: 45 }
{ i: 8, base: 63, p: 46.818239387999334, ppm: 17477, v: 'USABLE,OPTIMAL', b: 46, w: 5, r: 26, rc: 5, cc: 45 }
{ i: 9, base: 63, p: 52.795519311499255, ppm: 15068, v: 'USABLE,OPTIMAL', b: 52, w: 4, r: 48, rc: 9, cc: 45 }
{ i: 10, base: 63, p: 58.77279923499917, ppm: 13149, v: 'USABLE,OPTIMAL', b: 58, w: 4, r: 24, rc: 5, cc: 45 }
{ i: 11, base: 63, p: 64.75007915849908, ppm: 11584, v: 'USABLE,OPTIMAL', b: 64, w: 4, r: 0, rc: 0, cc: 44 }
{ i: 12, base: 63, p: 70.727359081999, ppm: 10284, v: 'USABLE,OPTIMAL', b: 70, w: 3, r: 46, rc: 8, cc: 44 }
{ i: 13, base: 63, p: 76.70463900549892, ppm: 9186, v: 'USABLE,OPTIMAL', b: 76, w: 3, r: 28, rc: 5, cc: 44 }
{ i: 14, base: 63, p: 82.68191892899883, ppm: 8247, v: 'USABLE,OPTIMAL', b: 82, w: 3, r: 10, rc: 2, cc: 44 }
{ i: 15, base: 63, p: 88.65919885249875, ppm: 7435, v: 'USABLE,OPTIMAL', b: 88, w: 2, r: 80, rc: 14, cc: 44 }
{ i: 16, base: 63, p: 94.63647877599867, ppm: 6726, v: 'USABLE,OPTIMAL', b: 94, w: 2, r: 68, rc: 12, cc: 44 }
{ i: 17, base: 63, p: 100.61375869949859, ppm: 6100, v: 'USABLE,OPTIMAL', b: 100, w: 2, r: 56, rc: 10, cc: 44 }
{ i: 18, base: 63, p: 106.59103862299851, ppm: 5545, v: 'USABLE,OPTIMAL', b: 106, w: 2, r: 44, rc: 8, cc: 44 }
{ i: 19, base: 63, p: 112.56831854649842, ppm: 5049, v: 'USABLE,OPTIMAL', b: 112, w: 2, r: 32, rc: 6, cc: 44 }
{ i: 20, base: 63, p: 118.54559846999834, ppm: 4602, v: 'USABLE,OPTIMAL', b: 118, w: 2, r: 20, rc: 4, cc: 44 }
{ i: 21, base: 63, p: 124.52287839349826, ppm: 4199, v: 'USABLE,OPTIMAL', b: 124, w: 2, r: 8, rc: 2, cc: 44 }
{ i: 22, base: 63, p: 130.50015831699815, ppm: 3833, v: 'USABLE,OPTIMAL', b: 130, w: 1, r: 126, rc: 22, cc: 44 }
{ i: 23, base: 63, p: 137.4774382404981, ppm: 3473, v: 'USABLE,OPTIMAL', b: 137, w: 1, r: 119, rc: 20, cc: 43 }
{ i: 24, base: 63, p: 143.454718163998, ppm: 3170, v: 'USABLE,OPTIMAL', b: 143, w: 1, r: 113, rc: 19, cc: 43 }
{ i: 25, base: 63, p: 149.4319980874979, ppm: 2891, v: 'USABLE,OPTIMAL', b: 149, w: 1, r: 107, rc: 18, cc: 43 }
{ i: 26, base: 63, p: 155.40927801099784, ppm: 2634, v: 'USABLE,OPTIMAL', b: 155, w: 1, r: 101, rc: 17, cc: 43 }
{ i: 27, base: 63, p: 161.38655793449774, ppm: 2395, v: 'USABLE,OPTIMAL', b: 161, w: 1, r: 95, rc: 16, cc: 43 }
{ i: 28, base: 63, p: 167.36383785799765, ppm: 2174, v: 'USABLE,OPTIMAL', b: 167, w: 1, r: 89, rc: 15, cc: 43 }
{ i: 29, base: 63, p: 173.3411177814976, ppm: 1968, v: 'USABLE,OPTIMAL', b: 173, w: 1, r: 83, rc: 14, cc: 43 }
{ i: 30, base: 63, p: 179.3183977049975, ppm: 1776, v: 'USABLE,OPTIMAL', b: 179, w: 1, r: 77, rc: 13, cc: 43 }
{ i: 31, base: 63, p: 185.2956776284974, ppm: 1596, v: 'USABLE,OPTIMAL', b: 185, w: 1, r: 71, rc: 12, cc: 43 }
{ i: 32, base: 63, p: 191.27295755199734, ppm: 1427, v: 'USABLE,OPTIMAL', b: 191, w: 1, r: 65, rc: 11, cc: 43 }
{ i: 33, base: 63, p: 197.25023747549724, ppm: 1269, v: 'USABLE,OPTIMAL', b: 197, w: 1, r: 59, rc: 10, cc: 43 }
{ i: 34, base: 63, p: 203.22751739899718, ppm: 1120, v: 'USABLE,OPTIMAL', b: 203, w: 1, r: 53, rc: 9, cc: 43 }
{ i: 35, base: 63, p: 209.2047973224971, ppm: 979, v: 'USABLE,OPTIMAL', b: 209, w: 1, r: 47, rc: 8, cc: 43 }
{ i: 36, base: 63, p: 215.18207724599702, ppm: 846, v: 'USABLE,OPTIMAL', b: 215, w: 1, r: 41, rc: 7, cc: 43 }
{ i: 37, base: 63, p: 221.1593571694969, ppm: 721, v: 'USABLE,OPTIMAL', b: 221, w: 1, r: 35, rc: 6, cc: 43 }
{ i: 38, base: 63, p: 227.13663709299684, ppm: 602, v: 'USABLE,OPTIMAL', b: 227, w: 1, r: 29, rc: 5, cc: 43 }
{ i: 39, base: 63, p: 233.11391701649674, ppm: 489, v: 'USABLE,OPTIMAL', b: 233, w: 1, r: 23, rc: 4, cc: 43 }
{ i: 40, base: 63, p: 239.09119693999668, ppm: 381, v: 'USABLE,OPTIMAL', b: 239, w: 1, r: 17, rc: 3, cc: 43 }
{ i: 41, base: 63, p: 245.06847686349658, ppm: 279, v: 'USABLE,OPTIMAL', b: 245, w: 1, r: 11, rc: 2, cc: 43 }
{ i: 42, base: 63, p: 251.04575678699652, ppm: 182, v: 'USABLE,OPTIMAL', b: 251, w: 1, r: 5, rc: 1, cc: 43 }
{ i: 43, base: 63, p: 257.0230367104964, ppm: 90, v: 'USABLE,OPTIMAL', b: 257, w: 0, r: 256, rc: 43, cc: 43 }
{ i: 1, base: 64, p: 6, ppm: 0, v: 'USABLE,OPTIMAL', b: 6, w: 42, r: 4, rc: 1, cc: 43 }

which tells us that we can do **1 character better** than our base58-producing 44 characters (at base=58, i=7 --> cc=44)
iff we choose to use base62 or higher: the first to hit that spot is base=62, i=43:

{ i: 43, base: 62, p: 256.03044134663566, ppm: 119, v: 'USABLE,OPTIMAL', b: 256, w: 1, r: 0, rc: 0, cc: 43 }

Here w=1 and rc=0 (RemainingCharactersNeeded) tell us that we get this optimum result when we encode the 256-bit number
(b=256, also visible in the raw p number, that's slightly higher than 256) as *a single BigNum*.

Can we achieve encoding in 43 characters in any of the bases investigated iff we DO NOT wish to resort to any
BigNum arithmetic? I.e. is there a cc=43 record with p < 64? 

> WARNING: values like p=64.1617, i.e. slightly *above* 64, all mean we need to perform BigNum arithmetic in 
> any 64-bit environment as the basic divisor then has a log2 power of (slightly) above 64.

*Yes*, we can, but there's only *one* option then: base=64. At i=1, this record says we can then encode the entire
256 bit number in 6-bit chunks, giving us 42 characters in the encoded base (w=42), leaving a left-over of 4 bits 
(r=4), which costs 1 (rc=1) additional character in the encoding base, running up a grand total of 43 characters.

### Do we care about 1 character less?

Well, we started out looking for a minimal size encoding of 256 bit numbers, while using an encoding that MAY be
relatively easy to read (and NOT make mistakes) by humans. Base58 is sub-optimal on that last condition as we still
have o and O vs. 0 in there, but we originally decided base56 (which does not include these o and O characters) wasn't
good enough as it takes 45 characters at best (base=56, i=4), while you CAN get 44 characters wide results when you only
drop the O vs. 0 but keep the o (base=57, i=11): this, however, would mean we'ld still need to do BigNum work as
p is above 64.

Given that we revisited the above and did the more thorough check, wouldn't the same design decisions now mark BASE64 as 
'even better'?

After all, we realize that 'human reading' of these hashes is a rare occasion on the one hand, and the arithmetic
required to encode the number becomes very much simpler (and potentially *faster*) as we now would use a power-of-2
base (base=64) for our encoding, meaning and division and remainder operations become very cheap bit shift operations
instead.

However, base64 means we assume a 27(!) character alphabet and thus have to drop another of our original design criteria:
the encoded hash SHOULD, at first glance, look like a 'word' to a human and a machine (technically, this criterion might
be read as "*behaves as you'ld expect when using regex operators `\b` and `\w`*") as you'ld need *two* punctuation characters
to arrive at an alphabet of 27 (base64 = 10+26+26 + 2). Which is a condition we don't wish to let go.

Incidentally, do note that base58 is the first to hit the 44 character size mark without the need for BigNum arithmetic
(p < 64); base59 and beyond also reach this cc number quickly, but base58 is preferred as it means we have the smallest
required alphabet to produce this result.

### Conclusion

A slightly deeper and wider investigation suggests we could improve on our encoded size of 44 characters for a 256 bit
binary number iff we were to accept a base64 encoding, which requires the use of two punctuation characters next to our
usual alphanumeric alphabet set of 62 (10+26+26).

While this would lead to a faster (and smaller) implementation of the encoder, compared to BASE58X, we do not wish 
to venture there as the '*looks like a word*' criterion is deemed important enough to keep.

Cost of calculus considerations include the assumption that the BASE58X encoding/decoding operation is not expected 
to become a performance bottleneck under any circumstances, thus the benefit of switching to a base64 encoding is meager.
(Where lots of hashes will be processed, e.g. in database queries, the encoding/decoding will not occur at all, as almost
all those checks and comparisons are expected to be executed as *string comparisons* in the database itself. Reducing the
string size from 44 down to 43 would also only gain us 1/44 ~ 2.3% performance gain as the theoretical upper bound: in 
practice, several other actions are required alongside the comparison, swiftly reducing this gain. Which is therefor
considered negligible.

BASE64 might look good on paper, but given our design criteria, BASE58X remains the proper (optimal) choice for encoding
binary numbers to alphanumeric 'words' with shortest possible string length.

BASE58X is a very machine and SQL database (*STRING* / *VARCHAR* !) friendly encoding, human readable (with the appropriate fonts) 
and does neither include *spaces* nor "punctuation", making it easy to transfer and process in all kinds of scripting languages 
and comm protocols: the encoded number has only alphanumeric characters and thus can be regarded as a 'word' in regexes, 
etc. -- the \w and \b regex operators would work as you'ld expect.
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
	// sanity checks
	if (!src || !dst || srcsize == 0 || dstsize == 0)
		return NULL;

	// Consume `src` in 41-bit chunks; pad last incomplete chunk with zero bits in MSBs.
	// Treat every 41-bit chunk as a little endian binary number and decode that into 7 Base58X digits.
	char* rv = dst;
	uint64_t prevbitsbuf = 0;
	int bitpos = 0;

	while (srcsize >= 8)
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
			// and store the remaining bits in the temporary buffer;
			prevbitsbuf = word >> (41 - bitpos);
			bitpos += 64 - 41;
		}
		else
		{
			// temporary buffer is large enough to hold an entire 41-bit number:
			v = prevbitsbuf;
			// mask at 41 bits.
			v &= N41_MASK;
			// and keep the remaining bits in the temporary buffer;
			prevbitsbuf >>= 41;
			bitpos -= 41;
		}

		// now translate this 41-bit number `v` to a 7-digit base58X BIG ENDIAN string segment:
		// (In our case, using BIG ENDIAN notation here makes the decoding logic simpler
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
		ASSERT0(v < 58);
		dst[0] = srcBase58[v];
		dstsize -= 7;
		dst += 7;
	}

	// process the remainder of `src':
	uint64_t last_legs_value = 0;
	uint64_t last_legs_bitcount = 0;

	while (srcsize > 0 || bitpos > 0)
	{
		uint64_t v;

		if (bitpos < 41)
		{
			if (srcsize > 0)
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
				// and store the remaining bits in the temporary buffer;
				prevbitsbuf = word >> (41 - bitpos);
				bitpos += feedbits - 41;
				// before we go and treat v as a full (non-truncated) number,
				// we quickly check if we're on our last legs, i.e. if we've reached the end
				// and have only a few more bits to encode.
				// We'll do that *outside* the loop as we won't need to print all those 7
				// base58 digits, won't we?
				if (bitpos < 0)
				{
					ASSERT0(prevbitsbuf == 0);
					last_legs_value = v;
					last_legs_bitcount = bitpos + 41;
					ASSERT0(last_legs_bitcount >= 0);
					break;
				}
			}
			else
			{
				// we're on our last legs, i.e. we've reached the end and have only a few more bits to encode.
				// We'll do that *outside* the loop as we won't need to print all those 7
				// base58 digits, won't we?
				last_legs_value = prevbitsbuf;
				last_legs_bitcount = bitpos;
				ASSERT0(last_legs_bitcount >= 0);
				break;
			}
		}
		else
		{
			// temporary buffer is large enough to hold an entire 41-bit number
			// (or there's no more bits left to feed us at all)
			v = prevbitsbuf;
			// mask at 41 bits.
			v &= N41_MASK;
			// and keep the remaining bits in the temporary buffer
			prevbitsbuf >>= 41;
			bitpos -= 41;
		}

		// now translate this 41-bit number `v` to a 7-digit base58X BIG ENDIAN string segment:
		// (In our case, using BIG ENDIAN notation here makes the decoding logic simpler
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
		ASSERT0(v < 58);
		dst[0] = srcBase58[v];
		dstsize -= 7;
		dst += 7;
	}

	// We do not print all 7 base58 digits for the last few bits: this would otherwise produce a tail
	// of useless, space-consuming '1' characters we can do without.
	// Of course, this makes the decoder a tad more complicated too as now we must accept that
	// the decoder MAY run into a truncated/partial base58 number at the end of its input.   :'-(
	// The things we do for speed and space savings some times...   ;-D
	if (last_legs_bitcount > 0)
	{
		// determine how many base58 digits we actually need to encode those remaining `last_legs_bitcount` bits:
		uint64_t number_mask = (((uint64_t)1) << last_legs_bitcount) - 1;
		char dst58buf[7];
		int di = 0;

		while (number_mask)
		{
			ASSERT0(di < 7);
			dst58buf[di++] = srcBase58[last_legs_value % 58];
			last_legs_value /= 58;
			number_mask /= 58;
		}

		// we now have the base58 number in little endian form in the intermediate buffer.
		// reverse print those significant base58 digits to `dst` and we're done!
		if (dstsize < di)
			return NULL; // dst buffer overflow

		dstsize -= di;
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
	// sanity init
	if (targetsize_ref)
		*targetsize_ref = 0;

	// sanity checks; also do not accept empty inputs; any valid base58x inputs MUST have at least 2 base58x characters!
	// (with only 1 base58x 'digit' you cannot encode 1 byte. You need at least 2 of those base58x digits to accomplish that!)
	if (!src || !dst || dstsize == 0 || !targetsize_ref || !*src || !src[1])
		return NULL;

	// Consume `src` in 7-digit chunks; when the `src` is not sufficiently large
	// for such a "41-bit encoded number", that's an error.
	// Treat every 7-digit Base58X chunk as a big endian number and decode that into a 41-bit binary number.
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

		// When there's not a *full* (non-truncated) BASE58X number available any more, we
		// quit the loop and go process that tail.
		if (!src[0] || !src[1] || !src[2] || !src[3] || !src[4] || !src[5] || !src[6])
			break;

		// get 7 new digits.
		// big endian, arbitrary memory alignment
		uint64_t word = 0;

#define DECODE_MACRO(pos)					\
		{									\
			uint8_t c = *src++;				\
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
			// Those will be the least significant bits as we're working little endian here in 64-bit binary number land.
			uint64_t v = prevbitsbuf | (word << bitpos);
			// and store the remaining bits in the temporary buffer;
			prevbitsbuf = word >> (64 - bitpos);
			bitpos += 41 - 64;

			// now translate this 64-bit number `v` to an 8-byte buffer segment:
			if (dstsize < 8)
				return NULL; /* dst buffer overflow */

#define PUT_MACRO()											\
		{													\
			uint8_t c = (uint8_t)v;							\
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
			// temporary buffer is not yet large enough to help deliver an entire 64-bit number on the next round:
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
	//
	// We MAY also run into the scenario where the original input was encoded
	// entirely in 'full' BASE58X words and was a non-64bit binary number, i.e.
	// bitpos may indicate we still have some bytes left to output.
	src = last_legs_src;
	if (*src || bitpos > 0)
	{
		// whoops! still a truncated base58 number to do then!
		//
		// get less than 7 new digits.
		// They're little endian, arbitrary memory alignment
		uint64_t word = 0;
		// and make sure we account for those bits in our mask value too:
		ASSERT0(bitpos < 64);

		// to prevent overflow in `number_mask` and the tail-end fetch loop below, we dump as many
		// 8 bit 'digits' of binary value as we can: we're working in a little endian setting
		// there so it's safe to fill the intermediate buffer with those right now, so we can
		// keep our bitpos value minimal.
		while (bitpos >= 8)
		{
			uint8_t c = (uint8_t)prevbitsbuf;
			if (!dstsize)
				return NULL; /* dst buffer overflow */
			*dst++ = c;
			dstsize--;
			prevbitsbuf >>= 8;
			bitpos -= 8;
		}

		uint64_t number_mask = (((uint64_t)1) << bitpos) - 1;

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
			ASSERT0(number_mask < (~((uint64_t)0) - 1) / 58);
			number_mask *= 58;
			number_mask += 57;   // this is kinda like saying `~0` in base 58!   :-)
		}

		// We *know* we won't have enough bits for a full 64-bit word, but that's okay now.
		//
		// merge with the bits we still have from the previous round.
		// Those will be the least significant bits as we're working little endian here in 64-bit binary number land.
		prevbitsbuf |= (word << bitpos);
		// Note: we already accounted for those bits in our mask value thanks to the initialization and continuous update in the loop above.

		// now translate this 1..64-bit number `v` to an 8-byte buffer segment:

		// do we still have a full byte worth of encoded data?
		// When number_mask doesn't /span/ an entire byte, it means we're looking at the tail end bits of
		// the decode and thus these are the extra bits that /seem/ to sit in the last encoded character, but
		// ARE NOT part of the encoded original binary input. This is caused by the size disparity of binary bytes
		// (1 unit = 2^8) vs. base58 'digits' (1 unit = 58).
		while (number_mask >= 0xFF)
		{
			uint8_t c = (uint8_t)prevbitsbuf;
			if (!dstsize)
				return NULL; /* dst buffer overflow */
			*dst++ = c;
			dstsize--;
			prevbitsbuf >>= 8;
			number_mask >>= 8;
		}
		ASSERT0(prevbitsbuf == 0 || src[-1] == 'z');  // this assert is correct, but it also fires in the test rig, when we feed this decoder all-Z inputs to decode: those things would carry surplus bits. Hence the weird src[-1] additional check in there.
	}

	*targetsize_ref = dst - rv;
	return rv;
}

