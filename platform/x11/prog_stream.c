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
#include "mupdf/assertions.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "curl_stream.h"
#include "timeval.h"
#include "mupdf/helpers/dir.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#endif

/* File stream - progressive reading to simulate http download */

typedef struct prog_state
{
	FILE *file;
	int64_t length;
	int64_t available; /* guarded by lock below */
	int kbps;
	struct curltime start_time; /* in microseconds since epoch */
	unsigned char buffer[4096];
	void (*on_data)(void*,int);
	void *on_data_arg;

	/* We assume either Windows threads or pthreads here. */
#ifdef _WIN32
	void *thread;
	DWORD thread_id;
	HANDLE mutex;
#else
	pthread_t thread;
	pthread_mutex_t mutex;
#endif
} prog_state;

static struct curltime get_current_time(void)
{
	return Curl_now();
}

#ifdef _WIN32
static int locked;

static void
lock(prog_state *state)
{
	WaitForSingleObject(state->mutex, INFINITE);
	assert(locked == 0);
	locked = 1;
}

static void
unlock(prog_state *state)
{
	assert(locked == 1);
	locked = 0;
	ReleaseMutex(state->mutex);
}
#else
static void
lock(prog_state *state)
{
	pthread_mutex_lock(&state->mutex);
}

static void
unlock(prog_state *state)
{
	pthread_mutex_unlock(&state->mutex);
}
#endif

static int next_prog(fz_context *ctx, fz_stream *stm, size_t len)
{
	prog_state *ps = (prog_state *)stm->state;
	size_t n;

	if (len > sizeof(ps->buffer))
		len = sizeof(ps->buffer);

	/* Simulate not all data available yet. */
	lock(ps);
	if (ps->available < ps->length)
	{
		if (stm->pos > ps->available || ps->available - stm->pos <= 0)
		{
			unlock(ps);
			fz_throw(ctx, FZ_ERROR_TRYLATER, "Not enough data yet");
		}
		len = fz_mini(len, ps->available - stm->pos);
	}
	unlock(ps);

	n = fread(ps->buffer, 1, len, ps->file);
	stm->rp = ps->buffer;
	stm->wp = ps->buffer + n;
	stm->pos += (int64_t)n;

	if (n < len)
	{
		if (ferror(ps->file))
			fz_throw(ctx, FZ_ERROR_GENERIC, "prog read error: %s", strerror(errno));
		if (n == 0)
			return EOF;
	}
	return *stm->rp++;
}

static void seek_prog(fz_context *ctx, fz_stream *stm, int64_t offset, int whence)
{
	prog_state *ps = (prog_state *)stm->state;

	if (whence == SEEK_END)
	{
		whence = SEEK_SET;
		offset += ps->length;
	}
	else if (whence == SEEK_CUR)
	{
		whence = SEEK_SET;
		offset += stm->pos;
	}

	if (fseek(ps->file, offset, whence) != 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot seek: %s", strerror(errno));
	stm->pos = offset;
	stm->wp = stm->rp;
}

static void close_prog(fz_context *ctx, fz_stream* stm)
{
	prog_state *ps = (prog_state *)stm->state;
	int n = 0;
	if (ps->file)
		n = fclose(ps->file);
	if (n < 0)
	{
		fz_warn(ctx, "cannot fclose: %s", strerror(errno));
	}
	fz_free(ctx, ps);
	stm->state = NULL;
}

static void fetcher_thread(prog_state *ps)
{
	int complete = 0;

	ps->start_time = get_current_time();

	while (!complete)
	{
		/* Wait a while. */
#ifdef _WIN32
		Sleep(200);
#else
		usleep(200000);
#endif

		lock(ps);

		/* Simulate more data having arrived. */
		if (ps->available < ps->length)
		{
			struct curltime now = Curl_now();
			timediff_t delta_t = Curl_timediff(now, ps->start_time);
			int64_t av = delta_t * ps->kbps;
			if (av > ps->length)
				av = ps->length;
			ps->available = av;
		}
		else
		{
			complete = 1;
		}

		unlock(ps);

		/* Ping callback with new data. */
		if (ps->on_data)
			ps->on_data(ps->on_data_arg, complete);
	}
}

#ifdef _WIN32
static DWORD WINAPI win_thread(void *lparam)
{
	fetcher_thread((prog_state *)lparam);
	return 0;
}
#else
static void *pthread_thread(void *arg)
{
	fetcher_thread((prog_state *)arg);
	return NULL;
}
#endif

static fz_stream *
fz_open_file_ptr_progressive(fz_context *ctx, FILE *file, int kbps, void (*on_data)(void*,int), void *opaque)
{
	fz_stream *stm;
	prog_state *state;

	state = fz_malloc_struct(ctx, prog_state);
	state->file = file;
	state->kbps = kbps;
	state->available = 0;

	state->on_data = on_data;
	state->on_data_arg = opaque;

	fseek(state->file, 0, SEEK_END);
	state->length = ftell(state->file);
	fseek(state->file, 0, SEEK_SET);

#ifdef _WIN32
	state->mutex = CreateMutex(NULL, FALSE, NULL);
	if (state->mutex == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "mutex creation failed");

	state->thread = CreateThread(NULL, 0, win_thread, state, 0, &state->thread_id);
	if (state->thread == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "thread creation failed");
#else
	if (pthread_mutex_init(&state->mutex, NULL))
		fz_throw(ctx, FZ_ERROR_GENERIC, "mutex creation failed");

	if (pthread_create(&state->thread, NULL, pthread_thread, state))
		fz_throw(ctx, FZ_ERROR_GENERIC, "thread creation failed");
#endif

	stm = fz_new_stream(ctx, state, next_prog, close_prog);
	stm->progressive = 1;
	stm->seek = seek_prog;

	return stm;
}

fz_stream *
fz_open_file_progressive(fz_context *ctx, const char *name, int kbps, void (*on_data)(void*,int), void *opaque)
{
	FILE *f;
	f = fz_fopen_utf8(ctx, name, "rb");
	if (f == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open %s: %s", name, fz_ctx_pop_system_errormsg(ctx));
	return fz_open_file_ptr_progressive(ctx, f, kbps, on_data, opaque);
}
