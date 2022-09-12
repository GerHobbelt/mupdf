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

#include "webp/encode.h"
#include "webp/mux.h"

#include <string.h>
#include <math.h>

typedef struct webp_band_writer_s
{
	fz_band_writer super;

	WebPMux* mux;
	WebPConfig config;
	WebPPicture pic;
	WebPPicture band_scratch_pic;
	WebPAuxStats stats;
	WebPMemoryWriter wrt;
	fz_context* ctx;
	const char* filepath;
	int stream_ended;
} webp_band_writer;


static float webp_quality = 90.0f;

static int webp_encode_progress_reporter(int percent, const WebPPicture* picture)
{
	webp_band_writer* writer = (webp_band_writer*)picture->user_data;
	assert0(writer != NULL);
	fz_context* ctx = writer->ctx;
	assert0(ctx != NULL);

	fz_info(ctx, "WEbP encoding @ %d%% (%s)\n", percent, fz_basename(writer->filepath));

	return 1;
}

static void webp_init_config(webp_band_writer* writer, float webp_quality)
{
	fz_context* ctx = writer->ctx;

	// Setup a config, starting from a preset and tuning some additional parameters
	assert(webp_quality < 0 || webp_quality >= 1.0f);
	if (webp_quality < 0)
	{
		int level = lroundf(-webp_quality);

		if (!WebPConfigLosslessPreset(&writer->config, level))
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "WEBP library configuration error (unsupported lossless preset %d)", level);
		}

		writer->pic.use_argb = 1;
		assert(writer->config.lossless);
	}
	else
	{
		if (!WebPConfigPreset(&writer->config, WEBP_PRESET_TEXT, webp_quality))
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "WEBP library configuration error (lossy preset %d @ quality %.1f%%", WEBP_PRESET_TEXT, webp_quality);
		}
		// ... additional tuning
		writer->config.sns_strength = 90;
		writer->config.filter_sharpness = 6;

		writer->pic.use_argb = 0;
		assert(!writer->config.lossless);
	}

	if (!WebPValidateConfig(&writer->config))
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "WEBP configuration is invalid");
	}
}

static int webp_memory_writer_handler(const uint8_t* data, size_t data_size, const WebPPicture* picture)
{
	return 0;
}

