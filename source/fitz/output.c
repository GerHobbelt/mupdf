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

#define _LARGEFILE_SOURCE
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif


static inline void
fzoutput_lock(fz_output* out)
{
	ASSERT0(out);
	if (out->flags & FZOF_HAS_LOCKS)
	{
#ifndef DISABLE_MUTHREADS
		mu_lock_mutex(&out->buf_mutex);
#endif
		out->flags |= FZOF_IS_INSIDE_LOCK;
	}
}

static inline void
fzoutput_unlock(fz_output* out)
{
	ASSERT0(out);
	if (out->flags & FZOF_HAS_LOCKS)
	{
		ASSERT0(out->flags & FZOF_IS_INSIDE_LOCK);
		out->flags &= ~FZOF_IS_INSIDE_LOCK;
#ifndef DISABLE_MUTHREADS
		mu_unlock_mutex(&out->buf_mutex);
#endif
	}
}

static inline void
printf_lock(fz_output* out)
{
	ASSERT0(out);
	if (out->flags & FZOF_HAS_LOCKS)
	{
#ifndef DISABLE_MUTHREADS
		mu_lock_mutex(&out->printf_mutex);
#endif
		out->flags |= FZOF_IS_INSIDE_PRINTF_LOCK;
	}
}

static inline void
printf_unlock(fz_output* out)
{
	ASSERT0(out);
	if (out->flags & FZOF_HAS_LOCKS)
	{
		ASSERT0(out->flags & FZOF_IS_INSIDE_PRINTF_LOCK);
		out->flags &= ~FZOF_IS_INSIDE_PRINTF_LOCK;
#ifndef DISABLE_MUTHREADS
		mu_unlock_mutex(&out->printf_mutex);
#endif
	}
}

static int64_t file_tell(fz_context* ctx, fz_output* out);

static void analyze_and_improve_fwrite_error(fz_context* ctx, fz_output* out, size_t count)
{
	int64_t pos = file_tell(ctx, out);
	// heuristic: are we near, over or at 2GB/4GB boundary?
	if (pos + count > (1LL << 32) - 10240)
	{
		char msg[1024];
		int64_t fourgb = 1LL << 32;
		int64_t delta = fourgb - pos;

		fz_snprintf(msg, sizeof msg, "\n    Analysis: looks like you're near the magic 4GByte file size boundary so this may just be your run-time library or filesystem giving up on writing any file larger than %lld bytes (this file is reported as currently %lld bytes, which is %lld bytes removed from that 4GB boundary).", fourgb, pos, delta);

		int idx = ctx->error.system_errdepth;
		fz_strncat_s(ctx, ctx->error.system_error_message[idx], msg, sizeof(ctx->error.system_error_message[idx]));
	}
}

static int
file_write(fz_context *ctx, fz_output* out, const void *buffer, size_t count)
{
	ASSERT(out);
	FILE *file = (FILE *)out->state;
	size_t n;

	ASSERT(file);
	if (count == 0)
		return 0;

	if (count == 1)
	{
		int x = putc(((unsigned char*)buffer)[0], file);
		if (x == EOF && ferror(file))
		{
			fz_copy_ephemeral_errno(ctx);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			analyze_and_improve_fwrite_error(ctx, out, count);
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot fwrite: %s (%s)", fz_ctx_pop_system_errormsg(ctx), out->filepath);
		}
		return 0;
	}

	n = fwrite(buffer, 1, count, file);
	if (n < count)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		analyze_and_improve_fwrite_error(ctx, out, count - n);
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot fwrite: %s (written %zu of %zu bytes) (%s)", fz_ctx_pop_system_errormsg(ctx), n, count, out->filepath);
	}
	return 0;
}

#if defined(_WIN32) 

