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

#include "mupdf/fitz.h"

#include "mupdf/helpers/system-header-files.h"

#include "mupdf/assertions.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(WASM_SKIP_TRY_CATCH)
#include "emscripten.h"
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
#if !defined(WASM_SKIP_TRY_CATCH)
	if (ctx->error.top > ctx->error.stack_base)
	{
		ctx->error.top->state += 2;
		if (ctx->error.top->code != FZ_ERROR_NONE)
			fz_warn(ctx, "clobbering previous error code and message (throw in always block?)");
		ctx->error.top->code = code;
		fz_flush_all_std_logging_channels(ctx);
		fz_longjmp(ctx->error.top->buffer, 1);
	}
	else
	{
		ASSERT(ctx->error.top == ctx->error.stack_base);
		fz_flush_warnings(ctx);
		if (ctx->error.print)
			ctx->error.print(ctx, ctx->error.print_user, "aborting process from uncaught error!");
		fz_flush_all_std_logging_channels(ctx);
		exit(EXIT_FAILURE);
	}
#else
		EM_ASM({
			let message = UTF8ToString($0);
			console.error("mupdf:", message);
			throw new libmupdf.MupdfError(message);
		}, ctx->error.message);
		// Unreachable
		exit(EXIT_FAILURE);
#endif
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
	ASSERT(idx >= 0 && idx < 3);
	if (ctx->error.system_errcode[idx])
	{
		++idx;
		if (idx >= 3)
			idx = 2;
		// except that the LAST level will always carry the LATEST error...
		ASSERT0(idx >= 0 && idx < 3);
		ctx->error.system_errdepth = idx;
	}

	if (errorcode == 0)
		errorcode = -1; // unknown/unidentified error.

	// keep a copy of the ephemeral system error code:
	ASSERT0(idx >= 0 && idx < 3);
	ctx->error.system_errcode[idx] = category_code | (errorcode & errorcode_mask);

	const char* category_lead_msg = (category_code == FZ_ERROR_C_RTL_SERIES ? "rtl error: " : "system error: ");
	fz_strncpy_s(ctx, ctx->error.system_error_message[idx], category_lead_msg, sizeof(ctx->error.system_error_message[idx]));

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
				size_t offset = strlen(ctx->error.system_error_message[idx]);
				FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS), NULL, errorcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), ctx->error.system_error_message[idx] + offset, sizeof(ctx->error.system_error_message[idx]) - offset, NULL);
				return;
#endif
			}
		}

		if (!errormessage || !errormessage[0])
		{
			size_t offset = strlen(ctx->error.system_error_message[idx]);

			if (errorcode < 0xFFFF)
				fz_snprintf(ctx->error.system_error_message[idx] + offset, sizeof(ctx->error.system_error_message[idx]) - offset, "unknown/unidentified error code %d", errorcode);
			else
				// some segmented errorcode: dumnp as HEX value!
				fz_snprintf(ctx->error.system_error_message[idx] + offset, sizeof(ctx->error.system_error_message[idx]) - offset, "unknown/unidentified error code 0x%08x", errorcode);
			return;
		}
	}

	fz_strncat_s(ctx, ctx->error.system_error_message[idx], errormessage, sizeof(ctx->error.system_error_message[idx]));
}


void fz_replace_ephemeral_system_error(fz_context* ctx, int errorcode, const char* errormessage)
{
	// brutally replace any existing errorcode!
	int idx = ctx->error.system_errdepth;

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
				fz_strncpy_s(ctx, ctx->error.system_error_message[idx], category_lead_msg, sizeof(ctx->error.system_error_message[idx]));

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
						size_t offset = strlen(ctx->error.system_error_message[idx]);
						FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS), NULL, errorcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), ctx->error.system_error_message[idx] + offset, sizeof(ctx->error.system_error_message[idx]) - offset, NULL);
						return;
#endif
					}
				}

				if (!errormessage || !errormessage[0])
				{
					size_t offset = strlen(ctx->error.system_error_message[idx]);
					if (errorcode < 0xFFFF)
						fz_snprintf(ctx->error.system_error_message[idx] + offset, sizeof(ctx->error.system_error_message[idx]) - offset, "unknown/unidentified error code %d", errorcode);
					else
						// some segmented errorcode: dumnp as HEX value!
						fz_snprintf(ctx->error.system_error_message[idx] + offset, sizeof(ctx->error.system_error_message[idx]) - offset, "unknown/unidentified error code 0x%08x", errorcode);
					return;
				}
			}

			fz_strncpy_s(ctx, ctx->error.system_error_message[idx], errormessage, sizeof(ctx->error.system_error_message[idx]));
		}
	}
	else
	{
		// ONLY non-empty error message overrides. We don't do defaults here as the `replace` API is intended for explicit overrides only!
		if (errormessage != NULL && errormessage[0])
		{
			fz_strncpy_s(ctx, ctx->error.system_error_message[idx], errormessage, sizeof(ctx->error.system_error_message[idx]));
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

			fz_write_string(ctx, err, msgbuf);
			if (dbg != err)
				fz_write_string(ctx, dbg, msgbuf);
		}

		const char* fatal_msg = "\nERROR:FATAL: Application code throws an fz_throw() exception without a valid context! Aborting process from uncaught error!\n";
		fz_write_string(ctx, err, fatal_msg);
		if (dbg != err)
			fz_write_string(ctx, dbg, fatal_msg);

		fz_flush_all_std_logging_channels(ctx);

		within_throw_call--;

		exit(EXIT_FAILURE);
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

void fz_rethrow_if(fz_context *ctx, int err)
{
	assert(ctx && ctx->error.errcode != FZ_ERROR_NONE);
	if (ctx->error.errcode == err)
		fz_rethrow(ctx);
}

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
