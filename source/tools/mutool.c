// Copyright (C) 2004-2024 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

/*
 * mutool -- swiss army knife of pdf manipulation tools
 */

#include "mupdf/fitz.h"
#include "mupdf/mutool.h"
#include "mupdf/helpers/mu-threads.h"
#include "mupdf/helpers/jmemcust.h"
#include "mupdf/helpers/debugheap.h"

#define BUILD_MONOLITHIC 1
#include "../../thirdparty/tesseract/include/tesseract/capi_training_tools.h"
#include "../../thirdparty/tesseract_ocr_test/testing/monolithic_tests.h"
#include "../../thirdparty/owemdjee/libwebp/extras/tools.h"
#include "../../thirdparty/owemdjee/pmt-png-tools/pngtools-monolithic.h"
#include "../../thirdparty/owemdjee/upskirt-markdown/bin/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libxml2/include/libxml/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libxslt/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS/monolithic_examples.h"
#include "../../thirdparty/owemdjee/brotli/c/include/monolithic_examples.h"
#include "../../thirdparty/owemdjee/jpeg-xl/lib/include/monolithic_examples.h"
#include "../../thirdparty/owemdjee/sqlite/monolithic_examples.h"
#include "../../thirdparty/owemdjee/crow/include/crow/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libjpeg-turbo/src/monolithic_examples.h"
#include "../../thirdparty/owemdjee/pcre/src/monolithic_examples.h"
#include "../../thirdparty/owemdjee/bibutils/src/bin/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libzopfli/src/zopfli/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libzopfli/src/zopflipng/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS-C++-Wrapper2/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS-C++-Wrapper/monolithic_examples.h"
#include "../../thirdparty/owemdjee/BLAKE3/c/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libarchive/contrib/monolithic_examples.h"
#include "../../thirdparty/owemdjee/tesslinesplit/monolithic_examples.h"
#include "../../thirdparty/owemdjee/uchardet/src/tools/monolithic_examples.h"
#include "../../thirdparty/owemdjee/tvision/include/tvision/monolithic_examples.h"
//#include "../../thirdparty/owemdjee/cpp-terminal/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libbf/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libcsv2/include/monolithic_examples.h"
#include "../../thirdparty/owemdjee/nanosvg/example/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libgif/gif_lib.h"
#include "../../thirdparty/jbig2dec/monolithic_examples.h"
#include "../../thirdparty/owemdjee/jbig2enc/src/monolithic_examples.h"
#include "../../thirdparty/lcms2/include/monolithic_examples.h"
#include "../../thirdparty/leptonica/prog/monolithic_examples.h"
#include "../../thirdparty/owemdjee/filesystem/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/glob/standalone/source/monolithic_examples.h"
#include "../../thirdparty/owemdjee/xsimd/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/dirent/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/edit-distance/src/monolithic_examples.h"
#include "../../thirdparty/owemdjee/spdlog/include/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libchaos/include/chaos/monolithic_examples.h"
#include "../../thirdparty/owemdjee/gumbo-query/example/monolithic_examples.h"
#include "../../thirdparty/owemdjee/linenoise/monolithic_examples.h"
#include "../../thirdparty/owemdjee/replxx/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/fph-table/tests/monolithic_examples.h"
#include "../../thirdparty/owemdjee/cpptoml/examples/monolithic_examples.h"
#include "../../source/fitz/tessocr.h"
#include "../../thirdparty/gumbo-parser/src/gumbo.h"
#include "../../thirdparty/owemdjee/gumbo-libxml/gumbo_libxml.h"
#include "../../thirdparty/owemdjee/tidy-html5/console/monolithic_examples.h"
#include "../../thirdparty/owemdjee/cpuinfo/tools/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libdeflate/programs/monolithic_examples.h"
#include "../../thirdparty/owemdjee/half/include/monolithic_examples.h"
#include "../../thirdparty/owemdjee/OpenEXR/src/bin/monolithic_examples.h"
#include "../../thirdparty/owemdjee/dcmtk/dcmapps/include/monolithic_examples.h"
#undef BUILD_MONOLITHIC

#include "../../scripts/MuPDFLib/versions-api.h"

#include <string.h>
#include <stdio.h>
#include <limits.h>

#ifdef _MSC_VER
#define main main_utf8
#endif


static int report_version(int argc, const char** argv);


typedef int tool_f(void);
typedef int tool_fa(int argc, const char** argv);

static struct tool_spec {
	union
	{
		tool_f* f;
		tool_fa* fa;
	} func;