static int
stdio_write(fz_context* ctx, DWORD channel, const void* buffer, size_t count)
{
	// Windows: https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile#pipes
	// > "When writing to a non-blocking, byte-mode pipe handle with insufficient buffer space,
	// > WriteFile returns TRUE with *lpNumberOfBytesWritten < nNumberOfBytesToWrite."

	ASSERT(channel == STD_OUTPUT_HANDLE || channel == STD_ERROR_HANDLE);
	ASSERT(buffer);
	ASSERT(count > 0);
	if (count == 0)
		return 0;

	//fprintf(stderr, "stdout_write: %d bytes, %p\n", (int)count, buffer);
	unsigned char* p = (unsigned char*)buffer;
	size_t n = count;
	const int PIPE_MAX_NONBLOCK_BUFFER_SIZE = 65536 / 2; // Modern Win10 has a nonblocking buffer of 64K, old systems and old UNIXes (Linux kernel 2.6.11 and below IIRC) used only a single memory page: 4K
	clock_t tick = 0;
	static int caller_is_aborted = 0;
	// when a previous call to this function already discovered that the caller has aborted, don't even bother to try:
	if (caller_is_aborted)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot write to %s: previous timeout while waiting for FileWrite() API signaled caller has aborted already.", (channel == STD_OUTPUT_HANDLE ? "STDOUT" : "STDERR"));
	}
	while (n > 0)
	{
		DWORD written = 0;
		// Write the data to the pipe in chunks of limited size, so that we won't lock
		// on a chunk. That's also why we size our chunks to HALF the known pipe nonblocking buffer size!
		DWORD n_lim = fz_minz(PIPE_MAX_NONBLOCK_BUFFER_SIZE, n);
		int rv = WriteFile(GetStdHandle(channel), p, n_lim, &written, NULL);
		int err = GetLastError();
		//fprintf(stderr, "stdout_write:WriteFile: %d bytes, %p, %d written, rv:%d, err:%d\n", (int)n_lim, p, (int)written, rv, err);
		n -= written;
		p += written;

		if (!rv && !(err == ERROR_IO_PENDING || err == ERROR_NO_DATA || err == DMLERR_EXECACKTIMEOUT))
		{
			LPSTR errmsgbuf = NULL;
			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, (LPSTR)&errmsgbuf, 0, NULL);
			char errmsg[512];
			// DMLERR_EXECACKTIMEOUT happens when you try to debug-run this stuff in the MSVC2019 debugger. Should be fine in other settings...
			strncpy(errmsg, errmsgbuf ? errmsgbuf : err == DMLERR_EXECACKTIMEOUT ? "DMLERR_EXECACKTIMEOUT: A request for a synchronous execute transaction has timed out." : "Unidentified Windows error.", sizeof(errmsg));
			if (errmsgbuf)
				LocalFree(errmsgbuf);

			// - https://docs.microsoft.com/en-us/windows/console/console-handles
			//HANDLE conout = CreateFileA("CONOUT$", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			//fz_snprintf(errmsg, sizeof(errmsg), "countout vs stdout: %p - %p", (void*)conout, (void*)GetStdHandle(STD_OUTPUT_HANDLE));
			//DWORD conMode = 0;
			//rv = GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &conMode);
			//rv = GetConsoleMode(conout, &conMode);
			//rv = SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_WRAP_AT_EOL_OUTPUT);
			//rv = SetConsoleMode(conout, ENABLE_WRAP_AT_EOL_OUTPUT);
			//rv = WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), p, min(16, n), &written, NULL);
			//rv = WriteConsole(conout, p, min(256, n), &written, NULL);
			//rv = CloseHandle(conout);
			//if (!rv)
			fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot write to %s: %08x: %s (written %zu of %zu bytes)", (channel == STD_OUTPUT_HANDLE ? "STDOUT" : "STDERR"), err, errmsg, count - n, count);
			//else
			//{
			//	n -= written;
			//	p += written;
			//}
		}
		// wait until STDOUT pipe becomes empty again, but don't wait too long: timeout after a "sensible" 15 seconds:
		else if (written == 0)
		{
			if (!tick)
			{
				tick = clock();
			}
			else if (clock() - tick >= 15 * CLOCKS_PER_SEC)
			{
				caller_is_aborted = 1;
				fz_enable_dbg_output_on_stdio_unreachable();
				fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot write to %s: timeout (15 seconds) while waiting for FileWrite() API to accept a byte to write (written %zu of %zu bytes)", (channel == STD_OUTPUT_HANDLE ? "STDOUT" : "STDERR"), count - n, count);
			}
			// Don't load the CPU for a while: we'll have to wait for the calling process to gobble the bytes buffered in the pipe before we can continue here.
			SleepEx(2, TRUE);
		}
	}
	return 0;
}

#endif

static int
stdout_write(fz_context *ctx, fz_output* out, const void *buffer, size_t count)
{
#ifdef _WIN32
	return stdio_write(ctx, STD_OUTPUT_HANDLE, buffer, count);
#else
	return file_write(ctx, stdout, buffer, count);
#endif
}

static void
stdout_flush_on_close(fz_context* ctx, fz_output *out)
{
	fz_flush_output(ctx, out);
#ifndef _WIN32
	fflush(stdout);
#endif
}

static void
stdout_flush_on_drop(fz_context* ctx, fz_output* out)
{
	fz_flush_output_no_lock(ctx, out);
#ifndef _WIN32
	fflush(stdout);
#endif
}

static fz_output fz_stdout_global = {
	NULL,
	stdout_write,
	NULL,
	NULL,
	stdout_flush_on_close,
	stdout_flush_on_drop,
};

fz_output *fz_stdout(fz_context *ctx)
{
	return &fz_stdout_global;
}

static int
stderr_write(fz_context *ctx, fz_output* out, const void *buffer, size_t count)
{
#ifdef _WIN32
	return stdio_write(ctx, STD_ERROR_HANDLE, buffer, count);
#else
	return file_write(ctx, stderr, buffer, count);
#endif
}

static void
stderr_flush_on_close(fz_context* ctx, fz_output* out)
{
	fz_flush_output(ctx, out);
#ifndef _WIN32
	fflush(stderr);
#endif
}

static void
stderr_flush_on_drop(fz_context* ctx, fz_output* out)
{
	fz_flush_output_no_lock(ctx, out);
#ifndef _WIN32
	fflush(stderr);
#endif
}

static fz_output fz_stderr_global = {
	NULL,
	stderr_write,
	NULL,
	NULL,
	stderr_flush_on_close,
	stderr_flush_on_drop,
};

fz_output *fz_stderr(fz_context *ctx)
{
	return &fz_stderr_global;
}

#ifdef _WIN32

static int
stdods_write(fz_context *ctx, fz_output* out, const void *buffer, size_t count)
{
	// Assume that the heap MAY be corrupted when we call into here.
	// Such last effort error messages will invariably be short-ish.
	// Besides, using a bit of stack for smaller messages reduces heap alloc+free
	// call overhead.
	char stkbuf[LONGLINE + 1];
	char* buf = stkbuf;
	if (count > sizeof(stkbuf) - 1)
		buf = fz_malloc(ctx, count + 1);

	memcpy(buf, buffer, count);
	buf[count] = 0;
	OutputDebugStringA(buf);

	if (buf != stkbuf)
		fz_free(ctx, buf);

	return 0;
}

static void
stdods_flush_on_close(fz_context* ctx, fz_output* out)
{
	fz_flush_output(ctx, out);
}

static void
stdods_flush_on_drop(fz_context* ctx, fz_output* out)
{
	fz_flush_output_no_lock(ctx, out);
}

static fz_output fz_stdods_global = {
	NULL,
	stdods_write,
	NULL,
	NULL,
	stdods_flush_on_close,
	stdods_flush_on_drop,
};

fz_output *fz_stdods(fz_context *ctx)
{
	return &fz_stdods_global;
}

#elif defined(__ANDROID__)

//#include <android/log.h>

