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

#include <stdio.h>
#include <jpeglib.h>
#include "jerror.h"


typedef struct
{
	fz_stream *chain;
	fz_stream *jpegtables;
	fz_stream *curr_stm;
	fz_context *ctx;
	int color_transform;
	int invert_cmyk; /* has inverted CMYK polarity */
	int init;
	int stride;
	int l2factor;
	unsigned char *scanline;
	unsigned char *rp, *wp;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_source_mgr srcmgr;
	struct jpeg_error_mgr errmgr;
	jmp_buf jb;
	char msg[JMSG_LENGTH_MAX];

	unsigned char buffer[4096];
} fz_dctd;


#define JZ_DCT_STATE_FROM_CINFO(c) (fz_dctd *)((c)->client_data_ref)

static void *
fz_dct_mem_alloc(j_common_ptr cinfo, size_t size)
{
	fz_dctd *state = JZ_DCT_STATE_FROM_CINFO(cinfo);
	fz_context* ctx = state->ctx;
	return Memento_label(fz_malloc_no_throw(ctx, size), "dct_alloc");
}

static void
fz_dct_mem_free(j_common_ptr cinfo, void *object, size_t size)
{
	fz_dctd *state = JZ_DCT_STATE_FROM_CINFO(cinfo);
	fz_context* ctx = state->ctx;
	fz_free(ctx, object);
}

static long
fz_dct_mem_init(j_common_ptr cinfo)
{
	return 0; 			/* just set max_memory_to_use to 0 */
}

static void
fz_dct_mem_term(j_common_ptr cinfo)
{
	cinfo->client_data_ref = NULL;
	cinfo->client_init_callback = NULL;
}


static size_t fz_dct_mem_available(j_common_ptr cinfo, size_t min_bytes_needed,
	size_t max_bytes_needed,
	size_t already_allocated)
{
	// Here we always say, "we got all you want bud!"
	long ret = max_bytes_needed;

	return ret;
}

static void fz_dct_open_backing_store(j_common_ptr cinfo,
	backing_store_ptr info,
	long total_bytes_needed)
{
	ERREXIT(cinfo, JERR_NO_BACKING_STORE);
}



static int fz_jpeg_dct_sys_mem_register(j_common_ptr _cinfo)
{
	struct jpeg_decompress_struct* cinfo = (struct jpeg_decompress_struct*)_cinfo;

	cinfo->sys_mem_if.get_small = fz_dct_mem_alloc;
	cinfo->sys_mem_if.free_small = fz_dct_mem_free;

	cinfo->sys_mem_if.get_large = fz_dct_mem_alloc;
	cinfo->sys_mem_if.free_large = fz_dct_mem_free;

	cinfo->sys_mem_if.mem_available = fz_dct_mem_available;

	cinfo->sys_mem_if.open_backing_store = fz_dct_open_backing_store;

	cinfo->sys_mem_if.mem_init = fz_dct_mem_init;
	cinfo->sys_mem_if.mem_term = fz_dct_mem_term;

	return 0;
}

static void error_exit_dct(j_common_ptr cinfo)
{
	char msg[JMSG_LENGTH_MAX];
	fz_dctd *state = JZ_DCT_STATE_FROM_CINFO(cinfo);
	fz_context* ctx = state->ctx;
	cinfo->err->format_message(cinfo, msg);
	fz_throw(ctx, FZ_ERROR_LIBRARY, "jpeg error: %s", msg);
}

static void output_message_dct(j_common_ptr cinfo)
{
	/* swallow message */
}

static void init_source_dct(j_decompress_ptr cinfo)
{
	/* nothing to do */
}

static void term_source_dct(j_decompress_ptr cinfo)
{
	/* nothing to do */
}

