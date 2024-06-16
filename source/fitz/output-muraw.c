// Copyright (C) 2004-2024 Artifex Software, Inc.
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

#include "z-imp.h"

#include <string.h>

#if FZ_ENABLE_RENDER_CORE 

static void putchunk(fz_context *ctx, fz_output *out, const char *tag, const void *data, size_t size)
{
	if ((uint32_t)size != size)
		fz_throw(ctx, FZ_ERROR_GENERIC, "PNG chunk too large");

	fz_write_int32_le(ctx, out, (int)size);
	fz_write_data(ctx, out, tag, strlen(tag));
	fz_write_data(ctx, out, data, size);

#if 0
	unsigned int sum;

	sum = zng_crc32(0, NULL, 0);
	sum = zng_crc32(sum, (unsigned char*)tag, 4);
	sum = zng_crc32(sum, data, (unsigned int)size);
	fz_write_int32_le(ctx, out, sum);
#endif
}

void
fz_save_pixmap_as_muraw(fz_context *ctx, const fz_pixmap *pixmap, const char *filename)
{
	fz_output *out = fz_new_output_with_path(ctx, filename, 0);
	fz_band_writer *writer = NULL;

	fz_var(writer);

	fz_try(ctx)
	{
		writer = fz_new_muraw_band_writer(ctx, out);
		fz_write_header(ctx, writer, pixmap->w, pixmap->h, pixmap->n, pixmap->alpha, pixmap->xres, pixmap->yres, 0, pixmap->colorspace, pixmap->seps);
		fz_write_band(ctx, writer, pixmap->stride, pixmap->h, pixmap->samples);
		fz_close_band_writer(ctx, writer);
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_band_writer(ctx, writer);
		fz_drop_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void
fz_write_pixmap_as_muraw(fz_context *ctx, fz_output *out, const fz_pixmap *pixmap)
{
	fz_band_writer *writer;

	if (!out)
		return;

	writer = fz_new_muraw_band_writer(ctx, out);

	fz_try(ctx)
	{
		fz_write_header(ctx, writer, pixmap->w, pixmap->h, pixmap->n, pixmap->alpha, pixmap->xres, pixmap->yres, 0, pixmap->colorspace, pixmap->seps);
		fz_write_band(ctx, writer, pixmap->stride, pixmap->h, pixmap->samples);
	}
	fz_always(ctx)
	{
		fz_close_band_writer(ctx, writer);
		fz_drop_band_writer(ctx, writer);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

typedef struct muraw_band_writer_s
{
	fz_band_writer super;
	unsigned char *data;
	uLong datasize;
	int stream_ended;
} muraw_band_writer;

static void
muraw_write_icc(fz_context *ctx, muraw_band_writer *writer, fz_colorspace *cs)
{
#if FZ_ENABLE_ICC
	if (cs)
	{
		fz_output *out = writer->super.out;
		size_t size;
		fz_buffer *buffer = cs->u.icc.buffer;
		const char *name;

		if (!buffer || buffer->len == 0)
			return;

		name = cs->name;
		size = strlen(name) + 1;

		putchunk(ctx, out, "iCCprofile", name, size);
		putchunk(ctx, out, "colorspace", cs, sizeof(*cs));
		putchunk(ctx, out, "colorspace-buffer", buffer, buffer->len);
	}
#endif
}

static void
muraw_write_header(fz_context *ctx, fz_band_writer *writer_, fz_colorspace *cs)
{
	muraw_band_writer *writer = (muraw_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;

	putchunk(ctx, out, "header", &writer->super, sizeof(writer->super));

	uint32_t rez[2];
	rez[0] = writer->super.xres * 100/2.54f + 0.5f;
	rez[1] = writer->super.yres * 100/2.54f + 0.5f;
	putchunk(ctx, out, "physical-size", rez, sizeof(rez));

	muraw_write_icc(ctx, writer, cs);
}

static void
muraw_write_band(fz_context* ctx, fz_band_writer* writer_, int stride, int band_start, int band_height, const unsigned char* sp)
{
	muraw_band_writer* writer = (muraw_band_writer*)(void*)writer_;
	fz_output* out = writer->super.out;

	struct info
	{
		int stride;
		int band_start;
		int band_height;
	} hdr = { stride, band_start, band_height };

	if (!out)
		return;

	putchunk(ctx, out, "band-header", &hdr, sizeof(hdr));
	putchunk(ctx, out, "band-writer", &writer->super, sizeof(writer->super));

	// lifted from PSD output:

	int y, x, k, finalband;
	int w, h, n;
	unsigned char buffer[256];
	unsigned char* buffer_end = &buffer[sizeof(buffer)];
	unsigned char* b;
	int plane_inc;
	int line_skip;

	w = writer->super.w;
	h = writer->super.h;
	n = writer->super.n;

	finalband = (band_start + band_height >= h);
	if (finalband)
		band_height = h - band_start;

	plane_inc = w * (h - band_height);
	line_skip = stride - w * n;
	b = buffer;
	if (writer->super.alpha)
	{
		const unsigned char* ap = &sp[n - 1];
		for (k = 0; k < n - 1; k++)
		{
			for (y = 0; y < band_height; y++)
			{
				for (x = 0; x < w; x++)
				{
					int a = *ap;
					ap += n;
					*b++ = a != 0 ? (*sp * 255 + 128) / a : 0;
					sp += n;
					if (b == buffer_end)
					{
						fz_write_data(ctx, out, buffer, sizeof(buffer));
						b = buffer;
					}
				}
				sp += line_skip;
				ap += line_skip;
			}
			sp -= stride * (ptrdiff_t)band_height - 1;
			ap -= stride * (ptrdiff_t)band_height;
			if (b != buffer)
			{
				fz_write_data(ctx, out, buffer, b - buffer);
				b = buffer;
			}
			fz_seek_output(ctx, out, plane_inc, SEEK_CUR);
		}
		for (y = 0; y < band_height; y++)
		{
			for (x = 0; x < w; x++)
			{
				*b++ = *sp;
				sp += n;
				if (b == buffer_end)
				{
					fz_write_data(ctx, out, buffer, sizeof(buffer));
					b = buffer;
				}
			}
			sp += line_skip;
		}
		if (b != buffer)
		{
			fz_write_data(ctx, out, buffer, b - buffer);
			b = buffer;
		}
		fz_seek_output(ctx, out, plane_inc, SEEK_CUR);
	}
	else
	{
		for (k = 0; k < n; k++)
		{
			for (y = 0; y < band_height; y++)
			{
				for (x = 0; x < w; x++)
				{
					*b++ = *sp;
					sp += n;
					if (b == buffer_end)
					{
						fz_write_data(ctx, out, buffer, sizeof(buffer));
						b = buffer;
					}
				}
				sp += line_skip;
			}
			sp -= stride * (ptrdiff_t)band_height - 1;
			if (b != buffer)
			{
				fz_write_data(ctx, out, buffer, b - buffer);
				b = buffer;
			}
			fz_seek_output(ctx, out, plane_inc, SEEK_CUR);
		}
	}
	fz_seek_output(ctx, out, w * (band_height - h * (int64_t)n), SEEK_CUR);
}

static void
muraw_write_trailer(fz_context *ctx, fz_band_writer *writer_)
{
	muraw_band_writer *writer = (muraw_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;
	static const char block[1] = { 0 };

	writer->stream_ended = 1;

	putchunk(ctx, out, "trailer", &writer->super, sizeof(writer->super));

	putchunk(ctx, out, "IEND", block, 0);
}

static void
muraw_drop_band_writer(fz_context *ctx, fz_band_writer *writer_)
{
	muraw_band_writer *writer = (muraw_band_writer *)(void *)writer_;
}

fz_band_writer *fz_new_muraw_band_writer(fz_context *ctx, fz_output *out)
{
	muraw_band_writer *writer = fz_new_band_writer(ctx, muraw_band_writer, out);

	writer->super.header = muraw_write_header;
	writer->super.band = muraw_write_band;
	writer->super.trailer = muraw_write_trailer;
	writer->super.drop = muraw_drop_band_writer;

	return &writer->super;
}

/* We use an auxiliary function to do pixmap_as_muraw, as it can enable us to
 * drop pix early in the case where we have to convert, potentially saving
 * us having to have 2 copies of the pixmap and a buffer open at once. */
static fz_buffer *
muraw_from_pixmap(fz_context *ctx, fz_pixmap *pix, fz_color_params color_params, int drop)
{
	fz_buffer *buf = NULL;
	fz_output *out = NULL;
	fz_pixmap *pix2 = NULL;

	fz_var(buf);
	fz_var(out);
	fz_var(pix2);

	if (pix->w == 0 || pix->h == 0)
	{
		if (drop)
			fz_drop_pixmap(ctx, pix);
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
		fz_write_pixmap_as_muraw(ctx, out, pix);
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
fz_new_buffer_from_image_as_muraw(fz_context *ctx, const fz_image *image, fz_color_params color_params)
{
	fz_pixmap *pix = fz_get_pixmap_from_image(ctx, image, NULL, NULL, NULL, NULL);
	return muraw_from_pixmap(ctx, pix, color_params, 1);
}

fz_buffer *
fz_new_buffer_from_pixmap_as_muraw(fz_context *ctx, const fz_pixmap *pix, fz_color_params color_params)
{
	return muraw_from_pixmap(ctx, (fz_pixmap *)pix, color_params, 0);
}

#endif