static int
stdods_write(fz_context* ctx, fz_output* out, const void* buffer, size_t count)
{
	// Assume that the heap MAY be corrupted when we call into here.
	// Such last effort error messages will invariably be short-ish.
	// Besides, using a bit of stack for smaller messages reduces heap alloc+free
	// call overhead.
	char stkbuf[LONGLINE + 1];
	char* buf = stkbuf;
	if (count > sizeof(stkbuf) - 1)
		buf = fz_malloc(ctx, count + 1);

	memcpy(buf, buffer, count);
	buf[count] = 0;

	static const int lvl_map[] = { ANDROID_LOG_ERROR, ANDROID_LOG_ERROR, ANDROID_LOG_WARN, ANDROID_LOG_INFO, ANDROID_LOG_INFO, ANDROID_LOG_INFO };
	int lvl = fz_output_get_severity_level(ctx, out);
	if (lvl < 0)
		lvl = 0;
	else if (lvl >= countof(lvl_map))
		lvl >= countof(lvl_map) - 1;

	__android_log_print(lvl_map[lvl], "libmupdf", "%s", buf);

	if (buf != stkbuf)
		fz_free(ctx, buf);

	return 0;
}

static void
stdods_flush_on_close(fz_context* ctx, fz_output* out)
{
	fz_flush_output(ctx, out);
}

static void
stdods_flush_on_drop(fz_context* ctx, fz_output* out)
{
	fz_flush_output_no_lock(ctx, out);
}

static fz_output fz_stdods_global = {
	NULL,
	stdods_write,
	NULL,
	NULL,
	stdods_flush_on_close,
	stdods_flush_on_drop,
};

fz_output* fz_stdods(fz_context* ctx)
{
	return &fz_stdods_global;
}

#else

fz_output* fz_stdods(fz_context* ctx)
{
	return NULL;
}

#endif

fz_output *fz_stddbg(fz_context *ctx)
{
	if (ctx == NULL)
		return fz_stdods(NULL);

	if (ctx->stddbg)
		return ctx->stddbg;

	fz_output* rv = fz_stdods(ctx);
	if (rv)
		return rv;

	return fz_stderr(ctx);
}

void fz_set_stddbg(fz_context *ctx, fz_output *out)
{
	if (ctx == NULL)
		return;

	ctx->stddbg = out;
}

static int
file_seek(fz_context *ctx, fz_output* out, int64_t off, int whence)
{
	FILE *file = out->state;
	ASSERT(file);
#ifdef _WIN32
	int n = _fseeki64(file, off, whence);
#else
	int n = fseeko(file, off, whence);
#endif
	if (n < 0)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot fseek: %s (%s)", fz_ctx_pop_system_errormsg(ctx), out->filepath);
	}
	return 0;
}

static int64_t
file_tell(fz_context *ctx, fz_output* out)
{
	FILE *file = out->state;
	ASSERT(file);
#ifdef _WIN32
	int64_t off = _ftelli64(file);
#else
	int64_t off = ftello(file);
#endif
	if (off == -1)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot ftell: %s (%s)", fz_ctx_pop_system_errormsg(ctx), out->filepath);
	}
	return off;
}

static void
file_close(fz_context* ctx, fz_output* out)
{
	FILE* file = out->state;
	ASSERT(file);
	if (file)
	{
		int n = fclose(file);
		if (n < 0)
		{
			fz_copy_ephemeral_errno(ctx);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			fz_warn(ctx, "cannot fclose: %s (%s)", fz_ctx_get_system_errormsg(ctx), out->filepath);
		}
		else
		{
			// prevent double-close:
			out->state = NULL;
		}
	}
}

static void
file_drop(fz_context *ctx, fz_output* out)
{
	FILE *file = out->state;
	if (file)
	{
		int n = fclose(file);
		if (n < 0)
		{
			fz_copy_ephemeral_errno(ctx);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			fz_warn(ctx, "cannot drop/close file: %s (%s)", fz_ctx_get_system_errormsg(ctx), out->filepath);
			// accept our loss.
			out->state = NULL;
		}
		else
		{
			// prevent double-close:
			out->state = NULL;
		}
	}
}

static fz_stream *
file_as_stream(fz_context *ctx, fz_output* out)
{
	FILE *file = out->state;
	ASSERT(file);
	fflush(file);
	return fz_open_file_ptr_no_close(ctx, file);
}

static int file_truncate(fz_context* ctx, fz_output* out)
{
	FILE* file = out->state;
	ASSERT(file);
	if (fflush(file))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}

#ifdef _WIN32
	{
		__int64 pos = _ftelli64(file);
		if (pos < 0)
		{
			fz_copy_ephemeral_errno(ctx);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			return -1;
		}
		if (pos >= 0)
		{
			if (_chsize_s(fileno(file), pos))
			{
				fz_copy_ephemeral_errno(ctx);
				ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
				return -1;
			}
		}
	}
#else
	{
		off_t pos = ftello(file);
		if (pos < 0)
		{
			fz_copy_ephemeral_errno(ctx);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			return -1;
		}
		if (pos >= 0)
		{
			if (ftruncate(fileno(file), pos))
			{
				fz_copy_ephemeral_errno(ctx);
				ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
				return -1;
			}
		}
	}
#endif
	return 0;
}

static int safe_output_mode = 0;

void fz_set_safe_file_output_mode(int enable)
{
	safe_output_mode = enable;
}

static int fz_output_safe_mode_on_create(fz_context* ctx, fz_output* out)
{
	return 0;
}

static int fz_output_safe_mode_on_create_append(fz_context* ctx, fz_output* out)
{
	return 0;
}

static int fz_output_safe_mode_on_success(fz_context* ctx, fz_output* out)
{
	return 0;
}

static int fz_output_safe_mode_on_fail(fz_context* ctx, fz_output* out)
{
	return 0;
}

static int fz_output_safe_mode_on_rename_collision(fz_context* ctx, fz_output* out)
{
	return 0;
}

