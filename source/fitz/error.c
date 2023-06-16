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
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

#include "mupdf/fitz.h"

#include "mupdf/helpers/system-header-files.h"

#include "mupdf/assertions.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <intrin.h>		// __debugbreak()

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef countof
#define countof(x)   (sizeof(x) / sizeof((x)[0]))
#endif


#if FZ_VERBOSE_EXCEPTIONS

#undef fz_vthrow
#undef fz_throw
#undef fz_rethrow
#undef fz_morph_error
#undef fz_vwarn
#undef fz_warn
#undef fz_rethrow_if
#undef fz_log_error_printf
#undef fz_vlog_error_printf
#undef fz_log_error

#endif


#define QUIET_ERROR				0x0001
#define QUIET_WARN				0x0002
#define QUIET_INFO				0x0004
#define QUIET_DEBUG				0x0008

#define QUIET_STDIO_FATALITY	0x0010

static int quiet_mode = 0;

int fz_bundle_str_msg_parts(char* dst, size_t dstsiz, const char* s1, const char* s2, const char* s3)
{
	size_t l1 = s1 ? strlen(s1) : 0;
	size_t l2 = s2 ? strlen(s2) : 0;
	size_t l3 = s3 ? strlen(s3) : 0;
	size_t required_size = 1;
	required_size += l1;
	required_size += l2;
	required_size += l3;

	ASSERT0(required_size <= dstsiz);
	if (required_size > dstsiz)
		return 0;

	dst[0] = 0;
	if (l1)
	{
		strcpy(dst, s1);
		dst += l1;
	}
	if (l2)
	{
		strcpy(dst, s2);
		dst += l2;
	}
	if (l3)
	{
		strcpy(dst, s3);
		dst += l3;
	}

	return (int)required_size;
}

void fz_default_error_callback(fz_context* ctx, void *user, const char *message)
{
	ASSERT(message != NULL);

	if (!(quiet_mode & QUIET_ERROR))
	{
		fz_write_strings(ctx, fz_stderr(ctx), "error: ", message, "\n", NULL);
	}

	if (quiet_mode & (QUIET_DEBUG | QUIET_STDIO_FATALITY))
	{
		fz_output* out = ctx->stddbg;
		if (out != fz_stderr(ctx))
		{
			fz_output_set_severity_level(ctx, out, FZO_SEVERITY_ERROR);

			fz_write_strings(ctx, out, "error: ", message, "\n", NULL);
		}
	}
}

void fz_default_warning_callback(fz_context* ctx, void* user, const char* message)
{
	ASSERT(message != NULL);

	if (!(quiet_mode & QUIET_WARN))
	{
		fz_write_strings(ctx, fz_stderr(ctx), "warning: ", message, "\n", NULL);
	}

	if (quiet_mode & (QUIET_DEBUG | QUIET_STDIO_FATALITY))
	{
		fz_output* out = ctx->stddbg;
		if (out != fz_stderr(ctx))
		{
			fz_output_set_severity_level(ctx, out, FZO_SEVERITY_WARNING);

			fz_write_strings(ctx, out, "warning: ", message, "\n", NULL);
		}
	}
}

/* Warning context */

fz_error_print_callback* fz_set_warning_callback(fz_context *ctx, fz_error_print_callback* print, void *user)
{
	fz_error_print_callback* rv = ctx->warn.print;
	ctx->warn.print_user = user;
	ctx->warn.print = print ? print : fz_default_warning_callback;
	return rv;
}

void fz_get_warning_callback(fz_context* ctx, fz_error_print_callback** print, void** user)
{
	if (user)
		*user = ctx->warn.print_user;
	if (print)
		*print = ctx->warn.print;
}

void fz_default_info_callback(fz_context* ctx, void* user, const char* message)
{
	ASSERT(message != NULL);

	if (!(quiet_mode & QUIET_INFO))
	{
		fz_write_strings(ctx, fz_stderr(ctx), message, "\n", NULL);
	}

	if (quiet_mode & (QUIET_DEBUG | QUIET_STDIO_FATALITY))
	{
		fz_output* out = ctx->stddbg;
		if (out != fz_stderr(ctx))
		{
			fz_output_set_severity_level(ctx, out, FZO_SEVERITY_INFO);

			fz_write_strings(ctx, out, message, "\n", NULL);
		}
	}
}

