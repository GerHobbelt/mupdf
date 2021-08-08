﻿#include "mupdf/fitz.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define USE_OUTPUT_DEBUG_STRING  1
#include <windows.h>
#endif

#ifdef __ANDROID__
#define USE_ANDROID_LOG
#include <android/log.h>
#endif

#define QUIET_ERROR				0x0001
#define QUIET_WARN				0x0002
#define QUIET_INFO				0x0004
#define QUIET_DEBUG				0x0008

#define QUIET_STDIO_FATALITY	0x0010

static int quiet_mode = 0;

void fz_default_error_callback(void *user, const char *message)
{
	if (!(quiet_mode & QUIET_ERROR))
	{
		fprintf(stderr, "error: %s\n", message);
	}
#ifdef USE_OUTPUT_DEBUG_STRING
	if (quiet_mode & (QUIET_DEBUG | QUIET_STDIO_FATALITY))
	{
		OutputDebugStringA("error: ");
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
	}
#endif
#ifdef USE_ANDROID_LOG
	__android_log_print(ANDROID_LOG_ERROR, "libmupdf", "%s", message);
#endif
}

void fz_default_warning_callback(void *user, const char *message)
{
	if (!(quiet_mode & QUIET_WARN))
	{
		fprintf(stderr, "warning: %s\n", message);
	}
#ifdef USE_OUTPUT_DEBUG_STRING
	if (quiet_mode & (QUIET_DEBUG | QUIET_STDIO_FATALITY))
	{
		OutputDebugStringA("warning: ");
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
	}
#endif
#ifdef USE_ANDROID_LOG
	__android_log_print(ANDROID_LOG_WARN, "libmupdf", "%s", message);
#endif
}

/* Warning context */

void fz_set_warning_callback(fz_context *ctx, fz_error_print_callback* print, void *user)
{
	ctx->warn.print_user = user;
	ctx->warn.print = print ? print : fz_default_warning_callback;
}

void fz_get_warning_callback(fz_context* ctx, fz_error_print_callback** print, void** user)
{
	*user = ctx->warn.print_user;
	*print = ctx->warn.print;
}

void fz_default_info_callback(void* user, const char* message)
{
	if (!(quiet_mode & QUIET_INFO))
	{
		fprintf(stderr, "%s\n", message);
	}
#ifdef USE_OUTPUT_DEBUG_STRING
	if (quiet_mode & QUIET_DEBUG)
	{
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
	}
#endif
#ifdef USE_ANDROID_LOG
	__android_log_print(ANDROID_LOG_INFO, "libmupdf", "%s", message);
#endif
}

/* Warning context */

void fz_set_info_callback(fz_context* ctx, fz_error_print_callback* print, void* user)
{
	ctx->info.print_user = user;
	ctx->info.print = print ? print : fz_default_info_callback;
}

void fz_get_info_callback(fz_context* ctx, fz_error_print_callback** print, void** user)
{
	*user = ctx->info.print_user;
	*print = ctx->info.print;
}

static inline int edit_bit(int user, int set)
{
	if (user < 0)
		return quiet_mode & set;  // current state of the flag
	if (user)
		return set;
	return 0;
}

void fz_default_error_warn_info_mode(int quiet_error, int quiet_warn, int quiet_info)
{
	quiet_mode = edit_bit(quiet_error, QUIET_ERROR) | edit_bit(quiet_warn, QUIET_WARN) | edit_bit(quiet_info, QUIET_INFO);
}

void fz_enable_dbg_output(int severity)
{
	quiet_mode |= (severity ? QUIET_STDIO_FATALITY : QUIET_DEBUG);
}

void fz_var_imp(void *var)
{
	/* Do nothing */
}

void fz_flush_warnings(fz_context *ctx)
{
	if (!ctx)
		return;

	if (ctx->warn.count > 1)
	{
		char buf[50];
		fz_snprintf(buf, sizeof buf, "... repeated %d times...", ctx->warn.count);
		if (ctx->warn.print)
			ctx->warn.print(ctx->warn.print_user, buf);
	}
	ctx->warn.message[0] = 0;
	ctx->warn.count = 0;
}

