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
#include "mupdf/pdf.h"

#if BUILDING_MUPDF_MINIMAL_CORE < 2

#include "../fitz/pixmap-imp.h"

void
fz_register_jbig2_encoder(fz_context *ctx, const fz_jbig2_encoder *enc)
{
	/* Note, there is no reference counting here. When we clone contexts
	 * we just copy the pointer. We never free this. We expect this to be
	 * a static structure that is just passed in on startup. */
	ctx->jbig2encoder = enc;
}

int fz_jbig2e_enabled(fz_context *ctx)
{
	return ctx && !!ctx->jbig2encoder;
}

fz_jbig2e *fz_new_jbig2e(fz_context *ctx)
{
	fz_jbig2_encoder *enc = (fz_jbig2_encoder *)ctx->jbig2encoder;

	if (ctx->jbig2encoder == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "No JBIG2Encoder registered!");

	return enc->init(ctx);
}

void
fz_jbig2e_feed_bitmap(fz_context *ctx, fz_jbig2e *inst, fz_bitmap *bit)
{
	fz_jbig2_encoder *enc = (fz_jbig2_encoder *)ctx->jbig2encoder;

	if (ctx->jbig2encoder == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "No JBIG2Encoder registered!");

	enc->feed_bitmap(ctx, inst, bit);
}

fz_buffer *
fz_jbig2e_pages_complete(fz_context *ctx, fz_jbig2e *inst)
{
	fz_jbig2_encoder *enc = (fz_jbig2_encoder *)ctx->jbig2encoder;

	if (ctx->jbig2encoder == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "No JBIG2Encoder registered!");

	return enc->pages_complete(ctx, inst);
}

fz_buffer *
fz_jbig2e_get_page(fz_context *ctx, fz_jbig2e *inst)
{
	fz_jbig2_encoder *enc = (fz_jbig2_encoder *)ctx->jbig2encoder;

	if (ctx->jbig2encoder == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "No JBIG2Encoder registered!");

	return enc->get_page(ctx, inst);
}

void fz_drop_jbig2e(fz_context *ctx, fz_jbig2e *inst)
{
	fz_jbig2_encoder *enc = (fz_jbig2_encoder *)ctx->jbig2encoder;

	if (ctx->jbig2encoder == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "No JBIG2Encoder registered!");

	enc->fin(ctx, inst);
}

#endif   // BUILDING_MUPDF_MINIMAL_CORE < 2
