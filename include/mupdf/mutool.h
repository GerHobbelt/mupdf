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
int pdftagged_main(int argc, const char** argv);
int pdfmetadump_main(int argc, const char** argv);

int mupdf_base64_test_main(int argc, const char** argv);

int cmapdump_main(int argc, const char** argv);

#if !defined(TESSERACT_STANDALONE) || defined(BUILD_MONOLITHIC)
int tesseract_main(int argc, const char** argv);
#endif

int curl_main(int argc, const char** argv);

int mujs_example_main(int argc, const char** argv);				
int mujs_prettyprint_main(int argc, const char** argv);

int qiqqa_fingerprint0_main(int argc, const char** argv);
int qiqqa_fingerprint1_main(int argc, const char** argv);
int qiqqa_documentid62_main(int argc, const char** argv);

int qiqqa_muannot_main(int argc, const char** argv);
int qiqqa_mucontent_main(int argc, const char** argv);
int qiqqa_muserver_main(int argc, const char** argv);

int sqlite_main(int argc, const char** argv);

//int jpeginfo_main(int argc, const char** argv);

int jpegturbo_jpegtran_main(int argc, const char** argv);
int jpegturbo_rdjpegcom_main(int argc, const char** argv);
int jpegturbo_wrjpegcom_main(int argc, const char** argv);
int jpegturbo_cjpeg_main(int argc, const char** argv);
int jpegturbo_djpeg_main(int argc, const char** argv);

int mupdf_example_main(int argc, const char** argv);
int mupdf_multithreaded_example_main(int argc, const char** argv);
int mupdf_storytest_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
