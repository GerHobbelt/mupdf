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

static void
file_write(fz_context *ctx, void *opaque, const void *buffer, size_t count)
{
	FILE *file = (FILE *)opaque;
	size_t n;

	if (count == 0)
		return;

	if (count == 1)
	{
		int x = putc(((unsigned char*)buffer)[0], file);
		if (x == EOF && ferror(file))
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot fwrite: %s", strerror(errno));
		return;
	}

	n = fwrite(buffer, 1, count, file);
	if (n < count && ferror(file))
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot fwrite: %s (written %zu of %zu bytes)", strerror(errno), n, count);
}

#if defined(_WIN32) 

static void
stdio_write(fz_context* ctx, DWORD channel, const void* buffer, size_t count)
{
	// Windows: https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile#pipes
	// > "When writing to a non-blocking, byte-mode pipe handle with insufficient buffer space,
	// > WriteFile returns TRUE with *lpNumberOfBytesWritten < nNumberOfBytesToWrite."

	assert(channel == STD_OUTPUT_HANDLE || channel == STD_ERROR_HANDLE);

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
		DWORD n_lim = min(PIPE_MAX_NONBLOCK_BUFFER_SIZE, n);
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
}

#endif

static void
stdout_write(fz_context *ctx, void *opaque, const void *buffer, size_t count)
{
#ifdef _WIN32
	stdio_write(ctx, STD_OUTPUT_HANDLE, buffer, count);
#else
	file_write(ctx, stdout, buffer, count);
#endif
}

static fz_output fz_stdout_global = {
	NULL,
	stdout_write,
	NULL,
	NULL,
	NULL,
};

fz_output *fz_stdout(fz_context *ctx)
{
	return &fz_stdout_global;
}

static void
stderr_write(fz_context *ctx, void *opaque, const void *buffer, size_t count)
{
#ifdef _WIN32
	stdio_write(ctx, STD_ERROR_HANDLE, buffer, count);
#else
	file_write(ctx, stderr, buffer, count);
#endif
}

static fz_output fz_stderr_global = {
	NULL,
	stderr_write,
	NULL,
	NULL,
	NULL,
};

fz_output *fz_stderr(fz_context *ctx)
{
	return &fz_stderr_global;
}

#ifdef _WIN32
static void
stdods_write(fz_context *ctx, void *opaque, const void *buffer, size_t count)
{
	// Assume that the heap MAY be corrupted when we call into here.
	// Such last effort error messages will invariably be short-ish.
	// Besides, using a bit of stack for smaller messages reduces heap alloc+free
	// call overhead.
	char stkbuf[LONGLINE + 1];
	char* buf = stkbuf;
	if (count > sizeof(stkbuf) - 1)
		buf = fz_malloc(ctx, count+1);

	memcpy(buf, buffer, count);
	buf[count] = 0;
	OutputDebugStringA(buf);

	if (buf != stkbuf)
		fz_free(ctx, buf);
}

static fz_output fz_stdods_global = {
	NULL,
	stdods_write,
	NULL,
	NULL,
	NULL,
};

fz_output *fz_stdods(fz_context *ctx)
{
	return &fz_stdods_global;
}
#endif

fz_output *fz_stddbg(fz_context *ctx)
{
	if (ctx->stddbg)
		return ctx->stddbg;

	return fz_stderr(ctx);
}

void fz_set_stddbg(fz_context *ctx, fz_output *out)
{
	if (ctx == NULL)
		return;

	ctx->stddbg = out;
}