/* Info context */

fz_error_print_callback* fz_set_info_callback(fz_context* ctx, fz_error_print_callback* print, void* user)
{
	fz_error_print_callback* rv = ctx->info.print;
	ctx->info.print_user = user;
	ctx->info.print = print ? print : fz_default_info_callback;
	return rv;
}

void fz_get_info_callback(fz_context* ctx, fz_error_print_callback** print, void** user)
{
	if (user)
		*user = ctx->info.print_user;
	if (print)
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
	// keep stdio unreachability + debug mode intact
	quiet_mode &= ~(QUIET_ERROR | QUIET_WARN | QUIET_INFO);

	quiet_mode |= edit_bit(quiet_error, QUIET_ERROR) | edit_bit(quiet_warn, QUIET_WARN) | edit_bit(quiet_info, QUIET_INFO);
}

void fz_enable_dbg_output(void)
{
	quiet_mode |= QUIET_DEBUG;
}

void fz_disable_dbg_output(void)
{
	// do NOT reset the stdio unreachability state
	quiet_mode &= ~QUIET_DEBUG;
}

void fz_enable_dbg_output_on_stdio_unreachable(void)
{
	quiet_mode |= QUIET_STDIO_FATALITY;
}


void fz_var_imp(void *var)
{
	/* Do nothing */
}

void fz_flush_warnings(fz_context *ctx)
{
	if (!ctx)
	{
		ctx = __fz_get_RAW_global_context();
	}
	if (!ctx)
		return;

	if (ctx->warn.count > 1)
	{
		char buf[50];
		fz_snprintf(buf, sizeof buf, "... repeated %d times...", ctx->warn.count);
		if (ctx->warn.print)
			ctx->warn.print(ctx, ctx->warn.print_user, buf);
	}
	ctx->warn.message[0] = 0;
	ctx->warn.count = 0;
}

