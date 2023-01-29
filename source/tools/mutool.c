// Copyright (C) 2004-2023 Artifex Software, Inc.
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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

/*
 * mutool -- swiss army knife of pdf manipulation tools
 */

#include "mupdf/fitz.h"
#include "mupdf/mutool.h"
#include "mupdf/helpers/mu-threads.h"
#include "mupdf/helpers/jmemcust.h"

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
#include "../../thirdparty/owemdjee/libjpeg-turbo/monolithic_examples.h"
#include "../../thirdparty/owemdjee/pcre/src/monolithic_examples.h"
#include "../../thirdparty/owemdjee/bibutils/src/bin/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libzopfli/src/zopfli/monolithic_examples.h"
#include "../../thirdparty/owemdjee/libzopfli/src/zopflipng/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS-C++-Wrapper2/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS-C++-Wrapper/monolithic_examples.h"
#include "../../thirdparty/owemdjee/BLAKE3/c/monolithic_examples.h"
#include "../../thirdparty/jbig2dec/monolithic_examples.h"
#undef BUILD_MONOLITHIC

#include "../../scripts/MuPDFLib/versions-api.h"

#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define main main_utf8
#endif


static int report_version(int argc, const char** argv);


typedef int tool_f(void);
typedef int tool_fa(int argc, const char** argv);

static struct {
	union
	{
		tool_f* f;
		tool_fa* fa;
	} func;

	const char *name;
    const char *desc;
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
	{ {.fa = pdftagged_main }, "tagged", "extract Tagged PDF content" },
#endif
#endif
#if FZ_ENABLE_JS
#if defined(MUTOOL_EX)
	{ {.fa = murun_main }, "run", "run javascript" },
#endif
#endif
#if FZ_ENABLE_PDF
	{ {.fa = pdfshow_main }, "show", "show internal pdf objects" },
#ifndef NDEBUG
#if defined(MUTOOL_EX)
	{ {.fa = cmapdump_main }, "cmapdump", "dump CMap resource as C source file" },
#endif
#endif
#endif
#if defined(MUTOOL_EX)
	{ {.fa = mupdf_base64_test_main }, "base64_test", "muPDF base64 fringe case tests" },
	{ {.fa = mupdf_example_main }, "example", "muPDF docs::example: render a single page and print the result as a PPM to stdout" },
	{ {.fa = mupdf_multithreaded_example_main }, "multithreaded_example", "muPDF multi-threaded rendering of all pages in a document to PNG images" },
	{ {.fa = mupdf_storytest_main }, "story_test", "muPDF fz_story showcase for converting HTML input to a PDF file" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = tesseract_main }, "tesseract", "OCR given image or PDF" },
#endif

#if defined(MUTOOL_EX)
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
#endif

	{ {.fa = curl_main }, "curl", "access/fetch a given URI" },

#if defined(MUTOOL_EX)
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

	{ {.fa = sqlite_main }, "sqlite", "SQLite3 tool" },
#if defined(MUTOOL_EX)
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
	//{ {.fa = jpeginfo_main }, "jpeginfo", "jpeginfo tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = jpegturbo_jpegtran_main }, "jpegtran", "jpegtran tool" },
	{ {.fa = jpegturbo_rdjpegcom_main }, "rdjpegcom", "rdjpegcom tool" },
	{ {.fa = jpegturbo_wrjpegcom_main }, "wrjpegcom", "wrjpegcom tool" },
	{ {.fa = jpegturbo_djpeg_main }, "djpeg", "djpeg tool" },
	{ {.fa = jpegturbo_cjpeg_main }, "cjpeg", "cjpeg tool" },
#endif

#if defined(MUTOOL_EX)
	{ {.fa = jbig2dec_main }, "jbig2dec", "jbig2dec tool" },
	{ {.fa = jbig2dec_arith_test_main }, "jbig2_arith_test", "jbig2dec test" },
	{ {.fa = jbig2dec_huffman_test_main }, "jbig2_huffman_test", "jbig2dec test" },
	{ {.fa = jbig2dec_pbm2png_main }, "jbig2dec_pbm2png", "jbig2dec_pbm2png tool" },
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
	{ {.fa = qjs_main }, "qjs", "qjs (QuickJS) tool" },
	{ {.fa = qjsc_main }, "qjsc", "qjsc (QuickJS Compiler) tool" },
