
#include "base58X.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// base test size: maximum binary input size for all tests: W = width
#define W     4096


#if defined(_MSC_VER) && 01
#pragma warning(disable: 4057 4133)
// warning C4057: 'function': 'uint8_t *' differs in indirection to slightly different base types from 'char [2048]')
// warning C4133: 'function': incompatible types - from 'uint64_t *' to 'uint8_t *'    -- we don't care; we know what we're doing in here!
#endif



#ifndef max
#define max(a, b)  ((a) >= (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b)  ((a) <= (b) ? (a) : (b))
#endif

static void assert_fail_msg(const char* msg, const char* file, int line)
{
	fprintf(stderr, "FAIL: %s (%s @ %d)\n", msg, file, line);
	exit(66);
}

#define assert(expression) (void)(                                                       \
            (!!(expression)) ||                                                          \
            (assert_fail_msg(#expression, __FILE__, __LINE__), 0)                        \
        )

uint64_t rand_seed = 0;
static uint64_t fast_rand()
{
	rand_seed *= 41;
	rand_seed ^= 0xDEADBEEF5a5A5A5AULL;
	return rand_seed;
}

static void fill_bindstbuf_with_random(uint8_t* buf, size_t size)
{
	// we KNOW we're 64-bit aligned as this is special purpose code.
	uint64_t* p = (uint64_t*)buf;
	size /= 8;

	// pre-heat random:
	fast_rand();
	fast_rand();

	for (size_t i = 0; i < size; i++)
	{
		*p++ = fast_rand();
	}
}

static void fill_bindstbuf_with_1_bits(uint8_t* buf, size_t size, size_t number_of_bits)
{
	if (number_of_bits / 8 > size)
		number_of_bits = size * 8;

	// we KNOW we're 64-bit aligned as this is special purpose code.
	uint64_t* p = (uint64_t*)buf;
	size /= 8;

	// zero up to the next 8bit boundary, RANDOM after that so we'll be able to see undesirable "pickup noise".

	// pre-heat random:
	fast_rand();
	fast_rand();

	size_t n = number_of_bits / 64;
	for (size_t i = 0; i < n; i++)
	{
		*p++ = ~((uint64_t)0ULL);
	}

	number_of_bits -= n * 64;
	assert(size >= n);
	size -= n;

	// think *bytes* from here on out:
	size *= 8;

	fill_bindstbuf_with_random(p, size);

	size_t n8 = number_of_bits / 8;
	buf = (uint8_t*)p;
	for (size_t i = 0; i < n8; i++)
	{
		*buf++ = ~((uint8_t)0);
	}

	number_of_bits -= n8 * 8;
	size -= n;

	assert(number_of_bits == 0 && "fill_bindstbuf_with_1_bits invoke fail; internal test error");
	assert(size > 0 && "fill_bindstbuf_with_1_bits invoke fail; internal test error");

	*buf++ = 0;
}

static void fill_bindstbuf_with_0_bits(uint8_t* buf, size_t size, size_t number_of_bits)
{
	if (number_of_bits / 8 > size)
		number_of_bits = size * 8;

	// we KNOW we're 64-bit aligned as this is special purpose code.
	uint64_t* p = (uint64_t*)buf;
	size /= 8;

	// zero up to the next 8bit boundary, RANDOM after that so we'll be able to see undesirable "pickup noise".

	// pre-heat random:
	fast_rand();
	fast_rand();

	size_t n = number_of_bits / 64;
	for (size_t i = 0; i < n; i++)
	{
		*p++ = 0;
	}

	number_of_bits -= n * 64;
	assert(size >= n);
	size -= n;

	// think *bytes* from here on out:
	size *= 8;

	fill_bindstbuf_with_random(p, size);

	size_t n8 = number_of_bits / 8;
	buf = (uint8_t*)p;
	for (size_t i = 0; i < n8; i++)
	{
		*buf++ = 0;
	}

	number_of_bits -= n8 * 8;
	size -= n;

	assert(number_of_bits == 0 && "fill_bindstbuf_with_0_bits invoke fail; internal test error");
	assert(size > 0 && "fill_bindstbuf_with_0_bits invoke fail; internal test error");

	*buf++ = ~0;
}

static void fill_bindstbuf_with_anti_touch(uint8_t* buf, size_t size)
{
	memset(buf, 0xA5, size);
}

static void fill_encbuf_with_1s_base58x(char* encbuf, size_t size, size_t enc_length, int c)
{
	fill_bindstbuf_with_anti_touch(encbuf, size);

	// the highest digit in base58x, i.e. the 'all-1s' digit, is 'z':
	assert(enc_length + 1 < size);
	for (size_t i = 0; i < enc_length; i++)
	{
		*encbuf++ = c;
	}
	*encbuf = 0; // NUL sentinel
}

// return 0 if tail of buf does not match anti_touch pattern:
static int check_anti_touch(uint8_t* buf, size_t size, size_t touched_lead_len)
{
	buf += touched_lead_len;
	size -= touched_lead_len;

	if (size > 0)
	{
		for (size_t i = 0; i < size; i++)
		{
			if (buf[i] != 0xA5)
			{
				fprintf(stderr, "FAIL: buffer touched at offset %zu.\n", i + touched_lead_len);
				return 0;
			}
		}
	}
	return 1;
}

// return 0 if a and b do not match exactly.
static int check_binary_match(uint8_t* buf_target, uint8_t* buf_src, size_t size)
{
	// we don't use a simple memcmp() for this as we want the offset reported:
	for (size_t i = 0; i < size; i++)
	{
		if (buf_target[i] != buf_src[i])
		{
			fprintf(stderr, "FAIL: binary data does not match at offset %zu: target: %02x <--> source: %02x.\n", i, buf_target[i], buf_src[i]);
			return 0;
		}
	}
	return 1;
}

// return 0 if a and b do not match near well enough (see test notes in section where this custom check is used).
static int check_base58x_near_match(const char* second, const char* master)
{
	size_t i;

	for (i = 0; master[i]; i++)
	{
		if (second[i] == master[i])
		{
			continue;
		}
		else
		{
			if (!second[i])
			{
				// shortened re-encode: re-encode dropped 1(one) surplus base58x 'digit' in the resulting string.
				if (master[i + 1] == 0)
				{
					return 1;
				}
				else
				{
					// fail!
					return 0;
				}
			}
			else
			{
				// when the digits differ after re-encode, than these are supposed to be the very last digit of the string:
				if (master[i + 1] == 0 && second[i + 1] == 0)
				{
					return 1;
				}
				else
				{
					// fail!
					return 0;
				}
			}
		}
	}
	assert(second[i] == 0);
	return 1;
}


int main(void)
{
	fprintf(stderr, "Testing BASE58X encoder/decoder:\n");

	int i;
	uint8_t binsrcbuf[W + 8];
	uint8_t bindstbuf[W + 8];
	char encbuf[W * 2];		// plenty space, oversized
	char re_encbuf[W * 2];

	const char* encrv;
	const uint8_t* binrv;
	size_t targetsize;

	fprintf(stderr, "API sanity checks:\n");

	encrv = EncodeBase58X(encbuf, sizeof(encbuf), bindstbuf, 0);
	assert(encrv == NULL && "src size is NIL");
	encrv = EncodeBase58X(encbuf, sizeof(encbuf), NULL, 1);
	assert(encrv == NULL && "src is NULL");
	encrv = EncodeBase58X(encbuf, 0, bindstbuf, 1);
	assert(encrv == NULL && "dst size is NIL");
	encrv = EncodeBase58X(NULL, sizeof(encbuf), bindstbuf, 1);
	assert(encrv == NULL && "dst is NULL");

	const char* dummy_enc_str = "12345671234567";
	targetsize = 1;
	binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, NULL);
	assert(binrv == NULL && "src is NULL");
	assert(targetsize == 0);
	targetsize = 1;
	binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, "");
	assert(binrv == NULL && "src is NIL");
	assert(targetsize == 0);
	binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), NULL, dummy_enc_str);
	assert(binrv == NULL && "targetsize_ref is NULL");
	targetsize = 1;
	binrv = DecodeBase58X(bindstbuf, 0, &targetsize, dummy_enc_str);
	assert(binrv == NULL && "dst size is NIL");
	assert(targetsize == 0);
	targetsize = 1;
	binrv = DecodeBase58X(NULL, sizeof(bindstbuf), &targetsize, dummy_enc_str);
	assert(binrv == NULL && "dst is NULL");
	assert(targetsize == 0);

	// now test encode+decode for various binary input sizes.
	for (i = 1; i < sizeof(bindstbuf); i++)
	{
#if 0
		fprintf(stderr, "testing size: %u bytes input\n", i);
#endif

		size_t enclen_base = 0;

#if defined(_DEBUG)
#define M                 13
#else
#define M                 101
#endif

		// N random attempts per size; perform a lot more tests for 256bit (BLAKE3 hash size) inputs so we can run this test code in the profiler and get a bit of an idea about performance:
		for (int n = (i == 256 / 8 ? 1000 : 1) * M; n > 0; n--)
		{
			fill_bindstbuf_with_random(binsrcbuf, sizeof(binsrcbuf));

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));
			fill_bindstbuf_with_anti_touch(encbuf, sizeof(encbuf));

			// encode + decode + verify a random binary number of i bytes:
			encrv = EncodeBase58X(encbuf, sizeof(encbuf), binsrcbuf, i);
			assert(encrv != NULL);
			assert(encrv == encbuf);

			size_t enclen = strlen(encbuf);
			assert(enclen > 0);
			assert(enclen + 1 < sizeof(encbuf));
			assert(check_anti_touch(encbuf, sizeof(encbuf), enclen + 1 /* NUL sentinel byte of string */));

			if (!enclen_base)
				enclen_base = enclen;
			else
			{
				assert(enclen == enclen_base && "all encodings of the same binary length are supposed to produce same-size base58x encodings");
			}


			targetsize = 0;

			binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, encrv);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i && "encode+decode is supposed to produce same-length binary code.");
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

			assert(check_binary_match(bindstbuf, binsrcbuf, targetsize) && "encode+decode is supposed to REproduce the original binary input data.");

			// -----------------------------------------------------------------------------------------
			// and the same thing, now with limited buffer size:

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));
			fill_bindstbuf_with_anti_touch(encbuf, sizeof(encbuf));

			// encode + decode + verify a random binary number of i bytes:
			encrv = EncodeBase58X(encbuf, enclen + 1, binsrcbuf, i);
			assert(encrv != NULL);
			assert(encrv == encbuf);

			size_t enclen2 = strlen(encbuf);
			assert(enclen2 > 0);
			assert(enclen2 == enclen && "destination buffer size should never impact this way");
			assert(check_anti_touch(encbuf, sizeof(encbuf), enclen + 1 /* NUL sentinel byte of string */));

			targetsize = 0;

			binrv = DecodeBase58X(bindstbuf, i, &targetsize, encrv);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i && "encode+decode is supposed to produce same-length binary code.");
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

			assert(check_binary_match(bindstbuf, binsrcbuf, targetsize) && "encode+decode is supposed to REproduce the original binary input data.");
		}

		// explicit test for all-1 and all-0 bit inputs
		{
			fill_bindstbuf_with_1_bits(binsrcbuf, sizeof(binsrcbuf), i * 8);

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));
			fill_bindstbuf_with_anti_touch(encbuf, sizeof(encbuf));

			// encode + decode + verify a random binary number of i bytes:
			encrv = EncodeBase58X(encbuf, sizeof(encbuf), binsrcbuf, i);
			assert(encrv != NULL);
			assert(encrv == encbuf);

			size_t enclen = strlen(encbuf);
			assert(enclen > 0);
			assert(enclen + 1 < sizeof(encbuf));
			assert(check_anti_touch(encbuf, sizeof(encbuf), enclen + 1));
			assert(enclen == enclen_base && "all encodings of the same binary length are supposed to produce same-size base58x encodings");

			targetsize = 0;
			binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, encrv);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i && "encode+decode is supposed to produce same-length binary code.");
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

			assert(check_binary_match(bindstbuf, binsrcbuf, targetsize) && "encode+decode is supposed to REproduce the original binary input data.");

			// -----------------------------------------------------------------------------------------
			// and the same thing, now with limited buffer size:

			fill_bindstbuf_with_1_bits(binsrcbuf, sizeof(binsrcbuf), i * 8);

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));
			fill_bindstbuf_with_anti_touch(encbuf, sizeof(encbuf));

			// encode + decode + verify a random binary number of i bytes:
			encrv = EncodeBase58X(encbuf, enclen + 1, binsrcbuf, i);
			assert(encrv != NULL);
			assert(encrv == encbuf);

			size_t enclen2 = strlen(encbuf);
			assert(enclen2 > 0);
			assert(enclen2 == enclen);
			assert(check_anti_touch(encbuf, sizeof(encbuf), enclen + 1));

			targetsize = 0;
			binrv = DecodeBase58X(bindstbuf, i, &targetsize, encrv);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i && "encode+decode is supposed to produce same-length binary code.");
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

			assert(check_binary_match(bindstbuf, binsrcbuf, targetsize) && "encode+decode is supposed to REproduce the original binary input data.");
		}
		{
			fill_bindstbuf_with_0_bits(binsrcbuf, sizeof(binsrcbuf), i * 8);

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));
			fill_bindstbuf_with_anti_touch(encbuf, sizeof(encbuf));

			// encode + decode + verify a random binary number of i bytes:
			encrv = EncodeBase58X(encbuf, sizeof(encbuf), binsrcbuf, i);
			assert(encrv != NULL);
			assert(encrv == encbuf);

			size_t enclen = strlen(encbuf);
			assert(enclen > 0);
			assert(enclen + 1 < sizeof(encbuf));
			assert(check_anti_touch(encbuf, sizeof(encbuf), enclen + 1));
			assert(enclen == enclen_base && "all encodings of the same binary length are supposed to produce same-size base58x encodings");

			targetsize = 0;
			binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, encrv);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i && "encode+decode is supposed to produce same-length binary code.");
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

			assert(check_binary_match(bindstbuf, binsrcbuf, targetsize) && "encode+decode is supposed to REproduce the original binary input data.");

			// -----------------------------------------------------------------------------------------
			// and the same thing, now with limited buffer size:

			fill_bindstbuf_with_0_bits(binsrcbuf, sizeof(binsrcbuf), i * 8);

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));
			fill_bindstbuf_with_anti_touch(encbuf, sizeof(encbuf));

			// encode + decode + verify a random binary number of i bytes:
			encrv = EncodeBase58X(encbuf, enclen + 1, binsrcbuf, i);
			assert(encrv != NULL);
			assert(encrv == encbuf);

			size_t enclen2 = strlen(encbuf);
			assert(enclen2 > 0);
			assert(enclen2 == enclen);
			assert(check_anti_touch(encbuf, sizeof(encbuf), enclen + 1));

			targetsize = 0;
			binrv = DecodeBase58X(bindstbuf, i, &targetsize, encrv);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i && "encode+decode is supposed to produce same-length binary code.");
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

			assert(check_binary_match(bindstbuf, binsrcbuf, targetsize) && "encode+decode is supposed to REproduce the original binary input data.");
		}

		// also check that feeding 'all-1' base58X digits into the decoder don't mess us up:
		//
		// NOTE/WARNING: as we construct fake base58x encodings in here, we know *some* will have sizes i that won't be
		// possible in practice: as we step through the binary encode sizes, 1 byte increment at a time, we *know* the
		// base58x encoding of these binary inputs will be increasing their size approximately 44 characters for every
		// 32 bytes of input. As we step-increment through those 32 input bytes, we always get the same output size, thus
		// we expect 32 (!) base58x output sizes. This means that from 44 consecutive base58x string lengths, 44-32 = 12
		// lengths will be 'invalid', i.e. impossible to produce with a properly functioning encoder.
		//
		// Still we do test all base58x strings lengths, so that will imply we will feed the *decoder* at least 12-from-every-44
		// base58x strings with **surplus bits**. Those surplus bits will be, or MUST BE, irrelevant as the surplus never SHOULD
		// cross a decoded binary byte boundary.
		// That we verify by re-encoding the binary decoded result: we will be sure to get a different encoding:
		// depending on which size we currently test, we expect either the same length or 1(one) less.
		//
		// Do also note that a base58x 7-digit 'number' is what we use as a base entity for decoding to binary. This means that
		// a 'zzzzzzz' (7 'z' sub-digits) is a base58x number that DOES NOT represent a legal 41-bit binary value, hence we MUST
		// expect all re-encodings to be entirely different.
		//
		// For that reason we also run this test with an all-'7' base58x input: that one should reproduce as a re-encode, apart
		// from some discrepancies in the tail end...
		// 
		// 2nd note: we never expect more than a 1 character length difference between faked original and re-encode because
		// 2 base58x digits encompass an entire binary byte (plus surplus), so the 'size gaps' can never be larger than 1.
		{
			// the highest digit in base58x, i.e. the 'all-1s' digit, is 'z':
			fill_encbuf_with_1s_base58x(encbuf, sizeof(encbuf), i, 'z');

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));

			size_t enclen = strlen(encbuf);
			assert(enclen > 0);
			assert(enclen + 1 < sizeof(encbuf));

			targetsize = 1;
			binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, encbuf);

			// only when the base58x size is larger than 1 do we expect sane decodings; otherwise we expect a failure feedback (binrv = NULL):
			if (i > 1)
			{
				assert(binrv != NULL);
				assert(binrv == bindstbuf);

				assert(targetsize > 0);
				assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

				// now go and re-encode the binary output: the resulting base58x re-encode must be
				// same size or 1 smaller and only different at the tail end.

				fill_bindstbuf_with_anti_touch(re_encbuf, sizeof(re_encbuf));

				encrv = EncodeBase58X(re_encbuf, enclen + 1, bindstbuf, targetsize);
				assert(encrv != NULL);
				assert(encrv == re_encbuf);

				size_t enclen2 = strlen(re_encbuf);
				assert(enclen2 > 0);
				assert(enclen2 == enclen || enclen2 == enclen - 1);
				assert(check_anti_touch(re_encbuf, sizeof(re_encbuf), enclen2 + 1));

				//assert(check_base58x_near_match(re_encbuf, encbuf) && "decode+encode is supposed to REproduce the original binary input data. See the test notes above this section for caveats");
			}
			else
			{
				// can't have a decent decode with only 1 character wide base58x input: you at least need 2 base58x 'digits' to encode a single byte!
				assert(binrv == NULL);
				assert(targetsize == 0);
			}

			// -----------------------------------------------------------------------------------------
			// and the same thing, now with a known decoded binary output size:

			fill_encbuf_with_1s_base58x(encbuf, sizeof(encbuf), enclen_base, 'z');

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));

			size_t enclen2 = strlen(encbuf);
			assert(enclen2 > 0);
			assert(enclen2 + 1 < sizeof(encbuf));
			assert(enclen2 == enclen_base);

			targetsize = 0;
			binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, encbuf);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i);
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));
		}

		// ... and re-run the above all-'z' test with all-0s, where an all-0 base58x digit is a legal representation of a 41-bit binary value:
		{
			const int c = '1';

			fill_encbuf_with_1s_base58x(encbuf, sizeof(encbuf), i, c);

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));

			size_t enclen = strlen(encbuf);
			assert(enclen > 0);
			assert(enclen + 1 < sizeof(encbuf));

			targetsize = 1;
			binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, encbuf);

			// only when the base58x size is larger than 1 do we expect sane decodings; otherwise we expect a failure feedback (binrv = NULL):
			if (i > 1)
			{
				assert(binrv != NULL);
				assert(binrv == bindstbuf);

				assert(targetsize > 0);
				assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

				// now go and re-encode the binary output: the resulting base58x re-encode must be
				// same size or 1 smaller and only different at the tail end.

				fill_bindstbuf_with_anti_touch(re_encbuf, sizeof(re_encbuf));

				encrv = EncodeBase58X(re_encbuf, enclen + 1, bindstbuf, targetsize);
				assert(encrv != NULL);
				assert(encrv == re_encbuf);

				size_t enclen2 = strlen(re_encbuf);
				assert(enclen2 > 0);
				assert(enclen2 == enclen || enclen2 == enclen - 1);
				assert(check_anti_touch(re_encbuf, sizeof(re_encbuf), enclen2 + 1));

				assert(check_base58x_near_match(re_encbuf, encbuf) && "decode+encode is supposed to REproduce the original binary input data. See the test notes above this section for caveats");
			}
			else
			{
				// can't have a decent decode with only 1 character wide base58x input: you at least need 2 base58x 'digits' to encode a single byte!
				assert(binrv == NULL);
				assert(targetsize == 0);
			}

			// -----------------------------------------------------------------------------------------
			// and the same thing, now with a known decoded binary output size:

			fill_encbuf_with_1s_base58x(encbuf, sizeof(encbuf), enclen_base, c);

			fill_bindstbuf_with_anti_touch(bindstbuf, sizeof(bindstbuf));

			size_t enclen2 = strlen(encbuf);
			assert(enclen2 > 0);
			assert(enclen2 + 1 < sizeof(encbuf));
			assert(enclen2 == enclen_base);

			targetsize = 0;
			binrv = DecodeBase58X(bindstbuf, sizeof(bindstbuf), &targetsize, encbuf);
			assert(binrv != NULL);
			assert(binrv == bindstbuf);

			assert(targetsize == i);
			assert(check_anti_touch(bindstbuf, sizeof(bindstbuf), targetsize));

			// re-encode to test length, etc.

			fill_bindstbuf_with_anti_touch(re_encbuf, sizeof(re_encbuf));

			encrv = EncodeBase58X(re_encbuf, enclen2 + 1, bindstbuf, targetsize);
			assert(encrv != NULL);
			assert(encrv == re_encbuf);

			size_t enclen3 = strlen(re_encbuf);
			assert(enclen3 > 0);
			assert(enclen3 == enclen2);
			assert(check_anti_touch(re_encbuf, sizeof(re_encbuf), enclen2 + 1));

			assert(check_base58x_near_match(re_encbuf, encbuf) && "decode+encode is supposed to REproduce the original binary input data. See the test notes above this section for caveats");
		}
	}

	fprintf(stderr, "OK: all tests passed!\n");
	return 0;
}
