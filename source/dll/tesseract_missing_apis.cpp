// define stubs for the possibly missing APIs:

#ifdef HAVE_TESSERACT_CONFIG_H
#  include "config_auto.h" // DISABLED_LEGACY_ENGINE
#endif

#include "mupdf/fitz.h"

#include "../../thirdparty/tesseract/include/tesseract/export.h"       // for TESS_API
#include "../../thirdparty/tesseract/include/tesseract/capi_training_tools.h"

#include <string.h>
#include <stdio.h>



#if !defined(TESSERACT_STANDALONE) || defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

// nada

#ifdef __cplusplus
}
#endif

#endif