void fz_flush_all_std_logging_channels(fz_context* ctx)
{
	if (!ctx)
	{
		ctx = __fz_get_RAW_global_context();
	}
	if (!ctx)
		return;

	fz_flush_warnings(ctx);

	// sequence in anticipation of spurious failures: we attempt to save all
	// log data as much as possible:
	//
	// - error
	// - debug (for up to date diagnostics)
	// - stdout
	// and then, just in case one or more of the above had anything to yak on the way out:
	// - error
	// - debug (for up to date diagnostics)
	fz_output* channel = fz_stderr(ctx);
	fz_flush_output(ctx, channel);
	channel = fz_stddbg(ctx);
	fz_flush_output(ctx, channel);
	channel = fz_stdout(ctx);
	fz_flush_output(ctx, channel);
	channel = fz_stderr(ctx);
	fz_flush_output(ctx, channel);
	channel = fz_stddbg(ctx);
	fz_flush_output(ctx, channel);
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
		strcpy(buf + bufsize - sizeof("(...truncated...)"), "(...truncated...)");
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

#define prepmsg(buf, fmt, ap)   prepare_message(buf, sizeof(buf), fmt, ap)

static void prepare_append_message(char* buf, size_t bufsize, const char *joinstr, const char* msg)
{
	size_t endidx = bufsize - 1;

	size_t len = strlen(buf);
	size_t msglen = strlen(msg);
	size_t joinlen = strlen(joinstr);
	// as each message will be automatically appended with a LINEFEED suitable for the output channel of choice,
	// we will now strip off one(1) LF at the end of the message, iff it has any.
	//
	// We only strip off one LF as messages with multiple LF characters at the end must have surely intended
	// to be output that way.
	if (msg[msglen - 1] == '\n')
	{
		msglen--;
	}

	// truncate the combo?
	if (len + joinlen + msglen >= endidx)
	{
		// heuristic: keep half of what was, allow half of the space for the new msg:
		const char trunc_msg[] = "(...truncated...)";
		const size_t trunc_msglen = sizeof(trunc_msg);

		size_t desired_len = bufsize / 2 - trunc_msglen / 2;
		// calculate where the truncation will land:
		size_t remaining_len = endidx - len - trunc_msglen;
		if (desired_len < remaining_len)
			desired_len = remaining_len;
		if (msglen > desired_len)
		{
			size_t offset = msglen - desired_len;
			msg += offset;
			msglen -= offset;
		}
		len = endidx - msglen - trunc_msglen;
		strcpy(buf + len, trunc_msg);
		len += trunc_msglen;
	}
	else
	{
		// inject separator between existing message and the piece to be appended:
		strcpy(buf + len, joinstr);
		len++;
	}

	memcpy(buf + len, msg, msglen);
	len += msglen;
	buf[len] = 0;
}

#define prepaddmsg(buf, join, msg)   prepare_append_message(buf, sizeof(buf), join, msg)

void fz_vwarn(fz_context *ctx, const char *fmt, va_list ap)
{
	if (!ctx)
	{
		ctx = __fz_get_RAW_global_context();
	}

	char buf[sizeof ctx->warn.message];

	prepmsg(buf, fmt, ap);

	if (!ctx)
	{
		fz_default_warning_callback(NULL, NULL, buf);
	}
	else
	{
		if (!strcmp(buf, ctx->warn.message))
		{
			ctx->warn.count++;
		}
		else
		{
			fz_flush_warnings(ctx);
			if (ctx->warn.print)
				ctx->warn.print(ctx, ctx->warn.print_user, buf);
			else
				fz_default_warning_callback(ctx, NULL, buf);

			strcpy(ctx->warn.message, buf);
			ctx->warn.count = 1;
		}
	}
}

void fz_warn(fz_context *ctx, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_vwarn(ctx, fmt, ap);
	va_end(ap);
}

#if FZ_VERBOSE_EXCEPTIONS
void fz_vwarnFL(fz_context *ctx, const char *file, int line, const char *fmt, va_list ap)
{
	char buf[sizeof ctx->warn.message];

	fz_vsnprintf(buf, sizeof buf, fmt, ap);
	buf[sizeof(buf) - 1] = 0;

	if (!strcmp(buf, ctx->warn.message))
	{
		ctx->warn.count++;
	}
	else
	{
		fz_flush_warnings(ctx);
		if (ctx->warn.print)
			ctx->warn.print(ctx->warn.print_user, buf);
		fz_strlcpy(ctx->warn.message, buf, sizeof ctx->warn.message);
		ctx->warn.count = 1;
	}
}

void fz_warnFL(fz_context *ctx, const char *file, int line, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_vwarnFL(ctx, file, line, fmt, ap);
	va_end(ap);
}
#endif

void fz_vinfo(fz_context* ctx, const char* fmt, va_list ap)
{
	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	char buf[sizeof ctx->warn.message];

	fz_flush_warnings(ctx);

	prepmsg(buf, fmt, ap);

	if (ctx && ctx->info.print)
		ctx->info.print(ctx, ctx->info.print_user, buf);
	else
		fz_default_info_callback(ctx, NULL, buf);
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
	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	char buf[sizeof ctx->error.message];

	fz_flush_warnings(ctx);

	prepmsg(buf, fmt, ap);

	if (ctx && ctx->error.print)
		ctx->error.print(ctx, ctx->error.print_user, buf);
	else
		fz_default_error_callback(ctx, NULL, buf);
}

void fz_error(fz_context* ctx, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_verror(ctx, fmt, ap);
	va_end(ap);
}

/* Error context */

fz_error_print_callback* fz_set_error_callback(fz_context *ctx, fz_error_print_callback* print, void *user)
{
	fz_error_print_callback* rv = ctx->error.print;
	ctx->error.print_user = user;
	ctx->error.print = print ? print : fz_default_error_callback;
	return rv;
}

void fz_get_error_callback(fz_context* ctx, fz_error_print_callback** print, void** user)
{
	if (user)
		*user = ctx->error.print_user;
	if (print)
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

FZ_NORETURN static void _throw(fz_context *ctx, int code)
{
	if (ctx->error.top > ctx->error.stack_base)
	{
		ctx->error.top->state += 2;
		if (ctx->error.top->code != FZ_ERROR_NONE)
			fz_warn(ctx, "clobbering previous error code and message (throw in always block?)");
		ctx->error.top->code = code;
		fz_flush_all_std_logging_channels(ctx);

		// SumatraPDF: https://fossies.org/linux/tcsh/win32/fork.c#l_212
		// https://stackoverflow.com/questions/26605063/an-invalid-or-unaligned-stack-was-encountered-during-an-unwind-operation
#if defined(_WIN32) && defined (_M_AMD64)
		/* This is a horrible hack to work around a reported problem with the windows
		 * runtimes. See https://bugs.ghostscript.com/show_bug.cgi?id=706403 for more
		 * details. Essentially, some combination of windows build flags, possibly
		 * related to jumping between boundaries between binaries built in DLL/non-DLL
		 * modes can apparently cause crashes. The workaround for this is to set the
		 * frame pointer to 0. */
		((_JUMP_BUFFER*)&ctx->error.top->buffer)->Frame = 0;
#endif
		fz_longjmp(ctx->error.top->buffer, 1);
	}
	else
	{
		ASSERT(ctx->error.top == ctx->error.stack_base);

		// wrap every 'independent' statement in its own try-catch block, for we want to
		// **ignore** their failure(s). Which is also why we only try to report this at the
		// first level, as nested exceptions are... an indication of bad things going *worse*.
		//
		// If all else fails, you at least get the identifying exit code and/or debugger invocation.
		//
		//                                   (fingers crossed)
		//
		if (ctx->within_throw_call == 0)
		{
			fz_try(ctx)
			{
				fz_flush_warnings(ctx);
			}
			fz_always(ctx)
			{
				ctx->within_throw_call = 0;
			}
			fz_catch(ctx)
			{
				// ignore internal failures: those will occur when we cannot write to stderr,
				// which is deemed a non-fatal problem.
			}

			fz_try(ctx)
			{
				if (ctx->error.print)
					ctx->error.print(ctx, ctx->error.print_user, "aborting process from uncaught error!");
			}
			fz_always(ctx)
			{
				ctx->within_throw_call = 0;
			}
			fz_catch(ctx)
			{
				// ignore internal failures: those will occur when we cannot write to stderr,
				// which is deemed a non-fatal problem.
			}

			fz_try(ctx)
			{
				fz_flush_all_std_logging_channels(ctx);
			}
			fz_always(ctx)
			{
				ctx->within_throw_call = 0;
			}
			fz_catch(ctx)
			{
				// ignore internal failures: those will occur when we cannot write to stderr,
				// which is deemed a non-fatal problem.
			}
		}

#if defined(_CrtDbgBreak)
		_CrtDbgBreak();
#endif
		__debugbreak();

#if 0
		/* SumatraPDF: crash rather than exit */
		char* p = 0;
		*p = 0;
#endif

		exit(666 /* EXIT_FAILURE */ );
	}
}

fz_jmp_buf *fz_push_try(fz_context *ctx)
{
	/* If we would overflow the exception stack, throw an exception instead
	 * of entering the try block. We assume that we always have room for
	 * 1 extra level on the stack here - i.e. we throw the error on us
	 * starting to use the last level.
	 *
	 * Also take the FZ_JMPBUF_ALIGNment into consideration: we have active struct
	 * members immediately after the stack, so any alignment correction *will*
	 * have reduced the usable stack size. Use a compile-time worst-case estimate
	 * instead of the run-time pointers `__stack` vs. `stack_base`, to ensure we
	 * have the lightest & fastest boundary check here.
	 * (Technically, we round UP `FZ_JMPBUF_ALIGN`, expressed as number of stack
	 * elements, and subtract that number from the *raw* `nelem(...)` stack size.)
	 */
	if (ctx->error.top + 2 >= ctx->error.stack_base + nelem(ctx->error.__stack) - (FZ_JMPBUF_ALIGN + sizeof(ctx->error.stack_base[0]) - 1) / sizeof(ctx->error.stack_base[0]))
	{
		fz_strncpy_s(ctx, ctx->error.message, "exception stack overflow!", sizeof ctx->error.message);

		fz_flush_warnings(ctx);
		if (ctx->error.print)
			ctx->error.print(ctx, ctx->error.print_user, ctx->error.message);

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

void fz_copy_ephemeral_system_error_explicit(fz_context* ctx, int errorcode, const char* errormessage, int category_code, int errorcode_mask)
{
	// do not replace any existing errorcode! First comer is the winner!
	int idx = ctx->error.system_errdepth;
	ASSERT(idx >= 0 && idx < countof(ctx->error.system_errcode));
	if (ctx->error.system_errcode[idx])
	{
		++idx;
		if (idx >= countof(ctx->error.system_errcode))
			idx = countof(ctx->error.system_errcode) - 1;
		// except that the LAST level will always carry the LATEST error...
		ASSERT0(idx >= 0 && idx < countof(ctx->error.system_errcode));
		ctx->error.system_errdepth = idx;
	}

	if (errorcode == 0)
		errorcode = -1; // unknown/unidentified error.

	// keep a copy of the ephemeral system error code:
	ASSERT0(idx >= 0 && idx < countof(ctx->error.system_errcode));
	ctx->error.system_errcode[idx] = category_code | (errorcode & errorcode_mask);

	const char* category_lead_msg = (category_code == FZ_ERROR_C_RTL_SERIES ? "rtl error: " : "system error: ");
	char* const errmsgbuf = ctx->error.system_error_message[idx];
	const size_t errmsgbufsize = sizeof(ctx->error.system_error_message[idx]);
	fz_strncpy_s(ctx, errmsgbuf, category_lead_msg, errmsgbufsize);

	if (!errormessage || !errormessage[0])
	{
		if (errorcode == -1)
			errormessage = "unknown/unidentified error";
		else
		{
			if (category_code == FZ_ERROR_C_RTL_SERIES)
			{
				errormessage = strerror(errorcode);
			}
			else
			{
#if defined(_WIN32)
				size_t offset = strlen(errmsgbuf);
				FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS), NULL, errorcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errmsgbuf + offset, errmsgbufsize - offset, NULL);
				return;
#endif
			}
		}

		if (!errormessage || !errormessage[0])
		{
			size_t offset = strlen(errmsgbuf);

			if (errorcode < 0xFFFF)
				fz_snprintf(errmsgbuf + offset, errmsgbufsize - offset, "unknown/unidentified error code %d", errorcode);
			else
				// some segmented errorcode: dumnp as HEX value!
				fz_snprintf(errmsgbuf + offset, errmsgbufsize - offset, "unknown/unidentified error code 0x%08x", errorcode);
			return;
		}
	}

	errmsgbuf[errmsgbufsize - 2] = 0;   // helper to quickly detect buffer 'overflow' --> truncated message
	fz_strncat_s(ctx, errmsgbuf, errormessage, errmsgbufsize);
	if (errmsgbuf[errmsgbufsize - 2])
	{
		strcpy(errmsgbuf + errmsgbufsize - sizeof("(...truncated...)"), "(...truncated...)");
	}
}


void fz_replace_ephemeral_system_error(fz_context* ctx, int errorcode, const char* errormessage)
{
	// brutally replace any existing errorcode!
	int idx = ctx->error.system_errdepth;
	char* const errmsgbuf = ctx->error.system_error_message[idx];
	const size_t errmsgbufsize = sizeof(ctx->error.system_error_message[idx]);

	if (errorcode != 0)
	{
		// keep a copy of the ephemeral system error code:
		ctx->error.system_errcode[idx] = errorcode;

		// check if the user also gave us a custom error message; if not, we'll fetch a system-level message instead.
		// 
		// EMPTY error message means: produce the system default message for me, please.
		// NULL error message means: keep the existing one as-is.
		// Anything else is the explicit overriding message.
		if (errormessage)
		{
			if (!errormessage[0])
			{
				const char* category_lead_msg = (fz_is_rtl_error(errorcode) ? "rtl error: " : "system error: ");
				fz_strncpy_s(ctx, errmsgbuf, category_lead_msg, errmsgbufsize);

				if (errorcode == -1)
					errormessage = "unknown/unidentified error";
				else
				{
					if (fz_is_rtl_error(errorcode))
					{
						errormessage = strerror(errorcode);
					}
					else
					{
#if defined(_WIN32)
						size_t offset = strlen(errmsgbuf);
						FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS), NULL, errorcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errmsgbuf + offset, errmsgbufsize - offset, NULL);
						return;
#endif
					}
				}

				if (!errormessage || !errormessage[0])
				{
					size_t offset = strlen(errmsgbuf);
					if (errorcode < 0xFFFF)
						fz_snprintf(errmsgbuf + offset, errmsgbufsize - offset, "unknown/unidentified error code %d", errorcode);
					else
						// some segmented errorcode: dumnp as HEX value!
						fz_snprintf(errmsgbuf + offset, errmsgbufsize - offset, "unknown/unidentified error code 0x%08x", errorcode);
					return;
				}
			}
		}
	}

	// ONLY non-empty error message overrides. We don't do defaults here as the `replace` API is intended for explicit overrides only!
	if (errormessage != NULL && errormessage[0])
	{
		errmsgbuf[errmsgbufsize - 2] = 0;   // helper to quickly detect buffer 'overflow' --> truncated message
		fz_strncpy_s(ctx, errmsgbuf, errormessage, errmsgbufsize);
		if (errmsgbuf[errmsgbufsize - 2])
		{
			strcpy(errmsgbuf + errmsgbufsize - sizeof("(...truncated...)"), "(...truncated...)");
		}
	}
}

