// Copyright (C) 2004-2022 Artifex Software, Inc.
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

#if defined(HAVE_WEBP) && FZ_ENABLE_WEBP

#include <webp/decode.h>

fz_pixmap *
fz_load_webp(fz_context *ctx, const unsigned char *rbuf, size_t rlen)
{
	WebPBitstreamFeatures feat;
	fz_pixmap *pix;
	uint8_t *data;
	int error;

	error = WebPGetFeatures(rbuf, rlen, &feat);
	if (error)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode WebP image info");

	pix = fz_new_pixmap(ctx, fz_device_rgb(ctx), feat.width, feat.height, NULL, feat.has_alpha);
	if (feat.has_alpha)
		data = WebPDecodeRGBAInto(rbuf, rlen, pix->samples, pix->h * pix->stride, pix->stride);
	else
		data = WebPDecodeRGBInto(rbuf, rlen, pix->samples, pix->h * pix->stride, pix->stride);
	if (!data)
	{
		fz_drop_pixmap(ctx, pix);
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode WebP image data");
	}
	return pix;
}

void
fz_load_webp_info(fz_context *ctx, const unsigned char *rbuf, size_t rlen,
		int *wp, int *hp, int *xresp, int *yresp, fz_colorspace **cspacep, uint8_t* orientation)
{
	WebPBitstreamFeatures feat;
	int error;

	error = WebPGetFeatures(rbuf, rlen, &feat);
	if (error)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode WebP image info");

	if (wp) *wp = feat.width;
	if (hp) *hp = feat.height;
	if (xresp) *xresp = 96;
	if (yresp) *yresp = 96;
	if (orientation) *orientation = 1;
	if (cspacep) *cspacep = fz_keep_colorspace(ctx, fz_device_rgb(ctx));
}

#else

fz_pixmap *
fz_load_webp(fz_context *ctx, const unsigned char *rbuf, size_t rlen)
{
	fz_throw(ctx, FZ_ERROR_GENERIC, "WebP codec not available");
}

void
fz_load_webp_info(fz_context *ctx, const unsigned char *rbuf, size_t rlen,
		int *wp, int *hp, int *xresp, int *yresp, fz_colorspace **cspacep, uint8_t* orientation)
{
	fz_throw(ctx, FZ_ERROR_GENERIC, "WebP codec not available");
}

#endif
