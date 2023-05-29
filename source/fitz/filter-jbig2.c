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

#include <jbig2.h>

typedef struct
{
	Jbig2Allocator alloc;
} fz_jbig2_allocators;

struct fz_jbig2_globals
{
	fz_storable storable;
	Jbig2GlobalCtx *gctx;
	fz_jbig2_allocators alloc;
	fz_buffer *data;
};

typedef struct
{
	fz_stream *chain;
	Jbig2Ctx *ctx;
	fz_jbig2_allocators alloc;
	fz_jbig2_globals *gctx;
	Jbig2Image *page;
	int idx;
	unsigned char buffer[4096];
} fz_jbig2d;

static void *fz_jbig2_alloc(Jbig2Allocator *allocator, size_t size)
{
	fz_context *ctx = ((fz_jbig2_allocators *) allocator)->alloc.user_context;
	return Memento_label(fz_malloc_no_throw(ctx, size), "jbig2_alloc");
}

static void fz_jbig2_free(Jbig2Allocator *allocator, void *p)
{
	fz_context *ctx = ((fz_jbig2_allocators *) allocator)->alloc.user_context;
	fz_free(ctx, p);
}

static void *fz_jbig2_realloc(Jbig2Allocator *allocator, void *p, size_t size)
{
	fz_context *ctx = ((fz_jbig2_allocators *) allocator)->alloc.user_context;
	if (size == 0)
	{
		fz_free(ctx, p);
		return NULL;
	}
	if (p == NULL)
		return Memento_label(fz_malloc(ctx, size), "jbig2_realloc");
	return Memento_label(fz_realloc_no_throw(ctx, p, size), "jbig2_realloc");
}

static void
error_callback(void* data, const char* msg, Jbig2Severity severity, uint32_t seg_idx)
{
	fz_context* ctx = data;
	char idxbuf[50] = "";

	if (seg_idx != JBIG2_UNKNOWN_SEGMENT_NUMBER)
		fz_snprintf(idxbuf, sizeof idxbuf, " (segment 0x%02x)", seg_idx);

	if (severity == JBIG2_SEVERITY_FATAL)
		fz_error(ctx, "jbig2dec error: %s%s", msg, idxbuf);
	else if (severity == JBIG2_SEVERITY_WARNING)
		fz_warn(ctx, "jbig2dec warning: %s%s", msg, idxbuf);
	else if (severity == JBIG2_SEVERITY_INFO)
		fz_info(ctx, "jbig2dec info: %s%s", msg, idxbuf);
	else
		fz_info(ctx, "jbig2dec debug: %s%s", msg, idxbuf);
}

static inline void fz_lock_jbig2(fz_context* ctx)
{
	//fz_lock(ctx, FZ_LOCK_JBIG2);
}

static inline void fz_unlock_jbig2(fz_context* ctx)
{
	//fz_unlock(ctx, FZ_LOCK_JBIG2);
}

fz_jbig2_globals *
fz_keep_jbig2_globals(fz_context *ctx, fz_jbig2_globals *globals)
{
	if (!globals)
		return NULL;
	return fz_keep_storable(ctx, &globals->storable);
}

void
fz_drop_jbig2_globals(fz_context *ctx, fz_jbig2_globals *globals)
{
	if (!globals)
		return;
	fz_drop_storable(ctx, &globals->storable);
}