void fz_freplace_ephemeral_system_error(fz_context* ctx, int errorcode, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_vreplace_ephemeral_system_error(ctx, errorcode, fmt, ap);
	va_end(ap);
}

void fz_vreplace_ephemeral_system_error(fz_context* ctx, int errorcode, const char* fmt, va_list ap)
{
	char buf[sizeof ctx->warn.message];

	prepmsg(buf, fmt, ap);

	fz_replace_ephemeral_system_error(ctx, errorcode, buf);
}

void fz_log_error_printf(fz_context *ctx, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fz_vlog_error_printf(ctx, fmt, ap);
	va_end(ap);
}

void fz_vlog_error_printf(fz_context *ctx, const char *fmt, va_list ap)
{
	char buf[sizeof ctx->error.message];

	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	fz_flush_warnings(ctx);

	prepmsg(buf, fmt, ap);

	fz_log_error(ctx, buf);
}

void fz_log_error(fz_context *ctx, const char *str)
{
	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	fz_flush_warnings(ctx);

	if (ctx && ctx->error.print)
		ctx->error.print(ctx, ctx->error.print_user, str);
	else
		fz_default_error_callback(ctx, NULL, str);
}


/* coverity[+kill] */
FZ_NORETURN void fz_vthrow(fz_context* ctx, int code, const char* fmt, va_list ap)
{
	if (ctx)
	{
		if (!ctx->within_throw_call)
		{
			ctx->within_throw_call = 1;

			fz_try(ctx)
			{
				fz_vsnprintf(ctx->error.message, sizeof(ctx->error.message), fmt, ap);

				if (code != FZ_ERROR_ABORT && code != FZ_ERROR_TRYLATER)
				{
					fz_flush_warnings(ctx);
					if (ctx->error.print)
						ctx->error.print(ctx, ctx->error.print_user, ctx->error.message);
				}
			}
			fz_always(ctx)
			{
				ctx->within_throw_call = 0;
			}
			fz_catch(ctx)
			{
				// ignore internal failures: those will occur when we cannot write to stderr,
				// which is deemed a non-fatal problem.
			}

			_throw(ctx, code);
		}
		else
		{
			ctx->within_throw_call++;

			if (ctx->within_throw_call == 2)
			{
				fz_try(ctx)
				{
					fz_flush_all_std_logging_channels(ctx);
				}
				fz_always(ctx)
				{
					ctx->within_throw_call = 2;
				}
				fz_catch(ctx)
				{
					// ignore internal failures: those will occur when we cannot write to stderr,
					// which is deemed a non-fatal problem.
				}

				fz_output* err = fz_stderr(ctx);
				fz_output* dbg = fz_stdods(ctx);

				fz_try(ctx)
				{
					char msgbuf[LONGLINE];
					fz_vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);

					if (dbg != err)
					{
						fz_try(ctx)
						{
							fz_write_string(ctx, dbg, msgbuf);
						}
						fz_always(ctx)
						{
							ctx->within_throw_call = 2;
						}
						fz_catch(ctx)
						{
							// ignore internal failures: those will occur when we cannot write to stderr,
							// which is deemed a non-fatal problem.
						}
					}

					fz_try(ctx)
					{
						fz_write_string(ctx, err, msgbuf);
					}
					fz_always(ctx)
					{
						ctx->within_throw_call = 2;
					}
					fz_catch(ctx)
					{
						// ignore internal failures: those will occur when we cannot write to stderr,
						// which is deemed a non-fatal problem.
					}
				}
				fz_always(ctx)
				{
					ctx->within_throw_call = 2;
				}
				fz_catch(ctx)
				{
					// ignore internal failures: those will occur when we cannot write to stderr,
					// which is deemed a non-fatal problem.
				}

				const char* fatal_msg = "\nERROR:FATAL: Application code throws an fz_throw() exception without a valid context! Aborting process from uncaught error!\n";
				if (dbg != err)
				{
					fz_try(ctx)
					{
						fz_write_string(ctx, dbg, fatal_msg);
					}
					fz_always(ctx)
					{
						ctx->within_throw_call = 2;
					}
					fz_catch(ctx)
					{
						// ignore internal failures: those will occur when we cannot write to stderr,
						// which is deemed a non-fatal problem.
					}
				}

				fz_try(ctx)
				{
					fz_write_string(ctx, err, fatal_msg);
				}
				fz_always(ctx)
				{
					ctx->within_throw_call = 2;
				}
				fz_catch(ctx)
				{
					// ignore internal failures: those will occur when we cannot write to stderr,
					// which is deemed a non-fatal problem.
				}

				fz_try(ctx)
				{
					fz_flush_all_std_logging_channels(ctx);
				}
				fz_always(ctx)
				{
					ctx->within_throw_call = 2;
				}
				fz_catch(ctx)
				{
					// ignore internal failures: those will occur when we cannot write to stderr,
					// which is deemed a non-fatal problem.
				}

				ctx->within_throw_call = 2;
			}

			ctx->within_throw_call--;

			_throw(ctx, FZ_ERROR_ABORT); // internal state is not sane any more: abort abort abort
		}
		// we never get here: if & else both _throw()
	}
	else
	{
		static int within_throw_call = 0;

		within_throw_call++;

		ASSERT(ctx == NULL);
		fz_flush_all_std_logging_channels(ctx);

		fz_output* err = fz_stderr(ctx);
		fz_output* dbg = fz_stdods(ctx);

		if (within_throw_call == 1)
		{
			char msgbuf[LONGLINE];
			fz_vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);

			// First write to the debug channel, rather than the stderr channel, as at least on Win32
			// that channel is less prone to curious failures: stderr can be closed (my mistake) or
			// redirected/piped to another app, which fails to fetch the data and thus locks up the 
			// system's pipe buffer (others' mistake). Meanwhile, the debug channel doesn't suffer
			// from those issues... unless the application code itself redirected it, in which case YMMV.
			//
			// Either way, `ctx == NULL` so there's no exception stack, so there's no try/catch
			// possible here-abouts.
			
			if (dbg != err)
			{
				fz_write_string(ctx, dbg, msgbuf);
			}
			fz_write_string(ctx, err, msgbuf);
		}

		const char* fatal_msg = "\nERROR:FATAL: Application code throws an fz_throw() exception without a valid context! Aborting process from uncaught error!\n";
		if (dbg != err)
		{
			fz_write_string(ctx, dbg, fatal_msg);
		}
		fz_write_string(ctx, err, fatal_msg);

		fz_flush_all_std_logging_channels(ctx);

		within_throw_call--;

		exit(665 /* EXIT_FAILURE */);  // 666 is reserved for another place: see further above.
	}
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
		assert(ctx->error.last_nonzero_errcode != FZ_ERROR_NONE);
		ctx->error.errcode = ctx->error.last_nonzero_errcode;
	}
	_throw(ctx, ctx->error.errcode);
}

