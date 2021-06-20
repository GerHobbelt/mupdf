/*
 * mutool -- swiss army knife of pdf manipulation tools
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "../../thirdparty/tesseract/include/tesseract/capi_training_tools.h"
#include "../../thirdparty/owemdjee/libwebp/extras/tools.h"
#include "../../thirdparty/owemdjee/pmt-png-tools/pngtools-monolithic.h"

#include "../../scripts/MuPDFLib/versions-api.h"

#ifndef DISABLE_MUTHREADS
#include "mupdf/helpers/mu-threads.h"
#endif

#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define main main_utf8
#endif


extern int charter_svg_main(int argc, const char* argv[]);
extern int charter_tex_main(int argc, const char* argv[]);
extern int tiny_expr_repl_main(int argc, const char* argv[]);

extern int smartypants_main(int argc, const char* argv[]);
extern int upskirt_main(int argc, const char* argv[]);

extern int qjscompress_main(int argc, const char* argv[]);
extern int qjs_main(int argc, const char* argv[]);
extern int qjsc_main(int argc, const char* argv[]);
extern int qjs_unicode_gen_main(int argc, const char* argv[]);


static int report_version(int argc, const char* argv[]);

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

	{ tesseract_ambiguous_words_main, "tess_ambiguous_words", "OCR training helper utility" },
	{ tesseract_classifier_tester_main, "tess_classifier_tester", "OCR training helper utility" },
	{ tesseract_cn_training_main, "tess_cn_training", "OCR training helper utility" },
	{ tesseract_mf_training_main, "tess_mf_training", "OCR training helper utility" },
	{ tesseract_shape_clustering_main, "tess_shape_clustering", "OCR training helper utility" },

	{ tesseract_merge_unicharsets_main, "tess_merge_unicharsets", "OCR training helper utility" },
	{ tesseract_set_unicharset_properties_main, "tess_set_unicharset_properties", "OCR training helper utility" },
	{ tesseract_unicharset_extractor_main, "tess_unicharset_extractor", "OCR training helper utility" },
	{ tesseract_combine_lang_model_main, "tess_combine_lang_model", "OCR training helper utility" },

	{ tesseract_combine_tessdata_main, "tess_combine_tessdata", "OCR training helper utility" },
	{ tesseract_dawg2wordlist_main, "tess_dawg2wordlist", "OCR training helper utility" },
	{ tesseract_wordlist2dawg_main, "tess_wordlist2dawg", "OCR training helper utility" },
	{ tesseract_lstm_eval_main, "tess_lstm_eval", "OCR training helper utility" },
	{ tesseract_lstm_training_main, "tess_lstm_training", "OCR training helper utility" },

	{ tesseract_text2image_main, "tess_text2image", "OCR training helper utility" },

	{ curl_main, "curl", "access/fetch a given URI" },
	{ mujs_main, "js", "basic REPL for MuJS JavaScript interpreter" },
	{ mujs_prettyprint_main, "jspretty", "prettyprint (reformat) MuJS JavaScript source files" },
	{ pdfmetadump_main, "metadump", "multitool shows information about pdf in JSON format" },

	{ qiqqa_fingerprint0_main, "qiqqa_fingerprint0", "calculate the classic Qiqqa fingerprint hash for a file" },
	{ qiqqa_fingerprint1_main, "qiqqa_fingerprint1", "calculate the new v2 Qiqqa fingerprint hash for a file" },

	{ sqlite_main, "sqlite", "SQLite3 tool" },

	{ jpegtran_main, "jpegtran", "jpegtran tool" },
	{ rdjpegcom_main, "rdjpegcom", "rdjpegcom tool" },
	{ wrjpegcom_main, "wrjpegcom", "wrjpegcom tool" },
	{ djpeg_main, "djpeg", "djpeg tool" },
	{ cjpeg_main, "cjpeg", "cjpeg tool" },

	{ cwebp_main, "cwebp", "cwebp tool" },
	{ dwebp_main, "dwebp", "dwebp tool" },
	{ gif2webp_main, "gif2webp", "gif2webp tool" },
	{ img2webp_main, "img2webp", "img2webp tool" },
	{ vwebp_main, "vwebp", "vwebp tool" },
	{ vwebp_sdl_main, "vwebp_sdl", "vwebp_sdl tool" },
	{ webp_anim_diff_main, "webp_anim_diff", "webp: webp_anim_diff tool" },
	{ webp_anim_dump_main, "webp_anim_dump", "webp_anim_dump tool" },
	{ webp_get_disto_main, "webp_get_disto", "webp_get_disto tool" },
	{ webp_quality_main, "webp_quality", "webp_quality tool" },
	{ webpinfo_main, "webpinfo", "webpinfo tool" },
	{ webpmux_main, "webpmux", "webpmux tool" },

	{ pngcrush_main, "pngcrush", "pngcrush tool" },
	{ pngmeta_main, "pngmeta", "pngmeta tool" },
	{ pngzop_zlib_to_idat_main, "pngzop_zlib_to_idat", "pngzop_zlib_to_idat tool" },
	{ pngidat_main, "pngidat", "pngidat tool" },
	{ pngiend_main, "pngiend", "pngiend tool" },
	{ pngihdr_main, "pngihdr", "pngihdr tool" },

	{ charter_svg_main, "charter_svg", "charter_svg tool" },
	{ charter_tex_main, "charter_tex", "charter_tex tool" },
	{ tiny_expr_repl_main, "tiny_expr_repl", "tiny_expr_repl tool" },

	{ smartypants_main, "smartypants", "smartypants tool" },
	{ upskirt_main, "upskirt", "upskirt tool" },

	{ qjscompress_main, "qjscompress", "qjscompress tool" },
	{ qjs_main, "qjs", "qjs (QuickJS) tool" },
	{ qjsc_main, "qjsc", "qjsc (QuickJS Compiler) tool" },
	{ qjs_unicode_gen_main, "qjs_unicode_gen", "qjs_unicode_gen tool" },

	{ report_version, "version", "report version of this build / tools" },
};

static int
namematch(const char *end, const char *start, const char *match)
{
	if (!start)
		return 0;
	size_t len = strlen(match);
	const char* p = end - len;
	return ((p >= start) && (strncmp(p, match, len) == 0));
}

static int
report_version(int argc, const char* argv[])
{
	const char* opt = (argc == 2 ? argv[1] : NULL);
	const char* end = (opt ? opt + strlen(opt) : NULL);

	const char* bn = FZ_VERSION_BUILD;

	if (namematch(end, opt, "-f"))
	{
		printf("%s\n", muq_report_version(MUQ_VERSION_FULL_PACKAGE));
	}
	else if (namematch(end, opt, "-0"))
	{
		printf("%s\n", muq_report_version(MUQ_VERSION_FULL));
	}
	else if (namematch(end, opt, "-1"))
	{
		printf("%s\n", muq_report_version(MUQ_VERSION_MAJOR));
	}
	else if (namematch(end, opt, "-2"))
	{
		printf("%s\n", muq_report_version(MUQ_VERSION_MINOR));
	}
	else if (namematch(end, opt, "-3"))
	{
		printf("%s\n", muq_report_version(MUQ_VERSION_PATCH));
	}
	else if (namematch(end, opt, "-4"))
	{
		printf("%s\n", muq_report_version(MUQ_VERSION_BUILD));
	}
	else if (namematch(end, opt, "-a"))
	{
		const char* buf[64];
		const char** arr = buf;
		muq_report_bundled_software(buf, 64);

		for (; *arr; arr++)
		{
			printf("%s\n", *arr);
		}
	}
	else if (!opt)
	{
		printf("%s\n", muq_report_version(MUQ_VERSION_FULL_PACKAGE));
	}
	else
	{
		printf("version [option]\n"
			"\n"
			"Option:\n"
			"-f         full version, including 'Qiqqa-PDF-Tooling' bundle name as prefix\n"
			"-0         full version sans prefixes, e.g. " FZ_VERSION "\n"
			"-1         major (1st) version part only, e.g. " FZ_VERSION_ELEMENT_STR(FZ_VERSION_MAJOR) "\n"
			"-2         minor (2nd) version part only, e.g. " FZ_VERSION_ELEMENT_STR(FZ_VERSION_MINOR) "\n"
			"-3         patch level (3rd) version part only, e.g. " FZ_VERSION_ELEMENT_STR(FZ_VERSION_PATCH) "\n"
			"-4         build sequence part, i.e. 4th version part only (sans 'GHO' prefix), e.g. %s\n"
			"-a         also list all bundled libraries and their versions, one per line.\n"
			"\n"
			"No option? Default behaviour is identical to `-f` mode.\n",
			bn + 3);
	}
	return EXIT_SUCCESS;
}

/*
	In the presence of pthreads or Windows threads, we can offer
	a multi-threaded option. In the absence of such we degrade
	nicely.
*/
#ifndef DISABLE_MUTHREADS

