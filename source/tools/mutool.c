/*
 * mutool -- swiss army knife of pdf manipulation tools
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"

#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define main main_utf8
#endif

static struct {
	int (*func)(int argc, const char *argv[]);
	const char *name;
	const char *desc;
} tools[] = {
#if FZ_ENABLE_PDF
	{ pdfclean_main, "clean", "rewrite pdf file" },
#endif
	{ muconvert_main, "convert", "convert document" },
#if FZ_ENABLE_PDF
	{ pdfcreate_main, "create", "create pdf document" },
#endif
	{ mudraw_main, "draw", "convert document" },
	{ muraster_main, "raster", "convert document to a raster file" },
	{ mutrace_main, "trace", "trace device calls" },
#if FZ_ENABLE_PDF
	{ pdfextract_main, "extract", "extract font and image resources" },
#endif
#if FZ_ENABLE_PDF
	{ pdfinfo_main, "info", "show information about pdf resources" },
	{ pdfmerge_main, "merge", "merge pages from multiple pdf sources into a new pdf" },
	{ pdfpages_main, "pages", "show information about pdf pages" },
	{ pdfposter_main, "poster", "split large page into many tiles" },
	{ pdfsign_main, "sign", "manipulate PDF digital signatures" },
#endif
#if FZ_ENABLE_JS
	{ murun_main, "run", "run javascript" },
#endif
#if FZ_ENABLE_PDF
	{ pdfshow_main, "show", "show internal pdf objects" },
#ifndef NDEBUG
	{ cmapdump_main, "cmapdump", "dump CMap resource as C source file" },
#endif
#endif
	{ tesseract_main, "tesseract", "OCR given image or PDF" },
	{ curl_main, "curl", "access/fetch a given URI" },
	{ mujs_main, "js", "basic REPL for MuJS JavaScript interpreter" },
	{ mujs_prettyprint_main, "jspretty", "prettyprint (reformat) MuJS JavaScript source files" },
	{ pdfmultipurp_main, "multipurp", "multitool shows information about pdf in JSON format" },
};

static int
namematch(const char *end, const char *start, const char *match)
{
	size_t len = strlen(match);
	const char* p = end - len;
	return ((p >= start) && (strncmp(p, match, len) == 0));
}


#ifdef GPERF
#include "gperftools/profiler.h"

static int profiled_main(int argc, const char **argv);

int main(int argc, const char **argv)
{
	int ret;
	ProfilerStart("mutool.prof");
	ret = profiled_main(argc, argv);
	ProfilerStop();
	return ret;
}

static int profiled_main(int argc, const char **argv)
#else
#ifndef MUTOOL_AS_MUJSTEST_CMD
int main(int argc, const char **argv)
#else
int mutool_main(int argc, const char** argv)
#endif
#endif
{
	const char *start, *end;
	char buf[32];
	int i;
	fz_context* ctx = NULL;

	if (!fz_has_global_context())
	{
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}

	if (argc == 0)
	{
		fz_error(ctx, "No command name found!");
		return EXIT_FAILURE;
	}

	/* Check argv[0] */

	if (argc > 0)
	{
		start = fz_path_basename(argv[0]);
		end = start + strlen(argv[0]);
		if ((end-4 >= start) && (end[-4] == '.') && (end[-3] == 'e') && (end[-2] == 'x') && (end[-1] == 'e'))
			end = end-4;
		for (i = 0; i < (int)nelem(tools); i++)
		{
			// test for variants: mupdf<NAME>, pdf<NAME>, mu<NAME> and <NAME>:
			strcpy(buf, "mupdf");
			strcat(buf, tools[i].name);
			if (namematch(end, start, buf) || namematch(end, start, buf + 2))
				return tools[i].func(argc, argv);
			strcpy(buf, "mu");
			strcat(buf, tools[i].name);
			if (namematch(end, start, buf) || namematch(end, start, buf + 2))
				return tools[i].func(argc, argv);
		}
	}

	/* Check argv[1] */

	if (argc > 1)
	{
		for (i = 0; i < (int)nelem(tools); i++)
		{
			start = argv[1];
			end = start + strlen(start);
			// test for variants: mupdf<NAME>, pdf<NAME>, mu<NAME> and <NAME>:
			strcpy(buf, "mupdf");
			strcat(buf, tools[i].name);
			if (namematch(end, start, buf) || namematch(end, start, buf + 2))
				return tools[i].func(argc - 1, argv + 1);
			strcpy(buf, "mu");
			strcat(buf, tools[i].name);
			if (namematch(end, start, buf) || namematch(end, start, buf + 2))
				return tools[i].func(argc - 1, argv + 1);
		}
		if (!strcmp(argv[1], "-v"))
		{
			fz_info(ctx, "mutool version %s", FZ_VERSION);
			return EXIT_SUCCESS;
		}
		fz_error(ctx, "mutool: unrecognized command '%s'\n", argv[1]);
	}

	/* Print usage */

	fz_info(ctx, "usage: mutool <command> [options]");

	for (i = 0; i < (int)nelem(tools); i++)
		fz_info(ctx, "\t%s\t-- %s", tools[i].name, tools[i].desc);

	return EXIT_FAILURE;
}

#ifndef MUTOOL_AS_MUJSTEST_CMD

#ifdef _MSC_VER
int wmain(int argc, const wchar_t *wargv[])
{
	const char **argv = fz_argv_from_wargv(argc, wargv);
	if (!argv)
		return EXIT_FAILURE;
	int ret = main(argc, argv);
	fz_free_argv(argc, argv);
	return ret;
}
#endif

#endif // MUTOOL_AS_MUJSTEST_CMD