void fz_morph_error(fz_context *ctx, int fromerr, int toerr)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	if (ctx->error.errcode == fromerr)
		ctx->error.errcode = toerr;
}

void fz_rethrow_if(fz_context *ctx, int err)
{
	assert(ctx && ctx->error.errcode != FZ_ERROR_NONE);
	if (ctx->error.errcode == err)
		fz_rethrow(ctx);
}

#if FZ_VERBOSE_EXCEPTIONS

static const char *
errcode_to_string(int exc)
{
	switch (exc)
	{
	case FZ_ERROR_NONE:
		return "NONE";
	case FZ_ERROR_MEMORY:
		return "MEMORY";
	case FZ_ERROR_GENERIC:
		return "GENERIC";
	case FZ_ERROR_SYNTAX:
		return "SYNTAX";
	case FZ_ERROR_MINOR:
		return "MINOR";
	case FZ_ERROR_TRYLATER:
		return "TRYLATER";
	case FZ_ERROR_ABORT:
		return "ABORT";
	case FZ_ERROR_REPAIRED:
		return "REPAIRED";
	case FZ_ERROR_COUNT:
		return "COUNT";
	default:
		return "<Invalid>";
	}
}


void fz_log_error_printfFL(fz_context *ctx, const char *file, int line, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fz_vlog_error_printfFL(ctx, file, line, fmt, ap);
	va_end(ap);
}