static void prepare_message(char* buf, size_t bufsize, const char* fmt, va_list ap)
{
	size_t endidx = bufsize - 1;

	fz_vsnprintf(buf, bufsize, fmt, ap);
	buf[endidx] = 0;

	size_t len = strlen(buf);
	// has the input been truncated?
	if (len == endidx)
	{
		strcpy(buf - sizeof("(...truncated...)"), "(...truncated...)");
		return;
	}

	// as each message will be automatically appended with a LINEFEED suitable for the output channel of choice,
	// we will now strip off one(1) LF at the end of the message, iff it has any.
	//
	// We only strip off one LF as messages with multiple LF characters at the end must have surely intended
	// to be output that way.
	if (buf[len - 1] == '\n')
	{
		buf[len - 1] = 0;
	}
}

#define prepmsg(buf, fmt, ap)   prepare_message(buf, sizeof buf, fmt, ap)

void fz_vwarn(fz_context *ctx, const char *fmt, va_list ap)
{
	char buf[sizeof ctx->warn.message];

	prepmsg(buf, fmt, ap);

	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	if (!ctx)
	{
		fz_default_warning_callback(NULL, buf);
		return;
	}

	if (!strcmp(buf, ctx->warn.message))
	{
		ctx->warn.count++;
	}
	else
	{
		fz_flush_warnings(ctx);
		if (ctx->warn.print)
			ctx->warn.print(ctx->warn.print_user, buf);
		strcpy(ctx->warn.message, buf);
		ctx->warn.count = 1;
	}
}

void fz_warn(fz_context *ctx, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_vwarn(ctx, fmt, ap);
	va_end(ap);
}

void fz_vinfo(fz_context* ctx, const char* fmt, va_list ap)
{
	char buf[4096];

	fz_flush_warnings(ctx);

	prepmsg(buf, fmt, ap);

	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	if (ctx && ctx->info.print)
		ctx->info.print(ctx->info.print_user, buf);
	else
		fz_default_info_callback(NULL, buf);
}

void fz_info(fz_context* ctx, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_vinfo(ctx, fmt, ap);
	va_end(ap);
}

void fz_verror(fz_context* ctx, const char* fmt, va_list ap)
{
	char buf[4096];

	fz_flush_warnings(ctx);

	prepmsg(buf, fmt, ap);

	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	if (ctx && ctx->error.print)
		ctx->error.print(ctx->error.print_user, buf);
	else
		fz_default_error_callback(NULL, buf);
}

void fz_error(fz_context* ctx, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_verror(ctx, fmt, ap);
	va_end(ap);
}

/* Error context */

void fz_set_error_callback(fz_context *ctx, fz_error_print_callback* print, void *user)
{
	ctx->error.print_user = user;
	ctx->error.print = print ? print : fz_default_error_callback;
}

void fz_get_error_callback(fz_context* ctx, fz_error_print_callback** print, void** user)
{
	*user = ctx->error.print_user;
	*print = ctx->error.print;
}

/* When we first setjmp, state is set to 0. Whenever we throw, we add 2 to
 * this state. Whenever we enter the always block, we add 1.
 *
 * fz_push_try sets state to 0.
 * If (fz_throw called within fz_try)
 *     fz_throw makes state = 2.
 *     If (no always block present)
 *         enter catch region with state = 2. OK.
 *     else
 *         fz_always entered as state < 3; Makes state = 3;
 *         if (fz_throw called within fz_always)
 *             fz_throw makes state = 5
 *             fz_always is not reentered.
 *             catch region entered with state = 5. OK.
 *         else
 *             catch region entered with state = 3. OK
 * else
 *     if (no always block present)
 *         catch region not entered as state = 0. OK.
 *     else
 *         fz_always entered as state < 3. makes state = 1
 *         if (fz_throw called within fz_always)
 *             fz_throw makes state = 3;
 *             fz_always NOT entered as state >= 3
 *             catch region entered with state = 3. OK.
 *         else
 *             catch region entered with state = 1.
 */

FZ_NORETURN static void throw(fz_context *ctx, int code)
{
	if (ctx->error.top > ctx->error.stack)
	{
		ctx->error.top->state += 2;
		if (ctx->error.top->code != FZ_ERROR_NONE)
			fz_warn(ctx, "clobbering previous error code and message (throw in always block?)");
		ctx->error.top->code = code;
		fz_longjmp(ctx->error.top->buffer, 1);
	}
	else
	{
		fz_flush_warnings(ctx);
		if (ctx->error.print)
			ctx->error.print(ctx->error.print_user, "aborting process from uncaught error!");
		exit(EXIT_FAILURE);
	}
}