void
fz_drop_jbig2_globals_imp(fz_context* ctx, fz_storable* globals_)
{
	fz_try(ctx)
	{
		fz_lock_jbig2(ctx);

		fz_jbig2_globals* globals = (fz_jbig2_globals*)globals_;
		globals->alloc.alloc.user_context = ctx;
		jbig2_global_ctx_free(globals->gctx);
		fz_drop_buffer(ctx, globals->data);
		fz_free(ctx, globals);
	}
	fz_always(ctx)
	{
		fz_unlock_jbig2(ctx);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}


fz_jbig2_globals *
fz_load_jbig2_globals(fz_context *ctx, fz_buffer *buf)
{
	fz_jbig2_globals *globals = fz_malloc_struct(ctx, fz_jbig2_globals);
	Jbig2Ctx *jctx;

	globals->alloc.alloc.user_context = ctx;
	globals->alloc.alloc.alloc_ = fz_jbig2_alloc;
	globals->alloc.alloc.free_ = fz_jbig2_free;
	globals->alloc.alloc.realloc_ = fz_jbig2_realloc;

	fz_try(ctx)
	{
		fz_lock_jbig2(ctx);

		jctx = jbig2_ctx_new((Jbig2Allocator*)&globals->alloc, JBIG2_OPTIONS_EMBEDDED, NULL, error_callback, ctx);
		if (!jctx)
		{
			fz_free(ctx, globals);
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot allocate jbig2 globals context");
		}

		/* Decode the encoded jbig2 segments into a globals context that can be used
		 * by jbig2dec later when decoding images. */
		if (jbig2_data_in(jctx, buf->data, buf->len) < 0)
		{
			jbig2_global_ctx_free(jbig2_make_global_ctx(jctx));
			fz_free(ctx, globals);
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode jbig2 globals");
		}

		FZ_INIT_STORABLE(globals, 1, fz_drop_jbig2_globals_imp);
		globals->gctx = jbig2_make_global_ctx(jctx);

		globals->data = fz_keep_buffer(ctx, buf);
	}
	fz_always(ctx)
	{
		fz_unlock_jbig2(ctx);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}

	return globals;
}

static void
close_jbig2d(fz_context *ctx, fz_stream* stm)
{
	fz_jbig2d *state = stm->state;
	fz_try(ctx)
	{
		fz_lock_jbig2(ctx);

		if (state->page)
		{
			jbig2_release_page(state->ctx, state->page);
		}
		fz_drop_jbig2_globals(ctx, state->gctx);
		jbig2_ctx_free(state->ctx);
	}
	fz_always(ctx)
	{
		fz_unlock_jbig2(ctx);

		fz_drop_stream(ctx, state->chain);
		fz_free(ctx, state);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

static int
next_jbig2d(fz_context *ctx, fz_stream *stm, size_t len)
{
	fz_jbig2d *state = stm->state;
	unsigned char tmp[4096];
	unsigned char *buf = state->buffer;
	unsigned char *p = buf;
	unsigned char *ep;
	unsigned char *s;
	int x, w;
	size_t n;

	if (len > sizeof(state->buffer))
		len = sizeof(state->buffer);
	ep = buf + len;

	if (!state->page)
	{
		fz_try(ctx)
		{
			fz_lock_jbig2(ctx);

			while (1)
			{
				n = fz_read(ctx, state->chain, tmp, sizeof tmp);
				if (n == 0)
					break;

				if (jbig2_data_in(state->ctx, tmp, n) < 0)
					fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode jbig2 image");
			}

			if (jbig2_complete_page(state->ctx) < 0)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot complete jbig2 image");

			state->page = jbig2_page_out(state->ctx);
			if (!state->page)
				fz_throw(ctx, FZ_ERROR_GENERIC, "no jbig2 image decoded");
		}
		fz_always(ctx)
		{
			fz_unlock_jbig2(ctx);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}

	s = state->page->data;
	w = state->page->height * state->page->stride;
	x = state->idx;
	while (p < ep && x < w)
		*p++ = s[x++] ^ 0xff;
	state->idx = x;

	stm->rp = buf;
	stm->wp = p;
	if (p == buf)
		return EOF;
	stm->pos += p - buf;
	return *stm->rp++;
}

fz_stream *
fz_open_jbig2d(fz_context *ctx, fz_stream *chain, fz_jbig2_globals *globals, int embedded)
{
	fz_jbig2d *state = NULL;
	Jbig2Options options;

	fz_var(state);

	state = fz_malloc_struct(ctx, fz_jbig2d);
	state->gctx = fz_keep_jbig2_globals(ctx, globals);
	state->alloc.alloc.user_context = ctx;
	state->alloc.alloc.alloc_ = fz_jbig2_alloc;
	state->alloc.alloc.free_ = fz_jbig2_free;
	state->alloc.alloc.realloc_ = fz_jbig2_realloc;

	options = JBIG2_OPTIONS_NONE;
	if (embedded)
		options |= JBIG2_OPTIONS_EMBEDDED;

	state->ctx = jbig2_ctx_new((Jbig2Allocator *) &state->alloc, options, globals ? globals->gctx : NULL, error_callback, ctx);
	if (state->ctx == NULL)
	{
		fz_drop_jbig2_globals(ctx, state->gctx);
		fz_free(ctx, state);
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot allocate jbig2 context");
	}

	state->page = NULL;
	state->idx = 0;
	state->chain = fz_keep_stream(ctx, chain);

	return fz_new_stream(ctx, state, next_jbig2d, close_jbig2d);
}

fz_buffer *
fz_jbig2_globals_data(fz_context *ctx, fz_jbig2_globals *globals)
{
	return globals ? globals->data : NULL;
}