void fz_vlog_error_printfFL(fz_context *ctx, const char *file, int line, const char *fmt, va_list ap)
{
	char message[256];

	fz_flush_warnings(ctx);
	if (ctx->error.print)
	{
		fz_vsnprintf(message, sizeof message, fmt, ap);
		message[sizeof(message) - 1] = 0;

		fz_log_errorFL(ctx, file, line, message);
	}
}

void fz_log_errorFL(fz_context *ctx, const char *file, int line, const char *str)
{
	char message[256];

	fz_flush_warnings(ctx);
	if (ctx->error.print)
	{
		fz_snprintf(message, sizeof message, "%s:%d '%s'", file, line, str);
		message[sizeof(message) - 1] = 0;
		ctx->error.print(ctx->error.print_user, message);
	}
}

/* coverity[+kill] */
FZ_NORETURN void fz_vthrowFL(fz_context *ctx, const char *file, int line, int code, const char *fmt, va_list ap)
{
	fz_vsnprintf(ctx->error.message, sizeof ctx->error.message, fmt, ap);
	ctx->error.message[sizeof(ctx->error.message) - 1] = 0;

	(fz_log_error_printf)(ctx, "%s:%d: Throwing %s '%s'", file, line, errcode_to_string(code), ctx->error.message);

	throw(ctx, code);
}

