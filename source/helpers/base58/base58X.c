
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
// WE DO NOT USE THIS FOR BITCOIN WORK. This implementation is used in comjunction
// with BLAKE3-based fingerprinting of documents, where the generated fingerprint should
// be *short*(ish) to speed string comparisons anywhere, while the underlying hash
// should *virtually guarantee* uniqueness of hash for any content/file so file content
// can be uniquely identified using only the fingerprint.
//
// Therefor we also not apply any checksums or other extra 'validation mechanisms'
// to the generated fingerprint STRING.
//
// Also note that the length is not encoded either as the wwidth of the raw fingerprint
// is known to both encoder and decoder and *predetermined/fixed*.
//
// Also note that we DO NOT encode zeroes in a special way: we expect to encode random
// data (the bytes of a cryptographic hash) so no byte values should 'predominate' in
// the raw hash, thus making special handling of any of those a moot point.

/*
> for (let i = 1; i < 16; i++) { console.log({i, p: Math.log(Math.pow(58,i))/Math.log(2) }); }
{ i: 1, p: 5.857980995127572 }
{ i: 2, p: 11.715961990255144 }
{ i: 3, p: 17.57394298538272 }
{ i: 4, p: 23.431923980510287 }
{ i: 5, p: 29.289904975637864 }
{ i: 6, p: 35.14788597076544 }
{ i: 7, p: 41.005866965893006 }    <-- 41 bits of binary data is VERY close to 7 Base58 characters. Nice!
{ i: 8, p: 46.863847961020575 }
{ i: 9, p: 52.72182895614815 }
{ i: 10, p: 58.57980995127573 }
{ i: 11, p: 64.4377909464033 }
{ i: 12, p: 70.29577194153087 }
{ i: 13, p: 76.15375293665845 }
{ i: 14, p: 82.01173393178601 }
{ i: 15, p: 87.86971492691359 }
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
	uint64_t last_legs_value;
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

