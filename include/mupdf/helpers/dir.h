#ifndef __MUPDF_HELPERS_DIR_H__
#define __MUPDF_HELPERS_DIR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mupdf/fitz/version.h"
#include "mupdf/fitz/config.h"
#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"

#include <errno.h>

#ifndef E_OK
#define E_OK 0
#endif

int fz_chdir(fz_context* ctx, const char* path);
/**
  Create directory for given *file* path, so that a subsequent file-create action will not fail due to the given path not existing.

  Throws exception when errors occur.
*/
void fz_mkdir_for_file(fz_context* ctx, const char* path);

#ifdef __cplusplus
}
#endif

#endif