/* coverity[+kill] */
FZ_NORETURN void fz_throwFL(fz_context *ctx, const char *file, int line, int code, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fz_vthrowFL(ctx, file, line, code, fmt, ap);
	va_end(ap);
}

/* coverity[+kill] */
FZ_NORETURN void fz_rethrowFL(fz_context *ctx, const char *file, int line)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	(fz_log_error_printf)(ctx, "%s:%d: Rethrowing", file, line);
	throw(ctx, ctx->error.errcode);
}

void fz_morph_errorFL(fz_context *ctx, const char *file, int line, int fromerr, int toerr)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	if (ctx->error.errcode == fromerr)
	{
		(fz_log_error_printf)(ctx, "%s:%d: Morphing %s->%s", file, line, errcode_to_string(fromerr), errcode_to_string(toerr));
		ctx->error.errcode = toerr;
	}
}

void fz_rethrow_ifFL(fz_context *ctx, const char *file, int line, int err)
{
	assert(ctx && ctx->error.errcode >= FZ_ERROR_NONE);
	if (ctx->error.errcode == err)
	{
		(fz_log_error_printf)(ctx, "%s:%d: Rethrowing");
		(fz_rethrow)(ctx);
	}
}

#endif

void fz_start_throw_on_repair(fz_context *ctx)
{
	fz_lock(ctx, FZ_LOCK_ALLOC);
	ctx->throw_on_repair++;
	fz_unlock(ctx, FZ_LOCK_ALLOC);
}

void fz_end_throw_on_repair(fz_context *ctx)
{
	fz_lock(ctx, FZ_LOCK_ALLOC);
	ctx->throw_on_repair--;
	fz_unlock(ctx, FZ_LOCK_ALLOC);
}
