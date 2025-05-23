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

#include "z-imp.h"

#include <string.h>

typedef struct
{
	fz_stream *chain;
	zng_stream z;
	unsigned char buffer[4096];
} fz_inflate_state;

void *fz_zlib_alloc(void *ctx, unsigned int items, unsigned int size)
{
	return Memento_label(fz_malloc_no_throw(ctx, (size_t)items * size), "zlib_alloc");
}

void fz_zlib_free(void *ctx, void *ptr)
{
	fz_free(ctx, ptr);
}

static int
next_flated(fz_context *ctx, fz_stream *stm, size_t required)
{
	fz_inflate_state *state = stm->state;
	fz_stream *chain = state->chain;
	zng_streamp zp = &state->z;
	int code;
	unsigned char *outbuf = state->buffer;
	int outlen = sizeof(state->buffer);

	if (stm->eof)
		return EOF;

	zp->next_out = outbuf;
	zp->avail_out = outlen;

	while (zp->avail_out > 0)
	{
		zp->avail_in = (uInt)fz_available(ctx, chain, 1);
		zp->next_in = chain->rp;

		code = zng_inflate(zp, Z_SYNC_FLUSH);

		chain->rp = chain->wp - zp->avail_in;

		if (code == Z_STREAM_END)
		{
			break;
		}
		else if (code == Z_BUF_ERROR)
		{
			fz_warn(ctx, "premature end of data in flate filter");
			break;
		}
		else if (code == Z_DATA_ERROR && zp->avail_in == 0)
		{
			fz_warn(ctx, "ignoring zlib error: %s", zp->msg);
			break;
		}
		else if (code == Z_DATA_ERROR && !strcmp(zp->msg, "incorrect data check"))
		{
			fz_warn(ctx, "ignoring zlib error: %s", zp->msg);
			chain->rp = chain->wp;
			break;
		}
		else if (code != Z_OK)
		{
			fz_throw(ctx, FZ_ERROR_LIBRARY, "zlib error: %s", zp->msg);
		}
	}

	stm->rp = state->buffer;
	stm->wp = state->buffer + outlen - zp->avail_out;
	stm->pos += outlen - zp->avail_out;
	if (stm->rp == stm->wp)
	{
		stm->eof = 1;
		return EOF;
	}
	return *stm->rp++;
}

static void
close_flated(fz_context *ctx, fz_stream* stm)
{
	fz_inflate_state *state = (fz_inflate_state *)stm->state;
	int code;

	code = zng_inflateEnd(&state->z);
	if (code != Z_OK)
		fz_warn(ctx, "zlib error: inflateEnd: %s", state->z.msg);

	fz_drop_stream(ctx, state->chain);
	fz_free(ctx, state);
}

fz_stream *
fz_open_flated(fz_context *ctx, fz_stream *chain, int window_bits)
{
	fz_inflate_state *state;
	int code;

	state = fz_malloc_struct(ctx, fz_inflate_state);
	state->z.zalloc = fz_zlib_alloc;
	state->z.zfree = fz_zlib_free;
	state->z.opaque = ctx;
	state->z.next_in = NULL;
	state->z.avail_in = 0;

	code = zng_inflateInit2(&state->z, window_bits);
	if (code != Z_OK)
	{
		fz_free(ctx, state);
		fz_throw(ctx, FZ_ERROR_LIBRARY, "zlib error: inflateInit2 failed");
	}

	state->chain = fz_keep_stream(ctx, chain);

	return fz_new_stream(ctx, state, next_flated, close_flated);
}
