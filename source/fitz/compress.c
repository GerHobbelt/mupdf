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

#include <zlib-ng.h> /* for compressBound() and compress() */

fz_buffer *fz_deflate(fz_context *ctx, fz_buffer *input)
{
	unsigned char *input_p = input->data;
	uLong input_n = (uLong) input->len;
	unsigned char *output_p;
	size_t output_n;
	int result;

	/* check possible size_t / uLong precision mismatch */
	if (input->len != (size_t)input_n)
		fz_throw(ctx, FZ_ERROR_GENERIC, "buffer is too large to deflate");

	output_n = zng_compressBound(input_n);
	output_p = (unsigned char *)Memento_label(fz_malloc(ctx, output_n), "fz_deflate");
	result = zng_compress(output_p, &output_n, input_p, input_n);
	if (result != Z_OK)
	{
		fz_free(ctx, output_p);
		fz_throw(ctx, FZ_ERROR_GENERIC, "zlib error when deflating data");
	}

	fz_try(ctx)
		output_p = (unsigned char *)fz_realloc(ctx, output_p, output_n);
	fz_catch(ctx)
	{
		fz_free(ctx, output_p);
		fz_rethrow(ctx);
	}

	return fz_new_buffer_from_data(ctx, output_p, output_n);
}