static fz_output_safe_mode_on_event_fn* fzevt_on_create = fz_output_safe_mode_on_create;
static fz_output_safe_mode_on_event_fn* fzevt_on_create_append = fz_output_safe_mode_on_create_append;
static fz_output_safe_mode_on_event_fn* fzevt_on_success = fz_output_safe_mode_on_success;
static fz_output_safe_mode_on_event_fn* fzevt_on_rename_collision = fz_output_safe_mode_on_rename_collision;
static fz_output_safe_mode_on_event_fn* fzevt_on_fail = fz_output_safe_mode_on_fail;


void fz_set_custom_safe_file_output_mode_handlers(fz_output_safe_mode_on_event_fn *on_create, fz_output_safe_mode_on_event_fn *on_create_append, fz_output_safe_mode_on_event_fn *on_success, fz_output_safe_mode_on_event_fn *on_rename_collision, fz_output_safe_mode_on_event_fn *on_fail)
{
	fzevt_on_create = on_create ? on_create : fz_output_safe_mode_on_create;
	fzevt_on_create_append = on_create_append ? on_create_append : fz_output_safe_mode_on_create_append;
	fzevt_on_success = on_success ? on_success : fz_output_safe_mode_on_success;
	fzevt_on_rename_collision = on_rename_collision ? on_rename_collision : fz_output_safe_mode_on_rename_collision;
	fzevt_on_fail = on_fail ? on_fail : fz_output_safe_mode_on_fail;
}

fz_output *
fz_new_output(fz_context *ctx, int bufsiz, void *state, fz_output_write_fn *write, fz_output_close_fn *close, fz_output_drop_fn *drop)
{
	fz_output *out = NULL;

	fz_var(out);

	fz_try(ctx)
	{
		out = fz_malloc_struct(ctx, fz_output);
		out->state = state;
		out->write = write;
		out->close = close;
		out->drop = drop;
#ifndef DISABLE_MUTHREADS
		mu_create_mutex(&out->buf_mutex);
		mu_create_mutex(&out->printf_mutex);
#endif
		ASSERT(out->flags == FZOF_NONE);
		out->flags = FZOF_HAS_LOCKS;
		ASSERT(out->filepath == NULL);
		ASSERT(out->bp == NULL);
		if (bufsiz > 1)
		{
			out->bp = Memento_label(fz_malloc(ctx, bufsiz), "output_buf");
			out->wp = out->bp;
			out->ep = out->bp + bufsiz;
		}
	}
	fz_catch(ctx)
	{
		if (drop)
			drop(ctx, out);

		if (out)
		{
#ifndef DISABLE_MUTHREADS
			if (out->flags & FZOF_HAS_LOCKS)
			{
				mu_destroy_mutex(&out->printf_mutex);
				mu_destroy_mutex(&out->buf_mutex);
			}
#endif
			out->flags &= ~FZOF_HAS_LOCKS | FZOF_IS_INSIDE_PRINTF_LOCK | FZOF_IS_INSIDE_LOCK;
			fz_free(ctx, out->bp);
			fz_free(ctx, out);
		}

		fz_rethrow(ctx);
	}
	return out;
}

int fz_set_output_buffer(fz_context* ctx, fz_output* out, int bufsiz)
{
	int rv = 1;

	ASSERT(out);

	// shortcut check if we need to do anything here: this quick check MAY FAIL
	// but only when another modifies the buffer size at the same time. Which, thanks
	// to the critical section further below, will be dealt will properly after all.
	//
	// Meanwhile, our quick check here will prevent a lot of unnecessary locking+unlocking
	// of the mutex due to this API being called from `stddbgchannel()` et al.
	if (out->bp ? bufsiz == (out->ep - out->bp) : bufsiz == 0)
		return 0; // nothing to do, *guaranteed*.

	// are we looking at an fz_output which was NOT created using the fz_new_output() API?
	// if so, create the buffer mutex after the fact.
	if (!(out->flags & FZOF_HAS_LOCKS) && (bufsiz > 1))
	{
#ifndef DISABLE_MUTHREADS
		ASSERT(mu_mutex_is_zeroed(&out->buf_mutex));
		ASSERT(mu_mutex_is_zeroed(&out->printf_mutex));
#endif
		ASSERT((out->flags & (FZOF_IS_INSIDE_LOCK | FZOF_IS_INSIDE_PRINTF_LOCK)) == 0);

#ifndef DISABLE_MUTHREADS
		mu_create_mutex(&out->buf_mutex);
		mu_create_mutex(&out->printf_mutex);
#endif
		out->flags |= FZOF_HAS_LOCKS;
	}
	else if ((out->flags & FZOF_HAS_LOCKS) && bufsiz <= 1)
	{
		ASSERT((out->flags & (FZOF_IS_INSIDE_LOCK | FZOF_IS_INSIDE_PRINTF_LOCK)) == 0);

#ifndef DISABLE_MUTHREADS
		mu_destroy_mutex(&out->buf_mutex);
		mu_destroy_mutex(&out->printf_mutex);
		ASSERT(mu_mutex_is_zeroed(&out->buf_mutex));
		ASSERT(mu_mutex_is_zeroed(&out->printf_mutex));
#endif
		out->flags &= ~FZOF_HAS_LOCKS;
	}

	fzoutput_lock(out);
	if (out->bp == NULL)
	{
		rv = 0;
		if (bufsiz > 1)
		{
			out->bp = Memento_label(fz_malloc_no_throw(ctx, bufsiz), "output_buf");
			out->wp = out->bp;
			out->ep = out->bp + bufsiz;
			rv = !!out->bp;
		}
	}
	else if (out->bp != NULL)
	{
		rv = 0;
		// only setup the buffer when its size is different from what already is.
		if (bufsiz != out->ep - out->bp)
		{
			fz_flush_output_no_lock(ctx, out);
			assert(out->wp == out->bp);
			fz_free(ctx, out->bp);
			out->bp = NULL;

			if (bufsiz > 1)
			{
				out->bp = Memento_label(fz_malloc_no_throw(ctx, bufsiz), "output_buf");
				out->wp = out->bp;
				out->ep = out->bp + bufsiz;
				rv = !!out->bp;
			}
		}
	}
	fzoutput_unlock(out);
	return rv;
}

