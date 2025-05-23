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

/*
 * mu-office-test - Simple test for Muoffice.
 */

#include "mupdf/memento.h"
#include "mupdf/helpers/mu-office-lib.h"
#include "mupdf/fitz.h"

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif


#define TEST_PDF_FILEPATH  "1905.07689.pdf"

static HANDLE loaded;

/* Forward definition */
static int save_png(const MuOfficeBitmap *bitmap, const char *filename);

static void
load_progress(void *cookie, int pages_loaded, int complete)
{
	assert((intptr_t)cookie == 1234);

	fz_info(NULL, "load_progress: pages_loaded=%d complete=%d", pages_loaded, complete);

	if (complete)
		(void)ReleaseSemaphore(loaded, 1, NULL);
}

static void
load_error(void *cookie, MuOfficeDocErrorType error)
{
	assert((intptr_t)cookie == 1234);

	fz_error(NULL, "load_error: error=%d", error);
}

static void render_progress(void *cookie, MuError error)
{
	assert((intptr_t)cookie == 5678);

	fz_error(NULL, "render_progress: error=%d", error);
	(void)ReleaseSemaphore(loaded, 1, NULL);
}

static int
test_async(MuOfficeLib *mu)
{
	MuOfficeDoc *doc;
	MuError err;
	int count;
	MuOfficePage *page;
	float w, h;
	MuOfficeBitmap bitmap;
	MuOfficeRenderArea area;
	MuOfficeRender *render;
	fz_context* ctx = fz_get_global_context();

	err = MuOfficeLib_loadDocument(mu,
					TEST_PDF_FILEPATH,
					load_progress,
					load_error,
					(void *)1234, /* Arbitrary */
					&doc);

	/* Wait for the load to complete */
	WaitForSingleObject(loaded, INFINITE);

	/* Test number of pages */
	err = MuOfficeDoc_getNumPages(doc, &count);
	if (err)
	{
		fz_error(ctx, "Failed to count pages: error=%d", err);
		return EXIT_FAILURE;
	}
	fz_info(ctx, "%d Pages in document", count);

	/* Get a page */
	err = MuOfficeDoc_getPage(doc, 0, NULL, (void *)4321, &page);
	if (err)
	{
		fz_error(ctx, "Failed to get page: error=%d", err);
		return EXIT_FAILURE;
	}

	/* Get size of page */
	err = MuOfficePage_getSize(page, &w, &h);
	if (err)
	{
		fz_error(ctx, "Failed to get page size: error=%d", err);
		return EXIT_FAILURE;
	}
	fz_info(ctx, "Page size = %g x %g", w, h);

	/* Allocate ourselves a bitmap */
	bitmap.width = (int)(w * 1.5f + 0.5f);
	bitmap.height = (int)(h * 1.5f + 0.5f);
	bitmap.lineSkip = bitmap.width * 4;
	bitmap.memptr = fz_malloc(ctx, bitmap.lineSkip * bitmap.height);

	/* Set the area to render the whole bitmap */
	area.origin.x = 0;
	area.origin.y = 0;
	area.renderArea.x = 0;
	area.renderArea.y = 0;
	area.renderArea.width = bitmap.width;
	area.renderArea.height = bitmap.height;

	/* Render into the bitmap */
	err = MuOfficePage_render(page, 1.5f, &bitmap, &area, render_progress, (void *)5678, &render);
	if (err)
	{
		fz_error(ctx, "Page render failed: error=%d", err);
		return EXIT_FAILURE;
	}

	/* Wait for the render to complete */
	WaitForSingleObject(loaded, INFINITE);

	/* Kill the render */
	MuOfficeRender_destroy(render);

	/* Output the bitmap */
	int rv = save_png(&bitmap, "out_mu_office_async.png");
	fz_free(ctx, bitmap.memptr);

	MuOfficePage_destroy(page);

	MuOfficeDoc_destroy(doc);

	CloseHandle(loaded);
	loaded = NULL;

	return rv;
}

