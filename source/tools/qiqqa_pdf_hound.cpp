//
// hound a.k.a. hog: fetch the document you seek using maximum effort
//
// a tool for fetching *files* from the internet, specifically PDFs. 
// Intended to be used where you browse the 'Net and decide you want 
// to download a given PDF from any site: this can be done through 
// the browser itself, but is sometimes convoluted or neigh impossible 
// (ftp links require another tool, PDFs stored at servers which 
// report as having their SSL certificates *expired* are a hassle to 
// get through for the user-in-a-hurry, etc. etc.) and `hog` is meant 
// to cope with all these by providing:
//
// - ability to detect the listed PDF url and *download* it, i.e. has 
//   knowledge about several approaches used by various sites 
//   (publishers, etc.) which offer on-line PDFs for viewing & 
//   downloading,
//
// - allow scripting these seek&fetch behaviours so users can create 
//   their own, custom, procedures re DOM parsing, cookies, etc. as 
//   the sites change or other sites' access is desired, when not yet 
//   part of the "how to download" knowledge base of `hog` out of 
//   the box.
//
// - fundamentally a tool that sits somewhere between `cUrl` and 
//   `wget`, with some `cUrl-impersonation` abilities thrown in as 
//   well. This includes:
//
//   - desirable behaviour with minimal commandline arguments, 
//     i.e. "good defaults"
// 
//   - auto-naming the local filename when downloading
// 
//   - auto-sanitizing the local filename when downloading
// 
//   - create a *second* local file describing the metadata thus far, 
//     in particular the source URL, for it will be loaded by Qiqqa 
//     as part of the file's metadata later on
//
//   - ability to detect and follow HTTP 301, 302 responses, 
//     *but also* HTTP pages which come back as 200, yet have 
//     "timers" (sourceforge, f.e.) or other 
//     wait-before-you-get-it or go-here-instead "redirection" 
//     embedded in their HTML output
//
//   - `hog` being able to either fetch a *file* (PDF, image, ...) 
//     or a *full HTML page*: some whitepapers are published as 
//     blog articles or other HTML-only formats, and we want those 
//     too *as local files*. This implies that we should be able 
//     to tell `hog` to grab the page itself (auto-detect such a 
//     scenario?), plus all its relevant assets (CSS, images, ...) 
//     and bundle that into a HTMLZ or similar "HTML file archive".
//
// > I'm not too enthousiastic about the classical MHTML (Mime-HTML) 
// > format as it expands images using Base64, while we have *zero* 
// > need to raw-copy this into email or would otherwise benefit 
// > from the convoluted MIME encoding in our storage format -- better 
// > would be having it all in an accessible single file, e.g. a 
// > ZIP-style wrapper, hence my preference for 
// > [HTMLZ](https://wiki.mobileread.com/wiki/HTMLZ) (or the more 
// > modern [WARC](https://en.wikipedia.org/wiki/Web_ARChive) 
// > format, which seems to be preferred by internet libraries out 
// > there, e.g. archive.org)
//

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/system-header-files.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "qiqqa_monolithic_examples.h"


#pragma message(FZPM_TODO "implement this tool")


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
		"qq_db_importer: import / export Qiqqa metadata databases and dump files.\n"
		"\n"
		"Syntax: qq_db_importer [options] [filelist...]\n"
		"\n"
		"Options:\n"
		"  -v      verbose (repeat to increase the chattiness of the application)\n"
		"  -q      quiet ~ not verbose at all\n"
		"  -o db   write the result to the given database file ('-' for stdout)\n"
		"  -t dir  directory we can use for scratch space on disk (default: %%TEMP%%)\n"
		"          (specify '-' to disable scratch space usage completely)\n"
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
qiqqa_pdf_hound_main(int argc, const char** argv)
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
	while ((c = fz_getopt(argc, argv, "o:t:qvV")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;

		case 'q': verbosity = 0; break;

		case 'v': verbosity++; break;

		case 'V': fz_info(ctx, "qq_db_importer version %s/%s", FZ_VERSION, "SHA1"); return EXIT_FAILURE;

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

			// Finalize the hash. BLAKE3_OUT_LEN is the default output length, 32 bytes.
			uint8_t hashbytes[32] = { 1 };
			size_t hash_width = 32;

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
