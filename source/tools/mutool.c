/*
 * mutool -- swiss army knife of pdf manipulation tools
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"

#define BUILD_MONOLITHIC 1
#include "../../thirdparty/tesseract/include/tesseract/capi_training_tools.h"
#include "../../thirdparty/owemdjee/libwebp/extras/tools.h"
#include "../../thirdparty/owemdjee/pmt-png-tools/pngtools-monolithic.h"
#include "../../thirdparty/owemdjee/upskirt-markdown/bin/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libxml2/include/libxml/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS/monolithic_examples.h"
#include "../../thirdparty/owemdjee/brotli/c/include/monolithic_examples.h"
#include "../../thirdparty/owemdjee/jpeg-xl/lib/include/monolithic_examples.h"
#include "../../thirdparty/owemdjee/sqlite/monolithic_examples.h"
#include "../../thirdparty/owemdjee/crow/include/crow/monolithic_examples.h"
#undef BUILD_MONOLITHIC

#include "../../scripts/MuPDFLib/versions-api.h"

#ifndef DISABLE_MUTHREADS
#include "mupdf/helpers/mu-threads.h"
#endif

#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define main main_utf8
#endif


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

	{ qiqqa_muannot_main, "muannot", "Qiqqa annotation processing utility to help extract and inject (import/export) PDF annotations" },
	{ qiqqa_muserver_main, "muserver", "Qiqqa/MuPDF local web server: the Qiqqa database and file management backend" },

	{ qiqqa_fingerprint0_main, "qiqqa_fingerprint0", "calculate the classic Qiqqa fingerprint hash for a file" },
	{ qiqqa_fingerprint1_main, "qiqqa_fingerprint1", "calculate the new v2 Qiqqa fingerprint hash for a file" },

	{ sqlite_main, "sqlite", "SQLite3 tool" },
	{ sqlite_dbhash_main, "sqlite_dbhash", "SQLite3 dbhash tool" },
	{ sqlite_dbtotxt_main, "sqlite_dbtotxt", "SQLite3 dbtotxt tool" },
	{ sqlite_diff_main, "sqlite_diff", "SQLite3 diff tool" },
	{ sqlite_enlargedb_main, "sqlite_enlargedb", "SQLite3 enlargedb tool" },
	{ sqlite_fast_vacuum_main, "sqlite_fast_vacuum", "SQLite3 fast_vacuum tool" },
	{ sqlite_getlock_main, "sqlite_getlock", "SQLite3 getlock tool" },
	{ sqlite_index_usage_main, "sqlite_index_usage", "SQLite3 index_usage tool" },
	{ sqlite_libvers_main, "sqlite_libvers", "SQLite3 libvers tool" },
	{ sqlite_max_limits_main, "sqlite_max_limits", "SQLite3 max_limits tool" },
	{ sqlite_offsets_main, "sqlite_offsets", "SQLite3 offsets tool" },
	{ sqlite_pagesig_main, "sqlite_pagesig", "SQLite3 pagesig tool" },
	{ sqlite_showdb_main, "sqlite_showdb", "SQLite3 showdb tool" },
	{ sqlite_showjournal_main, "sqlite_showjournal", "SQLite3 showjournal tool" },
	{ sqlite_showlocks_main, "sqlite_showlocks", "SQLite3 showlocks tool" },
	{ sqlite_showshm_main, "sqlite_showshm", "SQLite3 showshm tool" },
	{ sqlite_showstat4_main, "sqlite_showstat4", "SQLite3 showstat4 tool" },
	{ sqlite_showwal_main, "sqlite_showwal", "SQLite3 showwal tool" },

	{ sqlite_logest_main, "sqlite_logest", "SQLite3 logest tool" },
	{ sqlite_varint_main, "sqlite_varint", "SQLite3 varint tool" },

	{ sqlite_kvtest_main, "sqlite_kvtest", "SQLite3 kvtest tool" },
//	{ sqlite_loadfts_main, "sqlite_loadfts", "SQLite3 loadfts tool" },
	{ sqlite_speedtest1_main, "sqlite_speedtest1", "SQLite3 speedtest1 tool" },
	{ sqlite_speedtest8_main, "sqlite_speedtest8", "SQLite3 speedtest8 tool" },
//	{ sqlite_speedtest8inst1_main, "sqlite_speedtest8inst1", "SQLite3 speedtest8i1 tool" },
	{ sqlite_test_startup_main, "sqlite_test_startup", "SQLite3 test_startup tool" },
	{ sqlite_threadtest1_main, "sqlite_threadtest1", "SQLite3 threadtest1 tool" },
	{ sqlite_threadtest2_main, "sqlite_threadtest2", "SQLite3 threadtest2 tool" },
	{ sqlite_threadtest3_main, "sqlite_threadtest3", "SQLite3 threadtest3 tool" },
	{ sqlite_threadtest4_main, "sqlite_threadtest4", "SQLite3 threadtest4 tool" },
	{ sqlite_threadtest5_main, "sqlite_threadtest5", "SQLite3 threadtest5 tool" },
	{ sqlite_wordcount_main, "sqlite_wordcount", "SQLite3 wordcount test tool" },

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

	{ brotli_main, "brotli", "brotli tool" },

	{ jpegXL_decode_oneshot_main, "jxl_decode_oneshot", "jxl_decode_oneshot tool" },
	{ jpegXL_encode_oneshot_main, "jxl_encode_oneshot", "jxl_encode_oneshot tool" },
	{ jpegXL_info_main, "jxl_info", "jxl_info: jpegXL tool" },
	{ jpegXL_box_list_main, "jxl_box_list", "jxl_box_list tool" },
	{ jpegXL_butteraugli_main, "jxl_butteraugli", "jxl_butteraugli tool" },
	{ jpegXL_compress_main, "jxl_compress", "jxl_compress tool" },
	{ jpegXL_conformance_main, "jxl_conformance", "jxl_conformance tool" },
	{ jpegXL_decompress_main, "jxl_decompress", "jxl_decompress tool" },
	{ jpegXL_dec_enc_main, "jxl_dec_enc", "jxl_dec_enc tool" },
	{ jpegXL_epf_main, "jxl_epf", "jxl_epf tool" },
	{ jpegXL_from_tree_main, "jxl_from_tree", "jxl_from_tree tool" },
	{ jpegXL_ssimulacra_main, "jxl_ssimulacra tool", "jxl_ssimulacra tool" },
//	{ jpegXL_ssimulacra_openCV_main, "jxl_ssimulacra_openCV", "jxl_ssimulacra_openCV tool" },
	{ jpegXL_xyb_range_main, "jxl_xyb_range", "jxl_xyb_range tool" },

	{ crow_example_basic_main, "crow_basic", "crow basic example tool" },
	{ crow_example_catch_all_main, "crow_catch_all", "crow catch-all example tool" },
	{ crow_example_chat_main, "crow_chat", "crow chat example tool" },
	{ crow_example_compression_main, "crow_compression", "crow compression example tool" },
	{ crow_example_json_map_main, "crow_json_map", "crow json map example tool" },
	{ crow_example_static_file_main, "crow_static_file", "crow static file example tool" },
	{ crow_example_vs_main, "crow_vs", "crow vs example tool" },
	{ crow_example_with_all_main, "crow_w_all", "crow 'with all features' example tool" },
	{ crow_hello_world_main, "crow_hello", "crow hello example tool" },
	{ crow_example_ssl_main, "crow_ssl", "crow ssl example tool" },
	{ crow_example_ws_main, "crow_ws", "crow ws example tool" },
	{ crow_mustache_main, "crow_mustache", "crow mustache example tool" },

	{ report_version, "version", "report version of this build / tools" },
};

static int
namematch(const char *end, const char *start, const char *match)
{
	if (!start)
		return 0;
	size_t len = strlen(match);
	const char* p = end - len;
	return ((p == start) && (strncmp(p, match, len) == 0));
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
		end = start + strlen(start);
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

	size_t max_tool_name_len = 0;
	for (i = 0; i < (int)nelem(tools); i++)
		max_tool_name_len = fz_maxi(max_tool_name_len, strlen(tools[i].name));
	const char* leaderdots = " . . . . . . . . . . . . . . . . . . . . . . . . . . .";
	for (i = 0; i < (int)nelem(tools); i++) {
		const char* name = tools[i].name;
		size_t name_len = strlen(tools[i].name);
		size_t lead = (max_tool_name_len - name_len - 4) & ~1; // print even number of leaderdots characters
		// ^^^ unsigned arithmetic so negative numbers are *huge* positive numbers instead!
		if (lead > max_tool_name_len)
			lead = 0;
		size_t width = 1 + max_tool_name_len - name_len - lead;
		fz_info(ctx, "\t%s%.*s%.*s -- %s", name, (int)width, "", (int)lead, leaderdots, tools[i].desc);
	}

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

