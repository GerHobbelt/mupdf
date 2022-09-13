//
// Create a non-zero *positive* 64-bit document id suitable for Manticore and other uses.
// The source is assumed to be a Qiqqa v2 fingerprint hash for a given input file (PDF)
//

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/system-header-files.h"

#include "blake3.h"
#include "../helpers/base58/base58X.h"

#include <zendian.h>   // thank you, zlib!  :-)

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <endian.h>
#endif



static inline void memclr(void* ptr, size_t size)
{
	memset(ptr, 0, size);
}


inline static int64_t fold_hash_to_documentid62_aligned64(const uint64_t* hash, int hash_length /* BLAKE3_OUT_LEN */)
{
	ASSERT0(hash_length == BLAKE3_OUT_LEN);
	ASSERT0(BLAKE3_OUT_LEN == 32);

	// phase 1: fold all qwords in the source hash as-is, using XOR: that's 32/8 == 4 qwords' worth
	uint64_t folded = hash[0]
		^ hash[1]
		^ hash[2]
		^ hash[3];

	// now compensate for the "positive values only" requirement by masking off the top 2 bits:
	folded &= ~(3ULL << 62);

	// and prep an additional value to fold into the mix later: this value will carry those chopped-off bits.
	// make it easier on the CPU and ourselves by addressing the hash as bytes: that way, our corrective
	// right-shift operations will be small -- if the CPU has a barrel-shifter, it will do this very swiftly.
	const uint8_t* h8 = (const uint8_t*)hash;

#if BYTE_ORDER == LITTLE_ENDIAN
	// this assumes LITTLE ENDIAN memory layout: goes for most hardware these days
	uint64_t v = h8[3] >> 6                   // unsigned int8 hence a *logical shift right* injecting zeroes at left
		| (h8[4 + 3] >> (6 - 2)) & 0x000C     // have this one occupy the next 2 bits in the `v` value
		| (h8[8 + 3] >> (6 - 4)) & 0x0030     // and so on...
		| (h8[12 + 3] & 0x00C0);
#elif BYTE_ORDER == BIG_ENDIAN
	uint64_t v = h8[0] >> 6                   // unsigned int8 hence a *logical shift right* injecting zeroes at left
		| (h8[4] >> (6 - 2)) & 0x000C         // have this one occupy the next 2 bits in the `v` value
		| (h8[8] >> (6 - 4)) & 0x0030         // and so on...
		| (h8[12] & 0x00C0);
#else
#error "Whoopsky! You need to set up the target system's Endianess for this to fly!"
#endif

	// do we distribute these bits across the folded value? No. We would come 6 bits short (or 2 surplus) anyway,
	// and it isn't expected to make an impact, knowing that we started with a thoroughly shuffled hash already,
	// so we do our best **not to nuke that one** but that's it!
#if 0
	uint64_t spread = v + (v << 8) + (v << 16) + (v << 24);
	spread &= ~(3ULL << 62);
	v = spread;
#endif

	// mix it into the folded value using another XOR:
	folded ^= v;

	// we now have a guaranteed-in-the-range {0..2^62-1} non-negative 64-bit integer range.
	// The last bit to do is take care of that "cannot be zero" requirement. That's easy as we're safe to do just this:
	return folded + 1;
}

inline static int64_t fold_hash_to_documentid62_unaligned(const uint8_t* hash, int hash_length /* BLAKE3_OUT_LEN */)
{
	ASSERT0(hash_length == BLAKE3_OUT_LEN);
	uint64_t hbuf[BLAKE3_OUT_LEN / 8];

	memcpy(hbuf, hash, BLAKE3_OUT_LEN);
	return fold_hash_to_documentid62_aligned64(hbuf, BLAKE3_OUT_LEN);
}

inline static int64_t fold_hash_to_documentid62(const uint8_t* hash, int hash_length /* BLAKE3_OUT_LEN */)
{
	// check memory alignment of the hash:
	if ((uint32_t)((intptr_t)hash) & 0x07)
	{
		return fold_hash_to_documentid62_unaligned(hash, hash_length);
	}
	// otherwise we've qword-aligned memory and start to word immediately
	return fold_hash_to_documentid62_aligned64((const uint64_t*)hash, hash_length);
}


static fz_context* ctx = NULL;
static fz_output* out = NULL;
static fz_stream* datafeed = NULL;

static void usage(void)
{
	fz_info(ctx,
		"documentid62: produce a modern (BLAKE3-based) Qiqqa fingerprint 64-bit document id for use with Manticore et al\n"
		"\n"
		"Syntax: documentid62 [options] <file-to-fingerprint>\n"
		"\n"
		"Options:\n"
		"  -d      output DocID62 as decimal number (default is implicit '-d')\n"
		"  -x      output DocID62 as hexadecimal number\n"
		"  -b      output DocID62 as binary number\n"
		"  -o [destination]\n"
		"          specify a destination other than stdout for the calculated hash.\n"
		"  -v      verbose (repeat to increase the chattiness of the application)\n"
		"  -q      quiet ~ not verbose at all\n"
		"\n"
		"  -V      display the version of this application and terminate\n"
	);
}