fz_jmp_buf *fz_push_try(fz_context *ctx)
{
	/* If we would overflow the exception stack, throw an exception instead
	 * of entering the try block. We assume that we always have room for
	 * 1 extra level on the stack here - i.e. we throw the error on us
	 * starting to use the last level. */
	if (ctx->error.top + 2 >= ctx->error.stack + nelem(ctx->error.stack))
	{
		fz_strlcpy(ctx->error.message, "exception stack overflow!", sizeof ctx->error.message);

		fz_flush_warnings(ctx);
		if (ctx->error.print)
			ctx->error.print(ctx->error.print_user, ctx->error.message);

		/* We need to arrive in the always/catch block as if throw had taken place. */
		ctx->error.top++;
		ctx->error.top->state = 2;
		ctx->error.top->code = FZ_ERROR_GENERIC;
	}
	else
	{
		ctx->error.top++;
		ctx->error.top->state = 0;
		ctx->error.top->code = FZ_ERROR_NONE;
	}
	return &ctx->error.top->buffer;
}

int fz_do_try(fz_context *ctx)
{
#ifdef __COVERITY__
	return 1;
#else
	return ctx->error.top->state == 0;
#endif
}

int fz_do_always(fz_context *ctx)
{
#ifdef __COVERITY__
	return 1;
#else
	if (ctx->error.top->state < 3)
	{
		ctx->error.top->state++;
		return 1;
	}
	return 0;
#endif
}

int fz_do_catch(fz_context *ctx)
{
	ctx->error.errcode = ctx->error.top->code;
	if (ctx->error.errcode != FZ_ERROR_NONE)
	{
		// See fz_rethrow() code comments for the complete story:
		ctx->error.last_nonzero_errcode = ctx->error.errcode;
	}
	int rv = ((ctx->error.top--)->state > 1);
	if (rv)
		return rv;
	else
		return 0;
}

int fz_caught(fz_context *ctx)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	return ctx->error.errcode;
}

const char *fz_caught_message(fz_context *ctx)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	return ctx->error.message;
}

/* coverity[+kill] */
FZ_NORETURN void fz_vthrow(fz_context *ctx, int code, const char *fmt, va_list ap)
{
	fz_vsnprintf(ctx->error.message, sizeof ctx->error.message, fmt, ap);
	ctx->error.message[sizeof(ctx->error.message) - 1] = 0;

	if (code != FZ_ERROR_ABORT && code != FZ_ERROR_TRYLATER)
	{
		fz_flush_warnings(ctx);
		if (ctx->error.print)
			ctx->error.print(ctx->error.print_user, ctx->error.message);
	}

	throw(ctx, code);
}

/* coverity[+kill] */
FZ_NORETURN void fz_throw(fz_context *ctx, int code, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_vthrow(ctx, code, fmt, ap);
	va_end(ap);
}

/* coverity[+kill] */
FZ_NORETURN void fz_rethrow(fz_context *ctx)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	if (ctx->error.errcode == FZ_ERROR_NONE)
	{
		// See pdf-xref.c for one occasion where this is relevant:
		//
		// keep the exception error code intact for the rethrow as the try/catch logic inside
		// fz_drop_document() will reset the errorcode -- as that try/catch is indiscernible
		// from a try/catch which *follows* this chunk as we *emulate* C++ exceptions but
		// DO NOT have access to the compiler's *scope analysis* which is required for this
		// emulation bug to go away.
		//
		// Hence we need to 'stow away' the error code for re-use by the fz_rethrow() call.
		// For our 'hacky' solution to this conundrum, see the fz_rethrow() implementation:
		// so as not having to wade through a zillion lines of code to patch all relevant try/catch/rethrow
		// blocks, we simply remember the last non-zero error code and use that iff the
		// rethrow would otherwise rethrow a zero=okay exception.
		assert(ctx->error.last_nonzero_errcode > FZ_ERROR_NONE);
		ctx->error.errcode = ctx->error.last_nonzero_errcode;
	}
	throw(ctx, ctx->error.errcode);
}

void fz_rethrow_if(fz_context *ctx, int err)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	if (ctx->error.errcode == err)
		fz_rethrow(ctx);
}