	const char *name;
    const char *desc;
	int keep_path;              // 1: copy the argv[0] path into the tool's argv[0]; i.e. the tool needs the full path to the executable.
} tools[] = {
#if FZ_ENABLE_PDF
	{ {.fa = pdfclean_main }, "clean", "rewrite pdf file" },
#endif
	{ {.fa = muconvert_main }, "convert", "convert document" },
#if FZ_ENABLE_PDF
	{ {.fa = pdfcreate_main }, "create", "create pdf document" },
	{ {.fa = mudraw_main }, "draw", "convert document" },
	{ {.fa = muraster_main }, "raster", "convert document to a raster file" },
	{ {.fa = mutrace_main }, "trace", "trace device calls" },
	{ {.fa = pdfextract_main }, "extract", "extract font and image resources" },
	{ {.fa = pdfinfo_main }, "info", "show information about pdf resources" },
	{ {.fa = pdfmerge_main }, "merge", "merge pages from multiple pdf sources into a new pdf" },
	{ {.fa = pdfpages_main }, "pages", "show information about pdf pages" },
#if defined(MUTOOL_EX)
	{ {.fa = pdfposter_main }, "poster", "split large page into many tiles" },
	{ {.fa = pdfrecolor_main }, "recolor", "change colorspace of pdf document" },
	{ {.fa = pdfsign_main }, "sign", "manipulate PDF digital signatures" },
	{ {.fa = pdftrim_main }, "trim", "trim PDF page contents" },
	{ {.fa = pdfbake_main }, "bake", "bake PDF form into static content" },
	{ {.fa = pdftagged_main }, "tagged", "extract Tagged PDF content" },
	{ {.fa = pdftag_main }, "tag", "perform operations on PDF tags" },
	{ {.fa = pdfstamp_main }, "stamp", "Add a stamp onto PDF pages" },
	{ {.fa = mutextextract_main }, "extextract", "Generates a .docx file from mudraw XML output" },
#endif
#endif
#if FZ_ENABLE_JS
#if defined(MUTOOL_EX)
	{ {.fa = murun_main }, "run", "run javascript" },
#endif
#endif
#if FZ_ENABLE_PDF
	{ {.fa = pdfshow_main }, "show", "show internal pdf objects" },
#if defined(MUTOOL_EX)
	{ {.fa = cmapdump_main }, "cmapdump", "dump CMap resource as C source file" },
#endif
	{ {.fa = pdfaudit_main }, "audit", "Produce usage stats from PDF files" },
#endif
#if defined(MUTOOL_EX)
	{ {.fa = mupdf_base64_test_main }, "base64_test", "muPDF base64 fringe case tests" },
	{ {.fa = mupdf_example_main }, "example", "muPDF docs::example: render a single page and print the result as a PPM to stdout" },
	{ {.fa = mupdf_multithreaded_example_main }, "multithreaded_example", "muPDF multi-threaded rendering of all pages in a document to PNG images" },
	{ {.fa = mupdf_storytest_main }, "story_test", "muPDF fz_story showcase for converting HTML input to a PDF file" },
	{ {.fa = mupdf_cffdump_main }, "cffdump", "muPDF cffdump font info dump tool" },
#endif

#if defined(MUTOOL_EX)
	// DO NOT use tesseract_main() for this as that one does not set up the mupdf/fitz+leptonica+jpeg environments the way we need & expect in monolithic build mode.
	{ {.fa = tesseract_tool_main }, "tesseract", "OCR given image or PDF" },
	{ {.fa = tesseract_basic_example_main }, "tess_basic_example", "tesseract::basic_example demo app" },
	{ {.fa = tesseract_get_page_gradient_main }, "tess_get_page_gradient", "tesseract::get_page_gradient tool" },
	{ {.fa = tesseract_svpaint_main }, "tess_svpaint_demo", "tesseract SVPaint demo app" },

	{ {.fa = tesseract_ambiguous_words_main }, "tess_ambiguous_words", "OCR training helper utility" },
	{ {.fa = tesseract_classifier_tester_main }, "tess_classifier_tester", "OCR training helper utility" },
	{ {.fa = tesseract_cn_training_main }, "tess_cn_training", "OCR training helper utility" },
	{ {.fa = tesseract_mf_training_main }, "tess_mf_training", "OCR training helper utility" },
	{ {.fa = tesseract_shape_clustering_main }, "tess_shape_clustering", "OCR training helper utility" },

	{ {.fa = tesseract_merge_unicharsets_main }, "tess_merge_unicharsets", "OCR training helper utility" },
	{ {.fa = tesseract_set_unicharset_properties_main }, "tess_set_unicharset_properties", "OCR training helper utility" },
	{ {.fa = tesseract_unicharset_extractor_main }, "tess_unicharset_extractor", "OCR training helper utility" },
	{ {.fa = tesseract_combine_lang_model_main }, "tess_combine_lang_model", "OCR training helper utility" },

	{ {.fa = tesseract_combine_tessdata_main }, "tess_combine_tessdata", "OCR training helper utility" },
	{ {.fa = tesseract_dawg2wordlist_main }, "tess_dawg2wordlist", "OCR training helper utility" },
	{ {.fa = tesseract_wordlist2dawg_main }, "tess_wordlist2dawg", "OCR training helper utility" },
	{ {.fa = tesseract_lstm_eval_main }, "tess_lstm_eval", "OCR training helper utility" },
	{ {.fa = tesseract_lstm_training_main }, "tess_lstm_training", "OCR training helper utility" },

	{ {.fa = tesseract_text2image_main }, "tess_text2image", "OCR training helper utility" },

	{ {.fa = tesslinesplit_kraken_main }, "tesslinesplit_kraken", "OCR tesseract linesplit (Kraken) helper utility" },
	{ {.fa = tesslinesplit_ocular_main }, "tesslinesplit_ocular", "OCR tesseract line split (Ocular) helper utility" },

	{ {.fa = tesseract_test_issue_845_main }, "tess_test_issue_845", "tesseract test issue #845 utility" },
	{ {.fa = tesseract_test_issue_ML_1bba6c_main }, "tess_test_issue_ML_1", "tesseract test issue ML-1bba6c utility" },

	{ {.fa = tesseract_basicAPI_test_main }, "tess_test_basicAPI", "tesseract test basicAPI" },
#endif

	{ {.fa = curl_main }, "curl", "access/fetch a given URI" },

#if defined(MUTOOL_EX)
	{ {.fa = curl_schematable_tool_main }, "curl_schematable", "regenerate curl schematable C code" },

	{ {.fa = mujs_example_main }, "js", "basic REPL for MuJS JavaScript interpreter" },
	{ {.fa = mujs_prettyprint_main }, "jspretty", "prettyprint (reformat) MuJS JavaScript source files" },
#endif

	{ {.fa = pdfmetadump_main }, "metadump", "multitool shows information about pdf in JSON format" },

	{ {.fa = qiqqa_muannot_main }, "muannot", "Qiqqa annotation processing utility to help extract and inject (import/export) PDF annotations" },
	{ {.fa = qiqqa_mucontent_main }, "mucontent", "Qiqqa PDF content extraction / injection utility to help extract and inject (import/export) PDF content in text and hOCR formats" },
	{ {.fa = qiqqa_muserver_main }, "muserver", "Qiqqa/MuPDF local web server: the Qiqqa database and file management backend" },

	{ {.fa = qiqqa_fingerprint0_main }, "qiqqa_fingerprint0", "calculate the classic Qiqqa fingerprint hash for a file" },
	{ {.fa = qiqqa_fingerprint1_main }, "qiqqa_fingerprint1", "calculate the new v2 Qiqqa fingerprint hash for a file" },
	{ {.fa = qiqqa_documentid62_main }, "qiqqa_docid62", "calculate the new v2 Qiqqa fingerprint hash-based positive non-zero 64bit document id for a file" },

#if defined(MUTOOL_EX)
	{ {.fa = qiqqa_chop_shop_main }, "chop_shop", "qiqqa::chop_shop tool" },
	{ {.fa = qiqqa_content_importer_main }, "content_import", "qiqqa::content_import tool" },
	{ {.fa = qiqqa_content_importer_main }, "content_importer", "qiqqa::content_importer tool" },
	{ {.fa = qiqqa_content_processor_main }, "content_proc", "qiqqa::content_proc tool" },
	{ {.fa = qiqqa_content_processor_main }, "content_processor", "qiqqa::content_processor tool" },
	{ {.fa = qiqqa_convert_legacy_annot_blobs_main }, "cvt_annot_blob", "qiqqa::cvt_annot_blob tool" },
	{ {.fa = qiqqa_convert_legacy_autotags_main }, "cvt_autotags", "qiqqa::cvt_autotags tool" },
	{ {.fa = qiqqa_convert_legacy_configuration_main }, "cvt_configfile", "qiqqa::cvt_configfile tool" },
	{ {.fa = qiqqa_convert_legacy_expedition_main }, "cvt_expedition", "qiqqa::cvt_expedition tool" },
	{ {.fa = qiqqa_db_exporter_main }, "db_export", "qiqqa::db_export tool" },
	{ {.fa = qiqqa_db_fts_indexer_main }, "db_fts_indexer", "qiqqa::db_fts_indexer tool" },
	{ {.fa = qiqqa_db_fts_search_main }, "db_fts_search", "qiqqa::db_fts_search tool" },
	{ {.fa = qiqqa_db_importer_main }, "db_import", "qiqqa::db_import tool" },
	{ {.fa = qiqqa_db_pappy_main }, "db_pappy", "qiqqa::db_pappy tool" },
	{ {.fa = qiqqa_doc_scrutinizer_main }, "doc_scrutinizer", "qiqqa::doc_scrutinizer tool" },
	{ {.fa = qiqqa_ingest_main }, "ingest", "qiqqa::ingest tool" },
	{ {.fa = qiqqa_meta_exporter_main }, "meta_exporter", "qiqqa::meta_exporter tool" },
	{ {.fa = qiqqa_meta_importer_main }, "meta_import", "qiqqa::meta_import tool" },
	{ {.fa = qiqqa_ocr_bezoar_main }, "bezoar", "qiqqa::ocr_bezoar tool" },
	{ {.fa = qiqqa_pdf_hound_main }, "pdf_hound", "qiqqa::pdf_hound tool" },
	{ {.fa = qiqqa_safe_file_copier_main }, "safe_file_copier", "qiqqa::safe_file_copier tool" },
	{ {.fa = qiqqa_snarfl_main }, "snarfl", "qiqqa::snarfl tool" },
	{ {.fa = qiqqa_web_api_server_main }, "web_api_server", "qiqqa::web_api_server tool" },
#endif

	{ {.fa = sqlite_main }, "sqlite", "SQLite3 tool" },
#if defined(MUTOOL_EX)
	{ {.fa = sqlite_ext_expert_main }, "sqlite_expert", "SQLite3 expert tool" },
	{ {.fa = sqlite_fts3view_main }, "sqlite_fts3view", "SQLite3 fts3view tool" },
	{ {.fa = sqlite_lsmtest_main }, "sqlite_lsmtest", "SQLite3 lsmtest tool" },
	{ {.fa = sqlite_rbu_main }, "sqlite_rbu", "SQLite3 rbu tool" },
	{ {.fa = sqlite_ext_session_changeset_main }, "sqlite_session_changeset", "SQLite3 session_changeset tool" },
	{ {.fa = sqlite_ext_session_speed_test_main }, "sqlite_session_speed_test", "SQLite3 session_speed_test tool" },
	{ {.fa = sqlite_lemon_main }, "sqlite_lemon", "SQLite3 lemon tool" },

	{ {.fa = sqlite_dbhash_main }, "sqlite_dbhash", "SQLite3 dbhash tool" },
	{ {.fa = sqlite_dbtotxt_main }, "sqlite_dbtotxt", "SQLite3 dbtotxt tool" },
	{ {.fa = sqlite_diff_main }, "sqlite_diff", "SQLite3 diff tool" },
	{ {.fa = sqlite_enlargedb_main }, "sqlite_enlargedb", "SQLite3 enlargedb tool" },
	{ {.fa = sqlite_fast_vacuum_main }, "sqlite_fast_vacuum", "SQLite3 fast_vacuum tool" },
	{ {.fa = sqlite_getlock_main }, "sqlite_getlock", "SQLite3 getlock tool" },
	{ {.fa = sqlite_index_usage_main }, "sqlite_index_usage", "SQLite3 index_usage tool" },
	{ {.fa = sqlite_libvers_main }, "sqlite_libvers", "SQLite3 libvers tool" },
	{ {.fa = sqlite_max_limits_main }, "sqlite_max_limits", "SQLite3 max_limits tool" },
	{ {.fa = sqlite_offsets_main }, "sqlite_offsets", "SQLite3 offsets tool" },
	{ {.fa = sqlite_pagesig_main }, "sqlite_pagesig", "SQLite3 pagesig tool" },
	{ {.fa = sqlite_showdb_main }, "sqlite_showdb", "SQLite3 show db tool" },
	{ {.fa = sqlite_showjournal_main }, "sqlite_showjournal", "SQLite3 show journal tool" },
	{ {.fa = sqlite_showlocks_main }, "sqlite_showlocks", "SQLite3 show locks tool" },
	{ {.fa = sqlite_showshm_main }, "sqlite_showshm", "SQLite3 show shm tool" },
	{ {.fa = sqlite_showstat4_main }, "sqlite_showstat4", "SQLite3 show stat4 tool" },
	{ {.fa = sqlite_showwal_main }, "sqlite_showwal", "SQLite3 show wal tool" },

	{ {.fa = sqlite_logest_main }, "sqlite_logest", "SQLite3 logest tool" },
	{ {.fa = sqlite_varint_main }, "sqlite_varint", "SQLite3 varint tool" },

	{ {.fa = sqlite_kvtest_main }, "sqlite_kvtest", "SQLite3 kvtest tool" },
	{ {.fa = sqlite_speedtest1_main }, "sqlite_speedtest1", "SQLite3 speedtest1 tool" },
	{ {.fa = sqlite_speedtest8_main }, "sqlite_speedtest8", "SQLite3 speedtest8 tool" },
	{ {.fa = sqlite_test_startup_main }, "sqlite_test_startup", "SQLite3 test_startup tool" },
	{ {.fa = sqlite_threadtest1_main }, "sqlite_threadtest1", "SQLite3 threadtest1 tool" },
	{ {.fa = sqlite_threadtest2_main }, "sqlite_threadtest2", "SQLite3 threadtest2 tool" },
	{ {.fa = sqlite_threadtest3_main }, "sqlite_threadtest3", "SQLite3 threadtest3 tool" },
	{ {.fa = sqlite_threadtest4_main }, "sqlite_threadtest4", "SQLite3 threadtest4 tool" },
	{ {.fa = sqlite_threadtest5_main }, "sqlite_threadtest5", "SQLite3 threadtest5 tool" },
	{ {.fa = sqlite_wordcount_main }, "sqlite_wordcount", "SQLite3 wordcount test tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = lept_adaptmap_dark_main }, "lept_adaptmap_dark", "leptonica adaptmap_dark test/tool" },
	{ {.fa = lept_adaptmap_reg_main }, "lept_adaptmap", "leptonica adaptmap_reg test/tool" },
	{ {.fa = lept_adaptnorm_reg_main }, "lept_adaptnorm", "leptonica adaptnorm_reg test/tool" },
	{ {.fa = lept_affine_reg_main }, "lept_affine", "leptonica affine_reg test/tool" },
	{ {.fa = lept_alltests_reg_main }, "lept_alltests", "leptonica alltests_reg test/tool", 1 },
	{ {.fa = lept_alphaops_reg_main }, "lept_alphaops", "leptonica alphaops_reg test/tool" },
	{ {.fa = lept_alphaxform_reg_main }, "lept_alphaxform", "leptonica alphaxform_reg test/tool" },
	{ {.fa = lept_arabic_lines_main }, "lept_arabic_lines", "leptonica arabic_lines test/tool" },
	{ {.fa = lept_arithtest_main }, "lept_arithtest", "leptonica arithtest test/tool" },
	{ {.fa = lept_autogentest1_main }, "lept_autogentest1", "leptonica autogentest1 test/tool" },
	{ {.fa = lept_autogentest2_main }, "lept_autogentest2", "leptonica autogentest2 test/tool" },
	{ {.fa = lept_barcodetest_main }, "lept_barcodetest", "leptonica barcodetest test/tool" },
	{ {.fa = lept_baseline_reg_main }, "lept_baseline", "leptonica baseline_reg test/tool" },
	{ {.fa = lept_bilateral1_reg_main }, "lept_bilateral1", "leptonica bilateral1_reg test/tool" },
	{ {.fa = lept_bilateral2_reg_main }, "lept_bilateral2", "leptonica bilateral2_reg test/tool" },
	{ {.fa = lept_bilinear_reg_main }, "lept_bilinear", "leptonica bilinear_reg test/tool" },
	{ {.fa = lept_binarize_reg_main }, "lept_binarize", "leptonica binarize_reg test/tool" },
	{ {.fa = lept_binarize_set_main }, "lept_binarize_set", "leptonica binarize_set test/tool" },
	{ {.fa = lept_binarizefiles_main }, "lept_binarizefiles", "leptonica binarizefiles test/tool" },
	{ {.fa = lept_bincompare_main }, "lept_bincompare", "leptonica bincompare test/tool" },
	{ {.fa = lept_binmorph1_reg_main }, "lept_binmorph1", "leptonica binmorph1_reg test/tool" },
	{ {.fa = lept_binmorph2_reg_main }, "lept_binmorph2", "leptonica binmorph2_reg test/tool" },
	{ {.fa = lept_binmorph3_reg_main }, "lept_binmorph3", "leptonica binmorph3_reg test/tool" },
	{ {.fa = lept_binmorph4_reg_main }, "lept_binmorph4", "leptonica binmorph4_reg test/tool" },
	{ {.fa = lept_binmorph5_reg_main }, "lept_binmorph5", "leptonica binmorph5_reg test/tool" },
	{ {.fa = lept_binmorph6_reg_main }, "lept_binmorph6", "leptonica binmorph6_reg test/tool" },
	{ {.fa = lept_blackwhite_reg_main }, "lept_blackwhite", "leptonica blackwhite_reg test/tool" },
	{ {.fa = lept_blend1_reg_main }, "lept_blend1", "leptonica blend1_reg test/tool" },
	{ {.fa = lept_blend2_reg_main }, "lept_blend2", "leptonica blend2_reg test/tool" },
	{ {.fa = lept_blend3_reg_main }, "lept_blend3", "leptonica blend3_reg test/tool" },
	{ {.fa = lept_blend4_reg_main }, "lept_blend4", "leptonica blend4_reg test/tool" },
	{ {.fa = lept_blend5_reg_main }, "lept_blend5", "leptonica blend5_reg test/tool" },
	{ {.fa = lept_blendcmaptest_main }, "lept_blendcmaptest", "leptonica blendcmaptest test/tool" },
	{ {.fa = lept_boxa1_reg_main }, "lept_boxa1", "leptonica boxa1_reg test/tool" },
	{ {.fa = lept_boxa2_reg_main }, "lept_boxa2", "leptonica boxa2_reg test/tool" },
	{ {.fa = lept_boxa3_reg_main }, "lept_boxa3", "leptonica boxa3_reg test/tool" },
	{ {.fa = lept_boxa4_reg_main }, "lept_boxa4", "leptonica boxa4_reg test/tool" },
	{ {.fa = lept_buffertest_main }, "lept_buffertest", "leptonica buffertest test/tool" },
	{ {.fa = lept_bytea_reg_main }, "lept_bytea", "leptonica bytea_reg test/tool" },
	{ {.fa = lept_ccbord_reg_main }, "lept_ccbord", "leptonica ccbord_reg test/tool" },
	{ {.fa = lept_ccbordtest_main }, "lept_ccbordtest", "leptonica ccbordtest test/tool" },
	{ {.fa = lept_cctest1_main }, "lept_cctest1", "leptonica cctest1 test/tool" },
	{ {.fa = lept_ccthin1_reg_main }, "lept_ccthin1", "leptonica ccthin1_reg test/tool" },
	{ {.fa = lept_ccthin2_reg_main }, "lept_ccthin2", "leptonica ccthin2_reg test/tool" },
	{ {.fa = lept_checkerboard_reg_main }, "lept_checkerboard", "leptonica checkerboard_reg test/tool" },
	{ {.fa = lept_circle_reg_main }, "lept_circle", "leptonica circle_reg test/tool" },
	{ {.fa = lept_cleanpdf_main }, "lept_cleanpdf", "leptonica cleanpdf test/tool" },
	{ {.fa = lept_cmapquant_reg_main }, "lept_cmapquant", "leptonica cmapquant_reg test/tool" },
	{ {.fa = lept_colorcontent_reg_main }, "lept_colorcontent", "leptonica colorcontent_reg test/tool" },
	{ {.fa = lept_colorfill_reg_main }, "lept_colorfill", "leptonica colorfill_reg test/tool" },
	{ {.fa = lept_coloring_reg_main }, "lept_coloring", "leptonica coloring_reg test/tool" },
	{ {.fa = lept_colorize_reg_main }, "lept_colorize", "leptonica colorize_reg test/tool" },
	{ {.fa = lept_colormask_reg_main }, "lept_colormask", "leptonica colormask_reg test/tool" },
	{ {.fa = lept_colormorph_reg_main }, "lept_colormorph", "leptonica colormorph_reg test/tool" },
	{ {.fa = lept_colorquant_reg_main }, "lept_colorquant", "leptonica colorquant_reg test/tool" },
	{ {.fa = lept_colorseg_reg_main }, "lept_colorseg", "leptonica colorseg_reg test/tool" },
	{ {.fa = lept_colorsegtest_main }, "lept_colorsegtest", "leptonica colorsegtest test/tool" },
	{ {.fa = lept_colorspace_reg_main }, "lept_colorspace", "leptonica colorspace_reg test/tool" },
	{ {.fa = lept_compare_reg_main }, "lept_compare", "leptonica compare_reg test/tool" },
	{ {.fa = lept_comparepages_main }, "lept_comparepages", "leptonica comparepages test/tool" },
	{ {.fa = lept_comparepixa_main }, "lept_comparepixa", "leptonica comparepixa test/tool" },
	{ {.fa = lept_comparetest_main }, "lept_comparetest", "leptonica comparetest test/tool" },
	{ {.fa = lept_compfilter_reg_main }, "lept_compfilter", "leptonica compfilter_reg test/tool" },
	{ {.fa = lept_compresspdf_main }, "lept_compresspdf", "leptonica compresspdf test/tool" },
	{ {.fa = lept_conncomp_reg_main }, "lept_conncomp", "leptonica conncomp_reg test/tool" },
	{ {.fa = lept_contrasttest_main }, "lept_contrasttest", "leptonica contrasttest test/tool" },
	{ {.fa = lept_conversion_reg_main }, "lept_conversion", "leptonica conversion_reg test/tool" },
	{ {.fa = lept_convertfilestopdf_main }, "lept_convertfilestopdf", "leptonica convertfilestopdf test/tool" },
	{ {.fa = lept_convertfilestops_main }, "lept_convertfilestops", "leptonica convertfilestops test/tool" },
	{ {.fa = lept_convertformat_main }, "lept_convertformat", "leptonica convertformat test/tool" },
	{ {.fa = lept_convertsegfilestopdf_main }, "lept_convertsegfilestopdf", "leptonica convertsegfilestopdf test/tool" },
	{ {.fa = lept_convertsegfilestops_main }, "lept_convertsegfilestops", "leptonica convertsegfilestops test/tool" },
	{ {.fa = lept_converttogray_main }, "lept_converttogray", "leptonica converttogray test/tool" },
	{ {.fa = lept_converttopdf_main }, "lept_converttopdf", "leptonica converttopdf test/tool" },
	{ {.fa = lept_converttops_main }, "lept_converttops", "leptonica converttops test/tool" },
	{ {.fa = lept_convolve_reg_main }, "lept_convolve", "leptonica convolve_reg test/tool" },
	{ {.fa = lept_cornertest_main }, "lept_cornertest", "leptonica cornertest test/tool" },
	{ {.fa = lept_corrupttest_main }, "lept_corrupttest", "leptonica corrupttest test/tool" },
	{ {.fa = lept_crop_reg_main }, "lept_crop", "leptonica crop_reg test/tool" },
	{ {.fa = lept_croppdf_main }, "lept_croppdf", "leptonica croppdf test/tool" },
	{ {.fa = lept_croptext_main }, "lept_croptext", "leptonica croptext test/tool" },
	{ {.fa = lept_custom_log_plot_test_main }, "lept_custom_log_plot_test", "leptonica custom log+plot output test" },
	{ {.fa = lept_deskew_it_main }, "lept_deskew_it", "leptonica deskew_it test/tool" },
	{ {.fa = lept_dewarp_reg_main }, "lept_dewarp", "leptonica dewarp_reg test/tool" },
	{ {.fa = lept_dewarprules_main }, "lept_dewarprules", "leptonica dewarprules test/tool" },
	{ {.fa = lept_dewarptest1_main }, "lept_dewarptest1", "leptonica dewarptest1 test/tool" },
	{ {.fa = lept_dewarptest2_main }, "lept_dewarptest2", "leptonica dewarptest2 test/tool" },
	{ {.fa = lept_dewarptest3_main }, "lept_dewarptest3", "leptonica dewarptest3 test/tool" },
	{ {.fa = lept_dewarptest4_main }, "lept_dewarptest4", "leptonica dewarptest4 test/tool" },
	{ {.fa = lept_dewarptest5_main }, "lept_dewarptest5", "leptonica dewarptest5 test/tool" },
	{ {.fa = lept_digitprep1_main }, "lept_digitprep1", "leptonica digitprep1 test/tool" },
	{ {.fa = lept_displayboxa_main }, "lept_displayboxa", "leptonica displayboxa test/tool" },
	{ {.fa = lept_displayboxes_on_pixa_main }, "lept_displayboxes_on_pixa", "leptonica displayboxes_on_pixa test/tool" },
	{ {.fa = lept_displaypix_main }, "lept_displaypix", "leptonica displaypix test/tool" },
	{ {.fa = lept_displaypixa_main }, "lept_displaypixa", "leptonica displaypixa test/tool" },
	{ {.fa = lept_distance_reg_main }, "lept_distance", "leptonica distance_reg test/tool" },
	{ {.fa = lept_dither_reg_main }, "lept_dither", "leptonica dither_reg test/tool" },
	{ {.fa = lept_dna_reg_main }, "lept_dna", "leptonica dna_reg test/tool" },
	{ {.fa = lept_dwalineargen_main }, "lept_dwalineargen", "leptonica dwalineargen test/tool" },
	{ {.fa = lept_dwamorph1_reg_main }, "lept_dwamorph1", "leptonica dwamorph1_reg test/tool" },
	{ {.fa = lept_dwamorph2_reg_main }, "lept_dwamorph2", "leptonica dwamorph2_reg test/tool" },
	{ {.fa = lept_edge_reg_main }, "lept_edge", "leptonica edge_reg test/tool" },
	{ {.fa = lept_encoding_reg_main }, "lept_encoding", "leptonica encoding_reg test/tool" },
	{ {.fa = lept_enhance_reg_main }, "lept_enhance", "leptonica enhance_reg test/tool" },
	{ {.fa = lept_equal_reg_main }, "lept_equal", "leptonica equal_reg test/tool" },
	{ {.fa = lept_expand_reg_main }, "lept_expand", "leptonica expand_reg test/tool" },
	{ {.fa = lept_extrema_reg_main }, "lept_extrema", "leptonica extrema_reg test/tool" },
	{ {.fa = lept_falsecolor_reg_main }, "lept_falsecolor", "leptonica falsecolor_reg test/tool" },
	{ {.fa = lept_fcombautogen_main }, "lept_fcombautogen", "leptonica fcombautogen test/tool" },
	{ {.fa = lept_fhmtauto_reg_main }, "lept_fhmtauto", "leptonica fhmtauto_reg test/tool" },
	{ {.fa = lept_fhmtautogen_main }, "lept_fhmtautogen", "leptonica fhmtautogen test/tool" },
	{ {.fa = lept_fileinfo_main }, "lept_fileinfo", "leptonica fileinfo test/tool" },
	{ {.fa = lept_files_reg_main }, "lept_files", "leptonica files_reg test/tool" },
	{ {.fa = lept_find_colorregions_main }, "lept_find_colorregions", "leptonica find_colorregions test/tool" },
	{ {.fa = lept_findbinding_main }, "lept_findbinding", "leptonica findbinding test/tool" },
	{ {.fa = lept_findcorners_reg_main }, "lept_findcorners", "leptonica findcorners_reg test/tool" },
	{ {.fa = lept_findpattern1_main }, "lept_findpattern1", "leptonica findpattern1 test/tool" },
	{ {.fa = lept_findpattern2_main }, "lept_findpattern2", "leptonica findpattern2 test/tool" },
	{ {.fa = lept_findpattern3_main }, "lept_findpattern3", "leptonica findpattern3 test/tool" },
	{ {.fa = lept_findpattern1_reg_main }, "lept_findpattern1R", "leptonica findpattern1_reg test/tool" },
	{ {.fa = lept_findpattern2_reg_main }, "lept_findpattern2R", "leptonica findpattern2_reg test/tool" },
	{ {.fa = lept_flipdetect_reg_main }, "lept_flipdetect", "leptonica flipdetect_reg test/tool" },
	{ {.fa = lept_fmorphauto_reg_main }, "lept_fmorphauto", "leptonica fmorphauto_reg test/tool" },
	{ {.fa = lept_fmorphautogen_main }, "lept_fmorphautogen", "leptonica fmorphautogen test/tool" },
	{ {.fa = lept_fpix1_reg_main }, "lept_fpix1", "leptonica fpix1_reg test/tool" },
	{ {.fa = lept_fpix2_reg_main }, "lept_fpix2", "leptonica fpix2_reg test/tool" },
	{ {.fa = lept_fpixcontours_main }, "lept_fpixcontours", "leptonica fpixcontours test/tool" },
	{ {.fa = lept_gammatest_main }, "lept_gammatest", "leptonica gammatest test/tool" },
	{ {.fa = lept_genfonts_reg_main }, "lept_genfonts", "leptonica genfonts_reg test/tool" },
	{ {.fa = lept_gifio_reg_main }, "lept_gifio", "leptonica gifio_reg test/tool" },
	{ {.fa = lept_graphicstest_main }, "lept_graphicstest", "leptonica graphicstest test/tool" },
	{ {.fa = lept_grayfill_reg_main }, "lept_grayfill", "leptonica grayfill_reg test/tool" },
	{ {.fa = lept_graymorph1_reg_main }, "lept_graymorph1", "leptonica graymorph1_reg test/tool" },
	{ {.fa = lept_graymorph2_reg_main }, "lept_graymorph2", "leptonica graymorph2_reg test/tool" },
	{ {.fa = lept_graymorphtest_main }, "lept_graymorphtest", "leptonica graymorphtest test/tool" },
	{ {.fa = lept_grayquant_reg_main }, "lept_grayquant", "leptonica grayquant_reg test/tool" },
	{ {.fa = lept_hardlight_reg_main }, "lept_hardlight", "leptonica hardlight_reg test/tool" },
	{ {.fa = lept_hash_reg_main }, "lept_hash", "leptonica hash_reg test/tool" },
	{ {.fa = lept_hashtest_main }, "lept_hashtest", "leptonica hashtest test/tool" },
	{ {.fa = lept_heap_reg_main }, "lept_heap", "leptonica heap_reg test/tool" },
	{ {.fa = lept_histoduptest_main }, "lept_histoduptest", "leptonica histoduptest test/tool" },
	{ {.fa = lept_histotest_main }, "lept_histotest", "leptonica histotest test/tool" },
	{ {.fa = lept_htmlviewer_main }, "lept_htmlviewer", "leptonica htmlviewer test/tool" },
	{ {.fa = lept_imagetops_main }, "lept_imagetops", "leptonica imagetops test/tool" },
	{ {.fa = lept_insert_reg_main }, "lept_insert", "leptonica insert_reg test/tool" },
	{ {.fa = lept_ioformats_reg_main }, "lept_ioformats", "leptonica ioformats_reg test/tool" },
	{ {.fa = lept_iomisc_reg_main }, "lept_iomisc", "leptonica iomisc_reg test/tool" },
	{ {.fa = lept_italic_reg_main }, "lept_italic", "leptonica italic_reg test/tool" },
	{ {.fa = lept_jbclass_reg_main }, "lept_jbclass", "leptonica jbclass_reg test/tool" },
	{ {.fa = lept_jbcorrelation_main }, "lept_jbcorrelation", "leptonica jbcorrelation test/tool" },
	{ {.fa = lept_jbrankhaus_main }, "lept_jbrankhaus", "leptonica jbrankhaus test/tool" },
	{ {.fa = lept_jbwords_main }, "lept_jbwords", "leptonica jbwords test/tool" },
	{ {.fa = lept_jp2kio_reg_main }, "lept_jp2kio", "leptonica jp2kio_reg test/tool" },
	{ {.fa = lept_jpegio_reg_main }, "lept_jpegio", "leptonica jpegio_reg test/tool" },
	{ {.fa = lept_kernel_reg_main }, "lept_kernel", "leptonica kernel_reg test/tool" },
	{ {.fa = lept_label_reg_main }, "lept_label", "leptonica label_reg test/tool" },
	{ {.fa = lept_lightcolortest_main }, "lept_lightcolortest", "leptonica lightcolortest test/tool" },
	{ {.fa = lept_lineremoval_reg_main }, "lept_lineremoval", "leptonica lineremoval_reg test/tool" },
	{ {.fa = lept_listtest_main }, "lept_listtest", "leptonica listtest test/tool" },
	{ {.fa = lept_livre_adapt_main }, "lept_livre_adapt", "leptonica livre_adapt test/tool" },
	{ {.fa = lept_livre_hmt_main }, "lept_livre_hmt", "leptonica livre_hmt test/tool" },
	{ {.fa = lept_livre_makefigs_main }, "lept_livre_makefigs", "leptonica livre_makefigs test/tool" },
	{ {.fa = lept_livre_orient_main }, "lept_livre_orient", "leptonica livre_orient test/tool" },
	{ {.fa = lept_livre_pageseg_main }, "lept_livre_pageseg", "leptonica livre_pageseg test/tool" },
	{ {.fa = lept_livre_seedgen_main }, "lept_livre_seedgen", "leptonica livre_seedgen test/tool" },
	{ {.fa = lept_livre_tophat_main }, "lept_livre_tophat", "leptonica livre_tophat test/tool" },
	{ {.fa = lept_locminmax_reg_main }, "lept_locminmax", "leptonica locminmax_reg test/tool" },
	{ {.fa = lept_logicops_reg_main }, "lept_logicops", "leptonica logicops_reg test/tool" },
	{ {.fa = lept_lowaccess_reg_main }, "lept_lowaccess", "leptonica lowaccess_reg test/tool" },
	{ {.fa = lept_lowsat_reg_main }, "lept_lowsat", "leptonica lowsat_reg test/tool" },
	{ {.fa = lept_maketile_main }, "lept_maketile", "leptonica maketile test/tool" },
	{ {.fa = lept_maptest_main }, "lept_maptest", "leptonica maptest test/tool" },
	{ {.fa = lept_maze_reg_main }, "lept_maze", "leptonica maze_reg test/tool" },
	{ {.fa = lept_misctest1_main }, "lept_misc1", "leptonica misc1 test/tool" },
	{ {.fa = lept_modifyhuesat_main }, "lept_modifyhuesat", "leptonica modifyhuesat test/tool" },
	{ {.fa = lept_morphseq_reg_main }, "lept_morphseq", "leptonica morphseq_reg test/tool" },
	{ {.fa = lept_morphtest1_main }, "lept_morphtest1", "leptonica morphtest1 test/tool" },
	{ {.fa = lept_mtiff_reg_main }, "lept_mtiff", "leptonica mtiff_reg test/tool" },
	{ {.fa = lept_multitype_reg_main }, "lept_multitype", "leptonica multitype_reg test/tool" },
	{ {.fa = lept_nearline_reg_main }, "lept_nearline", "leptonica nearline_reg test/tool" },
	{ {.fa = lept_newspaper_reg_main }, "lept_newspaper", "leptonica newspaper_reg test/tool" },
	{ {.fa = lept_numa1_reg_main }, "lept_numa1", "leptonica numa1_reg test/tool" },
	{ {.fa = lept_numa2_reg_main }, "lept_numa2", "leptonica numa2_reg test/tool" },
	{ {.fa = lept_numa3_reg_main }, "lept_numa3", "leptonica numa3_reg test/tool" },
	{ {.fa = lept_numaranktest_main }, "lept_numaranktest", "leptonica numaranktest test/tool" },
	{ {.fa = lept_otsutest1_main }, "lept_otsutest1", "leptonica otsutest1 test/tool" },
	{ {.fa = lept_otsutest2_main }, "lept_otsutest2", "leptonica otsutest2 test/tool" },
	{ {.fa = lept_overlap_reg_main }, "lept_overlap", "leptonica overlap_reg test/tool" },
	{ {.fa = lept_pageseg_reg_main }, "lept_pageseg", "leptonica pageseg_reg test/tool" },
	{ {.fa = lept_pagesegtest1_main }, "lept_pagesegtest1", "leptonica pagesegtest1 test/tool" },
	{ {.fa = lept_pagesegtest2_main }, "lept_pagesegtest2", "leptonica pagesegtest2 test/tool" },
	{ {.fa = lept_paint_reg_main }, "lept_paint", "leptonica paint_reg test/tool" },
	{ {.fa = lept_paintmask_reg_main }, "lept_paintmask", "leptonica paintmask_reg test/tool" },
	{ {.fa = lept_partifytest_main }, "lept_partifytest", "leptonica partifytest test/tool" },
	{ {.fa = lept_partition_reg_main }, "lept_partition", "leptonica partition_reg test/tool" },
	{ {.fa = lept_partitiontest_main }, "lept_partitiontest", "leptonica partitiontest test/tool" },
	{ {.fa = lept_pdfio1_reg_main }, "lept_pdfio1", "leptonica pdfio1_reg test/tool" },
	{ {.fa = lept_pdfio2_reg_main }, "lept_pdfio2", "leptonica pdfio2_reg test/tool" },
	{ {.fa = lept_pdfseg_reg_main }, "lept_pdfseg", "leptonica pdfseg_reg test/tool" },
	{ {.fa = lept_percolatetest_main }, "lept_percolatetest", "leptonica percolatetest test/tool" },
	{ {.fa = lept_pixa1_reg_main }, "lept_pixa1", "leptonica pixa1_reg test/tool" },
	{ {.fa = lept_pixa2_reg_main }, "lept_pixa2", "leptonica pixa2_reg test/tool" },
	{ {.fa = lept_pixaatest_main }, "lept_pixaatest", "leptonica pixaatest test/tool" },
	{ {.fa = lept_pixadisp_reg_main }, "lept_pixadisp", "leptonica pixadisp_reg test/tool" },
	{ {.fa = lept_pixafileinfo_main }, "lept_pixafileinfo", "leptonica pixafileinfo test/tool" },
	{ {.fa = lept_pixalloc_reg_main }, "lept_pixalloc", "leptonica pixalloc_reg test/tool" },
	{ {.fa = lept_pixcomp_reg_main }, "lept_pixcomp", "leptonica pixcomp_reg test/tool" },
	{ {.fa = lept_pixmem_reg_main }, "lept_pixmem", "leptonica pixmem_reg test/tool" },
	{ {.fa = lept_pixserial_reg_main }, "lept_pixserial", "leptonica pixserial_reg test/tool" },
	{ {.fa = lept_pixtile_reg_main }, "lept_pixtile", "leptonica pixtile_reg test/tool" },
	{ {.fa = lept_plottest_main }, "lept_plottest", "leptonica plottest test/tool" },
	{ {.fa = lept_pngio_reg_main }, "lept_pngio", "leptonica pngio_reg test/tool" },
	{ {.fa = lept_pnmio_reg_main }, "lept_pnmio", "leptonica pnmio_reg test/tool" },
	{ {.fa = lept_printimage_main }, "lept_printimage", "leptonica printimage test/tool" },
	{ {.fa = lept_printsplitimage_main }, "lept_printsplitimage", "leptonica printsplitimage test/tool" },
	{ {.fa = lept_printtiff_main }, "lept_printtiff", "leptonica printtiff test/tool" },
	{ {.fa = lept_projection_reg_main }, "lept_projection", "leptonica projection_reg test/tool" },
	{ {.fa = lept_projective_reg_main }, "lept_projective", "leptonica projective_reg test/tool" },
	{ {.fa = lept_psio_reg_main }, "lept_psio", "leptonica psio_reg test/tool" },
	{ {.fa = lept_psioseg_reg_main }, "lept_psioseg", "leptonica psioseg_reg test/tool" },
	{ {.fa = lept_pta_reg_main }, "lept_pta", "leptonica pta_reg test/tool" },
	{ {.fa = lept_ptra1_reg_main }, "lept_ptra1", "leptonica ptra1_reg test/tool" },
	{ {.fa = lept_ptra2_reg_main }, "lept_ptra2", "leptonica ptra2_reg test/tool" },
	{ {.fa = lept_quadtree_reg_main }, "lept_quadtree", "leptonica quadtree_reg test/tool" },
	{ {.fa = lept_rank_reg_main }, "lept_rank", "leptonica rank_reg test/tool" },
	{ {.fa = lept_rankbin_reg_main }, "lept_rankbin", "leptonica rankbin_reg test/tool" },
	{ {.fa = lept_rankhisto_reg_main }, "lept_rankhisto", "leptonica rankhisto_reg test/tool" },
	{ {.fa = lept_rasterop_reg_main }, "lept_rasterop", "leptonica rasterop_reg test/tool" },
	{ {.fa = lept_rasteropip_reg_main }, "lept_rasteropip", "leptonica rasteropip_reg test/tool" },
	{ {.fa = lept_rasteroptest_main }, "lept_rasteroptest", "leptonica rasteroptest test/tool" },
	{ {.fa = lept_rbtreetest_main }, "lept_rbtreetest", "leptonica rbtreetest test/tool" },
	{ {.fa = lept_recog_bootnum1_main }, "lept_recog_bootnum1", "leptonica recog_bootnum1 test/tool" },
	{ {.fa = lept_recog_bootnum2_main }, "lept_recog_bootnum2", "leptonica recog_bootnum2 test/tool" },
	{ {.fa = lept_recog_bootnum3_main }, "lept_recog_bootnum3", "leptonica recog_bootnum3 test/tool" },
	{ {.fa = lept_recogsort_main }, "lept_recogsort", "leptonica recogsort test/tool" },
	{ {.fa = lept_recogtest1_main }, "lept_recogtest1", "leptonica recogtest1 test/tool" },
	{ {.fa = lept_recogtest2_main }, "lept_recogtest2", "leptonica recogtest2 test/tool" },
	{ {.fa = lept_recogtest3_main }, "lept_recogtest3", "leptonica recogtest3 test/tool" },
	{ {.fa = lept_recogtest4_main }, "lept_recogtest4", "leptonica recogtest4 test/tool" },
	{ {.fa = lept_recogtest5_main }, "lept_recogtest5", "leptonica recogtest5 test/tool" },
	{ {.fa = lept_recogtest6_main }, "lept_recogtest6", "leptonica recogtest6 test/tool" },
	{ {.fa = lept_recogtest7_main }, "lept_recogtest7", "leptonica recogtest7 test/tool" },
	{ {.fa = lept_rectangle_reg_main }, "lept_rectangle", "leptonica rectangle_reg test/tool" },
	{ {.fa = lept_reducetest_main }, "lept_reducetest", "leptonica reducetest test/tool" },
	{ {.fa = lept_removecmap_main }, "lept_removecmap", "leptonica removecmap test/tool" },
	{ {.fa = lept_renderfonts_main }, "lept_renderfonts", "leptonica renderfonts test/tool" },
	{ {.fa = lept_replacebytes_main }, "lept_replacebytes", "leptonica replacebytes test/tool" },
	{ {.fa = lept_rotate1_reg_main }, "lept_rotate1", "leptonica rotate1_reg test/tool" },
	{ {.fa = lept_rotate2_reg_main }, "lept_rotate2", "leptonica rotate2_reg test/tool" },
	{ {.fa = lept_rotate_it_main }, "lept_rotate_it", "leptonica rotate_it test/tool" },
	{ {.fa = lept_rotatefastalt_main }, "lept_rotatefastalt", "leptonica rotatefastalt test/tool" },
	{ {.fa = lept_rotateorth_reg_main }, "lept_rotateorth", "leptonica rotateorth_reg test/tool" },
	{ {.fa = lept_rotateorthtest1_main }, "lept_rotateorthtest1", "leptonica rotateorthtest1 test/tool" },
	{ {.fa = lept_rotatetest1_main }, "lept_rotatetest1", "leptonica rotatetest1 test/tool" },
	{ {.fa = lept_runlengthtest_main }, "lept_runlengthtest", "leptonica runlengthtest test/tool" },
	{ {.fa = lept_scale_it_main }, "lept_scale_it", "leptonica scale_it test/tool" },
	{ {.fa = lept_scale_reg_main }, "lept_scale", "leptonica scale_reg test/tool" },
	{ {.fa = lept_scaleandtile_main }, "lept_scaleandtile", "leptonica scaleandtile test/tool" },
	{ {.fa = lept_scaletest1_main }, "lept_scaletest1", "leptonica scaletest1 test/tool" },
	{ {.fa = lept_scaletest2_main }, "lept_scaletest2", "leptonica scaletest2 test/tool" },
	{ {.fa = lept_scaleimages_main }, "lept_scaleimages", "leptonica scaleimages test/tool" },
	{ {.fa = lept_seedfilltest_main }, "lept_seedfilltest", "leptonica seedfilltest test/tool" },
	{ {.fa = lept_seedspread_reg_main }, "lept_seedspread", "leptonica seedspread_reg test/tool" },
	{ {.fa = lept_selio_reg_main }, "lept_selio", "leptonica selio_reg test/tool" },
	{ {.fa = lept_settest_main }, "lept_settest", "leptonica settest test/tool" },
	{ {.fa = lept_sharptest_main }, "lept_sharptest", "leptonica sharptest test/tool" },
	{ {.fa = lept_shear1_reg_main }, "lept_shear1", "leptonica shear1_reg test/tool" },
	{ {.fa = lept_shear2_reg_main }, "lept_shear2", "leptonica shear2_reg test/tool" },
	{ {.fa = lept_sheartest_main }, "lept_sheartest", "leptonica sheartest test/tool" },
	{ {.fa = lept_showedges_main }, "lept_showedges", "leptonica showedges test/tool" },
	{ {.fa = lept_skew_reg_main }, "lept_skew", "leptonica skew_reg test/tool" },
	{ {.fa = lept_skewtest_main }, "lept_skewtest", "leptonica skewtest test/tool" },
	{ {.fa = lept_smallpix_reg_main }, "lept_smallpix", "leptonica smallpix_reg test/tool" },
	{ {.fa = lept_smoothedge_reg_main }, "lept_smoothedge", "leptonica smoothedge_reg test/tool" },
	{ {.fa = lept_sorttest_main }, "lept_sorttest", "leptonica sorttest test/tool" },
	{ {.fa = lept_speckle_reg_main }, "lept_speckle", "leptonica speckle_reg test/tool" },
	{ {.fa = lept_splitcomp_reg_main }, "lept_splitcomp", "leptonica splitcomp_reg test/tool" },
	{ {.fa = lept_splitimage2pdf_main }, "lept_splitimage2pdf", "leptonica splitimage2pdf test/tool" },
	{ {.fa = lept_splitpdf_main }, "lept_splitpdf", "leptonica splitpdf test/tool" },
	{ {.fa = lept_string_reg_main }, "lept_string", "leptonica string_reg test/tool" },
	{ {.fa = lept_subpixel_reg_main }, "lept_subpixel", "leptonica subpixel_reg test/tool" },
	{ {.fa = lept_sudokutest_main }, "lept_sudokutest", "leptonica sudokutest test/tool" },
	{ {.fa = lept_textorient_main }, "lept_textorient", "leptonica textorient test/tool" },
	{ {.fa = lept_texturefill_reg_main }, "lept_texturefill", "leptonica texturefill_reg test/tool" },
	{ {.fa = lept_threshnorm_reg_main }, "lept_threshnorm", "leptonica threshnorm_reg test/tool" },
	{ {.fa = lept_thresholding_test_main }, "lept_thresholding", "leptonica thresholding demo test/tool" },
	{ {.fa = lept_tiffpdftest_main }, "lept_tiffpdftest", "leptonica tiffpdftest test/tool" },
	{ {.fa = lept_translate_reg_main }, "lept_translate", "leptonica translate_reg test/tool" },
	{ {.fa = lept_trctest_main }, "lept_trctest", "leptonica trctest test/tool" },
	{ {.fa = lept_underlinetest_main }, "lept_underlinetest", "leptonica underlinetest test/tool" },
	{ {.fa = lept_warper_reg_main }, "lept_warper", "leptonica warper_reg test/tool" },
	{ {.fa = lept_warpertest_main }, "lept_warpertest", "leptonica warpertest test/tool" },
	{ {.fa = lept_watershed_reg_main }, "lept_watershed", "leptonica watershed_reg test/tool" },
	{ {.fa = lept_webpanimio_reg_main }, "lept_webpanimio", "leptonica webpanimio_reg test/tool" },
	{ {.fa = lept_webpio_reg_main }, "lept_webpio", "leptonica webpio_reg test/tool" },
	{ {.fa = lept_wordboxes_reg_main }, "lept_wordboxes", "leptonica wordboxes_reg test/tool" },
	{ {.fa = lept_wordsinorder_main }, "lept_wordsinorder", "leptonica wordsinorder test/tool" },
	{ {.fa = lept_writemtiff_main }, "lept_writemtiff", "leptonica writemtiff test/tool" },
	{ {.fa = lept_writetext_reg_main }, "lept_writetext", "leptonica writetext_reg test/tool" },
	{ {.fa = lept_xformbox_reg_main }, "lept_xformbox", "leptonica xformbox_reg test/tool" },
	{ {.fa = lept_yuvtest_main }, "lept_yuvtest", "leptonica yuvtest test/tool" },
	{ {.fa = lept_issue675_check_main }, "lept_issue675", "leptonica BMP test for leptonica issue #675" },
	{ {.f = lept_message_test_main }, "lept_messagetest", "leptonica message test/tool" },
	{ {.fa = lept_misctest2_main }, "lept_misc2", "leptonica misc2 test/tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = jpeginfo_main }, "jpeginfo", "jpeginfo tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = jpegturbo_jpegtran_main }, "jpegtran", "jpegtran tool" },
	{ {.fa = jpegturbo_rdjpegcom_main }, "rdjpegcom", "rdjpegcom tool" },
	{ {.fa = jpegturbo_wrjpegcom_main }, "wrjpegcom", "wrjpegcom tool" },
	{ {.fa = jpegturbo_djpeg_main }, "djpeg", "djpeg tool" },
	{ {.fa = jpegturbo_cjpeg_main }, "cjpeg", "cjpeg tool" },
	{ {.fa = jpegturbo_md5sum_main }, "tj_md5sum", "jpeg_turbo's md5sum tool" },
	{ {.fa = jpegturbo_md5cmp_main }, "tj_md5cmp", "jpeg_turbo's md5cmp tool" },
	{ {.fa = jpegturbo_tjcomp_main }, "tjcomp", "tjcomp tool" },
	{ {.fa = jpegturbo_tjdecomp_main }, "tjdecomp", "tjdecomp tool" },
	{ {.fa = jpegturbo_tjbench_main }, "tjbench", "tjbench tool" },
	{ {.f = tj_test_intrinsic_bitcount_main }, "test_intrinsic_bitcount", "turbojpeg intrinsic_bitcount test tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = jbig2dec_main }, "jbig2dec", "jbig2dec tool" },
	{ {.fa = jbig2dec_arith_test_main }, "jbig2_arith_test", "jbig2dec test" },
	{ {.fa = jbig2dec_huffman_test_main }, "jbig2_huffman_test", "jbig2dec test" },
	{ {.fa = jbig2dec_pbm2png_main }, "jbig2dec_pbm2png", "jbig2dec_pbm2png tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = tidy_tab2space_main }, "tidy_tab2space", "htmltidy::tab2space tool" },
	{ {.f = tidy_test71_main }, "tidy_test71", "htmltidy::test71 test" },
	{ {.fa = tidy_cli_main }, "htmltidy", "htmltidy CLI application" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = jbig2enc_main }, "jbig2enc", "jbig2enc tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = cwebp_main }, "cwebp", "cwebp tool" },
	{ {.fa = dwebp_main }, "dwebp", "dwebp tool" },
	{ {.fa = gif2webp_main }, "gif2webp", "gif2webp tool" },
	{ {.fa = img2webp_main }, "img2webp", "img2webp tool" },
	{ {.fa = vwebp_main }, "vwebp", "vwebp tool" },
	{ {.fa = vwebp_sdl_main }, "vwebp_sdl", "vwebp_sdl tool" },
	{ {.fa = webp_anim_diff_main }, "webp_anim_diff", "webp: webp_anim_diff tool" },
	{ {.fa = webp_anim_dump_main }, "webp_anim_dump", "webp_anim_dump tool" },
	{ {.fa = webp_get_disto_main }, "webp_get_disto", "webp_get_disto tool" },
	{ {.fa = webp_quality_main }, "webp_quality", "webp_quality tool" },
	{ {.fa = webpinfo_main }, "webpinfo", "webpinfo tool" },
	{ {.fa = webpmux_main }, "webpmux", "webpmux tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = gif2rgb_main }, "gif2rgb", "libgif's gif2rgb tool" },
	{ {.fa = gifbg_main }, "gifbg", "libgif's gifbg tool" },
	{ {.fa = gifbuild_main }, "gifbuild", "libgif's gifbuild tool" },
	{ {.fa = gifclrmp_main }, "gifclrmp", "libgif's gifclrmp tool" },
	{ {.fa = gifcolor_main }, "gifcolor", "libgif's gifcolor tool" },
	{ {.fa = gifecho_main }, "gifecho", "libgif's gifecho tool" },
	{ {.fa = giffilter_main }, "giffilter", "libgif's giffilter tool" },
	{ {.fa = giffix_main }, "giffix", "libgif's giffix tool" },
	{ {.fa = gifhisto_main }, "gifhisto", "libgif's gifhisto tool" },
	{ {.fa = gifinto_main }, "gifinto", "libgif's gifinto tool" },
	{ {.fa = gifsponge_main }, "gifsponge", "libgif's gifsponge tool" },
	{ {.fa = giftext_main }, "giftext", "libgif's giftext tool" },
	{ {.fa = giftool_main }, "giftool", "libgif's giftool tool" },
	{ {.fa = gifwedge_main }, "gifwedge", "libgif's gifwedge tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = pngcrush_main }, "pngcrush", "pngcrush tool" },
	{ {.fa = pngmeta_main }, "pngmeta", "pngmeta tool" },
	{ {.fa = pngzop_zlib_to_idat_main }, "pngzop_zlib_to_idat", "pngzop_zlib_to_idat tool" },
	{ {.fa = pngidat_main }, "pngidat", "pngidat tool" },
	{ {.fa = pngiend_main }, "pngiend", "pngiend tool" },
	{ {.fa = pngihdr_main }, "pngihdr", "pngihdr tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = charter_svg_main }, "charter_svg", "charter_svg tool" },
	{ {.fa = charter_tex_main }, "charter_tex", "charter_tex tool" },

	{ {.fa = smartypants_main }, "smartypants", "smartypants tool" },
	{ {.fa = upskirt_main }, "upskirt", "upskirt tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = nanosvg_example1_main }, "nanosvg_example1", "nanosvg example1 demo" },
	{ {.f = nanosvg_example2_main }, "nanosvg_example2", "nanosvg example2 demo" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = qjs_main }, "qjs", "qjs (QuickJS) tool" },
	{ {.fa = qjsc_main }, "qjsc", "qjsc (QuickJS Compiler) tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = qjs_main }, "qjs", "qjs (QuickJS) tool" },
	{ {.fa = qjsc_main }, "qjsc", "qjsc (QuickJS Compiler) tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = pthw32_all_tests_main }, "pthw32_all_tests", "pthred-win32 test runner tool" },
#endif
	
#if defined(MUTOOL_EX)
	{ {.fa = qjsd_main }, "qjsd", "qjsd (Execute compiled QuickJS function / binary code blob) tool" },
	{ {.fa = qjscompress_main }, "qjscompress", "qjscompress tool" },
	{ {.fa = qjs_unicode_gen_main }, "qjs_unicode_gen", "qjs_unicode_gen tool" },
	{ {.fa = qjs_test262_main }, "qjs_test262", "qjs_test262 conformance test tool" },
	{ {.fa = qjs_sample_app_main }, "qjs_sample_app", "quickjs_sample_app tool" },
	{ {.fa = qjs_benchmark_main }, "qjs_benchmark", "quickjs benchmark tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = qjscpp_main }, "qjscpp", "qjscpp tool" },
	{ {.f = qjscpp_class_test_main }, "qjscpp_class_test", "qjscpp_class_test tool" },
	{ {.f = qjscpp_conversions_test_main }, "qjscpp_conversions_test", "qjscpp_conversions_test tool" },
	{ {.f = qjscpp_example_main }, "qjscpp_example", "qjscpp_example tool" },
	{ {.f = qjscpp_exception_test_main }, "qjscpp_exception_test", "qjscpp_exception_test tool" },
	{ {.f = qjscpp_function_call_test_main }, "qjscpp_function_call_test", "qjscpp_function_call_test tool" },
	{ {.f = qjscpp_inheritance_test_main }, "qjscpp_inheritance_test", "qjscpp_inheritance_test tool" },
	{ {.f = qjscpp_jobs_test_main }, "qjscpp_jobs_test", "qjscpp_jobs_test tool" },
	{ {.f = qjscpp_module_loader_test_main }, "qjscpp_module_loader_test", "qjscpp_module_loader_test tool" },
	{ {.f = qjscpp_multicontext_test_main }, "qjscpp_multicontext_test", "qjscpp_multicontext_test tool" },
	{ {.f = qjscpp_point_test_main }, "qjscpp_point_test", "qjscpp_point_test tool" },
	{ {.f = qjscpp_unhandled_rejection_test_main }, "qjscpp_unhandled_rejection_test", "qjscpp_unhandled_rejection_test tool" },
	{ {.f = qjscpp_value_test_main }, "qjscpp_value_test", "qjscpp_value_test tool" },
	{ {.f = qjscpp_variant_test_main }, "qjscpp_variant_test", "qjscpp_variant_test tool" },
#endif

#if defined(MUTOOL_EX)
	//{ {.fa = qjscpp_async_example_main }, "qjscpp_async_example", "qjscpp_async_example tool" },
	{ {.fa = qjscpp_classes_example_main }, "qjscpp_classes_example", "qjscpp_classes_example tool" },
	{ {.fa = qjscpp_closures_example_main }, "qjscpp_closures_example", "qjscpp_closures_example tool" },
	{ {.fa = qjscpp_exception_example_main }, "qjscpp_exception_example", "qjscpp_exception_example tool" },
	{ {.fa = qjscpp_simple_example_main }, "qjscpp_simple_example", "qjscpp_simple_example tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = zopfli_main }, "zopfli", "zopfli tool" },
	{ {.fa = zopflipng_main }, "zopflipng", "zopflipng tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = brotli_main }, "brotli", "brotli tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = dcmtk_cda2dcm_main }, "dcmtk_cda2dcm", "dcmtk_cda2dcm tool" },
	{ {.fa = dcmtk_dcm2cda_main }, "dcmtk_dcm2cda", "dcmtk_dcm2cda tool" },
	{ {.fa = dcmtk_dcm2img_main }, "dcmtk_dcm2img", "dcmtk_dcm2img tool" },
	{ {.fa = dcmtk_dcm2json_main }, "dcmtk_dcm2json", "dcmtk_dcm2json tool" },
	{ {.fa = dcmtk_dcm2pdf_main }, "dcmtk_dcm2pdf", "dcmtk_dcm2pdf tool" },
	{ {.fa = dcmtk_dcm2pnm_main }, "dcmtk_dcm2pnm", "dcmtk_dcm2pnm tool" },
	{ {.fa = dcmtk_dcm2xml_main }, "dcmtk_dcm2xml", "dcmtk_dcm2xml tool" },
	{ {.fa = dcmtk_dcmcjpeg_main }, "dcmtk_dcmcjpeg", "dcmtk_dcmcjpeg tool" },
	{ {.fa = dcmtk_dcmcjpls_main }, "dcmtk_dcmcjpls", "dcmtk_dcmcjpls tool" },
	{ {.fa = dcmtk_dcmconv_main }, "dcmtk_dcmconv", "dcmtk_dcmconv tool" },
	{ {.fa = dcmtk_dcmcrle_main }, "dcmtk_dcmcrle", "dcmtk_dcmcrle tool" },
	{ {.fa = dcmtk_dcmdata_tests_main }, "dcmtk_dcmdata_tests", "dcmtk_dcmdata_tests tool" },
	{ {.fa = dcmtk_dcmdjpeg_main }, "dcmtk_dcmdjpeg", "dcmtk_dcmdjpeg tool" },
	{ {.fa = dcmtk_dcmdjpls_main }, "dcmtk_dcmdjpls", "dcmtk_dcmdjpls tool" },
	{ {.fa = dcmtk_dcmdrle_main }, "dcmtk_dcmdrle", "dcmtk_dcmdrle tool" },
	{ {.fa = dcmtk_dcmdspfn_main }, "dcmtk_dcmdspfn", "dcmtk_dcmdspfn tool" },
	{ {.fa = dcmtk_dcmdump_main }, "dcmtk_dcmdump", "dcmtk_dcmdump tool" },
	{ {.fa = dcmtk_dcmect_tests_main }, "dcmtk_dcmect_tests", "dcmtk_dcmect_tests tool" },
	{ {.fa = dcmtk_dcmfg_tests_main }, "dcmtk_dcmfg_tests", "dcmtk_dcmfg_tests tool" },
	{ {.fa = dcmtk_dcmftest_main }, "dcmtk_dcmftest", "dcmtk_dcmftest tool" },
	{ {.fa = dcmtk_dcmgpdir_main }, "dcmtk_dcmgpdir", "dcmtk_dcmgpdir tool" },
	{ {.fa = dcmtk_dcmicmp_main }, "dcmtk_dcmicmp", "dcmtk_dcmicmp tool" },
	{ {.fa = dcmtk_dcmiod_tests_main }, "dcmtk_dcmiod_tests", "dcmtk_dcmiod_tests tool" },
	{ {.fa = dcmtk_dcmj2pnm_main }, "dcmtk_dcmj2pnm", "dcmtk_dcmj2pnm tool" },
	{ {.fa = dcmtk_dcml2pnm_main }, "dcmtk_dcml2pnm", "dcmtk_dcml2pnm tool" },
	{ {.fa = dcmtk_dcmmkcrv_main }, "dcmtk_dcmmkcrv", "dcmtk_dcmmkcrv tool" },
	{ {.fa = dcmtk_dcmmkdir_main }, "dcmtk_dcmmkdir", "dcmtk_dcmmkdir tool" },
	{ {.fa = dcmtk_dcmmklut_main }, "dcmtk_dcmmklut", "dcmtk_dcmmklut tool" },
	{ {.fa = dcmtk_dcmnet_tests_main }, "dcmtk_dcmnet_tests", "dcmtk_dcmnet_tests tool" },
	{ {.fa = dcmtk_dcmodify_main }, "dcmtk_dcmodify", "dcmtk_dcmodify tool" },
	{ {.fa = dcmtk_dcmp2pgm_main }, "dcmtk_dcmp2pgm", "dcmtk_dcmp2pgm tool" },
	{ {.fa = dcmtk_dcmprscp_main }, "dcmtk_dcmprscp", "dcmtk_dcmprscp tool" },
	{ {.fa = dcmtk_dcmprscu_main }, "dcmtk_dcmprscu", "dcmtk_dcmprscu tool" },
	{ {.fa = dcmtk_dcmpschk_main }, "dcmtk_dcmpschk", "dcmtk_dcmpschk tool" },
	{ {.fa = dcmtk_dcmpsmk_main }, "dcmtk_dcmpsmk", "dcmtk_dcmpsmk tool" },
	{ {.fa = dcmtk_dcmpsprt_main }, "dcmtk_dcmpsprt", "dcmtk_dcmpsprt tool" },
	{ {.fa = dcmtk_dcmpsrcv_main }, "dcmtk_dcmpsrcv", "dcmtk_dcmpsrcv tool" },
	{ {.fa = dcmtk_dcmpssnd_main }, "dcmtk_dcmpssnd", "dcmtk_dcmpssnd tool" },
	{ {.fa = dcmtk_dcmqridx_main }, "dcmtk_dcmqridx", "dcmtk_dcmqridx tool" },
	{ {.fa = dcmtk_dcmqrscp_main }, "dcmtk_dcmqrscp", "dcmtk_dcmqrscp tool" },
	{ {.fa = dcmtk_dcmqrti_main }, "dcmtk_dcmqrti", "dcmtk_dcmqrti tool" },
	{ {.fa = dcmtk_dcmquant_main }, "dcmtk_dcmquant", "dcmtk_dcmquant tool" },
	{ {.fa = dcmtk_dcmrecv_main }, "dcmtk_dcmrecv", "dcmtk_dcmrecv tool" },
	{ {.fa = dcmtk_dcmrt_tests_main }, "dcmtk_dcmrt_tests", "dcmtk_dcmrt_tests tool" },
	{ {.fa = dcmtk_dcmscale_main }, "dcmtk_dcmscale", "dcmtk_dcmscale tool" },
	{ {.fa = dcmtk_dcmseg_tests_main }, "dcmtk_dcmseg_tests", "dcmtk_dcmseg_tests tool" },
	{ {.fa = dcmtk_dcmsend_main }, "dcmtk_dcmsend", "dcmtk_dcmsend tool" },
	{ {.fa = dcmtk_dcmsign_main }, "dcmtk_dcmsign", "dcmtk_dcmsign tool" },
	{ {.fa = dcmtk_dcmsign_main }, "dcmtk_dcmsign", "dcmtk_dcmsign tool" },
	{ {.fa = dcmtk_dcmsr_tests_main }, "dcmtk_dcmsr_tests", "dcmtk_dcmsr_tests tool" },
	{ {.fa = dcmtk_dcmtls_tests_main }, "dcmtk_dcmtls_tests", "dcmtk_dcmtls_tests tool" },
	{ {.fa = dcmtk_dcod2lum_main }, "dcmtk_dcod2lum", "dcmtk_dcod2lum tool" },
	{ {.fa = dcmtk_dconvlum_main }, "dcmtk_dconvlum", "dcmtk_dconvlum tool" },
	{ {.fa = dcmtk_drtdump_main }, "dcmtk_drtdump", "dcmtk_drtdump tool" },
	{ {.f = dcmtk_drttest_main }, "dcmtk_drttest", "dcmtk_drttest tool" },
	{ {.fa = dcmtk_dsr2html_main }, "dcmtk_dsr2html", "dcmtk_dsr2html tool" },
	{ {.fa = dcmtk_dsr2xml_main }, "dcmtk_dsr2xml", "dcmtk_dsr2xml tool" },
	{ {.fa = dcmtk_dsrdump_main }, "dcmtk_dsrdump", "dcmtk_dsrdump tool" },
	{ {.fa = dcmtk_dump2dcm_main }, "dcmtk_dump2dcm", "dcmtk_dump2dcm tool" },
	{ {.fa = dcmtk_echoscu_main }, "dcmtk_echoscu", "dcmtk_echoscu tool" },
	{ {.fa = dcmtk_findscu_main }, "dcmtk_findscu", "dcmtk_findscu tool" },
	{ {.fa = dcmtk_getscu_main }, "dcmtk_getscu", "dcmtk_getscu tool" },
	{ {.fa = dcmtk_img2dcm_main }, "dcmtk_img2dcm", "dcmtk_img2dcm tool" },
	{ {.fa = dcmtk_mkdeftag_main }, "dcmtk_mkdeftag", "dcmtk_mkdeftag tool" },
	{ {.fa = dcmtk_mkdictbi_main }, "dcmtk_mkdictbi", "dcmtk_mkdictbi tool" },
	{ {.fa = dcmtk_mkreport_main }, "dcmtk_mkreport", "dcmtk_mkreport tool" },
	{ {.fa = dcmtk_movescu_main }, "dcmtk_movescu", "dcmtk_movescu tool" },
	{ {.fa = dcmtk_msgserv_main }, "dcmtk_msgserv", "dcmtk_msgserv tool" },
	{ {.fa = dcmtk_oficonv_tests_main }, "dcmtk_oficonv_tests", "dcmtk_oficonv_tests tool" },
	{ {.fa = dcmtk_ofstd_tests_main }, "dcmtk_ofstd_tests", "dcmtk_ofstd_tests tool" },
	{ {.fa = dcmtk_pdf2dcm_main }, "dcmtk_pdf2dcm", "dcmtk_pdf2dcm tool" },
	{ {.fa = dcmtk_stl2dcm_main }, "dcmtk_stl2dcm", "dcmtk_stl2dcm tool" },
	{ {.fa = dcmtk_storescp_main }, "dcmtk_storescp", "dcmtk_storescp tool" },
	{ {.fa = dcmtk_storescu_main }, "dcmtk_storescu", "dcmtk_storescu tool" },
	{ {.fa = dcmtk_termscu_main }, "dcmtk_termscu", "dcmtk_termscu tool" },
	{ {.fa = dcmtk_wlmscpfs_main }, "dcmtk_wlmscpfs", "dcmtk_wlmscpfs tool" },
	{ {.fa = dcmtk_wltest_main }, "dcmtk_wltest", "dcmtk_wltest tool" },
	{ {.fa = dcmtk_xml2dcm_main }, "dcmtk_xml2dcm", "dcmtk_xml2dcm tool" },
	{ {.fa = dcmtk_xml2dsr_main }, "dcmtk_xml2dsr", "dcmtk_xml2dsr tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = half_test11_main }, "half_test11", "half_test11 test" },
	{ {.f = half_test03_main }, "half_test03", "half_test03 test" },
	{ {.f = half_test03b_main }, "half_test03b", "half_test03b test" },
	{ {.f = half_performance_test_main }, "half_performance_test", "half performance test tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = deflate_benchmark_main }, "deflate_benchmark", "deflate_benchmark tool" },
	{ {.fa = deflate_checksum_main }, "deflate_checksum", "deflate_checksum tool" },
	{ {.fa = deflate_gzip_main }, "deflate_gzip", "deflate_gzip tool" },
	{ {.fa = deflate_test_checksums_main }, "deflate_test_checksums", "deflate_test_checksums tool" },
	{ {.fa = deflate_test_custom_malloc_main }, "deflate_test_custom_malloc", "deflate_test_custom_malloc tool" },
	{ {.fa = deflate_test_incomplete_codes_main }, "deflate_test_incomplete_codes", "deflate_test_incomplete_codes tool" },
	{ {.fa = deflate_test_invalid_streams_main }, "deflate_test_invalid_streams", "deflate_test_invalid_streams tool" },
	{ {.fa = deflate_test_litrunlen_overflow_main }, "deflate_test_litrunlen_overflow", "deflate_test_litrunlen_overflow tool" },
	{ {.fa = deflate_test_overread_main }, "deflate_test_overread", "deflate_test_overread tool" },
	{ {.fa = deflate_test_slow_decompression_main }, "deflate_test_slow_decompression", "deflate_test_slow_decompression tool" },
	{ {.fa = deflate_test_trailing_byte_main }, "deflate_test_trailing_byte", "deflate_test_trailing_byte tool" },
	{ {.f = deflate_gen_crc32_multipliers_main }, "deflate_gen_crc32_multipliers", "deflate_gen_crc32_multipliers tool" },
	{ {.f = deflate_gen_crc32_tables_main }, "deflate_gen_crc32_tables", "deflate_gen_crc32_tables tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = OpenEXR_b44ExpLogTable_tool_main }, "EXR_b44ExpLogTable_tool", "OpenEXR_b44ExpLogTable_tool tool" },
	{ {.fa = OpenEXR_deepIdSelect_example_main }, "EXR_deepIdSelect_example", "OpenEXR_deepIdSelect_example tool" },
	{ {.fa = OpenEXR_deepId_example_main }, "EXR_deepId_example", "OpenEXR_deepId_example tool" },
	{ {.fa = OpenEXR_examples_main }, "EXR_examples", "OpenEXR_examples tool" },
	{ {.fa = OpenEXR_exr2aces_main }, "EXR_exr2aces", "OpenEXR_exr2aces tool" },
	{ {.fa = OpenEXR_exrcheck_main }, "EXR_exrcheck", "OpenEXR_exrcheck tool" },
	{ {.fa = OpenEXR_exrenvmap_main }, "EXR_exrenvmap", "OpenEXR_exrenvmap tool" },
	{ {.fa = OpenEXR_exrheader_main }, "EXR_exrheader", "OpenEXR_exrheader tool" },
	{ {.fa = OpenEXR_exrinfo_main }, "EXR_exrinfo", "OpenEXR_exrinfo tool" },
	{ {.fa = OpenEXR_exrmakepreview_main }, "EXR_exrmakepreview", "OpenEXR_exrmakepreview tool" },
	{ {.fa = OpenEXR_exrmaketiled_main }, "EXR_exrmaketiled", "OpenEXR_exrmaketiled tool" },
	{ {.fa = OpenEXR_exrmanifest_main }, "EXR_exrmanifest", "OpenEXR_exrmanifest tool" },
	{ {.fa = OpenEXR_exrmetrics_main }, "EXR_exrmetrics", "OpenEXR_exrmetrics tool" },
	{ {.fa = OpenEXR_exrmultipart_main }, "EXR_exrmultipart", "OpenEXR_exrmultipart tool" },
	{ {.fa = OpenEXR_exrmultiview_main }, "EXR_exrmultiview", "OpenEXR_exrmultiview tool" },
	{ {.fa = OpenEXR_exrstdattr_main }, "EXR_exrstdattr", "OpenEXR_exrstdattr tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = cpuinfo_auxv_dump_main }, "cpuinfo_auxv_dump", "cpuinfo_auxv_dump tool" },
	{ {.fa = cpuinfo_cache_info_main }, "cpuinfo_cache_info", "cpuinfo_cache_info tool" },
	{ {.fa = cpuinfo_cpu_info_main }, "cpuinfo_cpu_info", "cpuinfo_cpu_info tool" },
	{ {.fa = cpuinfo_cpuid_dump_main }, "cpuinfo_cpuid_dump", "cpuinfo_cpuid_dump tool" },
	{ {.fa = cpuinfo_dump_main }, "cpuinfo_dump", "cpuinfo_dump tool" },
	{ {.fa = cpuinfo_gnu_dump_main }, "cpuinfo_gnu_dump", "cpuinfo_gnu_dump tool" },
	{ {.fa = cpuinfo_isa_info_main }, "cpuinfo_isa_info", "cpuinfo_isa_info tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = editdist_test_main }, "editdist_test", "edit-distance test tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = xsimd_benchmark_main }, "xsimd_benchmark", "xsimd benchmark tool" },
	{ {.f = xsimd_sample_mandelbrot_main }, "xsimd_sample_mandelbrot", "xsimd xsimd mandelbrot example" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = jpegXL_decode_oneshot_main }, "jxl_decode_oneshot", "jxl_decode_oneshot tool" },
	{ {.fa = jpegXL_encode_oneshot_main }, "jxl_encode_oneshot", "jxl_encode_oneshot tool" },
	{ {.fa = jpegXL_info_main }, "jxl_info", "jxl_info: jpegXL tool" },
	{ {.fa = jpegXL_decode_EXIF_metadata_main }, "jpegXL_decode_EXIF_metadata", "jpegXL_decode_EXIF_metadata: jpegXL tool" },
	{ {.fa = jpegXL_decode_progressive_main }, "jpegXL_decode_progressive", "jpegXL_decode_progressive: jpegXL tool" },
	{ {.fa = jpegXL_butteraugli_main }, "jxl_butteraugli", "jxl_butteraugli tool" },
	{ {.fa = jpegXL_compress_main }, "jxl_compress", "jxl_compress tool" },
	{ {.fa = jpegXL_decompress_main }, "jxl_decompress", "jxl_decompress tool" },
	{ {.fa = jpegXL_dec_enc_main }, "jxl_dec_enc", "jxl_dec_enc tool" },
	{ {.fa = jpegXL_from_tree_main }, "jxl_from_tree", "jxl_from_tree tool" },
	{ {.fa = jpegXL_ssimulacra_main }, "jxl_ssimulacra tool", "jxl_ssimulacra tool" },
	{ {.fa = jpegXL_generate_LUT_template_main }, "jpegXL_generate_LUT_template", "jpegXL_generate_LUT_template tool" },
	{ {.fa = jpegXL_pq_to_hlg_main }, "jpegXL_pq_to_hlg", "jpegXL_pq_to_hlg tool" },
	{ {.fa = jpegXL_render_hlg_main }, "jpegXL_render_hlg", "jpegXL_render_hlg tool" },
	{ {.fa = jpegXL_texture_to_cube_main }, "jpegXL_texture_to_cube", "jpegXL_texture_to_cube tool" },
	{ {.fa = jpegXL_exr_to_pq_main }, "jpegXL_exr_to_pq", "jpegXL_exr_to_pq tool" },
	{ {.fa = jpegXL_from_tree_main }, "jpegXL_from_tree", "jpegXL_from_tree tool" },
	{ {.fa = jpegXL_ssimulacra2_main }, "jpegXL_ssimulacra2", "jpegXL_ssimulacra2 tool" },
	{ {.fa = jpegXL_tone_map_main }, "jpegXL_tone_map", "jpegXL_tone_map tool" },
	{ {.fa = jpegXL_local_tone_map_main }, "jpegXL_local_tone_map", "jpegXL_local_tone_map tool" },
	{ {.fa = jpegXL_icc_simplify_main }, "jpegXL_icc_simplify", "jpegXL_icc_simplify tool" },
	{ {.fa = jpegXL_cjpegli_main }, "jpegXL_cjpegli", "jpegXL_cjpegli tool" },
	{ {.fa = jpegXL_djepgli_main }, "jpegXL_djepgli", "jpegXL_djepgli tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = bibutils_bib2xml_main }, "bib2xml", "bibutils::bib2xml tool" },
	{ {.fa = bibutils_bibdiff_main }, "bibdiff", "bibutils::bibdiff tool" },
	{ {.fa = bibutils_biblatex2xml_main }, "biblatex2xml", "bibutils::biblatex2xml tool" },
	{ {.fa = bibutils_copac2xml_main }, "copac2xml", "bibutils::copac2xml tool" },
	{ {.fa = bibutils_ebi2xml_main }, "ebi2xml", "bibutils::ebi2xml tool" },
	{ {.fa = bibutils_end2xml_main }, "end2xml", "bibutils::end2xml tool" },
	{ {.fa = bibutils_endx2xml_main }, "endx2xml", "bibutils::endx2xml tool" },
	{ {.fa = bibutils_isi2xml_main }, "isi2xml", "bibutils::isi2xml tool" },
	{ {.fa = bibutils_med2xml_main }, "med2xml", "bibutils::med2xml tool" },
	{ {.fa = bibutils_modsclean_main }, "bibutils_modsclean", "bibutils::modsclean tool" },
	{ {.fa = bibutils_nbib2xml_main }, "nbib2xml", "bibutils::nbib2xml tool" },
	{ {.fa = bibutils_ris2xml_main }, "ris2xml", "bibutils::ris2xml tool" },
	{ {.fa = bibutils_wordbib2xml_main }, "wordbib2xml", "bibutils::wordbib2xml tool" },
	{ {.fa = bibutils_xml2ads_main }, "xml2ads", "bibutils::xml2ads tool" },
	{ {.fa = bibutils_xml2bib_main }, "xml2bib", "bibutils::xml2bib tool" },
	{ {.fa = bibutils_xml2biblatex_main }, "xml2biblatex", "bibutils::xml2biblatex tool" },
	{ {.fa = bibutils_xml2end_main }, "xml2end", "bibutils::xml2end tool" },
	{ {.fa = bibutils_xml2isi_main }, "xml2isi", "bibutils::xml2isi tool" },
	{ {.fa = bibutils_xml2nbib_main }, "xml2nbib", "bibutils::xml2nbib tool" },
	{ {.fa = bibutils_xml2ris_main }, "xml2ris", "bibutils::xml2ris tool" },
	{ {.fa = bibutils_xml2wordbib_main }, "xml2wordbib", "bibutils::xml2wordbib tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = bibutils_hash_bu_main }, "bibutils_hash_bu", "bibutils::hash_bu tool" },
	{ {.fa = bibutils_hash_marc_main }, "bibutils_hash_marc", "bibutils::hash_marc tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = bibutils_buauth_test_main }, "bibutils_buauth_test", "bibutils::buauth_test tool" },
	{ {.f = bibutils_doi_test_main }, "bibutils_doi_test", "bibutils::doi_test tool" },
	{ {.f = bibutils_entities_test_main }, "bibutils_entities_test", "bibutils::entities_test tool" },
	{ {.fa = bibutils_fields_test_main }, "bibutils_fields_test", "bibutils::fields_test tool" },
	{ {.f = bibutils_intlist_test_main }, "bibutils_intlist_test", "bibutils::intlist_test tool" },
	{ {.fa = bibutils_marcauth_test_main }, "bibutils_marcauth_test", "bibutils::marcauth_test tool" },
	{ {.f = bibutils_slist_test_main }, "bibutils_slist_test", "bibutils::slist_test tool" },
	{ {.f = bibutils_str_test_main }, "bibutils_str_test", "bibutils::str_test tool" },
	{ {.fa = bibutils_test_main }, "bibutils_test", "bibutils::test tool" },
	{ {.f = bibutils_utf8_test_main }, "bibutils_utf8_test", "bibutils::utf8_test tool" },
	{ {.f = bibutils_vplist_test_main }, "bibutils_vplist_test", "bibutils::vplist_test tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = BLAKE3_example_main }, "BLAKE3_example", "BLAKE3_example tool" },
	{ {.fa = BLAKE3_demo_main }, "BLAKE3_demo", "BLAKE3_demo tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = uchardet_tool_main }, "uchardet", "uchardet tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = chaos_benchmark_graph_main }, "chaos_benchmark_graph", "chaos benchmark_graph tool" },
	{ {.fa = chaos_file_checksum_main }, "chaos_file_checksum", "chaos file_checksum tool" },
	{ {.f = chaos_machine_interface_main }, "chaos_machine_interface", "chaos machine_interface tool" },
	{ {.f = chaos_interface_main }, "chaos_interface", "chaos interface tool" },
	{ {.f = chaos_long_period_urandom_main }, "chaos_long_period_urandom", "chaos long_period_urandom tool" },
	{ {.f = chaos_normal_dist_diagram_main }, "chaos_normal_dist_diagram", "chaos normal_dist_diagram tool" },
	{ {.f = chaos_tests_library_main }, "chaos_tests_library", "chaos tests_library tool" },
//	{ {.f = chaos_tests_testU01_main }, "chaos_tests_testU01", "chaos tests_testU01 tool" },
	{ {.f = chaos_truely_random_main }, "chaos_truely_random", "chaos truely_random tool" },
	{ {.fa = chaos_deep_main }, "chaos_deep", "chaos deep tool" },
	{ {.f = chaos_sandbox_main }, "chaos_sandbox", "chaos sandbox tool" },
//	{ {.f = chaos_testu01_main }, "chaos_testu01", "chaos testu01 tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = arch_bsdcat_main }, "bsdcat", "bsdcat tool" },
#if 0   // TODO: properly port the shar example (when we feel the need) to Win32/64 and then remove this condition right here...
	{ {.fa = arch_shar_main }, "shar", "shar tool" },
#endif
	{ {.fa = arch_untar_contrib_main }, "untar_alt", "libarchive's untar_contrib tool" },
	{ {.fa = arch_cpio_main }, "cpio", "cpio tool" },
	{ {.fa = arch_untar_main }, "untar", "untar tool" },
	{ {.fa = arch_minitar_main }, "minitar", "minitar tool" },
	{ {.fa = arch_tarfilter_main }, "tarfilter", "tarfilter tool" },
	{ {.fa = arch_bsdtar_main }, "bsdtar", "bsdtar tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = tvision_mmenu_main }, "tv_mmenu", "Turbo Vision mmenu demo/test/tool" },
	{ {.f = tvision_palette_test_main }, "tv_palette", "Turbo Vision palette demo/test/tool" },
	{ {.fa = tvision_tvdemo_main }, "tv_tvdemo", "Turbo Vision tvdemo demo/test/tool" },
	{ {.fa = tvision_tvdir_main }, "tv_tvdir", "Turbo Vision tvdir demo/test/tool" },
	{ {.fa = tvision_tvedit_main }, "tv_tvedit", "Turbo Vision tvedit demo/test/tool" },
	{ {.f = tvision_genphone_main }, "tv_genphone", "Turbo Vision genphone demo/test/tool" },
	{ {.f = tvision_genparts_main }, "tv_genparts", "Turbo Vision genparts demo/test/tool" },
	{ {.f = tvision_tvforms_main }, "tv_tvforms", "Turbo Vision tvforms demo/test/tool" },
	{ {.fa = tvision_tvhc_main }, "tv_tvhc", "Turbo Vision tvhc demo/test/tool" },
	{ {.f = tvision_hello_main }, "tv_hello", "Turbo Vision hello demo/test/tool" },
	{ {.f = tvision_geninc_main }, "tv_geninc", "Turbo Vision geninc demo/test/tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = gumbo_benchmark_main }, "gumbo_benchmark", "gumbo benchmark demo/test/tool" },
	{ {.fa = gumbo_clean_text_main }, "gumbo_clean_text", "gumbo clean_text demo/test/tool" },
	{ {.fa = gumbo_find_links_main }, "gumbo_find_links", "gumbo find_links demo/test/tool" },
	{ {.fa = gumbo_get_title_main }, "gumbo_get_title", "gumbo get_title demo/test/tool" },
	{ {.fa = gumbo_positions_of_class_main }, "gumbo_positions_of_class", "gumbo positions_of_class demo/test/tool" },
	{ {.fa = gumbo_prettyprint_main }, "gumbo_prettyprint", "gumbo prettyprint demo/test/tool" },
	{ {.fa = gumbo_serialize_main }, "gumbo_serialize", "gumbo serialize demo/test/tool" },
	{ {.fa = gumbo_eval_xpath_main }, "gumbo_eval_xpath", "gumbo eval_xpath demo/test/tool" },
	{ {.fa = gumbo_print_main }, "gumbo_print", "gumbo print demo/test/tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = gumboquery_example_main }, "gumboquery_example", "gumbo-query example" },
	{ {.f = gumboquery_test_main }, "gumboquery_test", "gumbo-query test" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = gumbo_libxml_example_main }, "gumbo_libxml_example", "gumbo-libxml example" },
#endif

#if 0 
#if defined(MUTOOL_EX)
	{ {.f = cppterminal_args_example_main }, "cppterm_args", "cpp-terminal args demo" },
	{ {.f = cppterminal_attach_console_example_main }, "cppterm_attach_console", "cpp-terminal attach_console demo" },
	{ {.f = cppterminal_attach_console_minimal_example_main }, "cppterm_attach_console_minimal", "cpp-terminal attach_console_minimal demo" },
	{ {.f = cppterminal_cin_cooked_example_main }, "cppterm_cin_cooked", "cpp-terminal cin_cooked demo" },
	{ {.f = cppterminal_cin_raw_example_main }, "cppterm_cin_raw", "cpp-terminal cin_raw demo" },
	{ {.f = cppterminal_colors_example_main }, "cppterm_colors", "cpp-terminal colors demo" },
	{ {.f = cppterminal_cout_example_main }, "cppterm_cout", "cpp-terminal cout demo" },
	{ {.f = cppterminal_events_example_main }, "cppterm_events", "cpp-terminal events demo" },
	{ {.f = cppterminal_keys_example_main }, "cppterm_keys", "cpp-terminal keys demo" },
	{ {.fa = cppterminal_kilo_example_main }, "cppterm_kilo", "cpp-terminal kilo demo" },
	{ {.f = cppterminal_menu_example_main }, "cppterm_menu", "cpp-terminal menu demo" },
	{ {.f = cppterminal_menu_window_example_main }, "cppterm_menu_window", "cpp-terminal menu_window demo" },
	{ {.f = cppterminal_minimal_example_main }, "cppterm_minimal", "cpp-terminal minimal demo" },
	{ {.f = cppterminal_prompt_immediate_example_main }, "cppterm_prompt_immediate", "cpp-terminal prompt_immediate demo" },
	{ {.f = cppterminal_prompt_multiline_example_main }, "cppterm_prompt_multiline", "cpp-terminal prompt_multiline demo" },
	{ {.f = cppterminal_prompt_not_immediate_example_main }, "cppterm_prompt_not_immediate", "cpp-terminal prompt_not_immediate demo" },
	{ {.f = cppterminal_prompt_simple_example_main }, "cppterm_prompt_simple", "cpp-terminal prompt_simple demo" },
	{ {.f = cppterminal_slow_events_example_main }, "cppterm_slow_events", "cpp-terminal slow_events demo" },
	{ {.f = cppterminal_styles_example_main }, "cppterm_styles", "cpp-terminal styles demo" },
	{ {.f = cppterminal_utf8_example_main }, "cppterm_utf8", "cpp-terminal utf8 demo" },
#endif
#endif

#if defined(MUTOOL_EX)
	{ {.fa = libbf_bench_main }, "bf_bench", "libbf benchmark tool" },
	{ {.fa = libbf_test_main }, "bf_test", "libbf test tool" },
	{ {.fa = libbf_tinypi_main }, "bf_tinypi", "libbf tiny PI demo" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = fph_bits_array_test_main }, "fph_bits_array_test", "fph bits array test" },
	{ {.f = fph_sample_test_main }, "fph_sample_test", "fph sample test" },
	{ {.f = fph_tests_main }, "fph_tests", "fph tests" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = cpptoml_build_toml_example_main }, "cpptoml_build_toml_example", "cpptoml build_toml example" },
	{ {.fa = cpptoml_parse_example_main }, "cpptoml_parse_example", "cpptoml parse example" },
	{ {.f = cpptoml_conversions_example_main }, "cpptoml_conversions_example", "cpptoml conversions example" },
	{ {.f = cpptoml_parse_stdin_example_main }, "cpptoml_parse_stdin_example", "cpptoml parse_stdin example" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = fs_dir_main }, "dir", "fs::dir tool" },
	{ {.fa = fs_du_main }, "du", "fs::du tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = glob_standalone_main }, "glob", "fs::glob tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = dirent_cat_main }, "dirent_cat", "dirent:cat example" },
	{ {.fa = dirent_dir_main }, "dirent_dir", "dirent:dir example" },
	{ {.fa = dirent_du_main }, "dirent_du", "dirent:du example" },
	{ {.fa = dirent_example_lookup_main }, "dirent_example_lookup", "dirent:example_lookup example" },
	{ {.fa = dirent_find_main }, "dirent_find", "dirent:find example" },
	{ {.fa = dirent_ls_main }, "dirent_ls", "dirent:ls example" },
	{ {.fa = dirent_scandir_main }, "dirent_scandir", "dirent:scandir example" },
	{ {.fa = dirent_updatedb_main }, "dirent_updatedb", "dirent:updatedb example" },

	{ {.f = dirent_compile_test_main }, "dirent_compile_test", "dirent: compile test" },
	{ {.f = dirent_cpp_test_main }, "dirent_cpp_test", "dirent: cpp test" },
	{ {.f = dirent_dirent_test_main }, "dirent_dirent_test", "dirent: dirent test" },
	{ {.f = dirent_scandir_test_main }, "dirent_scandir_test", "dirent: scandir test" },
	{ {.f = dirent_strverscmp_test_main }, "dirent_strverscmp_test", "dirent: strverscmp test" },
	{ {.f = dirent_telldir_test_main }, "dirent_telldir_test", "dirent: telldir test" },
	{ {.f = dirent_unicode_test_main }, "dirent_unicode_test", "dirent: unicode test" },
	{ {.f = dirent_utf8_test_main }, "dirent_utf8_test", "dirent: utf8 test" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = crow_example_basic_main }, "crow_basic", "crow example: basic" },
	{ {.f = crow_example_catch_all_main }, "crow_catch_all", "crow example: catch_all" },
	{ {.f = crow_example_chat_main }, "crow_chat", "crow example: chat" },
	{ {.f = crow_example_compression_main }, "crow_compression", "crow example: compression" },
	{ {.f = crow_example_json_map_main }, "crow_json_map", "crow example: json_map" },
	{ {.f = crow_example_static_file_main }, "crow_static_file", "crow example: static_file" },
	{ {.f = crow_example_vs_main }, "crow_vs", "crow example: vs" },
	{ {.f = crow_example_with_all_main }, "crow_with_all", "crow example: with_all" },
	{ {.f = crow_hello_world_main }, "crow_hello_world", "crow example: hello_world" },
	{ {.f = crow_example_ssl_main }, "crow_ssl", "crow example: ssl" },
	{ {.f = crow_example_ws_main }, "crow_ws", "crow example: ws" },
	{ {.f = crow_mustache_main }, "crow_mustache", "crow example: mustache" },
	{ {.f = crow_example_blueprint_main }, "crow_blueprint", "crow example: blueprint" },
	{ {.f = crow_example_middleware_main }, "crow_middleware", "crow example: middleware" },
	{ {.f = crow_test_multi_file_main }, "crow_multi_file", "crow example/test: multi_file" },
	{ {.f = crow_test_external_definition_main }, "crow_external_definition", "crow example/test: external_definition" },
	{ {.f = crow_example_session_main }, "crow_session", "crow example: session" },
	{ {.f = crow_example_file_upload_main }, "crow_file_upload", "crow example: file_upload" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = csv2_bench_main }, "csv2_bench", "libcsv2 benchmark tool" },
	{ {.fa = csv2_csv_count_main }, "csv_count", "libcsv2 csv_count row count tool" },
	{ {.fa = csv2_fieldcount_main }, "csv_fieldcount", "libcsv2 fieldcount tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = linenoise_example_main }, "linenoise_example", "linenoise example" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = replxx_c_api_main }, "replxx_c_api", "replxx C API demo" },
	{ {.fa = replxx_cpp_api_main }, "replxx_cpp_api", "replxx C++ API demo" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = spdlog_async_bench_main }, "spdlog_async_bench", "spdlog async benchmark tool" },
	{ {.fa = spdlog_bench_main }, "spdlog_bench", "spdlog main benchmark tool" },
	{ {.fa = spdlog_formatter_bench_main }, "spdlog_formatter_bench", "spdlog formatter benchmark tool" },
	{ {.fa = spdlog_latency_bench_main }, "spdlog_latency_bench", "spdlog latency benchmark tool" },
	{ {.fa = spdlog_example_main }, "spdlog_example", "spdlog example/demo" },
	{ {.fa = spdlog_example2_main }, "spdlog_ex2", "spdlog example/demo #2" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = lcms2_demo_cmyk_main }, "lcms2_demo_cmyk", "lcms2 demo_cmyk demo/tool" },
	{ {.f = lcms2_fast_float_testbed_main }, "lcms2_fast_float_testbed", "lcms2 fast_float_testbed demo/tool" },
	{ {.f = lcms2_threaded_testbed_main }, "lcms2_threaded_testbed", "lcms2 threaded_testbed demo/tool" },
	{ {.f = lcms2_alpha_test_main }, "lcms2_alpha_test", "lcms2 alpha_test demo/tool" },
	{ {.fa = lcms2_test_main }, "lcms2_test", "lcms2 test demo/tool" },
	{ {.fa = lcms2_jpgicc_util_main }, "lcms2_jpgicc_util", "lcms2 jpgicc_util demo/tool" },
	{ {.fa = lcms2_linkicc_util_main }, "lcms2_linkicc_util", "lcms2 linkicc_util demo/tool" },
	{ {.fa = lcms2_psicc_util_main }, "lcms2_psicc_util", "lcms2 psicc_util demo/tool" },
	//{ {.fa = lcms2_itufax_example_main }, "lcms2_itufax_example", "lcms2 itufax_example demo/tool" },
	//{ {.f = lcms2_mkcmy_example_main }, "lcms2_mkcmy_example", "lcms2 mkcmy_example demo/tool" },
	//{ {.fa = lcms2_mkgrayer_example_main }, "lcms2_mkgrayer_example", "lcms2 mkgrayer_example demo/tool" },
	{ {.fa = lcms2_mktiff8_example_main }, "lcms2_mktiff8_example", "lcms2 mktiff8_example demo/tool" },
	{ {.fa = lcms2_roundtrip_example_main }, "lcms2_roundtrip_example", "lcms2 roundtrip_example demo/tool" },
	{ {.fa = lcms2_vericc_example_main }, "lcms2_vericc_example", "lcms2 vericc_example demo/tool" },
	{ {.fa = lcms2_wtpt_example_main }, "lcms2_wtpt_example", "lcms2 wtpt_example demo/tool" },
	{ {.fa = lcms2_tiffdiff_util_main }, "lcms2_tiffdiff_util", "lcms2 tiffdiff_util demo/tool" },
	{ {.fa = lcms2_tificc_util_main }, "lcms2_tificc_util", "lcms2 tificc_util demo/tool" },
	{ {.fa = lcms2_transicc_util_main }, "transicc", "lcms2 transicc tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = xml_gio_bread_example_main }, "xml_gio_bread_example", "libxml gio_bread_example demo/tool" },
	//{ {.fa = xml_nanohttp_main }, "xml_nanohttp", "libxml nanohttp demo/tool" },
	{ {.fa = xml_runsuite_tests_main }, "xml_runsuite_tests", "libxml runsuite_tests demo/tool" },
	{ {.fa = xml_runtest_main }, "xml_runtest", "libxml runtest demo/tool" },
	{ {.fa = xml_runxmlconfig_main }, "xml_runxmlconfig", "libxml runxmlconfig demo/tool" },
	//{ {.fa = xml_schematron_main }, "xml_schematron", "libxml schematron demo/tool" },
	{ {.fa = xml_testapi_main }, "xml_testapi", "libxml testapi demo/tool" },
	{ {.fa = xml_testC14N_main }, "xml_testC14N", "libxml testC14N demo/tool" },
	{ {.f = xml_testdict_main }, "xml_testdict", "libxml testdict demo/tool" },
	{ {.fa = xml_testhtml_main }, "xml_testhtml", "libxml testhtml demo/tool" },
	{ {.fa = xml_testlimits_main }, "xml_testlimits", "libxml testlimits demo/tool" },
	{ {.fa = xml_testmodule_main }, "xml_testmodule", "libxml testmodule demo/tool" },
	{ {.fa = xml_testreader_main }, "xml_testreader", "libxml testreader demo/tool" },
	{ {.fa = xml_testrecurse_main }, "xml_testrecurse", "libxml testrecurse demo/tool" },
	{ {.fa = xml_testregexp_main }, "xml_testregexp", "libxml testregexp demo/tool" },
	{ {.fa = xml_testrelax_main }, "xml_testrelax", "libxml testrelax demo/tool" },
	{ {.fa = xml_testSAX_main }, "xml_testSAX", "libxml testSAX demo/tool" },
	{ {.fa = xml_testschemas_main }, "xml_testschemas", "libxml testschemas demo/tool" },
	{ {.fa = xml_testURI_main }, "xml_testURI", "libxml testURI demo/tool" },
	{ {.fa = xml_testXPath_main }, "xml_testXPath", "libxml testXPath demo/tool" },
	{ {.fa = xml_xmlcatalog_main }, "xmlcatalog", "libxml xmlcatalog demo/tool" },
	{ {.fa = xml_xmllint_main }, "xmllint", "libxml xmllint demo/tool" },
	//{ {.fa = xml_test_xmlreader_main }, "xml_test_xmlreader", "libxml test_xmlreader demo/tool" },
	{ {.f = xml_testchar_main }, "xml_testchar", "libxml testchar demo/tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.f = xslt_icu_sort_sample_main }, "xslt_icu_sort_sample", "libxslt icu_sort_sample demo" },
	{ {.fa = xslt_runtest_main }, "xslt_runtest", "libxslt runtest demo/tool" },
	{ {.f = xslt_test_threads_main }, "xslt_test_threads", "libxslt test_threads demo/tool" },
	{ {.fa = xsltproc_main }, "xsltproc", "libxslt xsltproc tool" },
#endif

	{ {.fa = report_version }, "version", "report version of this build / tools" },
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
namematch_partial(const char *end, const char *start, const char *match)
{
	if (!start)
		return 0;
	int len = (int)strlen(match);
	int w = (int)(end - start);
	if (w >= len)
		return 0;

	// viability factor: 
	// 0 = no match at all
	// >0 = partial match. Smaller value means better ~ partial has been found closer to the start of the name and is a bigger part of the entire name.
	int best_match_score = 0;
	for (int i = 0; i <= len - w; i++)
	{
		if (strnicmp(start, match + i, w) == 0)
		{
			// higher score is worse.
			// higher: more distant from command start/prefix --> score: 1 + i
			// higher: smaller part of entire command --> 1.0 - (w / len)    == (len - w) / len
			//
			// calculated in integer arithmetic (fixed point values: decimal point is at bit 4, i.e. factor 16)
			int score = (1 + i) + 16 * (len - w);
			// ROUND-UP while dividing so we'll be sure never to hit zero as a score value:
			score = (score + len - 1) / len;
			if (best_match_score == 0 || score < best_match_score)
				best_match_score = score;
		}
	}

	return best_match_score;
}

// Calculate a metric similar to an edit distance. Better is closer to zero.
// Returned value is always greater than 1(one).
//
// BTW: w applies to s1.
static int str_approx_match(const char* s1, const char* s2, int w)
{
	int score = 2; // just a value for when w == 0
	for (; w > 0 && *s1 && *s2; w--)
	{
		if (tolower(*s1) == tolower(*s2))
		{
			// match for this char. Do not worsen the score.
			score++;
			s1++;
			s2++;
			continue;
		}

		// edit-dist like check: can we match next char, or can next char match us?
		int sc1 = 0;
		int sc2 = 0;
		if (tolower(*s1) == tolower(s2[1]))
		{
			sc1 = str_approx_match(s1 + 1, s2 + 2, w - 1);
		}
		if (tolower(s1[1]) == tolower(*s2))
		{
			sc2 = 1 + str_approx_match(s1 + 2, s2 + 1, w - 2);
		}
		// if skip, pick the best score.
		// no skip? Then this is worse: ignore char.
		int scbest = 1 + str_approx_match(s1 + 1, s2 + 1, w - 1);
		if (sc1 > 0 && sc1 < scbest)
		{
			scbest = sc1;
		}
		if (sc2 > 0 && sc2 < scbest)
		{
			scbest = sc2;
		}
		w = 0;
		score += scbest;
		break;
	}

	// did we process the entire snippet or did we hit the EOS wall?
	score += 3 * w;
	return score;
}

static int
namematch_partial_approx(const char* end, const char* start, const char* match)
{
	if (!start)
		return 0;
	int len = (int)strlen(match);
	int w = (int)(end - start);
	if (w >= len)
		return 0;

	// viability factor: 
	// 0 = no match at all
	// >0 = partial match. Smaller value means better ~ partial has been found closer to the start of the name and is a bigger part of the entire name.
	int best_match_score = 0;
	for (int i = 0; i <= len - w; i++)
	{
		int approx = str_approx_match(start, match + i, w);
		if (approx < 3 + 2 * w)
		{
			// same score calculus as above, but with the added factor that this one is an INEXACT MATCH:
			// higher score is worse.
			// higher: more deviation / less near to current snippet --> score: approx
			// i.e. score is a factor of the cost per character, 1 for optimal.
			//
			// ~~higher: more distant from command start/prefix --> score: 1 + i~~
			// higher: smaller part of entire command --> 1.0 - (w / len)    == (len - w) / len
			//
			// calculated in integer arithmetic (fixed point values: decimal point is at bit 4, i.e. factor 16)
			int score = 121 + 16 * (len - w) * approx;
			// ROUND-UP while dividing so we'll be sure never to hit zero as a score value:
			score = (score + len - 1) / len;
			if (score > 81)
				continue;
			if (best_match_score == 0 || score < best_match_score)
				best_match_score = score;
		}
	}

	return best_match_score;
}


static int
report_version(int argc, const char** argv)
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
        fprintf(stderr, "version [option]\n"
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
        return EXIT_FAILURE;
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

static int tool_is_toplevel_ctx = 0;

static fz_context *ctx = NULL;

static void mu_drop_context(void)
{
    // WARNING: as we point `ctx` at the GLOBAL context in the app init phase, it MAY already be an INVALID
    // pointer reference by now!
    // 
    // WARNING: this assert fires when you run `mutool raster` (and probably other tools as well) and hit Ctrl+C
    // to ABORT/INTERRUPT the running application: the MSVC RTL calls this function in the atexit() handler
    // and the assert fires due to (ctx->error.top != ctx->error.stack).
    //
    // We are okay with that, as that scenario is an immediate abort anyway and the OS will be responsible
    // for cleaning up. That our fz_try/throw/catch exception stack hasn't been properly rewound at such times
    // is obvious, I suppose...
    ASSERT_AND_CONTINUE(!ctx || !fz_has_global_context() || (ctx->error.top == ctx->error.stack_base));

    if (tool_is_toplevel_ctx)
    {
        // as we registered a global context, we should clean the locks on it now
        // so the atexit handler won't have to bother with it.
        ASSERT_AND_CONTINUE(fz_has_global_context());
        ctx = fz_get_global_context();

		fz_clear_leptonica_mem(ctx);

        fz_drop_context_locks(ctx);
    }

    ctx = NULL;

    if (tool_is_toplevel_ctx)
    {
        fz_drop_global_context();

#ifndef DISABLE_MUTHREADS
        fin_mudraw_locks();
#endif /* DISABLE_MUTHREADS */

        tool_is_toplevel_ctx = 0;
    }
}

static void usage_mutool_options(void)
{
	fz_info(ctx, "\n\
mutool-options:\n\
\n\
-t     measure time elapsed and report at exit\n\
-d     report heap memory leakage for invoked tool\n\
-z     report total heap leakage at exit\n\
-v     print tool version number and exit\n\
-h     print usage help and exit\n\
\n");
}

static uint16_t namehash(const char* name)
{
	uint32_t h = 1;
	for (; *name; name++) {
		h *= 4211;			// prime
		uint8_t c = *name;
		h += c + 1;
		h ^= h >> 17;
	}

	// fold:
	uint32_t h2 = h >> 11;     // 2048
	h ^= h2;
	h2 >>= 11;
	h ^= h2;
	h2 >>= 11;
	h ^= h2;
	h &= 2048 - 1;

	return h;
}

// check to make sure the registered tool names are all unique!
static int sanitycheck_tools_table(void)
{
	uint16_t checklist[2048] = { 0 };
	for (int i = 0; i < (int)nelem(tools); i++) {
		const char* name = tools[i].name;
		uint16_t h = namehash(name);
		if (!checklist[h]) {
			checklist[h] = i + 1;
		}
		else {
			// potential collision!
			for (int j = 1; j < 2048; j++) {
				int idx = (i + j);
				idx &= 2048 - 1;
				if (!checklist[idx]) {
					// empty slot, no duplicate: plug it in.
					checklist[idx] = i + 1;
					break;
				}
				else if (!strcmp(tools[checklist[idx] - 1].name, name))
				{
					// collision!
					fz_error(ctx, "mutool_ex sanity check: tools name collision for name '%s'.\n", name);
					return 0;
				}
			}
		}
	}
	return 1;
}

/* Print usage */
static void usage(void)
{
	fz_info(ctx, "mutool version %s\n", FZ_VERSION);
	fz_info(ctx, "usage: mutool [mutool-options] <command> [command-options]\n");
	usage_mutool_options();
	fz_info(ctx, "\n\
Commands:\n\n");

	size_t max_tool_name_len = 0;
	for (int i = 0; i < (int)nelem(tools); i++)
		max_tool_name_len = fz_maxi(max_tool_name_len, strlen(tools[i].name));
	const char* leaderdots = " . . . . . . . . . . . . . . . . . . . . . . . . . . .";
	for (int i = 0; i < (int)nelem(tools); i++) {
		const char* name = tools[i].name;
		size_t name_len = strlen(tools[i].name);
		size_t lead = (max_tool_name_len - name_len - 4) & ~1; // print even number of leaderdots characters
		// ^^^ unsigned arithmetic so negative numbers are *huge* positive numbers instead!
		if (lead > max_tool_name_len)
			lead = 0;
		size_t width = 1 + max_tool_name_len - name_len - lead;
		fz_info(ctx, "\t%s%.*s%.*s -- %s", name, (int)width, "", (int)lead, leaderdots, tools[i].desc);
	}
}

static int run_tool(struct tool_spec *spec, int argc, const char **argv, int time_the_run, int show_heap_leakage, const char *app_argv0)
{
	void* snapshot = fz_TakeHeapSnapshot();
	char* exe_path = NULL;
	const char** argarr = (const char**)fz_malloc(ctx, (argc + 2) * sizeof(argarr[0]));

	// do NOT damage the original argv[] array any further: plugging in arbitrary strings in there
	// would cause memory corruption later on as we'll attempt to free() the elements at the end.
	//
	// So we make a local copy of the argv[] set and feed that one to the destination tool...
	switch (spec->keep_path)
	{
	default:
		argarr[0] = spec->name;
		break;

	case 1:
		const char *xp = app_argv0;
		const char* pe = fz_basename(xp);
		int plen = (pe - xp);
		exe_path = fz_asprintf(ctx, "%.*s%s%s", plen, xp, spec->name,
#if defined(_WIN32)
			".exe"
#else
			""
#endif
			);
		argarr[0] = exe_path;
		break;

	case 2:
		argarr[0] = app_argv0;
		break;
	}

	if (argc > 1)
		memcpy(argarr + 1, argv + 1, (argc - 1) * sizeof(argarr[0]));
	argarr[argc] = NULL;
	argarr[argc + 1] = NULL;

	argv = argarr;

	fflush(stdout);
	fflush(stderr);

	nanotimer_data_t timer;
	nanotimer(&timer);
	nanotimer_start(&timer);
	int rv = spec->func.fa(argc, argv);
	double dt = nanotimer_get_elapsed_ms(&timer);

	fz_free(ctx, exe_path);
	fz_free(ctx, argarr);

	fflush(stdout);
	fflush(stderr);

	if (show_heap_leakage)
	{
		fz_ReportHeapLeakageAgainstSnapshot(snapshot);
	}

	if (time_the_run)
	{
		fz_info(ctx, "### Elapsed time: %f milliseconds\n", dt);
	}

	fz_ReleaseHeapSnapshot(snapshot);
	return rv;
}

struct found_t
{
	int found;
	int exit_code;
};

static struct found_t find_and_exec_tool(const char *start, const char *end, int argc, const char **argv, const char *argv0, int time_the_run, int show_heap_leakage)
{
	char buf[64];

	for (int i = 0; i < (int)nelem(tools); i++)
	{
		// test for variants: mupdf<NAME>, mu<NAME> and <NAME>:
		strcpy(buf, "mupdf");
		strcat(buf, tools[i].name);
		assert(strlen(buf) < sizeof(buf));
		if (namematch(end, start, buf) || namematch(end, start, buf + 2))
		{
			struct found_t rv = {
				TRUE,
				run_tool(&tools[i], argc, argv, time_the_run, show_heap_leakage, argv0)
			};
			return rv;
		}
		strcpy(buf, "mu");
		strcat(buf, tools[i].name);
		assert(strlen(buf) < sizeof(buf));
		if (namematch(end, start, buf) || namematch(end, start, buf + 2))
		{
			struct found_t rv = {
				TRUE,
				run_tool(&tools[i], argc, argv, time_the_run, show_heap_leakage, argv0)
			};
			return rv;
		}
	}
	
	struct found_t rv = {
		FALSE,
		-1
	};
	return rv;
}

struct approx_score_elem_t
{
	int score;
	int index;
};

static struct found_t find_approx_and_exec_tool(const char *start, const char *end, int argc, const char **argv, const char *argv0, int time_the_run, int show_heap_leakage, struct approx_score_elem_t ambuous_name_set[])
{
	// When we get here, we know we only MAY have partial matches. Hence we first match against each of the known commands,
	// while calculating a distance metric (viability) for each.
	// Then we check if we have a single (unique) match and iff we do, we execute that command.
	// Otherwise, we report failure and return the viability score array for the caller to process further, probably 
	// using it to print a hint list for the user.
	int match_count = 0;
	int last_match_index = -1;
	for (int i = 0; i < (int)nelem(tools); i++)
	{
		int viability = namematch_partial(end, start, tools[i].name);
		ambuous_name_set[i].score = viability;
		if (viability > 0) 
		{
			match_count++;
			last_match_index = i;
		}
	}

	// Do we have a unique match after all?
	if (match_count == 1)
	{
		fz_info(ctx, "Execute %s (approximate-matched by the '%.*s' command):\n", tools[last_match_index].name, (int)(end - start), start);

		struct found_t rv = {
			TRUE,
			run_tool(&tools[last_match_index], argc, argv, time_the_run, show_heap_leakage, argv0)
		};
		return rv;
	}

	for (int i = 0; i < (int)nelem(tools); i++)
	{
		int viability = namematch_partial_approx(end, start, tools[i].name);
		if (viability > 0 && (ambuous_name_set[i].score == 0 || viability < ambuous_name_set[i].score))
		{
			ambuous_name_set[i].score = viability;
		}
		if (viability > 0)
		{
			match_count++;
			last_match_index = i;
		}
	}

	struct found_t rv = {
		FALSE,
		-1
	};
	return rv;
}

static int x_index_compare_elems(const struct approx_score_elem_t *a, const struct approx_score_elem_t *b)
{
	int as = a->score;
	int bs = b->score;
	if (as == 0)
		as = INT_MAX - 2;
	if (bs == 0)
		bs = INT_MAX - 2;
	return as - bs;
}

#ifdef GPERF
#include "gperftools/profiler.h"

static int profiled_main(int argc, const char** argv);

int main(int argc, const char** argv)
{
    int ret;
    ProfilerStart("mutool.prof");
    ret = profiled_main(argc, argv);
    ProfilerStop();
    return ret;
}

static int profiled_main(int argc, const char** argv)
#else
#ifndef MUTOOL_AS_BULKTEST_CMD
int main(int argc, const char** argv)
#else
int mutool_main(int argc, const char** argv)
#endif
#endif
{
    // reset global vars: this tool MAY be re-invoked via bulktest or others and should RESET completely between runs:
    //ctx = NULL;
    //mutool_is_toplevel_ctx = 0;

    if (!fz_has_global_context())
    {
        ASSERT(ctx == NULL);
        
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

        tool_is_toplevel_ctx = 1;
    }
    else
    {
        ctx = fz_get_global_context();
    }

	if (tool_is_toplevel_ctx)
	{
		// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
		fz_set_default_jpeg_sys_mem_mgr();

		fz_set_leptonica_mem(ctx);
	}

	atexit(mu_drop_context);

    if (argc == 0)
    {
        fz_error(ctx, "No command name found!");
        return EXIT_FAILURE;
    }
		if (!sanitycheck_tools_table())
		{
			return EXIT_FAILURE;
		}

	int argstart = 1;
	int time_the_run = 0;
	int show_heap_leakage = 0;

	fz_getopt_reset();

	// WARNING:
	// This works *assuming* fz_getopt() DOES NOT re-order argv[] so as to find
	// and process all '-xyz' options and leave a set of non-ption argv[] arguments
	// at the tail end of the argv[] array: GNU getopt() has that behaviour, but
	// that is undesirable right here: we *only* want to see and parse mutool
	// options, which *precede* the mutool *command*, which itself may be followed
	// by an arbitrary number of command-specific '-xyz' options: we MUST NOT
	// parse those in here!
	int c;
	while ((c = fz_getopt(argc, argv, "tdzvh")) != EOF)
	{
		switch (c)
		{
		case 't':
			time_the_run = 1;
			break;
		case 'd':
			show_heap_leakage = 1;
			break;
		case 'z':
			fz_TurnHeapLeakReportingAtProgramExitOn();
			break;
		case 'v':
			fz_info(ctx, "mutool version %s\n", FZ_VERSION);
			return EXIT_SUCCESS;
		case 'h':
			usage();
			return EXIT_SUCCESS;
		default:
			fz_error(ctx, "Unsupported mutool option %c (%s).\n", c, argv[fz_optind]);
			usage_mutool_options();
			return EXIT_FAILURE;
		}
	}

	argstart = fz_optind;

	// array of numbers which flag which commands are viable in the approximate-match case: non-zero values indicate "nearness" / partial-match.
	struct approx_score_elem_t ambuous_name_set[nelem(tools)] = {0};
	for (int i = 0; i < (int)nelem(tools); i++)
	{
		ambuous_name_set[i].index = i;
	}

	const char *start, *end;

	/* Check argv[0] */

    if (argc > 0)
    {
        start = fz_basename(argv[0]);
        end = start + strlen(start);
        if ((end-4 >= start) && (end[-4] == '.') && (end[-3] == 'e') && (end[-2] == 'x') && (end[-1] == 'e'))
            end = end-4;
		struct found_t rv = find_and_exec_tool(start, end, argc - argstart + 1, argv + argstart - 1, argv[0], time_the_run, show_heap_leakage);
		if (rv.found)
			return rv.exit_code;
    }

    /* Check argv[1] */

    if (argc > argstart)
    {
		start = argv[argstart];
		end = start + strlen(start);
		struct found_t rv = find_and_exec_tool(start, end, argc - argstart, argv + argstart, argv[0], time_the_run, show_heap_leakage);
		if (rv.found)
			return rv.exit_code;

		rv = find_approx_and_exec_tool(start, end, argc - argstart, argv + argstart, argv[0], time_the_run, show_heap_leakage, ambuous_name_set);
		if (rv.found)
			return rv.exit_code;
	}
	else
	{
		// only attempt to approximate-match basename(argv[0]) when there's no actual command given!
		start = fz_basename(argv[0]);
		end = start + strlen(start);
		if ((end-4 >= start) && (end[-4] == '.') && (end[-3] == 'e') && (end[-2] == 'x') && (end[-1] == 'e'))
			end = end-4;
		struct found_t rv = find_approx_and_exec_tool(start, end, argc - argstart + 1, argv + argstart - 1, argv[0], time_the_run, show_heap_leakage, ambuous_name_set);
		if (rv.found)
			return rv.exit_code;
	}

	qsort(ambuous_name_set, nelem(tools), sizeof(ambuous_name_set[0]), x_index_compare_elems);

	// Now, ambuous_name_set[] elements are sorted in SCORE order: best-to-worst.
	//
	// List these as suggestions to the user, iff there are any suggestions like that:
	if (ambuous_name_set[0].score > 0)
	{
		fz_error(ctx, "mutool: ambiguous / unrecognized command '%s'. Did you mean to invoke one of these:\n", argv[argstart]);
		for (int i = 0; i < (int)nelem(tools); i++)
		{
			struct approx_score_elem_t *p = &ambuous_name_set[i];
			if (p->score == 0)
				break;

			fz_info(ctx, "    %s\n", tools[p->index].name);
		}
		return EXIT_FAILURE;
	}

	if (argc > argstart)
	{
		fz_error(ctx, "mutool: unrecognized command '%s'. Run '%s -h' to see generic usage help.\n", argv[argstart], fz_basename(argv[0]));
		return EXIT_FAILURE;
	}

	fz_error(ctx, "mutool: no command given.  You must specify a command, or run '%s -h' to see generic usage help.\n", fz_basename(argv[0]));
    return EXIT_FAILURE;
}

#ifndef MUTOOL_AS_BULKTEST_CMD

#ifdef _MSC_VER
int wmain(int argc, const wchar_t *wargv[])
{
	fz_context* ctx = fz_get_global_context();
	const char **argv = fz_argv_from_wargv(ctx, argc, wargv);
    if (!argv)
        return EXIT_FAILURE;
    int ret = main(argc, argv);
    fz_free_argv(ctx, argc, argv);
    return ret;
}
#endif

#endif // MUTOOL_AS_BULKTEST_CMD

