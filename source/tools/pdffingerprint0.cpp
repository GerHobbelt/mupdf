//
// Create a Qiqqa fingerprint hash for a given input file (PDF)
//
// Qiqqa's fingerprint is a b0rked variant use of SHA1, originally coded in C#/.NET,
// where the SHA1 hash of the file contents were calculated, then converted to
// a hexadecimal string representation.
// The b0rk is in that hexadecimal transformation: when a byte in the original hash value
// has a most significant ZERO nibble, that hex digit is lost, i.ee. 0A becomes 'A' instead of '0A'.
//
// Consequently, Qiqqa fingerprint hashes have *variable length*.
// As a (non-cryptographic) fingerprint it's still sort of okay, but its use is now
// outmoded as I intend to store PDFs in a Qiqqa library which exhibit the SHA1 collision,
// thus these PDFs will also have the same Qiqqa fingerprint yet be different in content.
//
// The original Qiqqa fingerprint hash is coded here for backwards-compatible access to
// Qiqqa metadata libraries from a C/C++/anything environment.
//

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#include <windows.h>
#endif

#define LONGLINE 4096

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
		"fingerprint0: produce a classic (SHA1B) Qiqqa fingerprint for the given PDF\n"
		"\n"
		"Syntax: fingerprint0 [options] <file-to-fingerprint>\n"
		"\n"
		"Options:\n"
		"  -o [destination]\n"
		"          specify a destination other than stdout for the calculated hash.\n"
		"  -v      verbose (toggle)\n"
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


extern "C" int
qiqqa_fingerprint0_main(int argc, const char* argv[])
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
	while ((c = fz_getopt(argc, argv, "o:vV")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;

		case 'v': verbosity ^= 1; break;

		case 'V': fz_info(ctx, "fingerprint0 version %s/%s", FZ_VERSION, "SHA1"); return EXIT_FAILURE;

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

#ifdef _WIN32
	// Get us a debug stream we have a chance of seeing *independently* of stderr.
	fz_set_stddbg(ctx, fz_stdods(ctx));
#endif

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
			char fbuf[4096];
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
			using namespace CryptoPP;

			SHA1 hash;

			// Read input bytes from file.
			unsigned char buf[65536];
			size_t n = fz_read(ctx, datafeed, buf, sizeof(buf));
			while (n > 0)
			{
				hash.Update((const byte*)buf, n);
				n = fz_read(ctx, datafeed, buf, sizeof(buf));
			}

			// Finalize the hash. BLAKE3_OUT_LEN is the default output length, 32 bytes.
			uint8_t hashbytes[32];
			size_t hash_width = hash.DigestSize();
			hash.Final((byte*)&hashbytes[0]);

			// Print the hash as hexadecimal.
			char qhbuf[60];
			fz_snprintf(qhbuf, sizeof(qhbuf), "%.0H", hashbytes, hash_width);
			// Now do the Qiqqa b0rk:
			char* d = qhbuf;
			char* s = qhbuf;
			while (*s)
			{
				// ditch most-significant-nibbles that are ZERO:
				if (*s != '0')
					*d++ = *s++;
				else
					s++;
				*d++ = *s++;
			}
			*d = 0;

			// now print the resulting qiqqa hash
			if (verbosity)
			{
				fz_write_printf(ctx, out, "%q: %s\n", datafilename, qhbuf);
			}
			else
			{
				fz_write_printf(ctx, out, "%s\n", qhbuf);
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
