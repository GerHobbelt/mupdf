// define stubs for the possibly missing APIs:

#include "mupdf/fitz.h"

#include "../../thirdparty/tesseract/include/tesseract/export.h"       // for TESS_API
#include "../../thirdparty/tesseract/include/tesseract/capi_training_tools.h"

#include <string.h>
#include <stdio.h>



#if !defined(TESSERACT_STANDALONE) || defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DISABLED_LEGACY_ENGINE)

TESS_API int tesseract_ambiguous_words_main(int argc, const char** argv) {
	fz_error(NULL, "tess_ambiguous_words is not available in this build.\n");
	return EXIT_FAILURE;
}

TESS_API int tesseract_classifier_tester_main(int argc, const char** argv) {
	fz_error(NULL, "tess_classifier_tester is not available in this build.\n");
	return EXIT_FAILURE;
}

TESS_API int tesseract_cn_training_main(int argc, const char** argv) {
	fz_error(NULL, "tess_cn_training is not available in this build.\n");
	return EXIT_FAILURE;
}

TESS_API int tesseract_mf_training_main(int argc, const char** argv) {
	fz_error(NULL, "tess_mf_training is not available in this build.\n");
	return EXIT_FAILURE;
}

TESS_API int tesseract_shape_clustering_main(int argc, const char** argv) {
	fz_error(NULL, "tess_shape_clustering is not available in this build.\n");
	return EXIT_FAILURE;
}

#endif

#if !defined(HAS_LIBICU)

TESS_API int tesseract_set_unicharset_properties_main(int argc, const char** argv) {
	fz_error(NULL, "tess_set_unicharset_properties is not available in this build.\n");
	return EXIT_FAILURE;
}

TESS_API int tesseract_unicharset_extractor_main(int argc, const char** argv) {
	fz_error(NULL, "tess_unicharset_extractor is not available in this build.\n");
	return EXIT_FAILURE;
}

TESS_API int tesseract_combine_lang_model_main(int argc, const char** argv) {
	fz_error(NULL, "tess_combine_lang_model is not available in this build.\n");
	return EXIT_FAILURE;
}

#endif

#if !defined(PANGO_ENABLE_ENGINE)

TESS_API int tesseract_text2image_main(int argc, const char** argv) {
	fz_error(NULL, "tess_text2image is not available in this build.\n");
	return EXIT_FAILURE;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
