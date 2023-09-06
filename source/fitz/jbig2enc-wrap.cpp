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

#ifdef HAVE_JBIG2ENC

#include "jbig2enc.h"

#include "allheaders.h"

extern "C" {

/*
	While developing this code, I ran into problems with Acrobat
	not liking streams that other decoders did. I have a hack here
	that causes it not to use a separate symbols stream that makes
	Acrobat happy. I think I've since fixed the problem (use JBIG2Globals
	rather than concatenate streams), but just in case, I leave this
	code in but disabled.
*/
#undef HACK_NO_GLOBALS

/* Horrible hack to allow us to free without memento interfering.
 * This is because jbig2enc doesn't allocate through memento. */
static void horrid_free(void *ptr)
{
	free(ptr);
}

#include "mupdf/fitz.h"

/* For the leptonica memory hacks... */
#include "leptonica-wrap.h"

typedef struct
{
	int xres;
	int yres;
} page_res;

struct fz_jbig2e
{
	struct jbig2ctx *jb2ctx;
	int max;
	int len;
	page_res *res;
	int n;
#ifdef HACK_NO_GLOBALS
	fz_buffer *buf;
#endif
};

static fz_jbig2e *
jb2e_init(fz_context *ctx)
{
	fz_jbig2e *jbig2e = fz_malloc_struct(ctx, fz_jbig2e);

	fz_set_leptonica_mem(ctx);
	jbig2e->jb2ctx = jbig2_init(0.85f, 0.5f, 0, 0, 0, -1);
	fz_clear_leptonica_mem(ctx);
	if (jbig2e->jb2ctx == NULL)
	{
		fz_free(ctx, jbig2e);
		fz_throw(ctx, FZ_ERROR_GENERIC, "JBig2 encoding init failed");
	}

	return jbig2e;
}

static inline int isbigendian(void)
{
	static const int one = 1;
	return *(char*)&one == 0;
}

static Pix *
pix_from_bitmap(fz_context *ctx, fz_bitmap *bit)
{
	Pix *image;

	fz_set_leptonica_mem(ctx);
	image = pixCreateHeader(bit->w, bit->h, 1);
	fz_clear_leptonica_mem(ctx);

	if (image == NULL)
		fz_throw(ctx, FZ_ERROR_SYSTEM, "JBIG2Enc image creation failed");
	pixSetData(image, (l_uint32 *)bit->samples);
	pixSetXRes(image, bit->xres);
	pixSetYRes(image, bit->yres);

	if (!isbigendian())
	{
		/* Frizzle the image */
		int x, y;
		uint32_t *d = (uint32_t *)bit->samples;
		for (y = bit->h; y > 0; y--)
			for (x = bit->w>>5; x > 0; x--)
			{
				uint32_t v = *d;
				((uint8_t *)d)[0] = v>>24;
				((uint8_t *)d)[1] = v>>16;
				((uint8_t *)d)[2] = v>>8;
				((uint8_t *)d)[3] = v;
				d++;
			}
	}
#ifdef DEBUG_DUMP_BITMAP
	fz_set_leptonica_mem(ctx);
	pixWrite("test.pnm", image, IFF_PNM);
	fz_clear_leptonica_mem(ctx);
#endif

	return image;
}

static void
jb2e_feed_bitmap(fz_context *ctx, fz_jbig2e *jbig2e, fz_bitmap *bitmap)
{
	Pix *pix;

	if (jbig2e->max == jbig2e->len)
	{
		int new_max = jbig2e->max * 2;
		if (new_max == 0)
			new_max = 4;

		jbig2e->res = (page_res *)fz_realloc(ctx, jbig2e->res, sizeof(*jbig2e->res) * new_max);
		jbig2e->max = new_max;
	}

	pix = pix_from_bitmap(ctx, bitmap);

	/* FIXME: Error handling is bad here in the encoder. */
	fz_set_leptonica_mem(ctx);
#ifdef HACK_NO_GLOBALS
	if (jbig2e->len > 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Only 1 image at a time with HACK_NO_GLOBALS");
	{
		uint8_t *res;
		int len;
		res = jbig2_encode_generic(pix, 0,0,0,0,&len);

		fz_try(ctx)
			jbig2e->buf = fz_new_buffer_from_copied_data(ctx, res, (size_t)len);
		fz_always(ctx)
			horrid_free(res);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}
#else
	jbig2_add_page(jbig2e->jb2ctx, pix);
#endif
	fz_clear_leptonica_mem(ctx);

	jbig2e->res[jbig2e->len].xres = bitmap->xres;
	jbig2e->res[jbig2e->len].yres = bitmap->yres;
	jbig2e->len++;
}

static fz_buffer *
mkbuf(fz_context *ctx, uint8_t *res, int len)
{
	fz_buffer *buf = NULL;

	fz_var(buf);

	fz_try(ctx)
		buf = fz_new_buffer_from_copied_data(ctx, res, (size_t)len);
	fz_always(ctx)
		horrid_free(res);
	fz_catch(ctx)
		fz_rethrow(ctx);

	return buf;
}

static fz_buffer *
jb2e_pages_complete(fz_context *ctx, fz_jbig2e *jbig2e)
{
#ifdef HACK_NO_GLOBALS
	return NULL;
#else
	int len;
	uint8_t *res;

	fz_set_leptonica_mem(ctx);
	res = jbig2_pages_complete(jbig2e->jb2ctx, &len);
	fz_clear_leptonica_mem(ctx);
	if (res == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "JBIG2 Encoding failure");

	return mkbuf(ctx, res, len);
#endif
}

static fz_buffer *
jb2e_get_page(fz_context *ctx, fz_jbig2e *jbig2e)
{
#ifdef HACK_NO_GLOBALS
	return jbig2e->buf;
#else
	int len;
	int n = jbig2e->n++;
	uint8_t *res;

	if (n == jbig2e->max)
		return NULL;
	if (n > jbig2e->max)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Can't encode more bitmaps than you have fed in!");

	fz_set_leptonica_mem(ctx);
	res = jbig2_produce_page(jbig2e->jb2ctx, n, jbig2e->res[n].xres, jbig2e->res[n].yres, &len);
	fz_clear_leptonica_mem(ctx);
	if (res == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "JBIG2 Encoding failure");

	return mkbuf(ctx, res, len);
#endif
}

static void
jb2e_fin(fz_context *ctx, fz_jbig2e *jbig2e)
{
	if (jbig2e == NULL)
		return;

	fz_set_leptonica_mem(ctx);
	jbig2_destroy(jbig2e->jb2ctx);
	fz_clear_leptonica_mem(ctx);
	fz_free(ctx, jbig2e->res);
	fz_free(ctx, jbig2e);
}

static const fz_jbig2_encoder encoder =
{
	jb2e_init,
	jb2e_feed_bitmap,
	jb2e_pages_complete,
	jb2e_get_page,
	jb2e_fin
};

const fz_jbig2_encoder *fz_jbig2enc_encoder(fz_context *ctx)
{
	return &encoder;
}

};

#endif /* HAVE_JBIG2ENC */
