// Copyright (C) 2020-2023 Artifex Software, Inc.
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

#include "leptonica-wrap.h"

//#ifdef HAVE_LEPTONICA

#include "leptonica/allheaders.h"
#include "../../thirdparty/leptonica/src/environ.h"  // <-- leptonica_free() + leptonica_malloc() prototypes


#define DEBUG_ALLOCS   0

/* When we build with our own leptonica, we want to intercept malloc/free etc.
 * Unfortunately we have to use a nasty global here. */
static fz_context *leptonica_mem = NULL;

#if DEBUG_ALLOCS
static int event = 0;
#endif


fz_context *get_leptonica_ctx(void)
{
	return leptonica_mem;
}

void *leptonica_malloc(size_t size)
{
	void *ret = Memento_label(fz_malloc_no_throw(leptonica_mem, size), "leptonica_malloc");
#if DEBUG_ALLOCS
	fz_info(leptonica_mem, "dbg: %d LEPTONICA_MALLOC(%p) %d -> %p\n", event++, leptonica_mem, (int)size, ret);
#endif
	return ret;
}

void leptonica_free(const void *ptr)
{
#if DEBUG_ALLOCS
	fz_info(leptonica_mem, "dbg: %d LEPTONICA_FREE(%p) %p\n", event++, leptonica_mem, ptr);
#endif
	fz_free(leptonica_mem, ptr);
}

void *leptonica_calloc(size_t numelm, size_t elemsize)
{
	void *ret = leptonica_malloc(numelm * elemsize);

	if (ret)
		memset(ret, 0, numelm * elemsize);
#if DEBUG_ALLOCS
	fz_info(leptonica_mem, "dbg: %d LEPTONICA_CALLOC %d,%d -> %p\n", event++, (int)numelm, (int)elemsize, ret);
#endif
	return ret;
}

/* Not currently actually used */
void *leptonica_realloc(void *ptr, size_t blocksize)
{
	void *ret = fz_realloc_no_throw(leptonica_mem, ptr, blocksize);

#if DEBUG_ALLOCS
	fz_info(leptonica_mem, "dbg: %d LEPTONICA_REALLOC %p,%d -> %p\n", event++, ptr, (int)blocksize, ret);
#endif
	return ret;
}


static void lept_stderr_print(const char *msg)
{
	fz_context *ctx = fz_get_global_context();

	//fz_write_string(ctx, fz_stderr(ctx), msg);

	if (strncmp(msg, "Error in ", 9) == 0) {
		fz_error(ctx, "ERROR: Leptonica::%s", msg + 9);
		return;
	}
	if (strncmp(msg, "Warning in ", 11) == 0) {
		fz_warn(ctx, "WARN: Leptonica::%s", msg + 11);
		return;
	}
	if (strncmp(msg, "Info in ", 8) == 0) {
		fz_info(ctx, "INFO: Leptonica::%s", msg + 8);
		return;
	}
	fz_info(ctx, "Leptonica::%s", msg);
}


static void fz_set_leptonica_stderr_handler(fz_context *ctx)
{
	leptSetStderrHandler(lept_stderr_print);
}


static void *dflt_ocr_malloc(size_t size) { 
	fz_context *ctx = fz_get_global_context();
	return fz_malloc(ctx, size); 
}

static void dflt_ocr_free(const void *ptr) { 
	fz_context *ctx = fz_get_global_context();
    fz_free(ctx, ptr);
}


void
fz_set_leptonica_mem(fz_context *ctx)
{
	int die = 0;

	fz_lock(ctx, FZ_LOCK_ALLOC);
	die = (leptonica_mem != NULL) && (leptonica_mem != ctx);
	if (!die)
		leptonica_mem = ctx;
	fz_unlock(ctx, FZ_LOCK_ALLOC);
	if (die)
		fz_throw(ctx, FZ_ERROR_ARGUMENT, "Attempt to use Tesseract from 2 threads at once!");
	fz_set_leptonica_stderr_handler(ctx);

// #ifdef LEPTONICA_INTERCEPT_ALLOC
#if !defined(LEPTONICA_NO_CUSTOM_MEM_MANAGER)
	setPixMemoryManager(leptonica_malloc, leptonica_free);
#endif
}

void
fz_clear_leptonica_mem(fz_context *ctx)
{
	int die = 0;

	fz_lock(ctx, FZ_LOCK_ALLOC);
	die = (leptonica_mem == NULL);
	if (!die)
		leptonica_mem = NULL;
	fz_unlock(ctx, FZ_LOCK_ALLOC);
	if (die)
		fz_throw(ctx, FZ_ERROR_ARGUMENT, "Attempt to use Tesseract from 2 threads at once!");
	//leptSetStderrHandler(NULL);

// #ifdef LEPTONICA_INTERCEPT_ALLOC
#if !defined(LEPTONICA_NO_CUSTOM_MEM_MANAGER)
	setPixMemoryManager(dflt_ocr_malloc, dflt_ocr_free);
#endif
}

//#else

//#error "MUST have leptonica to build mupdf, tesseract, et al"

//#endif /* HAVE_LEPTONICA */

