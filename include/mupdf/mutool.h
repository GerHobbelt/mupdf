#ifndef MUTOOL_H
#define MUTOOL_H

#include "mupdf/fitz.h"

#ifdef __cplusplus
extern "C" {
#endif

int muconvert_main(int argc, const char** argv);
#if !defined(MUDRAW_STANDALONE)
int mudraw_main(int argc, const char** argv);
#endif
#if !defined(MURASTER_STANDALONE)
int muraster_main(int argc, const char** argv);
#endif
int mutrace_main(int argc, const char** argv);
int murun_main(int argc, const char** argv);

int pdfclean_main(int argc, const char** argv);
int pdfextract_main(int argc, const char** argv);
int pdfinfo_main(int argc, const char** argv);
int pdfposter_main(int argc, const char** argv);
int pdfshow_main(int argc, const char** argv);
int pdfpages_main(int argc, const char** argv);
int pdfcreate_main(int argc, const char** argv);
int pdfmerge_main(int argc, const char** argv);
int pdfsign_main(int argc, const char** argv);
int pdfrecolor_main(int argc, const char** argv);
int pdftrim_main(int argc, const char** argv);
int pdfbake_main(int argc, const char** argv);
int pdftagged_main(int argc, const char** argv);
int pdfmetadump_main(int argc, const char** argv);

int mupdf_base64_test_main(int argc, const char** argv);

int cmapdump_main(int argc, const char** argv);
int mutextextract_main(int argc, const char** argv);

#if !defined(TESSERACT_STANDALONE) || defined(BUILD_MONOLITHIC)
int tesseract_main(int argc, const char** argv);
#endif

int curl_main(int argc, const char** argv);
int curl_schematable_tool_main(int argc, const char** argv);

int mujs_example_main(int argc, const char** argv);				
int mujs_prettyprint_main(int argc, const char** argv);

int qiqqa_fingerprint0_main(int argc, const char** argv);
int qiqqa_fingerprint1_main(int argc, const char** argv);
int qiqqa_documentid62_main(int argc, const char** argv);

int qiqqa_muannot_main(int argc, const char** argv);
int qiqqa_mucontent_main(int argc, const char** argv);
int qiqqa_muserver_main(int argc, const char** argv);

int qiqqa_chop_shop_main(int argc, const char** argv);
int qiqqa_content_importer_main(int argc, const char** argv);
int qiqqa_content_processor_main(int argc, const char** argv);
int qiqqa_convert_legacy_annot_blobs_main(int argc, const char** argv);
int qiqqa_convert_legacy_autotags_main(int argc, const char** argv);
int qiqqa_convert_legacy_configuration_main(int argc, const char** argv);
int qiqqa_convert_legacy_expedition_main(int argc, const char** argv);
int qiqqa_db_exporter_main(int argc, const char** argv);
int qiqqa_db_fts_indexer_main(int argc, const char** argv);
int qiqqa_db_fts_search_main(int argc, const char** argv);
int qiqqa_db_importer_main(int argc, const char** argv);
int qiqqa_db_pappy_main(int argc, const char** argv);
int qiqqa_doc_scrutinizer_main(int argc, const char** argv);
int qiqqa_ingest_main(int argc, const char** argv);
int qiqqa_meta_exporter_main(int argc, const char** argv);
int qiqqa_meta_importer_main(int argc, const char** argv);
int qiqqa_ocr_bezoar_main(int argc, const char** argv);
int qiqqa_pdf_hound_main(int argc, const char** argv);
int qiqqa_safe_file_copier_main(int argc, const char** argv);
int qiqqa_snarfl_main(int argc, const char** argv);
int qiqqa_web_api_server_main(int argc, const char** argv);

int sqlite_main(int argc, const char** argv);

int jpeginfo_main(int argc, const char** argv);

int jpegturbo_jpegtran_main(int argc, const char** argv);
int jpegturbo_rdjpegcom_main(int argc, const char** argv);
int jpegturbo_wrjpegcom_main(int argc, const char** argv);
int jpegturbo_cjpeg_main(int argc, const char** argv);
int jpegturbo_djpeg_main(int argc, const char** argv);

int mupdf_example_main(int argc, const char** argv);
int mupdf_multithreaded_example_main(int argc, const char** argv);
int mupdf_storytest_main(int argc, const char** argv);

int pthw32_all_tests_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