static void
file_seek(fz_context *ctx, void *opaque, int64_t off, int whence)
{
	FILE *file = (FILE *)opaque;
#ifdef _WIN32
	int n = _fseeki64(file, off, whence);
#else
	int n = fseeko(file, off, whence);
#endif
	if (n < 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot fseek: %s", strerror(errno));
}

static int64_t
file_tell(fz_context *ctx, void *opaque)
{
	FILE *file = (FILE *)opaque;
#ifdef _WIN32
	int64_t off = _ftelli64(file);
#else
	int64_t off = ftello(file);
#endif
	if (off == -1)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot ftell: %s", strerror(errno));
	return off;
}

static void
file_drop(fz_context *ctx, void *opaque)
{
	FILE *file = (FILE *)opaque;
	int n = fclose(file);
	if (n < 0)
		fz_warn(ctx, "cannot fclose: %s", strerror(errno));
}

static fz_stream *
file_as_stream(fz_context *ctx, void *opaque)
{
	FILE *file = (FILE *)opaque;
	fflush(file);
	return fz_open_file_ptr_no_close(ctx, file);
}

static void file_truncate(fz_context *ctx, void *opaque)
{
	FILE *file = opaque;
	fflush(file);

#ifdef _WIN32
	{
		__int64 pos = _ftelli64(file);
		if (pos >= 0)
			_chsize_s(fileno(file), pos);
	}
#else
	{
		off_t pos = ftello(file);
		if (pos >= 0)
			(void)ftruncate(fileno(file), pos);
	}
#endif
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
		mu_create_mutex(&out->buf_mutex);
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
			drop(ctx, state);
		mu_destroy_mutex(&out->buf_mutex);
		fz_free(ctx, out->bp);
		fz_free(ctx, out);
		fz_rethrow(ctx);
	}
	return out;
}

int fz_set_output_buffer(fz_context* ctx, fz_output* out, int bufsiz)
{
	int rv = 1;

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
	if (mu_mutex_is_zeroed(&out->buf_mutex))
	{
		mu_create_mutex(&out->buf_mutex);
	}

	mu_lock_mutex(&out->buf_mutex);
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
	mu_unlock_mutex(&out->buf_mutex);
	return rv;
}

static void null_write(fz_context *ctx, void *opaque, const void *buffer, size_t count)
{
}

static void null_seek(fz_context* ctx, void* state, int64_t offset, int whence)
{
}

static void null_truncate(fz_context* ctx, void* state)
{
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
#ifdef _WIN32
	/* Ensure we create a brand new file. We don't want to clobber our old file. */
	if (!append)
	{
		if (fz_remove_utf8(ctx, filename) < 0)
			if (errno != ENOENT)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot remove file '%s': %s", filename, strerror(errno));
	}
#if defined(__MINGW32__) || defined(__MINGW64__)
	file = fz_fopen_utf8(ctx, filename, append ? "rb+" : "wb+"); /* 'x' flag not supported. */
#else
	file = fz_fopen_utf8(ctx, filename, append ? "rb+" : "wb+x");
#endif
	if (append)
	{
		if (file == NULL)
			file = fz_fopen_utf8(ctx, filename, "wb+");
		else
			fseek(file, 0, SEEK_END);
	}
#else
	/* Ensure we create a brand new file. We don't want to clobber our old file. */
	if (!append)
	{
		if (remove(filename) < 0)
			if (errno != ENOENT)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot remove file '%s': %s", filename, strerror(errno));
	}
	file = fopen(filename, append ? "rb+" : "wb+x");
	if (file == NULL && append)
		file = fopen(filename, "wb+");
#endif
	if (!file)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open file '%s': %s", filename, strerror(errno));

	setvbuf(file, NULL, _IONBF, 0); /* we do our own buffering */
	out = fz_new_output(ctx, 8192, file, file_write, NULL, file_drop);
	out->seek = file_seek;
	out->tell = file_tell;
	out->as_stream = file_as_stream;
	out->truncate = file_truncate;

	return out;
}

static void
buffer_write(fz_context *ctx, void *opaque, const void *data, size_t len)
{
	fz_buffer *buffer = opaque;
	fz_append_data(ctx, buffer, data, len);
}

static void
buffer_seek(fz_context *ctx, void *opaque, int64_t off, int whence)
{
	fz_throw(ctx, FZ_ERROR_GENERIC, "cannot seek in buffer: %s", strerror(errno));
}

static int64_t
buffer_tell(fz_context *ctx, void *opaque)
{
	fz_buffer *buffer = opaque;
	return (int64_t)buffer->len;
}

static void
buffer_drop(fz_context *ctx, void *opaque)
{
	fz_buffer *buffer = opaque;
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
	mu_lock_mutex(&out->buf_mutex);
	fz_flush_output_no_lock(ctx, out);
	if (out->close)
		out->close(ctx, out->state);
	out->close = NULL;
	mu_unlock_mutex(&out->buf_mutex);
}

