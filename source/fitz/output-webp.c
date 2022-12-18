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

#if FZ_ENABLE_RENDER_CORE 

#include <string.h>
#include <webp/encode.h>

void
fz_save_pixmap_as_webp(fz_context *ctx, const fz_pixmap *pixmap, const char *filename)
{
	fz_output *out = fz_new_output_with_path(ctx, filename, 0);
	uint8_t *data = NULL;
	size_t size;

	fz_var(data);

	fz_try(ctx)
	{
		if (pixmap->alpha)
			size = WebPEncodeLosslessRGBA(pixmap->samples, pixmap->w, pixmap->h, pixmap->stride, &data);
		else
			size = WebPEncodeLosslessRGB(pixmap->samples, pixmap->w, pixmap->h, pixmap->stride, &data);
		fz_write_data(ctx, out, data, size);
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		WebPFree(data);
		fz_drop_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void
fz_write_pixmap_as_webp(fz_context *ctx, fz_output *out, const fz_pixmap *pixmap)
{
	uint8_t *data = NULL;
	size_t size;

	fz_var(data);

	if (!out)
		return;

	fz_try(ctx)
	{
		if (pixmap->alpha)
			size = WebPEncodeLosslessRGBA(pixmap->samples, pixmap->w, pixmap->h, pixmap->stride, &data);
		else
			size = WebPEncodeLosslessRGB(pixmap->samples, pixmap->w, pixmap->h, pixmap->stride, &data);
		fz_write_data(ctx, out, data, size);
	}
	fz_always(ctx)
	{
		WebPFree(data);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

/* We use an auxiliary function to do pixmap_as_webp, as it can enable us to
 * drop pix early in the case where we have to convert, potentially saving
 * us having to have 2 copies of the pixmap and a buffer open at once. */
static fz_buffer *
webp_from_pixmap(fz_context *ctx, fz_pixmap *pix, fz_color_params color_params, int drop)
{
	fz_buffer *buf = NULL;
	fz_output *out = NULL;
	fz_pixmap *pix2 = NULL;
	fz_cookie* cookie = ctx->cookie;

	fz_var(buf);
	fz_var(out);
	fz_var(pix2);

	if (pix->w == 0 || pix->h == 0)
	{
		if (drop)
			fz_drop_pixmap(ctx, pix);
		if (!cookie->d.ignore_minor_errors)
			fz_throw(ctx, FZ_ERROR_GENERIC, "content error: image dimensions are specified as (0 x 0)");
#pragma message("TODO: throw exception in strict mode. Also check out 'ignore_errors' in mudraw tool and link this to that setting.")
		return NULL;
	}

	fz_try(ctx)
	{
		if (pix->colorspace && pix->colorspace != fz_device_gray(ctx) && pix->colorspace != fz_device_rgb(ctx))
		{
			pix2 = fz_convert_pixmap(ctx, pix, fz_device_rgb(ctx), NULL, NULL, color_params, 1);
			if (drop)
				fz_drop_pixmap(ctx, pix);
			pix = pix2;
		}
		buf = fz_new_buffer(ctx, 1024);
		out = fz_new_output_with_buffer(ctx, buf);
		fz_write_pixmap_as_webp(ctx, out, pix);
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_pixmap(ctx, drop ? pix : pix2);
		fz_drop_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_drop_buffer(ctx, buf);
		fz_rethrow(ctx);
	}
	return buf;
}

fz_buffer *
fz_new_buffer_from_image_as_webp(fz_context *ctx, const fz_image *image, fz_color_params color_params)
{
	fz_pixmap *pix = fz_get_pixmap_from_image(ctx, image, NULL, NULL, NULL, NULL);
	return webp_from_pixmap(ctx, pix, color_params, 1);
}

fz_buffer *
fz_new_buffer_from_pixmap_as_webp(fz_context *ctx, const fz_pixmap *pix, fz_color_params color_params)
{
	return webp_from_pixmap(ctx, (fz_pixmap *)pix, color_params, 0);
}

typedef struct webp_band_writer_s
{
	fz_band_writer super;
	unsigned char *data;
	size_t size;
	int stride;
} webp_band_writer;

static void
webp_write_header(fz_context *ctx, fz_band_writer *writer_, fz_colorspace *cs)
{
}

static void
webp_write_band(fz_context *ctx, fz_band_writer *writer_, int stride, int band_start, int band_height, const unsigned char *p)
{
	webp_band_writer *writer = (webp_band_writer *) writer_;

	if (writer->stride == 0)
		writer->stride = stride;

	writer->data = fz_realloc(ctx, writer->data, writer->size + stride * band_height);
	memcpy(&writer->data[writer->size], p, stride * band_height);
}

static void
webp_close_band_writer(fz_context *ctx, fz_band_writer *writer_)
{
	webp_band_writer *writer = (webp_band_writer *) writer_;
	uint8_t *data = NULL;
	size_t size = 0;

	fz_var(data);

	fz_try(ctx)
	{
		if (writer->super.alpha)
			size = WebPEncodeLosslessRGBA(writer->data, writer->super.w, writer->super.h, writer->stride, &data);
		else
			size = WebPEncodeLosslessRGB(writer->data, writer->super.w, writer->super.h, writer->stride, &data);
		fz_write_data(ctx, writer->super.out, data, size);
	}
	fz_always(ctx)
		WebPFree(data);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
webp_drop_band_writer(fz_context *ctx, fz_band_writer *writer_)
{
	webp_band_writer *writer = (webp_band_writer *) writer_;

	fz_free(ctx, writer->data);
}

fz_band_writer *fz_new_webp_band_writer(fz_context *ctx, fz_output *out)
{
	webp_band_writer *writer = fz_new_band_writer(ctx, webp_band_writer, out);

	writer->super.header = webp_write_header;
	writer->super.band = webp_write_band;
	writer->super.close = webp_close_band_writer;
	writer->super.drop = webp_drop_band_writer;

	return &writer->super;
}

void fz_default_webp_quality(float quality)
{
	//TODO
}

#endif

