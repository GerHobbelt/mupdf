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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

#include "mupdf/fitz.h"

#include <limits.h>

#include <jpeglib.h>
#include "jerror.h"

#include "mupdf/helpers/jmemcust.h"


static void *
fz_jpeg_dflt_mem_alloc(j_common_ptr cinfo, size_t size)
{
	fz_context* ctx = fz_get_global_context();
	return fz_malloc_no_throw(ctx, size);
}

static void
fz_jpeg_dflt_mem_free(j_common_ptr cinfo, void *object, size_t size)
{
	fz_context *ctx = fz_get_global_context();
	fz_free(ctx, object);
}

static long
fz_jpeg_dflt_mem_init(j_common_ptr cinfo)
{
	return 0;			/* just set max_memory_to_use to 0 */
}

static void
fz_jpeg_dflt_mem_term(j_common_ptr cinfo)
{
	cinfo->client_data_ref = NULL;
	cinfo->client_init_callback = NULL;
}

static size_t
fz_jpeg_dflt_mem_available(j_common_ptr cinfo, size_t min_bytes_needed,
	size_t max_bytes_needed,
	size_t already_allocated)
{
	// Here we always say, "we got all you want bud!"
	long ret = max_bytes_needed;

	return ret;
}

static void
fz_jpeg_dflt_open_backing_store(j_common_ptr cinfo,
	backing_store_ptr info,
	long total_bytes_needed)
{
	ERREXIT(cinfo, JERR_NO_BACKING_STORE);
}


int fz_jpeg_dflt_sys_mem_register(j_common_ptr cinfo)
{
	cinfo->sys_mem_if.get_small = fz_jpeg_dflt_mem_alloc;
	cinfo->sys_mem_if.free_small = fz_jpeg_dflt_mem_free;

	cinfo->sys_mem_if.get_large = fz_jpeg_dflt_mem_alloc;
	cinfo->sys_mem_if.free_large = fz_jpeg_dflt_mem_free;

	cinfo->sys_mem_if.mem_available = fz_jpeg_dflt_mem_available;

	cinfo->sys_mem_if.open_backing_store = fz_jpeg_dflt_open_backing_store;

	cinfo->sys_mem_if.mem_init = fz_jpeg_dflt_mem_init;
	cinfo->sys_mem_if.mem_term = fz_jpeg_dflt_mem_term;

	return 0;
}


void fz_set_default_jpeg_sys_mem_mgr(void)
{
	// registeer a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	jpeg_sys_mem_set_default_setup(fz_jpeg_dflt_sys_mem_register);
}