static int null_write(fz_context *ctx, fz_output* out, const void *buffer, size_t count)
{
	return 0;
}

static int null_seek(fz_context* ctx, fz_output* out, int64_t offset, int whence)
{
	return 0;
}

static int null_truncate(fz_context* ctx, fz_output* out)
{
	return 0;
}


fz_output *
fz_new_output_with_path(fz_context *ctx, const char *filename, int append)
{
	FILE *file;
	fz_output *out;

	if (filename == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "no output to write to");

	if (!strcmp(filename, "/dev/null") || !fz_strcasecmp(filename, "nul:"))
	{
		out = fz_new_output(ctx, 0, NULL, null_write, NULL, NULL);
		out->seek = null_seek;
		out->truncate = null_truncate;
		return out;
	}

	if (!strcmp(filename, "/dev/stdout"))
	{
#ifdef _WIN32
		/* Windows specific code to make stdout binary to prevent automatic character conversions in image data. */
		setmode(fileno(stdout), O_BINARY);
#endif
		return fz_stdout(ctx);
	}

	fz_mkdir_for_file(ctx, filename);

	/* If <append> is false, we use fopen()'s 'x' flag to force an error if
	 * some other process creates the file immediately after we have removed
	 * it - this avoids vulnerability where a less-privileged process can create
	 * a link and get us to overwrite a different file. See:
	 * 	https://bugs.ghostscript.com/show_bug.cgi?id=701797
	 * 	http://www.open-std.org/jtc1/sc22//WG14/www/docs/n1339.pdf
	 */

	/* Ensure we create a brand new file. We don't want to clobber our old file. */
	if (!append)
	{
		if (fz_remove_utf8(ctx, filename) < 0)
		{
			int ec = fz_ctx_get_rtl_errno(ctx);
			if (ec != ENOENT)
			{
				ASSERT(ec != 0);
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot remove file '%s': %s", filename, fz_ctx_pop_system_errormsg(ctx));
			}
		}
	}
#if defined(__MINGW32__) || defined(__MINGW64__)
	file = fz_fopen_utf8(ctx, filename, append ? "rb+" : "wb+"); /* 'x' flag not supported. */
#else
	file = fz_fopen_utf8(ctx, filename, append ? "rb+" : "wb+x");
#endif
	if (append)
	{
		if (file == NULL)
		{
			file = fz_fopen_utf8(ctx, filename, "wb+");
		}
		else
			fseek(file, 0, SEEK_END);
	}
	if (!file)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open file '%s': %s", filename, fz_ctx_pop_system_errormsg(ctx));

	setvbuf(file, NULL, _IONBF, 0); /* we do our own buffering */
	out = fz_new_output(ctx, 8192, file, file_write, file_close, file_drop);
	out->seek = file_seek;
	out->tell = file_tell;
	out->as_stream = file_as_stream;
	out->truncate = file_truncate;
	out->filepath = fz_strdup(ctx, filename);

	return out;
}

static int
buffer_write(fz_context *ctx, fz_output* out, const void *data, size_t len)
{
	fz_buffer *buffer = out->state;
	fz_append_data(ctx, buffer, data, len);
	return 0;
}

static int
buffer_seek(fz_context *ctx, fz_output* out, int64_t off, int whence)
{
	fz_throw(ctx, FZ_ERROR_GENERIC, "cannot seek in buffer");
	return -1;	// unreachable code
}

static int64_t
buffer_tell(fz_context *ctx, fz_output* out)
{
	fz_buffer *buffer = out->state;
	return (int64_t)buffer->len;
}

static void
buffer_drop(fz_context *ctx, fz_output* out)
{
	fz_buffer *buffer = out->state;
	fz_drop_buffer(ctx, buffer);
}

fz_output *
fz_new_output_with_buffer(fz_context *ctx, fz_buffer *buf)
{
	fz_output *out = fz_new_output(ctx, 0, fz_keep_buffer(ctx, buf), buffer_write, NULL, buffer_drop);
	out->seek = buffer_seek;
	out->tell = buffer_tell;
	return out;
}

void
fz_close_output(fz_context *ctx, fz_output *out)
{
	if (out == NULL)
		return;
	fz_flush_output(ctx, out);
	if (out->close)
		out->close(ctx, out);
	out->close = NULL;
}

void
fz_drop_output(fz_context *ctx, fz_output *out)
{
	if (out)
	{
		if (out->close)
			fz_warn(ctx, "dropping unclosed output");

#ifndef DISABLE_MUTHREADS
		if (out->flags & FZOF_HAS_LOCKS)
		{
			// when we encounter a HELD LOCK, we release it before dropping it.
			// This can (theoretically at least, we haven't observed this in practice YET) happen
			// when custom userland code in a callback executed from inside a `fz_output` critical section
			// throws an exception.
			// We (**by design**) did not wrap those lock+unlock protected critical sections
			// around here with the regular fz_try/fz_always wrappers, because we wanted to produce
			// an *absolute minimal overhead* thread-safety net around each `fz_output` instance.
			//
			// Consequently, we can expect the need for cleanup here when this code is executed
			// from a fz_catch() section anywhere in the application.
			if (out->flags & FZOF_IS_INSIDE_PRINTF_LOCK)
				printf_unlock(out);
			if (out->flags & FZOF_IS_INSIDE_LOCK)
				fzoutput_unlock(out);
			mu_destroy_mutex(&out->printf_mutex);
			mu_destroy_mutex(&out->buf_mutex);
		}
#endif
		out->flags &= ~FZOF_HAS_LOCKS | FZOF_IS_INSIDE_PRINTF_LOCK | FZOF_IS_INSIDE_LOCK;
		if (out->drop)
			out->drop(ctx, out);
		fz_free(ctx, out->bp);
		fz_free(ctx, out->filepath);
		out->bp = NULL;
		out->ep = NULL;
		out->wp = NULL;
		out->filepath = NULL;
		//out->flags = 0;
		out->severity_level = 0;

		if (out != &fz_stdout_global && out != &fz_stderr_global && out != &fz_stdods_global)
		{
			fz_free(ctx, out);
		}
	}
}