void
fz_drop_output(fz_context *ctx, fz_output *out)
{
	if (out)
	{
		if (out->close)
			fz_warn(ctx, "dropping unclosed output");
		if (out->drop)
			out->drop(ctx, out->state);
		fz_free(ctx, out->bp);
		mu_destroy_mutex(&out->buf_mutex);

		if (out != &fz_stdout_global && out != &fz_stderr_global && out != &fz_stdods_global)
			fz_free(ctx, out);
	}
}

void
fz_seek_output(fz_context* ctx, fz_output* out, int64_t off, int whence)
{
	if (out->seek == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot seek in unseekable output stream");

	mu_lock_mutex(&out->buf_mutex);
	fz_flush_output(ctx, out);
	out->seek(ctx, out->state, off, whence);
	mu_unlock_mutex(&out->buf_mutex);
}

int64_t
fz_tell_output(fz_context *ctx, fz_output *out)
{
	if (out->tell == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot tell in untellable output stream");

	mu_lock_mutex(&out->buf_mutex);
	int64_t pos = out->tell(ctx, out->state);
	if (out->bp)
		pos += (out->wp - out->bp);
	mu_unlock_mutex(&out->buf_mutex);
	return pos;
}

fz_stream *
fz_stream_from_output(fz_context *ctx, fz_output *out)
{
	if (out->as_stream == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot derive input stream from output stream");
	fz_flush_output(ctx, out);
	return out->as_stream(ctx, out->state);
}

void
fz_truncate_output(fz_context *ctx, fz_output *out)
{
	if (out->truncate == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot truncate this output stream");
	fz_flush_output(ctx, out);
	out->truncate(ctx, out->state);
}

static void
fz_write_emit(fz_context *ctx, void *out, int c)
{
	fz_write_byte(ctx, (fz_output *)out, c);
}

void
fz_write_vprintf(fz_context *ctx, fz_output *out, const char *fmt, va_list args)
{
	fz_format_string(ctx, out, fz_write_emit, fmt, args);
}

void
fz_write_printf(fz_context *ctx, fz_output *out, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fz_format_string(ctx, out, fz_write_emit, fmt, args);
	va_end(args);
}

void
fz_flush_output(fz_context *ctx, fz_output *out)
{
	mu_lock_mutex(&out->buf_mutex);
	fz_flush_output_no_lock(ctx, out);
	mu_unlock_mutex(&out->buf_mutex);
}

void
fz_flush_output_no_lock(fz_context* ctx, fz_output* out)
{
	if (out->wp > out->bp)
	{
		out->write(ctx, out->state, out->bp, out->wp - out->bp);
		out->wp = out->bp;
	}
}

void
fz_write_byte(fz_context *ctx, fz_output *out, unsigned char x)
{
	mu_lock_mutex(&out->buf_mutex);
	fz_write_byte_no_lock(ctx, out, x);
	mu_unlock_mutex(&out->buf_mutex);
}

void
fz_write_byte_no_lock(fz_context* ctx, fz_output* out, unsigned char x)
{
	if (out->bp)
	{
		ASSERT(out->wp <= out->ep);
		ASSERT(out->wp >= out->bp);
		if (out->wp == out->ep)
		{
			out->write(ctx, out->state, out->bp, out->wp - out->bp);
			out->wp = out->bp;
		}
		*out->wp++ = x;
	}
	else
	{
		out->write(ctx, out->state, &x, 1);
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

	mu_lock_mutex(&out->buf_mutex);
	if (out->bp)
	{
		if (size >= (size_t) (out->ep - out->bp)) /* too large for buffer */
		{
			if (out->wp > out->bp)
			{
				out->write(ctx, out->state, out->bp, out->wp - out->bp);
				out->wp = out->bp;
			}
			out->write(ctx, out->state, data, size);
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
			out->write(ctx, out->state, out->bp, out->ep - out->bp);
			memcpy(out->bp, data + n, size - n);
			out->wp = out->bp + size - n;
		}
	}
	else
	{
		out->write(ctx, out->state, data, size);
	}
	mu_unlock_mutex(&out->buf_mutex);
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
	mu_lock_mutex(&out->buf_mutex);
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
	mu_unlock_mutex(&out->buf_mutex);
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

int fz_output_supports_stream(fz_context *ctx, fz_output *out)
{
	return out != NULL && out->as_stream != NULL;
}
