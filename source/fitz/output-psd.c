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

#include <string.h>

#if FZ_ENABLE_RENDER_CORE 

void
fz_save_pixmap_as_psd(fz_context *ctx, const fz_pixmap *pixmap, const char *filename)
{
	fz_output *out = fz_new_output_with_path(ctx, filename, 0);
	fz_band_writer *writer = NULL;

	fz_var(writer);

	fz_try(ctx)
	{
		writer = fz_new_psd_band_writer(ctx, out);
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
fz_write_pixmap_as_psd(fz_context *ctx, fz_output *out, const fz_pixmap *pixmap)
{
	fz_band_writer *writer;

	if (!out)
		return;

	writer = fz_new_psd_band_writer(ctx, out);

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

typedef struct psd_band_writer_s
{
	fz_band_writer super;
	int num_additive;
} psd_band_writer;

static void
psd_write_header(fz_context *ctx, fz_band_writer *writer_, fz_colorspace *cs)
{
	psd_band_writer *writer = (psd_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;
	int w = writer->super.w;
	int h = writer->super.h;
	int s = writer->super.s;
	int n = writer->super.n;
	int c = n - writer->super.alpha - s;
	fz_separations *seps = writer->super.seps;
	int i;
	size_t len;
	static const char psdsig[12] = { '8', 'B', 'P', 'S', 0, 1, 0, 0, 0, 0, 0, 0 };
	static const char ressig[4] = { '8', 'B', 'I', 'M' };
	unsigned char *data;
	size_t size;
	fz_colorspace *cs_cmyk = cs;

#if FZ_ENABLE_ICC
	size = fz_buffer_storage(ctx, cs->u.icc.buffer, &data);
#else
	size = 0;
	data = NULL;
#endif

	if (cs->n != 4)
		cs_cmyk = fz_device_cmyk(ctx);

	if (!fz_colorspace_is_subtractive(ctx, cs))
		writer->num_additive = cs->n;

	/* File Header Section */
	fz_write_data(ctx, out, psdsig, 12);
	fz_write_int16_be(ctx, out, n);
	fz_write_int32_be(ctx, out, h);
	fz_write_int32_be(ctx, out, w);
	fz_write_int16_be(ctx, out, 8); /* bits per channel */

	switch (c)
	{
	case 0:
	case 1:
		fz_write_int16_be(ctx, out, 1); /* Greyscale */
		break;
	case 3:
		fz_write_int16_be(ctx, out, 3); /* RGB */
		break;
	case 4:
		fz_write_int16_be(ctx, out, 4); /* CMYK */
		break;
	default:
		fz_write_int16_be(ctx, out, 7); /* Multichannel */
		break;
	}

	/* Color Mode Data Section - empty */
	fz_write_int32_be(ctx, out, 0);

	/* Image Resources Section - Spot Names, Equivalent colors, resolution, ICC Profile */
	/* Spot names */
	len = 0;
	for (i = 0; i < s; i++)
	{
		const char *name = fz_separation_name(ctx, seps, i);
		char text[32];
		size_t len2;
		if (name == NULL)
		{
			fz_snprintf(text, sizeof text, "Spot%d", i-4);
			name = text;
		}
		len2 = strlen(name);
		if (len2 > 255)
			len2 = 255;
		len += len2 + 1;
	}

	/* Write the size of all the following resources */
	fz_write_int32_be(ctx, out,
			(s ? 12 + ((len + 1)&~1) : 0) +	/* Spot Names */
			(s ? 12 + (14 * s) : 0) +	/* DisplayInfo */
			28 +				/* Resolutions */
			(size ? (size+19)&~1 : 0));	/* ICC Profile */

	/* Spot names */
	if (s != 0)
	{
		fz_write_data(ctx, out, ressig, 4);
		fz_write_int16_be(ctx, out, 0x03EE);
		fz_write_int16_be(ctx, out, 0); /* PString */
		fz_write_int32_be(ctx, out, (len + 1)&~1);
		for (i = 0; i < s; i++) {
			size_t len2;
			const char *name = fz_separation_name(ctx, seps, i);
			char text[32];
			if (name == NULL)
			{
				fz_snprintf(text, sizeof text, "Spot%d", i-4);
				name = text;
			}
			len2 = strlen(name);
			if (len2 > 255)
				len2 = 255;
			fz_write_byte(ctx, out, (unsigned char)len2);
			fz_write_data(ctx, out, name, len2);
		}
		if (len & 1)
		{
			fz_write_byte(ctx, out, 0);
		}

		/* DisplayInfo - Colors for each spot channel */
		fz_write_data(ctx, out, ressig, 4);
		fz_write_int16_be(ctx, out, 0x03EF);
		fz_write_int16_be(ctx, out, 0); /* PString */
		fz_write_int32_be(ctx, out, 14 * s); /* Length */
		for (i = 0; i < s; i++) {
			float cmyk[4];
			fz_separation_equivalent(ctx, seps, i, cs_cmyk, cmyk, NULL, fz_default_color_params);
			fz_write_int16_be(ctx, out, 02); /* CMYK */
			/* PhotoShop stores all component values as if they were additive. */
			fz_write_int16_be(ctx, out, 65535 * (1-cmyk[0]));/* Cyan */
			fz_write_int16_be(ctx, out, 65535 * (1-cmyk[1]));/* Magenta */
			fz_write_int16_be(ctx, out, 65535 * (1-cmyk[2]));/* Yellow */
			fz_write_int16_be(ctx, out, 65535 * (1-cmyk[3]));/* Black */
			fz_write_int16_be(ctx, out, 0); /* Opacity 0 to 100 */
			fz_write_byte(ctx, out, 2); /* Don't know */
			fz_write_byte(ctx, out, 0); /* Padding - Always Zero */
		}
	}

	/* ICC Profile - (size + 19)&~1 bytes */
	if (size != 0)
	{
		/* Image Resource block */
		fz_write_data(ctx, out, ressig, 4);
		fz_write_int16_be(ctx, out, 0x40f); /* ICC Profile */
		fz_write_data(ctx, out, "\x07Profile", 8); /* Profile name (must be even!) */
		fz_write_int32_be(ctx, out, (int)size);
		fz_write_data(ctx, out, data, size); /* Actual data */
		if (size & 1)
			fz_write_byte(ctx, out, 0); /* Pad to even */
	}

	/* Image resolution - 28 bytes */
	fz_write_data(ctx, out, ressig, 4);
	fz_write_int16_be(ctx, out, 0x03ED);
	fz_write_int16_be(ctx, out, 0); /* PString */
	fz_write_int32_be(ctx, out, 16); /* Length */
	/* Resolution is specified as a fixed 16.16 bits */
	fz_write_int32_be(ctx, out, writer->super.xres);
	fz_write_int16_be(ctx, out, 1); /* width:  1 --> resolution is pixels per inch */
	fz_write_int16_be(ctx, out, 1); /* width:  1 --> resolution is pixels per inch */
	fz_write_int32_be(ctx, out, writer->super.yres);
	fz_write_int16_be(ctx, out, 1); /* height:  1 --> resolution is pixels per inch */
	fz_write_int16_be(ctx, out, 1); /* height:  1 --> resolution is pixels per inch */

	/* Layer and Mask Information Section */
	fz_write_int32_be(ctx, out, 0);

	/* Image Data Section */
	fz_write_int16_be(ctx, out, 0); /* Raw image data */
}

static void
psd_invert_buffer(unsigned char *buffer, int size)
{
	int k;

	for (k = 0; k < size; k++)
		buffer[k] = 255 - buffer[k];
}

static void
psd_write_band(fz_context *ctx, fz_band_writer *writer_, int stride, int band_start, int band_height, const unsigned char *sp)
{
	psd_band_writer *writer = (psd_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;
	int y, x, k, finalband;
	int w, h, n;
	unsigned char buffer[256];
	unsigned char *buffer_end = &buffer[sizeof(buffer)];
	unsigned char *b;
	int plane_inc;
	int line_skip;
	int num_additive = writer->num_additive;

	if (!out)
		return;

	memset(buffer, 0, nelem(buffer));

	w = writer->super.w;
	h = writer->super.h;
	n = writer->super.n;

	finalband = (band_start+band_height >= h);
	if (finalband)
		band_height = h - band_start;

	plane_inc = w * (h - band_height);
	line_skip = stride - w*n;
	b = buffer;
	if (writer->super.alpha)
	{
		const unsigned char *ap = &sp[n-1];
		for (k = 0; k < n-1; k++)
		{
			for (y = 0; y < band_height; y++)
			{
				for (x = 0; x < w; x++)
				{
					int a = *ap;
					ap += n;
					*b++ = a != 0 ? (*sp * 255 + 128)/a : 0;
					sp += n;
					if (b == buffer_end)
					{
						if (k >= num_additive)
							psd_invert_buffer(buffer, sizeof(buffer));
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
				if (k >= num_additive)
					psd_invert_buffer(buffer, sizeof(buffer));
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
			(void) b;
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
						if (k >= num_additive)
							psd_invert_buffer(buffer, sizeof(buffer));
						fz_write_data(ctx, out, buffer, sizeof(buffer));
						b = buffer;
					}
				}
				sp += line_skip;
			}
			sp -= stride * (ptrdiff_t)band_height - 1;
			if (b != buffer)
			{
				if (k >= num_additive)
					psd_invert_buffer(buffer, sizeof(buffer));
				fz_write_data(ctx, out, buffer, b - buffer);
				b = buffer;
			}
			fz_seek_output(ctx, out, plane_inc, SEEK_CUR);
		}
	}
	fz_seek_output(ctx, out, w * (band_height - h * (int64_t)n), SEEK_CUR);
}

static void
psd_write_trailer(fz_context *ctx, fz_band_writer *writer_)
{
	psd_band_writer *writer = (psd_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;

	(void)out;
	(void)writer;
}

static void
psd_drop_band_writer(fz_context *ctx, fz_band_writer *writer_)
{
	psd_band_writer *writer = (psd_band_writer *)(void *)writer_;

	(void)writer;
}

fz_band_writer *fz_new_psd_band_writer(fz_context *ctx, fz_output *out)
{
	psd_band_writer *writer = fz_new_band_writer(ctx, psd_band_writer, out);

	writer->super.header = psd_write_header;
	writer->super.band = psd_write_band;
	writer->super.trailer = psd_write_trailer;
	writer->super.drop = psd_drop_band_writer;

	writer->num_additive = 0;

	return &writer->super;
}

static fz_buffer *
psd_from_pixmap(fz_context *ctx, fz_pixmap *pix, fz_color_params color_params, int drop)
{
	fz_buffer *buf = NULL;
	fz_output *out = NULL;

	fz_var(buf);
	fz_var(out);

	fz_try(ctx)
	{
		buf = fz_new_buffer(ctx, 1024);
		out = fz_new_output_with_buffer(ctx, buf);
		fz_write_pixmap_as_psd(ctx, out, pix);
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		if (drop)
			fz_drop_pixmap(ctx, pix);
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
fz_new_buffer_from_image_as_psd(fz_context *ctx, const fz_image *image, fz_color_params color_params)
{
	fz_pixmap *pix = fz_get_pixmap_from_image(ctx, image, NULL, NULL, NULL, NULL);
	return psd_from_pixmap(ctx, pix, color_params, 1);
}

fz_buffer *
fz_new_buffer_from_pixmap_as_psd(fz_context *ctx, const fz_pixmap *pix, fz_color_params color_params)
{
	return psd_from_pixmap(ctx, (fz_pixmap *)pix, color_params, 0);
}

#endif