void
fz_save_pixmap_as_webp(fz_context *ctx, const fz_pixmap *pixmap, const char *filename)
{
	fz_output *out = fz_new_output_with_path(ctx, filename, 0);
	fz_band_writer *writer = NULL;

	fz_var(writer);

	fz_try(ctx)
	{
		writer = fz_new_webp_band_writer(ctx, out);
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
fz_write_pixmap_as_webp(fz_context *ctx, fz_output *out, const fz_pixmap *pixmap)
{
	fz_band_writer *writer;

	if (!out)
		return;

	writer = fz_new_webp_band_writer(ctx, out);

	fz_try(ctx)
	{
		fz_write_header(ctx, writer, pixmap->w, pixmap->h, pixmap->n, pixmap->alpha, pixmap->xres, pixmap->yres, 0, pixmap->colorspace, pixmap->seps);
		fz_write_band(ctx, writer, pixmap->stride, pixmap->h, pixmap->samples);
		fz_close_band_writer(ctx, writer);
	}
	fz_always(ctx)
	{
		fz_drop_band_writer(ctx, writer);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

static void
webp_write_icc(fz_context *ctx, webp_band_writer *writer, fz_colorspace *cs)
{
#if FZ_ENABLE_ICC
	if (cs && !(cs->flags & FZ_COLORSPACE_IS_DEVICE) && (cs->flags & FZ_COLORSPACE_IS_ICC) && cs->u.icc.buffer)
	{
		fz_output *out = writer->super.out;
		size_t size;
		fz_buffer *buffer = cs->u.icc.buffer;
		fz_buffer *cbuffer;
		unsigned char *pos, *chunk = NULL;
		const char *name;

		if (!buffer || buffer->len == 0)
			return;

		/* Deflate the profile */
		cbuffer = fz_deflate(ctx, buffer);

		name = cs->name;
		size = cbuffer->len + strlen(name) + 2;

		fz_try(ctx)
		{
			chunk = fz_calloc(ctx, size, 1);
			pos = chunk;
			memcpy(chunk, name, strlen(name)); //-V575
			pos += strlen(name) + 2;
			memcpy(pos, cbuffer->data, cbuffer->len);
			//putchunk(ctx, out, "iCCP", chunk, size);
			//WebPMuxSetChunk(mux, "ICCP", &icc_profile, copy_data);
		}
		fz_always(ctx)
		{
			fz_drop_buffer(ctx, cbuffer);
			fz_free(ctx, chunk);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}
#endif
}

static void
webp_write_header(fz_context *ctx, fz_band_writer *writer_, fz_colorspace *cs)
{
	webp_band_writer *writer = (webp_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;
	int w = writer->super.w;
	int h = writer->super.h;
	int n = writer->super.n;
	int alpha = writer->super.alpha;

	WebPPicture* pix = &writer->pic;

	pix->width = w;
	pix->height = h;
	assert(pix->custom_ptr == writer);
	pix->progress_hook = webp_encode_progress_reporter;
	assert(pix->user_data == writer);

	webp_init_config(writer, webp_quality);

	if (writer->super.s != 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "WebP cannot contain spot colors");
	if (fz_colorspace_type(ctx, cs) == FZ_COLORSPACE_BGR)
		fz_throw(ctx, FZ_ERROR_GENERIC, "WebP: pixmap can not be BGR");

	/* Treat alpha only as greyscale */
	if (n == 1 && alpha)
		alpha = 0;

	int color;
	switch (n - alpha)
	{
	case 1: color = (alpha ? 4 : 0); break; /* 0 = Greyscale, 4 = Greyscale + Alpha */
	case 3: color = (alpha ? 6 : 2); break; /* 2 = RGB, 6 = RGBA */
	default:
		fz_throw(ctx, FZ_ERROR_GENERIC, "pixmap must be grayscale or rgb to write as webp");
	}

	writer->band_scratch_pic = writer->pic;
	assert(writer->band_scratch_pic.a == NULL);
	assert(writer->band_scratch_pic.argb == NULL);
	assert(writer->band_scratch_pic.memory_ == NULL);
	assert(writer->band_scratch_pic.y == NULL);
	assert(writer->band_scratch_pic.u == NULL);
	assert(writer->band_scratch_pic.v == NULL);

	if (!WebPPictureAlloc(&writer->pic))
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "WebP: out of memory while allocating space for the image buffer");
	}
}

/**
	Set the preferred/default quality for any subsequent WebP output.

	A **negative** quality signals you want **lossless** compression;
	a **positive** quality signals you want a **lossy** compression with the given quality percentage (1..100%)

	When the given quality is ZERO, a factory default quality is assumed.
*/
void fz_default_webp_quality(float quality)
{
	if (quality < 0)
	{
		int level = lroundf(-quality);

		if (level >= 1 && level <= 9 /* Webp: MAX_LEVEL */ )
			webp_quality = quality;
		else
			webp_quality = -9;
	}
	else
	{
		if (quality >= 1.0f && quality <= 100.0f)
			webp_quality = quality;
		else
			webp_quality = 90.0f;
	}
}

static void
webp_write_band(fz_context *ctx, fz_band_writer *writer_, int stride, int band_start, int band_height, const unsigned char *sp)
{
	webp_band_writer *writer = (webp_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;

#if 0
	unsigned char *dp;
	int y, x, k, err, finalband;
	int w, h, n;
	size_t remain;

	if (!out)
		return;

	w = writer->super.w;
	h = writer->super.h;
	n = writer->super.n;

	finalband = (band_start + band_height >= h);
	if (finalband)
		band_height = h - band_start;

	dp = writer->udata;
	stride -= w*n;
	assert(stride >= 0);
	if (writer->super.alpha)
	{
		/* Un-premultiply data */
		for (y = 0; y < band_height; y++)
		{
			int prev[FZ_MAX_COLORS];
			*dp++ = 1; /* sub prediction filter */
			for (x = 0; x < w; x++)
			{
				int a = sp[n-1];
				int inva = a ? 256*255/a : 0;
				int p;
				for (k = 0; k < n-1; k++)
				{
					int v = (sp[k] * inva + 128)>>8;
					p = x ? prev[k] : 0;
					prev[k] = v;
					v -= p;
					dp[k] = v;
				}
				p = x ? prev[k] : 0;
				prev[k] = a;
				a -= p;
				dp[k] = a;
				sp += n;
				dp += n;
			}
			sp += stride;
		}
	}
	else
	{
		for (y = 0; y < band_height; y++)
		{
			*dp++ = 1; /* sub prediction filter */
			for (x = 0; x < w; x++)
			{
				for (k = 0; k < n; k++)
				{
					if (x == 0)
						dp[k] = sp[k];
					else
						dp[k] = sp[k] - sp[k-n];
				}
				sp += n;
				dp += n;
			}
			sp += stride;
		}
	}
#endif

	if (!out)
		return;

	int w = writer->super.w;
	int h = writer->super.h;
	int n = writer->super.n;
	int alpha = writer->super.alpha;

	int finalband = (band_start + band_height >= h);
	if (finalband)
		band_height = h - band_start;

	int WebPPictureAlloc(WebPPicture * picture);


}

static void
webp_write_trailer(fz_context *ctx, fz_band_writer *writer_)
{
	webp_band_writer *writer = (webp_band_writer *)(void *)writer_;
	fz_output *out = writer->super.out;

	writer->stream_ended = 1;
}

static void
webp_close_band_writer(fz_context *ctx, fz_band_writer *writer_)
{
	webp_band_writer *writer = (webp_band_writer *)(void *)writer_;

	if (!writer->stream_ended)
	{
	}
}

static void
webp_drop_band_writer(fz_context* ctx, fz_band_writer* writer_)
{
	webp_band_writer* writer = (webp_band_writer*)(void*)writer_;

	if (!writer->stream_ended)
	{
	}
}

static int webp_lib_initialized = 0;

fz_band_writer *fz_new_webp_band_writer(fz_context *ctx, fz_output *out)
{
	webp_band_writer *writer = fz_new_band_writer(ctx, webp_band_writer, out);

	writer->super.header = webp_write_header;
	writer->super.band = webp_write_band;
	writer->super.trailer = webp_write_trailer;
	writer->super.close = webp_close_band_writer;
	writer->super.drop = webp_drop_band_writer;

	writer->ctx = ctx;

	fz_try(ctx)
	{
		if (!webp_lib_initialized)
		{
#if 0
			WebPMemoryWriterInit(&memory_writer);
			if (!WebPConfigInit(&config)) {
				fz_throw(ctx, FZ_ERROR_GENERIC, "WEBP error: version mismatch!");
			}
#endif
			webp_lib_initialized = 1;
		}

		writer->mux = WebPMuxNew();
		if (!writer->mux)
			fz_throw(ctx, FZ_ERROR_GENERIC, "WEBP writer: out of memory allocating control structures");

		// Setup the input data
		if (!WebPPictureInit(&writer->pic))
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "WEBP error: failed to set up the picture storage container");
		}
		writer->pic.stats = &writer->stats;

		// Set up a byte-output write method. 
		WebPMemoryWriterInit(&writer->wrt);

		writer->pic.writer = webp_memory_writer_handler;
		writer->pic.custom_ptr = writer;

		webp_init_config(writer, webp_quality);

		writer->band_scratch_pic = writer->pic;
	}
	fz_catch(ctx)
	{
		if (writer)
		{
			WebPMuxDelete(writer->mux);
		}
		fz_free(ctx, writer);

		fz_rethrow(ctx);
	}

	return &writer->super;
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