void
fz_seek_output(fz_context* ctx, fz_output* out, int64_t off, int whence)
{
	if (out->seek == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot seek in unseekable output stream");

	fzoutput_lock(out);
	fz_flush_output_no_lock(ctx, out);
	int rv = out->seek(ctx, out, off, whence);
	fzoutput_unlock(out);
	if (rv)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to seek to specified position in this output: %s (%s)", fz_ctx_pop_system_errormsg(ctx), out->filepath);
	}
}

int64_t
fz_tell_output(fz_context *ctx, fz_output *out)
{
	if (out->tell == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot tell in untellable output stream");

	fzoutput_lock(out);
	int64_t pos = out->tell(ctx, out);
	if (out->bp && pos >= 0)
		pos += (out->wp - out->bp);
	fzoutput_unlock(out);
	if (pos < 0)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to tell cursor position in this output: %s (%s)", fz_ctx_pop_system_errormsg(ctx), out->filepath);
	}
	return pos;
}

fz_stream *
fz_stream_from_output(fz_context *ctx, fz_output *out)
{
	if (out->as_stream == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot derive input stream from output stream");
	fzoutput_lock(out);
	fz_flush_output_no_lock(ctx, out);
	fz_stream *rv = out->as_stream(ctx, out);
	fzoutput_unlock(out);
	return rv;
}

void
fz_truncate_output(fz_context *ctx, fz_output *out)
{
	if (out->truncate == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot truncate this output stream");
	fzoutput_lock(out);
	fz_flush_output_no_lock(ctx, out);
	int rv = out->truncate(ctx, out);
	fzoutput_unlock(out);
	if (rv)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to truncate this output: %s (%s)", fz_ctx_pop_system_errormsg(ctx), out->filepath);
	}
}

static void
fz_write_emit(fz_context *ctx, void *out, int c)
{
	fz_write_byte(ctx, (fz_output *)out, c);
}

void
fz_write_vprintf(fz_context *ctx, fz_output *out, const char *fmt, va_list args)
{
	// Note: can't use the fzoutput_lock() critical section for these, as they will
	// be calling the lower level APIs (buffered I/O) internally, and quite legally so.
	//
	// That's why we need two mutexes per `fz_output` instance.
	printf_lock(out);
	fz_format_string(ctx, out, fz_write_emit, fmt, args);
	printf_unlock(out);
}

void
fz_write_printf(fz_context *ctx, fz_output *out, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf_lock(out);
	fz_format_string(ctx, out, fz_write_emit, fmt, args);
	printf_unlock(out);
	va_end(args);
}

void
fz_write_strings(fz_context* ctx, fz_output* out, ...)
{
	va_list args;
	va_start(args, out);
	fz_vwrite_strings(ctx, out, args);
	va_end(args);
}

void
fz_vwrite_strings(fz_context* ctx, fz_output* out, va_list ap)
{
	// Note: can't use the fzoutput_lock() critical section for these, as they will
	// be calling the lower level APIs (buffered I/O) internally, and quite legally so.
	//
	// That's why we need two mutexes per `fz_output` instance.
	int do_flush = 0;
	printf_lock(out);
	for (;;)
	{
		const char* s = va_arg(ap, const char*);
		if (!s)
			break;

		size_t l = strlen(s);
		if (l > 0)
		{
			fz_write_data(ctx, out, s, l);

			// starts or ends with a NEWLINE?
			do_flush |= (*s == '\n') || (s[l - 1] == '\n');
		}
	}
	if (do_flush)
		fz_flush_output(ctx, out);
	printf_unlock(out);
}

void
fz_flush_output(fz_context *ctx, fz_output *out)
{
	fzoutput_lock(out);
	fz_flush_output_no_lock(ctx, out);
	fzoutput_unlock(out);
}

void
fz_flush_output_no_lock(fz_context* ctx, fz_output* out)
{
	ASSERT(out);
	fz_write_bits_sync(ctx, out);
	if (out->bp && out->wp > out->bp)
	{
		out->write(ctx, out, out->bp, out->wp - out->bp);
	}
	out->wp = out->bp;
}

void
fz_write_byte(fz_context *ctx, fz_output *out, unsigned char x)
{
	ASSERT(out);
	fzoutput_lock(out);
	fz_write_byte_no_lock(ctx, out, x);
	fzoutput_unlock(out);
}

void
fz_write_byte_no_lock(fz_context* ctx, fz_output* out, unsigned char x)
{
	ASSERT(out);
	if (out->bp)
	{
		ASSERT(out->wp <= out->ep);
		ASSERT(out->wp >= out->bp);
		if (out->wp == out->ep)
		{
			out->write(ctx, out, out->bp, out->wp - out->bp);
			out->wp = out->bp;
		}
		*out->wp++ = x;
	}
	else
	{
		out->write(ctx, out, &x, 1);
	}
}

void
fz_write_char(fz_context *ctx, fz_output *out, char x)
{
	fz_write_byte(ctx, out, (unsigned char)x);
}

void
fz_write_data(fz_context *ctx, fz_output *out, const void *data_, size_t size)
{
	const char *data = (const char *)data_;

	fzoutput_lock(out);
	if (out->bp)
	{
		if (size >= (size_t) (out->ep - out->bp)) /* too large for buffer */
		{
			if (out->wp > out->bp)
			{
				out->write(ctx, out, out->bp, out->wp - out->bp);
				out->wp = out->bp;
			}
			out->write(ctx, out, data, size);
		}
		else if (out->wp + size <= out->ep) /* fits in current buffer */
		{
			memcpy(out->wp, data, size);
			out->wp += size;
		}
		else /* fits if we flush first */
		{
			size_t n = out->ep - out->wp;
			ASSERT(n <= size);
			memcpy(out->wp, data, n);
			out->write(ctx, out, out->bp, out->ep - out->bp);
			memcpy(out->bp, data + n, size - n);
			out->wp = out->bp + size - n;
		}
	}
	else
	{
		out->write(ctx, out, data, size);
	}
	fzoutput_unlock(out);
}

void
fz_write_buffer(fz_context *ctx, fz_output *out, fz_buffer *buf)
{
	fz_write_data(ctx, out, buf->data, buf->len);
}

void
fz_write_string(fz_context *ctx, fz_output *out, const char *s)
{
	fz_write_data(ctx, out, s, strlen(s));
}

void
fz_write_int64_be(fz_context* ctx, fz_output* out, int64_t x)
{
	char data[8];

	data[0] = x >> 56;
	data[1] = x >> 48;
	data[2] = x >> 40;
	data[3] = x >> 32;
	data[4] = x >> 24;
	data[5] = x >> 16;
	data[6] = x >> 8;
	data[7] = x;

	fz_write_data(ctx, out, data, 8);
}

void
fz_write_uint64_be(fz_context* ctx, fz_output* out, uint64_t x)
{
	fz_write_int64_be(ctx, out, x);
}

void
fz_write_int64_le(fz_context* ctx, fz_output* out, int64_t x)
{
	char data[8];

	data[0] = x;
	data[1] = x >> 8;
	data[2] = x >> 16;
	data[3] = x >> 24;
	data[4] = x >> 32;
	data[5] = x >> 40;
	data[6] = x >> 48;
	data[7] = x >> 56;

	fz_write_data(ctx, out, data, 8);
}

void
fz_write_uint64_le(fz_context* ctx, fz_output* out, uint64_t x)
{
	fz_write_int64_le(ctx, out, x);
}

void
fz_write_int32_be(fz_context *ctx, fz_output *out, int x)
{
	char data[4];

	data[0] = x>>24;
	data[1] = x>>16;
	data[2] = x>>8;
	data[3] = x;

	fz_write_data(ctx, out, data, 4);
}

void
fz_write_uint32_be(fz_context *ctx, fz_output *out, unsigned int x)
{
	fz_write_int32_be(ctx, out, (unsigned int)x);
}

void
fz_write_int32_le(fz_context *ctx, fz_output *out, int x)
{
	char data[4];

	data[0] = x;
	data[1] = x>>8;
	data[2] = x>>16;
	data[3] = x>>24;

	fz_write_data(ctx, out, data, 4);
}

void
fz_write_uint32_le(fz_context *ctx, fz_output *out, unsigned int x)
{
	fz_write_int32_le(ctx, out, (int)x);
}

void
fz_write_int16_be(fz_context *ctx, fz_output *out, int x)
{
	char data[2];

	data[0] = x>>8;
	data[1] = x;

	fz_write_data(ctx, out, data, 2);
}

void
fz_write_uint16_be(fz_context *ctx, fz_output *out, unsigned int x)
{
	fz_write_int16_be(ctx, out, (int)x);
}

void
fz_write_int16_le(fz_context *ctx, fz_output *out, int x)
{
	char data[2];

	data[0] = x;
	data[1] = x>>8;

	fz_write_data(ctx, out, data, 2);
}

void
fz_write_uint16_le(fz_context *ctx, fz_output *out, unsigned int x)
{
	fz_write_int16_le(ctx, out, (int)x);
}

void
fz_write_float_le(fz_context *ctx, fz_output *out, float f)
{
	union {float f; int32_t i;} u;
	u.f = f;
	fz_write_int32_le(ctx, out, u.i);
}

void
fz_write_float_be(fz_context *ctx, fz_output *out, float f)
{
	union {float f; int32_t i;} u;
	u.f = f;
	fz_write_int32_be(ctx, out, u.i);
}

void
fz_write_rune(fz_context *ctx, fz_output *out, int rune)
{
	char data[10];
	fz_write_data(ctx, out, data, fz_runetochar(data, rune));
}

void
fz_write_base64(fz_context *ctx, fz_output *out, const unsigned char *data, size_t size, int newline)
{
	static const char set[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	size_t i;
	fzoutput_lock(out);
	for (i = 0; i + 3 <= size; i += 3)
	{
		int c = data[i];
		int d = data[i+1];
		int e = data[i+2];
		if (newline && (i & 15) == 0)
			fz_write_byte_no_lock(ctx, out, '\n');
		fz_write_byte_no_lock(ctx, out, set[c>>2]);
		fz_write_byte_no_lock(ctx, out, set[((c&3)<<4)|(d>>4)]);
		fz_write_byte_no_lock(ctx, out, set[((d&15)<<2)|(e>>6)]);
		fz_write_byte_no_lock(ctx, out, set[e&63]);
	}
	if (size - i == 2)
	{
		int c = data[i];
		int d = data[i+1];
		fz_write_byte_no_lock(ctx, out, set[c>>2]);
		fz_write_byte_no_lock(ctx, out, set[((c&3)<<4)|(d>>4)]);
		fz_write_byte_no_lock(ctx, out, set[((d&15)<<2)]);
		fz_write_byte_no_lock(ctx, out, '=');
	}
	else if (size - i == 1)
	{
		int c = data[i];
		fz_write_byte_no_lock(ctx, out, set[c>>2]);
		fz_write_byte_no_lock(ctx, out, set[((c&3)<<4)]);
		fz_write_byte_no_lock(ctx, out, '=');
		fz_write_byte_no_lock(ctx, out, '=');
	}
	fzoutput_unlock(out);
}

void
fz_write_base64_buffer(fz_context *ctx, fz_output *out, fz_buffer *buf, int newline)
{
	unsigned char *data;
	size_t size = fz_buffer_storage(ctx, buf, &data);
	fz_write_base64(ctx, out, data, size, newline);
}

void
fz_append_base64(fz_context *ctx, fz_buffer *out, const unsigned char *data, size_t size, int newline)
{
	static const char set[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	size_t i;
	for (i = 0; i + 3 <= size; i += 3)
	{
		int c = data[i];
		int d = data[i+1];
		int e = data[i+2];
		if (newline && (i & 15) == 0)
			fz_append_byte(ctx, out, '\n');
		fz_append_byte(ctx, out, set[c>>2]);
		fz_append_byte(ctx, out, set[((c&3)<<4)|(d>>4)]);
		fz_append_byte(ctx, out, set[((d&15)<<2)|(e>>6)]);
		fz_append_byte(ctx, out, set[e&63]);
	}
	if (size - i == 2)
	{
		int c = data[i];
		int d = data[i+1];
		fz_append_byte(ctx, out, set[c>>2]);
		fz_append_byte(ctx, out, set[((c&3)<<4)|(d>>4)]);
		fz_append_byte(ctx, out, set[((d&15)<<2)]);
		fz_append_byte(ctx, out, '=');
	}
	else if (size - i == 1)
	{
		int c = data[i];
		fz_append_byte(ctx, out, set[c>>2]);
		fz_append_byte(ctx, out, set[((c&3)<<4)]);
		fz_append_byte(ctx, out, '=');
		fz_append_byte(ctx, out, '=');
	}
}

void
fz_append_base64_buffer(fz_context *ctx, fz_buffer *out, fz_buffer *buf, int newline)
{
	unsigned char *data;
	size_t size = fz_buffer_storage(ctx, buf, &data);
	fz_append_base64(ctx, out, data, size, newline);
}


void
fz_save_buffer(fz_context *ctx, fz_buffer *buf, const char *filename)
{
	if (!buf)
		return;

	fz_output *out = fz_new_output_with_path(ctx, filename, 0);
	fz_try(ctx)
	{
		fz_write_data(ctx, out, buf->data, buf->len);
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
		fz_drop_output(ctx, out);
	fz_catch(ctx)
		fz_rethrow(ctx);
}


#if BUILDING_MUPDF_MINIMAL_CORE < 2

fz_band_writer *fz_new_band_writer_of_size(fz_context *ctx, size_t size, fz_output *out)
{
	fz_band_writer *writer = fz_calloc(ctx, size, 1);
	writer->out = out;
	return writer;
}

void fz_write_header(fz_context *ctx, fz_band_writer *writer, int w, int h, int n, int alpha, int xres, int yres, int pagenum, fz_colorspace *cs, fz_separations *seps)
{
	if (writer == NULL || writer->band == NULL)
		return;

	if (w <= 0 || h <= 0 || n <= 0 || alpha < 0 || alpha > 1)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Invalid bandwriter header dimensions/setup");

	writer->w = w;
	writer->h = h;
	writer->s = fz_count_active_separations(ctx, seps);
	writer->n = n;
	writer->alpha = alpha;
	writer->xres = xres;
	writer->yres = yres;
	writer->pagenum = pagenum;
	writer->line = 0;
	writer->seps = fz_keep_separations(ctx, seps);
	writer->header(ctx, writer, cs);
}

void fz_write_band(fz_context *ctx, fz_band_writer *writer, int stride, int band_height, const unsigned char *samples)
{
	if (writer == NULL || writer->band == NULL)
		return;
	if (writer->line + band_height > writer->h)
		band_height = writer->h - writer->line;
	if (band_height < 0) {
		fz_throw(ctx, FZ_ERROR_GENERIC, "Too much band data!");
	}
	if (band_height > 0) {
		writer->band(ctx, writer, stride, writer->line, band_height, samples);
		writer->line += band_height;
	}
	if (writer->line == writer->h && writer->trailer) {
		writer->trailer(ctx, writer);
		/* Protect against more band_height == 0 calls */
		writer->line++;
	}
}

void fz_close_band_writer(fz_context *ctx, fz_band_writer *writer)
{
	if (writer == NULL)
		return;
	if (writer->close != NULL)
		writer->close(ctx, writer);
	writer->close = NULL;
}

void fz_drop_band_writer(fz_context *ctx, fz_band_writer *writer)
{
	if (writer == NULL)
		return;
	if (writer->drop != NULL)
		writer->drop(ctx, writer);
	fz_drop_separations(ctx, writer->seps);
	fz_free(ctx, writer);
}

#endif


int fz_output_supports_stream(fz_context *ctx, fz_output *out)
{
	return out != NULL && out->as_stream != NULL;
}

void fz_write_bits(fz_context *ctx, fz_output *out, unsigned int data, int num_bits)
{
	while (num_bits)
	{
		/* How many bits will be left in the current byte after we
		 * insert these bits? */
		int n = 8 - num_bits - out->buffered;
		if (n >= 0)
		{
			/* We can fit our data in. */
			out->bits |= data << n;
			out->buffered += num_bits;
			num_bits = 0;
		}
		else
		{
			/* There are 8 - out->buffered bits left to be filled. We have
			 * num_bits to fill it with, which is more, so we need to throw
			 * away the bottom 'num_bits - (8 - out->buffered)' bits. That's
			 * num_bits + out->buffered - 8 = -(8 - num_bits - out_buffered) = -n */
			out->bits |= data >> -n;
			data &= ~(out->bits << -n);
			num_bits = -n;
			out->buffered = 8;
		}
		if (out->buffered == 8)
		{
			fz_write_byte(ctx, out, out->bits);
			out->buffered = 0;
			out->bits = 0;
		}
	}

}

void fz_write_bits_sync(fz_context *ctx, fz_output *out)
{
	if (out->buffered == 0)
		return;
	fz_write_bits(ctx, out, 0, 8 - out->buffered);
}
