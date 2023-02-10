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

#include <jbig2.h>

typedef struct
{
	Jbig2Allocator alloc;
} fz_jbig2_allocators;

typedef struct
{
	fz_stream *chain;
	Jbig2Ctx *ctx;
	fz_jbig2_allocators alloc;
	fz_jbig2_globals *globals;
	Jbig2Image *page;
	int idx;
	unsigned char buffer[4096];
} fz_jbig2d;

static void *fz_jbig2_alloc(Jbig2Allocator *allocator, size_t size)
{
	fz_context *ctx = ((fz_jbig2_allocators *) allocator)->ctx;
	return Memento_label(fz_malloc_no_throw(ctx, size), "jbig2_alloc");
}

static void fz_jbig2_free(Jbig2Allocator *allocator, void *p)
{
	fz_context *ctx = ((fz_jbig2_allocators *) allocator)->ctx;
	fz_free(ctx, p);
}

static void *fz_jbig2_realloc(Jbig2Allocator *allocator, void *p, size_t size)
{
	fz_context *ctx = ((fz_jbig2_allocators *) allocator)->ctx;
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
error_callback(void *data, const char *msg, Jbig2Severity severity, uint32_t seg_idx)
{
	fz_context *ctx = data;
	if (severity == JBIG2_SEVERITY_FATAL)
		fz_warn(ctx, "jbig2dec error: %s (segment %u)", msg, seg_idx);
	else if (severity == JBIG2_SEVERITY_WARNING)
		fz_warn(ctx, "jbig2dec warning: %s (segment %u)", msg, seg_idx);
#ifdef JBIG2_DEBUG
	else if (severity == JBIG2_SEVERITY_INFO)
		fz_warn(ctx, "jbig2dec info: %s (segment %u)", msg, seg_idx);
	else if (severity == JBIG2_SEVERITY_DEBUG)
		fz_warn(ctx, "jbig2dec debug: %s (segment %u)", msg, seg_idx);
#endif
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

static void
fz_drop_jbig2_globals_imp(fz_context *ctx, fz_storable *globals_)
{
	fz_jbig2_globals *globals = (fz_jbig2_globals *)globals_;
	if (globals->decoded)
	{
		jbig2_global_ctx_free(globals->decoded);
		fz_free(ctx, globals->alloc);
	}
	fz_drop_buffer(ctx, globals->encoded);
	fz_free(ctx, globals);
}

fz_jbig2_globals *
fz_new_jbig2_globals(fz_context *ctx, fz_buffer *buffer)
{
	fz_jbig2_globals *globals = fz_malloc_struct(ctx, fz_jbig2_globals);
	FZ_INIT_STORABLE(globals, 1, fz_drop_jbig2_globals_imp);
	globals->encoded = fz_keep_buffer(ctx, buffer);
	globals->decoded = NULL;
	globals->alloc = NULL;
	return globals;
}

fz_jbig2_globals *
fz_lookup_jbig2_globals(fz_context *ctx, void *key, const fz_store_type *type)
{
	return fz_find_item(ctx, fz_drop_jbig2_globals_imp, key, type);
}

fz_jbig2_globals *
fz_store_jbig2_globals(fz_context *ctx, void *key, const fz_store_type *type, fz_buffer *buffer)
{
	size_t size = fz_buffer_storage(ctx, buffer, NULL);
	fz_jbig2_globals *globals = fz_new_jbig2_globals(ctx, buffer);
	fz_jbig2_globals *existing = fz_store_item(ctx, key, globals, size, type);
	if (existing)
	{
		/* We already have a globals object. This will either have been
		 * produced by a racing thread, or there is already
		 * an entry for this one in the store. */
		fz_drop_jbig2_globals(ctx, globals);
		globals = existing;
	}

	return globals;
}

static fz_jbig2_globals *
fz_load_jbig2_globals(fz_context *ctx, fz_jbig2_globals *globals, Jbig2GlobalCtx **gctx)
{
	Jbig2Ctx *jctx = NULL;
	fz_jbig2_allocators *alloc = NULL;

	if (!globals)
		return NULL;

	fz_lock(ctx, FZ_LOCK_ALLOC);
	if (globals->encoded && !globals->decoded)
	{
		fz_buffer *buffer = globals->encoded;
		fz_unlock(ctx, FZ_LOCK_ALLOC);

		alloc = fz_malloc_struct(ctx, fz_jbig2_allocators);
		alloc->ctx = ctx;
		alloc->alloc.alloc = fz_jbig2_alloc;
		alloc->alloc.free = fz_jbig2_free;
		alloc->alloc.realloc = fz_jbig2_realloc;

		jctx = jbig2_ctx_new(&alloc->alloc, JBIG2_OPTIONS_EMBEDDED, NULL, error_callback, ctx);
		if (!jctx)
		{
			fz_free(ctx, alloc);
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot allocate jbig2 globals context");
		}

		/* Decode the encoded jbig2 segments into a globals context that can be used
		 * by jbig2dec later when decoding images. */
		if (jbig2_data_in(jctx, buffer->data, buffer->len) < 0)
		{
			jbig2_ctx_free(jctx);
			fz_free(ctx, alloc);
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode jbig2 globals");
		}

		/* We have now decoded the buffer into a useful globals context. */
		fz_lock(ctx, FZ_LOCK_ALLOC);
		if (!globals->decoded)
		{
			*gctx = globals->decoded = jbig2_make_global_ctx(jctx);
			globals->alloc = alloc;
			fz_unlock(ctx, FZ_LOCK_ALLOC);
		}
		else
		{
			*gctx = globals->decoded;
			fz_unlock(ctx, FZ_LOCK_ALLOC);
			jbig2_ctx_free(jctx);
			fz_free(ctx, alloc);
		}
	}
	else
	{
		*gctx = globals->decoded;
		fz_unlock(ctx, FZ_LOCK_ALLOC);
	}

	return fz_keep_jbig2_globals(ctx, globals);
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
        fz_drop_jbig2_globals(ctx, state->globals);
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
	Jbig2GlobalCtx *gctx = NULL;

	fz_var(state);

	state = fz_malloc_struct(ctx, fz_jbig2d);
	state->alloc.alloc.user_context = ctx;
	state->alloc.alloc.alloc_ = fz_jbig2_alloc;
	state->alloc.alloc.free_ = fz_jbig2_free;
	state->alloc.alloc.realloc_ = fz_jbig2_realloc;

	state->globals = fz_load_jbig2_globals(ctx, globals, &gctx);

	options = JBIG2_OPTIONS_NONE;
	if (embedded)
		options |= JBIG2_OPTIONS_EMBEDDED;

	state->ctx = jbig2_ctx_new((Jbig2Allocator *) &state->alloc, options, gctx, error_callback, ctx);
	if (state->ctx == NULL)
	{
		fz_drop_jbig2_globals(ctx, state->globals);
		fz_free(ctx, state);
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot allocate jbig2 context");
	}

	state->page = NULL;
	state->idx = 0;
	state->chain = fz_keep_stream(ctx, chain);

	return fz_new_stream(ctx, state, next_jbig2d, close_jbig2d);
}