static void mu_drop_context(void)
{
	fz_close_output(ctx, out);
	fz_drop_output(ctx, out);
	out = NULL;
	fz_drop_stream(ctx, datafeed);
	datafeed = NULL;
	fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	ctx = NULL;
}


int qiqqa_documentid62_main(int argc, const char** argv)
{
	int verbosity = 0;
	int c;
	const char* output = NULL;
	int as_decimal = 0;
	int as_hexadecimal = 0;
	int as_binary = 0;
	int as_pos = 0;

	ctx = NULL;
	out = NULL;
	datafeed = NULL;

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:qvVdxb")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;

		case 'b': as_binary = ++as_pos; break;
		case 'd': as_decimal = ++as_pos; break;
		case 'x': as_hexadecimal = ++as_pos; break;

		case 'q': verbosity = 0; break;

		case 'v': verbosity++; break;

		case 'V': fz_info(ctx, "documentid62 version %s/%s", FZ_VERSION, blake3_version()); return EXIT_FAILURE;

		default: usage(); return EXIT_FAILURE;
		}
	}

	atexit(mu_drop_context);

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		usage();
		return EXIT_FAILURE;
	}

	// implicit 'print as decimal':
	if (!as_binary && !as_decimal && !as_hexadecimal)
		as_decimal = ++as_pos;

	const char* datafilename = NULL;
	int errored = 0;

	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
		{
			out = fz_stdout(ctx);
			output = NULL;
		}
		else
		{
			char fbuf[PATH_MAX];
			fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
			fz_normalize_path(ctx, fbuf, sizeof fbuf, fbuf);
			fz_sanitize_path(ctx, fbuf, sizeof fbuf, fbuf);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		while (fz_optind < argc)
		{
			// load a datafile if we already have a script AND we're in "template mode".
			datafilename = argv[fz_optind++];

			datafeed = fz_open_file(ctx, datafilename);
			if (datafeed == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open datafile: %s", datafilename);

			// Initialize the hasher.
			blake3_hasher hasher;
			blake3_hasher_init(&hasher);

			// Read input bytes from file.
			unsigned char buf[65536];
			size_t n = fz_read(ctx, datafeed, buf, sizeof(buf));
			while (n > 0)
			{
				blake3_hasher_update(&hasher, buf, n);
				n = fz_read(ctx, datafeed, buf, sizeof(buf));
			}

			// Finalize the hash. BLAKE3_OUT_LEN is the default output length, 32 bytes.
			uint8_t hash[BLAKE3_OUT_LEN];
			blake3_hasher_finalize(&hasher, hash, BLAKE3_OUT_LEN);

			// now fold the fingerprint into the document id:
			int64_t docid = fold_hash_to_documentid62(hash, BLAKE3_OUT_LEN);

			// Print the hash
			if (verbosity)
			{
				fz_write_printf(ctx, out, "%q: %s\n", datafilename, (as_pos > 1 ? "{" : ""));
				for (int pos = 1; pos <= as_pos; pos++)
				{
					if (as_decimal == pos)
					{
						if (as_pos > 1)
							fz_write_printf(ctx, out, "  %q: ", "decimal");
						fz_write_printf(ctx, out, "%I64d", docid);
						as_decimal = 0;
					}
					else if (as_hexadecimal == pos)
					{
						if (as_pos > 1)
							fz_write_printf(ctx, out, "  %q: ", "hex");
						fz_write_printf(ctx, out, "%08jI64X", docid);
						as_hexadecimal = 0;
					}
					else if (as_binary == pos)
					{
						if (as_pos > 1)
							fz_write_printf(ctx, out, "  %q: ", "binary");
						fz_write_printf(ctx, out, "%064jI64B", docid);
						as_binary = 0;
					}
					else
					{
						// somebody has been a smart alec and specified something like `-bdxdd` on the command line: only the last `d` will be printed (@ pos = 5)
						continue;
					}

					// at least one more to go?
					if (as_binary || as_decimal || as_hexadecimal)
						fz_write_char(ctx, out, ',');
					fz_write_char(ctx, out, '\n');
				}
				if (as_pos > 1)
					fz_write_string(ctx, out, "}\n");
			}
			else
			{
				for (int pos = 1; pos <= as_pos; pos++)
				{
					if (as_decimal == pos)
					{
						fz_write_printf(ctx, out, "%I64d", docid);
						as_decimal = 0;
					}
					else if (as_hexadecimal == pos)
					{
						fz_write_printf(ctx, out, "%08I64X", docid);
						as_hexadecimal = 0;
					}
					else if (as_binary == pos)
					{
						fz_write_printf(ctx, out, "%064I64B", docid);
						as_binary = 0;
					}

					// at least one more to go?
					if (as_binary || as_decimal || as_hexadecimal)
						fz_write_char(ctx, out, '\t');
				}
				fz_write_char(ctx, out, '\n');
			}
			if (datafeed)
			{
				fz_drop_stream(ctx, datafeed);
				datafeed = NULL;
			}
		}
	}
	fz_catch(ctx)
	{
		if (datafeed)
		{
			fz_drop_stream(ctx, datafeed);
			datafeed = NULL;
		}

		fz_error(ctx, "Failure while processing %q: %s", datafilename, fz_caught_message(ctx));

		errored = 1;
	}

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}
