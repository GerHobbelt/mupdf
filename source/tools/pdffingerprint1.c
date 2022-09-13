//
// Create a Qiqqa v2 fingerprint hash for a given input file (PDF)
//
// Qiqqa's classic fingerprint was a b0rked variant use of SHA1, originally coded in C#/.NET.
// See fingerprint0 for a C implementation of the same.
//
// Qiqqa v2 fingerprint is meant to replace the classic fingerprint and should give us:
// - a fast fingerprint
// - a fingerprint which virtually guarantees uniqueness for large sets of files,
//   including when those file sets contain specially crafted adversarial entries.
//
// (The second requirement excludes SHA1 and MD5 from the candidates as there are
// known collisions, for PDF files, available in the wild.)
//
// The cryptographic hash we've selected for the job is BLAKE3, which is a *faster*
// development based on BLAKE2 and further insights by its designers.
//

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/system-header-files.h"

#include "blake3.h"
#include "../helpers/base58/base58X.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#endif



// Tool config:
// 
// #define this one when you wish to include the test code as well; this will break the output of the tool though, as it will print more (and different) stuff than usual.
//
// Note: there's also the regular test rig for BASE58X: base58x_test, which tests the custom part following BLAKE3 more thoroughly.
#if !defined(NDEBUG)
#define FINGERPRINT1_TEST_VERIFY
#endif



static inline void memclr(void* ptr, size_t size)
{
	memset(ptr, 0, size);
}

static fz_context* ctx = NULL;
static fz_output* out = NULL;
static fz_stream* datafeed = NULL;

static void usage(void)
{
	fz_info(ctx,
		"fingerprint1: produce a modern (BLAKE3+B58X) Qiqqa fingerprint for the given PDF\n"
		"\n"
		"Syntax: fingerprint1 [options] <file-to-fingerprint>\n"
		"\n"
		"Options:\n"
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


int
qiqqa_fingerprint1_main(int argc, const char** argv)
{
	int verbosity = 0;
	int c;
	const char* output = NULL;

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
	while ((c = fz_getopt(argc, argv, "o:qvV")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;

		case 'q': verbosity = 0; break;

		case 'v': verbosity++; break;

		case 'V': fz_info(ctx, "fingerprint1 version %s/%s", FZ_VERSION, blake3_version()); return EXIT_FAILURE;

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

			// now encode the fingerprint in base58X:
			char b58buf[120];
			const char* b58X = EncodeBase58X(b58buf, sizeof(b58buf), hash, sizeof(hash) /* BLAKE3_OUT_LEN */);
#if defined(FINGERPRINT1_TEST_VERIFY)
			uint8_t b58rbuf[BLAKE3_OUT_LEN + 8];
			size_t b58rsize = 0;
			const uint8_t* b58R = DecodeBase58X(b58rbuf, sizeof(b58rbuf), &b58rsize, b58X);
			uint8_t b58rbuf2[BLAKE3_OUT_LEN];
			size_t b58r2size = 0;
			const uint8_t* b58R2 = DecodeBase58X(b58rbuf2, sizeof(b58rbuf2), &b58r2size, b58X);
#endif

			// Print the hash as hexadecimal.
			if (verbosity)
			{
				fz_write_printf(ctx, out, "%q: %s\n", datafilename, b58X);
			}
			else
			{
				fz_write_printf(ctx, out, "%s\n", b58X);
			}

#if defined(FINGERPRINT1_TEST_VERIFY)
			fz_write_printf(ctx, out, "base58X encodings: %s / %.0H / %.0H\n", b58X, b58R, b58rsize, b58R2, b58r2size);

			// check our custom encoding: the mixing of big and little endian handling plus 3 different number bases (2^64, 2^41, 58^7)
			// isn't stuff that's without its historical crap-ups, so I'd rather not add myself to that list of sad sacks...
			char testbuf[3][100];
			fz_snprintf(testbuf[0], sizeof(testbuf[0]), "%.0H", hash, sizeof(hash));
			fz_snprintf(testbuf[1], sizeof(testbuf[1]), "%.0H", b58R, b58rsize);
			fz_snprintf(testbuf[2], sizeof(testbuf[2]), "%.0H", b58R2, b58r2size);

			if (strcmp(testbuf[0], testbuf[1]))
			{
				fz_error(ctx, "FATAL ERROR: base58X encoding failed for %q vs %q.\n", testbuf[0], testbuf[1]);
			}
			if (strcmp(testbuf[0], testbuf[2]))
			{
				fz_error(ctx, "FATAL ERROR: base58X(2) encoding failed for %q vs %q.\n", testbuf[0], testbuf[2]);
			}
#endif

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
