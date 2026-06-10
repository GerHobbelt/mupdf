#ifndef FITZ_OPJ_MALLOC_REPLACEMENT_H
#define FITZ_OPJ_MALLOC_REPLACEMENT_H

#include <mupdf/fitz/system.h>
#include <mupdf/fitz/context.h>
//#include <mupdf/fitz/geometry.h>
//#include <mupdf/fitz/image.h>
//#include <mupdf/fitz/shade.h>
//#include <mupdf/fitz/path.h>
//#include <mupdf/fitz/text.h>
//#include <mupdf/fitz/options.h>

// special exception for when we compile this code for a monolithic tesseract stand-alone app: ALWAYS offer the leptonica_malloc/free APIs availabe in here!
//#if FZ_ENABLE_JPX
// hence we use this condition combo instead:
//#if (BUILDING_MUPDF_MINIMAL_CORE < 2) && FZ_ENABLE_JPX

#include <mupdf/assertions.h>

void fz_opj_lock(fz_context* ctx);
void fz_opj_unlock(fz_context* ctx);
void* fz_opj_malloc(size_t size);
void* fz_opj_calloc(size_t n, size_t size);
void* fz_opj_realloc(void* ptr, size_t size);
void fz_opj_free(void* ptr);

void* fz_opj_aligned_malloc(size_t size);
void* fz_opj_aligned_32_malloc(size_t size);
void* fz_opj_aligned_realloc(void* ptr, size_t size);
void* fz_opj_aligned_32_realloc(void* ptr, size_t size);
void fz_opj_aligned_free(void* ptr_);

#endif