static mu_mutex mutexes[FZ_LOCK_MAX];

static void mudraw_lock(void* user, int lock)
{
	mu_lock_mutex(&mutexes[lock]);
}

static void mudraw_unlock(void* user, int lock)
{
	mu_unlock_mutex(&mutexes[lock]);
}

static fz_locks_context mudraw_locks =
{
	NULL, mudraw_lock, mudraw_unlock
};

static void fin_mudraw_locks(void)
{
	int i;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		mu_destroy_mutex(&mutexes[i]);
}

static fz_locks_context* init_mudraw_locks(void)
{
	int i;
	int failed = 0;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		failed |= mu_create_mutex(&mutexes[i]);

	if (failed)
	{
		fin_mudraw_locks();
		return NULL;
	}

	return &mudraw_locks;
}

#endif

static int mutool_is_toplevel_ctx = 0;
static fz_context* ctx = NULL;

static void mu_drop_context(void)
{
	assert(!ctx || (ctx->error.top == ctx->error.stack));

	if (mutool_is_toplevel_ctx)
	{
		// as we registered a global context, we should clean the locks on it now
		// so the atexit handler won't have to bother with it.
		assert(fz_has_global_context());
		ctx = fz_get_global_context();
		fz_drop_context_locks(ctx);
		ctx = NULL;

		fz_drop_global_context();

#ifndef DISABLE_MUTHREADS
		fin_mudraw_locks();
#endif /* DISABLE_MUTHREADS */

		mutool_is_toplevel_ctx = 0;
	}
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
#ifndef MUTOOL_AS_BULKTEST_CMD
int main(int argc, const char **argv)
#else
int mutool_main(int argc, const char** argv)
#endif
#endif
{
	const char *start, *end;
	char buf[64];
	int i;

	ctx = NULL;
	mutool_is_toplevel_ctx = 0;

	if (!fz_has_global_context())
	{
		fz_locks_context* locks = NULL;

#ifndef DISABLE_MUTHREADS
		locks = init_mudraw_locks();
		if (locks == NULL)
		{
			fz_error(ctx, "mutex initialisation failed");
			return EXIT_FAILURE;
		}
#endif

		ctx = fz_new_context(NULL, locks, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);

		mutool_is_toplevel_ctx = 1;
	}
	else
	{
		ctx = fz_get_global_context();
	}
	atexit(mu_drop_context);

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
			assert(strlen(buf) < sizeof(buf));
			if (namematch(end, start, buf) || namematch(end, start, buf + 2))
				return tools[i].func(argc, argv);
			strcpy(buf, "mu");
			strcat(buf, tools[i].name);
			assert(strlen(buf) < sizeof(buf));
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
			assert(strlen(buf) < sizeof(buf));
			if (namematch(end, start, buf) || namematch(end, start, buf + 2))
				return tools[i].func(argc - 1, argv + 1);
			strcpy(buf, "mu");
			strcat(buf, tools[i].name);
			assert(strlen(buf) < sizeof(buf));
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

#ifndef MUTOOL_AS_BULKTEST_CMD

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

#endif // MUTOOL_AS_BULKTEST_CMD