static boolean fill_input_buffer_dct(j_decompress_ptr cinfo)
{
	static unsigned char eoi[2] = { 0xFF, JPEG_EOI };
	struct jpeg_source_mgr *src = cinfo->src;
	fz_dctd *state = JZ_DCT_STATE_FROM_CINFO(cinfo);
	fz_context* ctx = state->ctx;
	fz_stream *curr_stm = state->curr_stm;

	curr_stm->rp = curr_stm->wp;
	fz_try(ctx)
	{
		src->bytes_in_buffer = fz_available(ctx, curr_stm, 1);
	}
	fz_catch(ctx)
	{
		/* Since fz_available swallows all other errors, the only errors that can
		 * bubble up to here are TRYLATER and exception stack overflow.
		 * Ignore this catastrophic failure and treat it as end of file.
		 * NOTE: We do NOT handle TRYLATER here.
		 */
		fz_warn(state->ctx, "error when decoding jpeg");
		src->next_input_byte = eoi;
		src->bytes_in_buffer = 2;
		return 1;
	}

	src->next_input_byte = curr_stm->rp;

	if (src->bytes_in_buffer == 0)
	{
		fz_warn(state->ctx, "premature end of file in jpeg");
		src->bytes_in_buffer = 0;
	}

	return 1;
}

static void skip_input_data_dct(j_decompress_ptr cinfo, long num_bytes)
{
	struct jpeg_source_mgr *src = cinfo->src;
	if (num_bytes > 0)
	{
		while ((size_t)num_bytes > src->bytes_in_buffer)
		{
			num_bytes -= (long)src->bytes_in_buffer;
			(void) src->fill_input_buffer(cinfo);
		}
		src->next_input_byte += num_bytes;
		src->bytes_in_buffer -= num_bytes;
	}
}

/* Invert CMYK polarity if it is a standalone JPEG file.
 * For JPEG images embedded in PDF files, the CMYK data is normal.
 * For JPEG images created by Photoshop, the CMYK data is inverted.
 */
static void invert_cmyk(unsigned char *p, int n)
{
	int i;
	for (i = 0; i < n; ++i)
		p[i] = 255 - p[i];
}

static int
next_dctd(fz_context *ctx, fz_stream *stm, size_t max)
{
	fz_dctd *state = stm->state;
	j_decompress_ptr cinfo = &state->cinfo;
	unsigned char *p = state->buffer;
	unsigned char *ep;
	int c;

	if (max > sizeof(state->buffer))
		max = sizeof(state->buffer);
	ep = state->buffer + max;

	fz_try(ctx)
	{
		if (!state->init)
		{
			state->init = 1;

			/* Skip over any stray whitespace at the start of the stream */
			while ((c = fz_peek_byte(ctx, state->chain)) == '\n' || c == '\r' || c == ' ')
				(void)fz_read_byte(ctx, state->chain);

			jpeg_create_decompress(cinfo);

			cinfo->src = &state->srcmgr;
			cinfo->src->init_source = init_source_dct;
			cinfo->src->fill_input_buffer = fill_input_buffer_dct;
			cinfo->src->skip_input_data = skip_input_data_dct;
			cinfo->src->resync_to_restart = jpeg_resync_to_restart;
			cinfo->src->term_source = term_source_dct;

			/* optionally load additional JPEG tables first */
			if (state->jpegtables)
			{
				state->curr_stm = state->jpegtables;
				cinfo->src->next_input_byte = state->curr_stm->rp;
				cinfo->src->bytes_in_buffer = state->curr_stm->wp - state->curr_stm->rp;
				jpeg_read_header(cinfo, 0);
				state->curr_stm->rp = state->curr_stm->wp - state->cinfo.src->bytes_in_buffer;
				state->curr_stm = state->chain;
			}

			cinfo->src->next_input_byte = state->curr_stm->rp;
			cinfo->src->bytes_in_buffer = state->curr_stm->wp - state->curr_stm->rp;

			jpeg_read_header(cinfo, 1);

			/* Adobe APP marker overrides ColorTransform from PDF */
			if (cinfo->saw_Adobe_marker)
				state->color_transform = cinfo->Adobe_transform;

			/* Disable JPEG color transformations if ColorTransform is 0.
			 * This is usually handled by libjpeg, but since PDF can override
			 * the default behavior if the Adobe APP marker is missing
			 * we must do it here as well.
			 */
			if (state->color_transform == 0)
			{
				if (cinfo->num_components == 3)
					cinfo->jpeg_color_space = JCS_RGB;
				if (cinfo->num_components == 4)
					cinfo->jpeg_color_space = JCS_CMYK;
			}

			cinfo->scale_num = 8/(1<<state->l2factor);
			cinfo->scale_denom = 8;

			jpeg_start_decompress(cinfo);

			state->stride = cinfo->output_width * cinfo->output_components;
			state->scanline = Memento_label(fz_malloc(ctx, state->stride), "dct_scanline");
			state->rp = state->scanline;
			state->wp = state->scanline;
		}

		while (state->rp < state->wp && p < ep)
			*p++ = *state->rp++;

		while (p < ep)
		{
			if (cinfo->output_scanline == cinfo->output_height)
				break;

			if (p + state->stride <= ep)
			{
				jpeg_read_scanlines(cinfo, &p, 1);
				if (state->invert_cmyk && cinfo->num_components == 4)
					invert_cmyk(p, state->stride);
				p += state->stride;
			}
			else
			{
				jpeg_read_scanlines(cinfo, &state->scanline, 1);
				if (state->invert_cmyk && cinfo->num_components == 4)
					invert_cmyk(state->scanline, state->stride);
				state->rp = state->scanline;
				state->wp = state->scanline + state->stride;
			}

			while (state->rp < state->wp && p < ep)
				*p++ = *state->rp++;
		}
		stm->rp = state->buffer;
		stm->wp = p;
		stm->pos += (p - state->buffer);
	}
	fz_catch(ctx)
	{
		if (cinfo->src)
			state->curr_stm->rp = state->curr_stm->wp - cinfo->src->bytes_in_buffer;
		fz_rethrow(ctx);
	}

	if (p == stm->rp)
		return EOF;

	return *stm->rp++;
}