#endif
#if defined(MUTOOL_EX)
	{ {.fa = qjscompress_main }, "qjscompress", "qjscompress tool" },
	{ {.fa = qjs_unicode_gen_main }, "qjs_unicode_gen", "qjs_unicode_gen tool" },
	{ {.fa = qjs_test262_main }, "qjs_test262", "qjs_test262 conformance test tool" },
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
	{ {.fa = qjscpp_async_example_main }, "qjscpp_async_example", "qjscpp_async_example tool" },
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
	{ {.fa = jpegXL_decode_oneshot_main }, "jxl_decode_oneshot", "jxl_decode_oneshot tool" },
	{ {.fa = jpegXL_encode_oneshot_main }, "jxl_encode_oneshot", "jxl_encode_oneshot tool" },
	{ {.fa = jpegXL_info_main }, "jxl_info", "jxl_info: jpegXL tool" },
	{ {.fa = jpegXL_decode_EXIF_metadata_main }, "jpegXL_decode_EXIF_metadata", "jpegXL_decode_EXIF_metadata: jpegXL tool" },
	{ {.fa = jpegXL_decode_progressive_main }, "jpegXL_decode_progressive", "jpegXL_decode_progressive: jpegXL tool" },
	{ {.fa = jpegXL_box_list_main }, "jxl_box_list", "jxl_box_list tool" },
	{ {.fa = jpegXL_butteraugli_main }, "jxl_butteraugli", "jxl_butteraugli tool" },
	{ {.fa = jpegXL_compress_main }, "jxl_compress", "jxl_compress tool" },
	{ {.fa = jpegXL_cjpeg_hdr_main }, "jxl_cjpeg_hdr", "jxl_cjpeg_hdr tool" },
	{ {.fa = jpegXL_decompress_main }, "jxl_decompress", "jxl_decompress tool" },
	{ {.fa = jpegXL_dec_enc_main }, "jxl_dec_enc", "jxl_dec_enc tool" },
	{ {.fa = jpegXL_from_tree_main }, "jxl_from_tree", "jxl_from_tree tool" },
	{ {.fa = jpegXL_ssimulacra_main }, "jxl_ssimulacra tool", "jxl_ssimulacra tool" },
	{ {.fa = jpegXL_xyb_range_main }, "jxl_xyb_range", "jxl_xyb_range tool" },
	{ {.fa = jpegXL_generate_LUT_template_main }, "jpegXL_generate_LUT_template", "jpegXL_generate_LUT_template tool" },
	{ {.fa = jpegXL_pq_to_hlg_main }, "jpegXL_pq_to_hlg", "jpegXL_pq_to_hlg tool" },
	{ {.fa = jpegXL_render_hlg_main }, "jpegXL_render_hlg", "jpegXL_render_hlg tool" },
	{ {.fa = jpegXL_texture_to_cube_main }, "jpegXL_texture_to_cube", "jpegXL_texture_to_cube tool" },
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
	{ {.fa = BLAKE3_example_main }, "BLAKE3_example", "BLAKE3_example tool" },
	{ {.fa = BLAKE3_demo_main }, "BLAKE3_demo", "BLAKE3_demo tool" },
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
    const char *start, *end;
    char buf[64];
    int i;

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

	// registeer a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	atexit(mu_drop_context);

    if (argc == 0)
    {
        fz_error(ctx, "No command name found!");
        return EXIT_FAILURE;
    }

    /* Check argv[0] */

    if (argc > 0)
    {
        start = fz_basename(argv[0]);
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
                return tools[i].func.fa(argc, argv);
            strcpy(buf, "mu");
            strcat(buf, tools[i].name);
            assert(strlen(buf) < sizeof(buf));
            if (namematch(end, start, buf) || namematch(end, start, buf + 2))
                return tools[i].func.fa(argc, argv);
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
                return tools[i].func.fa(argc - 1, argv + 1);
            strcpy(buf, "mu");
            strcat(buf, tools[i].name);
            assert(strlen(buf) < sizeof(buf));
            if (namematch(end, start, buf) || namematch(end, start, buf + 2))
                return tools[i].func.fa(argc - 1, argv + 1);
        }
        if (!strcmp(argv[1], "-v"))
        {
            fz_info(ctx, "mutool version %s", FZ_VERSION);
            return EXIT_SUCCESS;
        }
        fz_error(ctx, "mutool: unrecognized command '%s'\n", argv[1]);
    }

    /* Print usage */

	fz_info(ctx, "mutool version %s\n", FZ_VERSION);
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

