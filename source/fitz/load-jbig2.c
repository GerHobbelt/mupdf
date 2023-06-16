// Copyright (C) 2004-2021 Artifex Software, Inc.
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

#include "mupdf/fitz.h"

#if BUILDING_MUPDF_MINIMAL_CORE < 2

#include "pixmap-imp.h"
#include "image-imp.h"

#include <jbig2.h>

struct info
{
	int width, height;
	int xres, yres;
	int pages;
	fz_colorspace *cspace;
};

struct fz_jbig2_allocator
{
	Jbig2Allocator super;
};

static inline void fz_lock_jbig2(fz_context* ctx)
{
	//fz_lock(ctx, FZ_LOCK_JBIG2);
}

static inline void fz_unlock_jbig2(fz_context* ctx)
{
	//fz_unlock(ctx, FZ_LOCK_JBIG2);
}

static void
error_callback(void* data, const char* msg, Jbig2Severity severity, uint32_t seg_idx)
{
	fz_context* ctx = data;
	char idxbuf[50] = "";

	if (seg_idx != JBIG2_UNKNOWN_SEGMENT_NUMBER)
		fz_snprintf(idxbuf, sizeof idxbuf, " (segment 0x%02x)", seg_idx);

	if (severity == JBIG2_SEVERITY_FATAL)
		fz_error(ctx, "jbig2dec error: %s%s", msg, idxbuf);
	else if (severity == JBIG2_SEVERITY_WARNING)
		fz_warn(ctx, "jbig2dec warning: %s%s", msg, idxbuf);
	else if (severity == JBIG2_SEVERITY_INFO)
		fz_info(ctx, "jbig2dec info: %s%s", msg, idxbuf);
	else
		fz_info(ctx, "jbig2dec debug: %s%s", msg, idxbuf);
}

static void *fz_jbig2_alloc(Jbig2Allocator *allocator, size_t size)
{
	fz_context *ctx = ((struct fz_jbig2_allocator *) allocator)->super.user_context;
	return fz_malloc_no_throw(ctx, size);
}

static void fz_jbig2_free(Jbig2Allocator *allocator, void *p)
{
	fz_context *ctx = ((struct fz_jbig2_allocator *) allocator)->super.user_context;
	fz_free(ctx, p);
}

static void *fz_jbig2_realloc(Jbig2Allocator *allocator, void *p, size_t size)
{
	fz_context *ctx = ((struct fz_jbig2_allocator *) allocator)->super.user_context;
	if (size == 0)
	{
		fz_free(ctx, p);
		return NULL;
	}
	if (p == NULL)
		return Memento_label(fz_malloc(ctx, size), "jbig2_realloc");
	return Memento_label(fz_realloc_no_throw(ctx, p, size), "jbig2_realloc");
}

static fz_pixmap *
jbig2_read_image(fz_context *ctx, struct info *jbig2, const unsigned char *buf, size_t len, int only_metadata, int subimage)
{
	Jbig2Ctx *jctx = NULL;
	Jbig2Image *page = NULL;
	struct fz_jbig2_allocator allocator;
	fz_pixmap *pix = NULL;

	allocator.super.alloc_ = fz_jbig2_alloc;
	allocator.super.free_ = fz_jbig2_free;
	allocator.super.realloc_ = fz_jbig2_realloc;
	allocator.super.user_context = ctx;

	fz_var(jctx);
	fz_var(page);
	fz_var(pix);

	fz_try(ctx)
	{
		fz_lock_jbig2(ctx);

		jctx = jbig2_ctx_new((Jbig2Allocator *) &allocator, JBIG2_OPTIONS_NONE, NULL, error_callback, ctx);
		if (jctx == NULL)
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot create jbig2 context");
		if (jbig2_data_in(jctx, buf, len) < 0)
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode jbig2 image");
		if (jbig2_complete_page(jctx) < 0)
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot complete jbig2 image");

		if (only_metadata && subimage < 0)
		{
			while ((page = jbig2_page_out(jctx)) != NULL)
			{
				jbig2_release_page(jctx, page);
				jbig2->pages++;
			}
		}
		else if (only_metadata && subimage >= 0)
		{
			while ((page = jbig2_page_out(jctx)) != NULL && subimage > 0)
			{
				jbig2_release_page(jctx, page);
				subimage--;
			}

			if (page == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "no jbig2 image decoded");

			jbig2->cspace = fz_device_gray(ctx);
			jbig2->width = page->width;
			jbig2->height = page->height;
			jbig2->xres = 72;
			jbig2->yres = 72;
		}
		else if (subimage >= 0)
		{
			while ((page = jbig2_page_out(jctx)) != NULL && subimage > 0)
			{
				jbig2_release_page(jctx, page);
				subimage--;
			}

			if (page == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "no jbig2 image decoded");

			jbig2->cspace = fz_device_gray(ctx);
			jbig2->width = page->width;
			jbig2->height = page->height;
			jbig2->xres = 72;
			jbig2->yres = 72;

			pix = fz_new_pixmap(ctx, jbig2->cspace, jbig2->width, jbig2->height, NULL, 0);
			fz_unpack_tile(ctx, pix, page->data, 1, 1, page->stride, 0);
			fz_invert_pixmap(ctx, pix);
		}
	}
	fz_always(ctx)
	{
		jbig2_release_page(jctx, page);
		jbig2_ctx_free(jctx);

		fz_unlock_jbig2(ctx);
	}
	fz_catch(ctx)
	{
		fz_drop_pixmap(ctx, pix);
		fz_rethrow(ctx);
	}

	return pix;
}

int
fz_load_jbig2_subimage_count(fz_context *ctx, const unsigned char *buf, size_t len)
{
	struct info jbig2 = { 0 };
	int subimage_count = 0;

	fz_try(ctx)
	{
		jbig2_read_image(ctx, &jbig2, buf, len, 1, -1);
		subimage_count = jbig2.pages;
	}
	fz_catch(ctx)
		fz_rethrow(ctx);

	return subimage_count;
}

void
fz_load_jbig2_info_subimage(fz_context *ctx, const unsigned char *buf, size_t len, int *wp, int *hp, int *xresp, int *yresp, fz_colorspace **cspacep, uint8_t *orientationp, int subimage)
{
	struct info jbig2 = { 0 };

	jbig2_read_image(ctx, &jbig2, buf, len, 1, subimage);
	*cspacep = fz_keep_colorspace(ctx, jbig2.cspace);
	*orientationp = 1;
	*wp = jbig2.width;
	*hp = jbig2.height;
	*xresp = jbig2.xres;
	*yresp = jbig2.yres;
}

fz_pixmap *
fz_load_jbig2_subimage(fz_context *ctx, const unsigned char *buf, size_t len, int subimage)
{
	struct info jbig2 = { 0 };
	return jbig2_read_image(ctx, &jbig2, buf, len, 0, subimage);
}

fz_pixmap *
fz_load_jbig2(fz_context *ctx, const unsigned char *buf, size_t len)
{
	return fz_load_jbig2_subimage(ctx, buf, len, 0);
}

void
fz_load_jbig2_info(fz_context *ctx, const unsigned char *buf, size_t len, int *wp, int *hp, int *xresp, int *yresp, fz_colorspace **cspacep, uint8_t *orientationp)
{
	fz_load_jbig2_info_subimage(ctx, buf, len, wp, hp, xresp, yresp, cspacep, orientationp, 0);
}

#endif