static int
test_sync(MuOfficeLib *mu)
{
	MuOfficeDoc *doc;
	MuError err;
	int count;
	MuOfficePage *page;
	float w, h;
	MuOfficeBitmap bitmap;
	MuOfficeRenderArea area;
	MuOfficeRender *render;
	fz_context* ctx = fz_get_global_context();

	loaded = CreateSemaphore(NULL, 0, 1, NULL);

	err = MuOfficeLib_loadDocument(mu,
					TEST_PDF_FILEPATH,
					NULL,
					NULL,
					NULL,
					&doc);

	/* Test number of pages */
	err = MuOfficeDoc_getNumPages(doc, &count);
	if (err)
	{
		fz_error(ctx, "Failed to count pages: error=%d", err);
		return EXIT_FAILURE;
	}
	fz_info(ctx, "%d Pages in document", count);

	/* Get a page */
	err = MuOfficeDoc_getPage(doc, 1, NULL, (void *)4321, &page);
	if (err)
	{
		fz_error(ctx, "Failed to get page: error=%d", err);
		return EXIT_FAILURE;
	}

	/* Get size of page */
	err = MuOfficePage_getSize(page, &w, &h);
	if (err)
	{
		fz_error(ctx, "Failed to get page size: error=%d", err);
		return EXIT_FAILURE;
	}
	fz_info(ctx, "Page size = %g x %g", w, h);

	/* Allocate ourselves a bitmap */
	bitmap.width = (int)(w * 1.5f + 0.5f);
	bitmap.height = (int)(h * 1.5f + 0.5f);
	bitmap.lineSkip = bitmap.width * 4;
	bitmap.memptr = fz_malloc(ctx, bitmap.lineSkip * bitmap.height);

	/* Set the area to render the whole bitmap */
	area.origin.x = 0;
	area.origin.y = 0;
	area.renderArea.x = 0;
	area.renderArea.y = 0;
	area.renderArea.width = bitmap.width;
	area.renderArea.height = bitmap.height;

	/* Render into the bitmap */
	err = MuOfficePage_render(page, 1.5f, &bitmap, &area, NULL, NULL, &render);
	if (err)
	{
		fz_error(ctx, "Page render failed: error=%d", err);
		return EXIT_FAILURE;
	}

	err = MuOfficeRender_waitUntilComplete(render);
	if (err)
	{
		fz_error(ctx, "Page render failed to complete: error=%d", err);
		return EXIT_FAILURE;
	}

	/* Kill the render */
	MuOfficeRender_destroy(render);

	/* Output the bitmap */
	int rv = save_png(&bitmap, "out_mu_office_sync.png");
	fz_free(ctx, bitmap.memptr);

	MuOfficePage_destroy(page);

	MuOfficeDoc_destroy(doc);

	return rv;
}

int main(int argc, const char **argv)
{
	MuOfficeLib *mu;
	MuError err;
	int ret;

	if (!fz_has_global_context())
	{
		fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}

	err = MuOfficeLib_create(&mu);
	if (err)
	{
		fz_error(NULL, "Failed to create lib instance: error=%d", err);
		return EXIT_FAILURE;
	}

	ret = test_async(mu);
	if (ret)
		return ret;

	ret = test_sync(mu);
	if (ret)
		return ret;

	MuOfficeLib_destroy(mu);

	return EXIT_SUCCESS;
}

/*
	Code beneath here calls MuPDF directly, purely because
	this is the easiest way to get PNG saving functionality.
	This is not part of the test, which is why this is put
	at the end.
*/

#include "mupdf/fitz.h"

static int
save_png(const MuOfficeBitmap *bitmap, const char *filename)
{
	fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	fz_pixmap *pix;
	int errored = 0;

	if (ctx == NULL)
	{
		fz_error(NULL, "save_png failed! (out of memory)");
		return EXIT_FAILURE;
	}

	pix = fz_new_pixmap_with_data(ctx, fz_device_rgb(ctx), bitmap->width, bitmap->height, NULL, 1, bitmap->lineSkip, bitmap->memptr);

	fz_try(ctx)
	{
		fz_save_pixmap_as_png(ctx, pix, filename);
	}
	fz_always(ctx)
	{
		fz_drop_pixmap(ctx, pix);
	}
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fz_log_error_printf(ctx, "save_png failed! file: %s", filename);
		errored = 1;
	}

	fz_flush_warnings(ctx);
	fz_drop_context(ctx);

	return errored;
}
