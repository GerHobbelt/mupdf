// Copyright (C) 2004-2023 Artifex Software, Inc.
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

#if FZ_ENABLE_RENDER_CORE 

#include <jpeglib.h>
#include "jerror.h"
#include "mupdf/helpers/jmemcust.h"


#define JZ_CTX_FROM_CINFO(c) (fz_context *)((c)->client_data_ref)

#define OUTPUT_BUF_SIZE (16<<10)

typedef struct {
	struct jpeg_destination_mgr pub;
	fz_output *out;
	JOCTET buffer[OUTPUT_BUF_SIZE];
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

static void error_exit(j_common_ptr cinfo)
{
	char msg[JMSG_LENGTH_MAX];
	fz_context *ctx = JZ_CTX_FROM_CINFO(cinfo);
	cinfo->err->format_message(cinfo, msg);
	fz_throw(ctx, FZ_ERROR_GENERIC, "jpeg error: %s", msg);
}

static void init_destination(j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

static boolean empty_output_buffer(j_compress_ptr cinfo)
{
	fz_context *ctx = JZ_CTX_FROM_CINFO(cinfo);
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
	fz_output *out = dest->out;

	fz_write_data(ctx, out, dest->buffer, OUTPUT_BUF_SIZE);

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}

static void term_destination(j_compress_ptr cinfo)
{
	fz_context *ctx = JZ_CTX_FROM_CINFO(cinfo);
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
	fz_output *out = dest->out;
	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

	fz_write_data(ctx, out, dest->buffer, datacount);
}

void
fz_write_pixmap_as_jpeg(fz_context *ctx, fz_output *out, const fz_pixmap *pix, int quality)
{
	struct jpeg_compress_struct cinfo = { 0 };
	struct jpeg_error_mgr err = { 0 };

	my_destination_mgr dest;
	JSAMPROW row_pointer[1];
	unsigned char *outbuffer = NULL;
	size_t outsize = 0;
	fz_colorspace *cs = pix->colorspace;
	int n = pix->n;
	int alpha = pix->alpha;

	if (pix->s > 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "pixmap may not have separations to save as JPEG");
	if (cs && !fz_colorspace_is_gray(ctx, cs) && !fz_colorspace_is_rgb(ctx, cs) && !fz_colorspace_is_cmyk(ctx, cs))
		fz_throw(ctx, FZ_ERROR_GENERIC, "pixmap must be Grayscale, RGB, or CMYK to save as JPEG");

	/* Treat alpha only as greyscale */
	if (n == 1 && alpha)
		alpha = 0;
	n -= alpha;

	if (alpha > 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "pixmap may not have alpha to save as JPEG");

	cinfo.mem = NULL;
	cinfo.global_state = 0;
	cinfo.err = jpeg_std_error(&err);
	err.error_exit = error_exit;

	cinfo.client_data_ref = (void*)ctx;
	cinfo.client_init_callback = fz_jpeg_sys_mem_register;

	fz_try(ctx)
	{
		jpeg_create_compress(&cinfo);

		cinfo.dest = (void*) &dest;
		dest.pub.init_destination = init_destination;
		dest.pub.empty_output_buffer = empty_output_buffer;
		dest.pub.term_destination = term_destination;
		dest.out = out;

		cinfo.image_width = pix->w;
		cinfo.image_height = pix->h;
		cinfo.input_components = n;
		switch (n) {
		case 1:
			cinfo.in_color_space = JCS_GRAYSCALE;
			break;
		case 3:
			cinfo.in_color_space = JCS_RGB;
			break;
		case 4:
			cinfo.in_color_space = JCS_CMYK;
			break;
		}

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, quality, FALSE);

		/* Write image resolution */
		cinfo.density_unit = 1; /* dots/inch */
		cinfo.X_density = pix->xres;
		cinfo.Y_density = pix->yres;

		/* Disable chroma subsampling */
		cinfo.comp_info[0].h_samp_factor = 1;
		cinfo.comp_info[0].v_samp_factor = 1;

		/* Progressive JPEGs are smaller */
		jpeg_simple_progression(&cinfo);

		jpeg_start_compress(&cinfo, TRUE);

		if (fz_colorspace_is_subtractive(ctx, pix->colorspace))
		{
			// treat `fz_pixmap *pix` as mutable:
			fz_invert_pixmap_raw(ctx, (fz_pixmap *)pix);
		}

		while (cinfo.next_scanline < cinfo.image_height) {
			row_pointer[0] = &pix->samples[cinfo.next_scanline * pix->stride];
			(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}

		if (fz_colorspace_is_subtractive(ctx, pix->colorspace))
		{
			// treat `fz_pixmap *pix` as mutable: reverse the invert op now.
			fz_invert_pixmap_raw(ctx, (fz_pixmap*)pix);
		}

		jpeg_finish_compress(&cinfo);

		fz_write_data(ctx, out, outbuffer, outsize);
	}
	fz_always(ctx)
	{
		jpeg_destroy_compress(&cinfo);
		fz_free(ctx, outbuffer);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void
fz_save_pixmap_as_jpeg(fz_context *ctx, const fz_pixmap *pixmap, const char *filename, int quality)
{
	fz_output *out = fz_new_output_with_path(ctx, filename, 0);
	fz_try(ctx)
	{
		fz_write_pixmap_as_jpeg(ctx, out, pixmap, quality);
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

static fz_buffer *
jpeg_from_pixmap(fz_context *ctx, fz_pixmap *pix, fz_color_params color_params, int quality, int drop)
{
	fz_buffer *buf = NULL;
	fz_output *out = NULL;

	fz_var(buf);
	fz_var(out);

	fz_try(ctx)
	{
		buf = fz_new_buffer(ctx, 1024);
		out = fz_new_output_with_buffer(ctx, buf);
		fz_write_pixmap_as_jpeg(ctx, out, pix, quality);
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
fz_new_buffer_from_image_as_jpeg(fz_context *ctx, const fz_image *image, fz_color_params color_params, int quality)
{
	fz_pixmap *pix = fz_get_pixmap_from_image(ctx, image, NULL, NULL, NULL, NULL);
	return jpeg_from_pixmap(ctx, pix, color_params, quality, 1);
}

fz_buffer *
fz_new_buffer_from_pixmap_as_jpeg(fz_context *ctx, const fz_pixmap *pix, fz_color_params color_params, int quality)
{
	return jpeg_from_pixmap(ctx, (fz_pixmap*)pix, color_params, quality, 0);
}

#endif