static void
close_dctd(fz_context *ctx, fz_stream* stm)
{
	fz_dctd *state = (fz_dctd *)stm->state;

	if (state->init)
	{
		/* We call jpeg_abort rather than the more usual
		 * jpeg_finish_decompress here. This has the same effect,
		 * but doesn't spew warnings if we didn't read enough data etc.
		 * Annoyingly jpeg_abort can throw
		 */
		fz_try(ctx)
			jpeg_abort((j_common_ptr)&state->cinfo);
		fz_catch(ctx)
		{
			/* Ignore any errors here */
		}

		jpeg_destroy_decompress(&state->cinfo);
	}

	fz_dct_mem_term((j_common_ptr) &state->cinfo);

	if (state->cinfo.src)
		state->curr_stm->rp = state->curr_stm->wp - state->cinfo.src->bytes_in_buffer;

	fz_free(ctx, state->scanline);
	fz_drop_stream(ctx, state->chain);
	fz_drop_stream(ctx, state->jpegtables);
	fz_free(ctx, state);
}

fz_stream *
fz_open_dctd(fz_context *ctx, fz_stream *chain, int color_transform, int invert_cmyk, int l2factor, fz_stream *jpegtables)
{
	fz_dctd *state = fz_malloc_struct(ctx, fz_dctd);
	j_decompress_ptr cinfo = &state->cinfo;

	state->ctx = ctx;

	cinfo->client_data_ref = state;
	cinfo->client_init_callback = fz_jpeg_dct_sys_mem_register;

	state->color_transform = color_transform;
	state->invert_cmyk = invert_cmyk;
	state->init = 0;
	state->l2factor = l2factor;
	state->chain = fz_keep_stream(ctx, chain);
	state->jpegtables = fz_keep_stream(ctx, jpegtables);
	state->curr_stm = state->chain;

	cinfo->src = NULL;
	cinfo->err = &state->errmgr;
	jpeg_std_error(cinfo->err);
	cinfo->err->output_message = output_message_dct;
	cinfo->err->error_exit = error_exit_dct;

	return fz_new_stream(ctx, state, next_dctd, close_dctd);
}
