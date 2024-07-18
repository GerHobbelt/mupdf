//
// `snarfl` :: snarfling all the metadata you need off The Net
//
// a companion tool to `ingest` for gathering metadata 
// "from abroad", e.g.:
//
// - processing large metadata databases (and their 
//   backup/export/import files), such as the sci-hub.ru 
//   metadata dump, and produce a "reference library" from 
//   those. 
//   To be used as search sources where we try to match DOI or 
//   title/author with every document in our actual *library* 
//   where we are still looking to improve our own metadata 
//   set (or rather: get a "reasonable" initial suggest for 
//   the metadata for our document X, so that we have a 
//   simplified and *faster* job of vetting the metadata 
//   gathered ("*snarfled*") through this and other automated 
//   processes.
//
// - roaming the Internet on the prowl for additional 
//   metadata opportunities, e.g. automated Google / Bing / 
//   DuckDuckGo searches for the title/author and automated 
//   decoding/extraction of "initial metadata suggestions": 
//   here *versioning*, *version control* and *ranking* / 
//   *marking* metadata for each document become important 
//   as we MUST be able to tell automated suggestions, 
//   automated "finds" in foreign metadata databases and 
//   *owner vetted* metadata apart, so that we can report a 
//   *reliable* grading/ranking of the same. Given the lousy 
//   Google Scholar metadata (my long-time experience with 
//   using that) and sometimes dubious quality of other 
//   sources' metadata "files", let alone the statistical 
//   *uncertainty* inherent in machine-derived/extracted 
//   metadata from web + search-engine *scrapes*, a clear 
//   and human-grokkable **_ranking per line item_** for 
//   any metadata is a **requirement**! Hence we need to 
//   equip our tooling (`snarfl`) with the ability to report 
//   *source* and *estimated/indicated ranking of said 
//   source*, so that the human owner/editor can jump in 
//   later and correct/complete/augment any entries she 
//   doesn't seem of sufficient quality.
//
// > *Abstracts* are of particular interest here and prone 
// > to the severest modes of "crapping up" when left to 
// > automated tools. Here we see a need for both:
// >
// > - *scripting* of particular scraping / extraction 
// >   processes within `snarfl`.  
// >
// > - *versioning* and *version control*, once again, as 
// >   we ant to be able to monitor, inspect and report on 
// >   the (gradual) metadata quality improvement process 
// >   itself. (😄😇 *meta* of the *meta*)
// >
// > - later on in this process *human intervention* a.k.a. 
// >   *editing* will be involved, but that's out of scope 
// >   for the `snarfl` tool itself as far as I am concerned: 
// >   what `snarfl` SHOULD provide is the "*departure point*" 
// >   for this metadata editorial quality improvement 
// >   process, such that it can commence with the least 
// >   amount of fuss.
// >
//
// **Thought**: 
// `text-extractor-analyzer`, `hog` and `snarfl` MAY become 
// one and the same tool as these processes, while in 
// different *arenas*, probably involve quite a bit of the 
// same *technology* and *type of processes*, so it makes 
// sense to roll these into one "*multitool*".
//
//   The two different *arenas*: text **content** 
//   "*snarfing*" & publication **metadata** "*snarfling*". 
//
//   🤡 Which one will we refer to as *snarfing* (*sans* 
//   *elle*)? And which one will be referred to as 
//   *snarfling* (*avec du elle*)? The 'l' for "*level up*" 
//   (= meta) or the 'l' for *mostly local*? 🤔 Doubting, 
//   but when I did *spontaneously write about it just 
//   now*, it's apparently the former that's preferred by 
//   this individual. 😆
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
#if defined(_WIN32)
#include <windows.h>
#endif


//#pragma message(FZPM_TODO "implement this tool")


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
		"muserver: Local web server for use as Qiqqa backend & generic (scripted?) access.\n"
		"\n"
		"Syntax: muserver [options]\n"
		"\n"
		"Options:\n"
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


extern "C" int
qiqqa_snarfl_main(int argc, const char** argv)
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

		case 'V': fz_info(ctx, "muserver version %s/%s", FZ_VERSION, "SHA1"); return EXIT_FAILURE;

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

		fz_error(ctx, "Failure while processing %q: %s", datafilename, fz_convert_error(ctx, NULL));

		errored = 1;
	}

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}
