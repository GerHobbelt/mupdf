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

#include "mupdf/helpers/system-header-files.h"

#include "context-imp.h"

#include "mupdf/assertions.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif


struct fz_style_context
{
	int refs;
	char *user_css;
	int use_document_css;
};

static void fz_new_style_context(fz_context *ctx)
{
	if (ctx)
	{
		ctx->style = fz_malloc_struct(ctx, fz_style_context);
		ctx->style->refs = 1;
		ctx->style->user_css = NULL;
		ctx->style->use_document_css = 1;
	}
}

static fz_style_context *fz_keep_style_context(fz_context *ctx)
{
	if (!ctx)
		return NULL;
	return (fz_style_context *)fz_keep_imp(ctx, ctx->style, &ctx->style->refs);
}

static void fz_drop_style_context(fz_context *ctx)
{
	if (!ctx)
		return;
	if (fz_drop_imp(ctx, ctx->style, &ctx->style->refs))
	{
		fz_free(ctx, ctx->style->user_css);
		fz_free(ctx, ctx->style);
	}
}

void fz_set_use_document_css(fz_context *ctx, int use)
{
	ctx->style->use_document_css = use;
}

int fz_use_document_css(fz_context *ctx)
{
	return ctx->style->use_document_css;
}

void fz_set_user_css(fz_context *ctx, const char *user_css)
{
	fz_free(ctx, ctx->style->user_css);
	ctx->style->user_css = user_css ? fz_strdup(ctx, user_css) : NULL;
}

const char *fz_user_css(fz_context *ctx)
{
	return ctx->style->user_css;
}

#if FZ_ENABLE_RENDER_CORE 

static void fz_new_tuning_context(fz_context *ctx)
{
	if (ctx)
	{
		ctx->tuning = fz_malloc_struct(ctx, fz_tuning_context);
		ctx->tuning->refs = 1;
		ctx->tuning->image_decode = fz_default_image_decode;
		ctx->tuning->image_scale = fz_default_image_scale;
	}
}

static fz_tuning_context *fz_keep_tuning_context(fz_context *ctx)
{
	if (!ctx)
		return NULL;
	return (fz_tuning_context *)fz_keep_imp(ctx, ctx->tuning, &ctx->tuning->refs);
}

static void fz_drop_tuning_context(fz_context *ctx)
{
	if (!ctx)
		return;
	if (fz_drop_imp(ctx, ctx->tuning, &ctx->tuning->refs))
	{
		fz_free(ctx, ctx->tuning);
	}
}

void fz_tune_image_decode(fz_context *ctx, fz_tune_image_decode_fn *image_decode, void *arg)
{
	ctx->tuning->image_decode = image_decode ? image_decode : fz_default_image_decode;
	ctx->tuning->image_decode_arg = arg;
}

void fz_tune_image_scale(fz_context *ctx, fz_tune_image_scale_fn *image_scale, void *arg)
{
	ctx->tuning->image_scale = image_scale ? image_scale : fz_default_image_scale;
	ctx->tuning->image_scale_arg = arg;
}

#endif // FZ_ENABLE_RENDER_CORE 

static void fz_init_random_context(fz_context *ctx)
{
	if (!ctx)
		return;

	ctx->seed48[0] = 0;
	ctx->seed48[1] = 0;
	ctx->seed48[2] = 0;
	ctx->seed48[3] = 0xe66d;
	ctx->seed48[4] = 0xdeec;
	ctx->seed48[5] = 0x5;
	ctx->seed48[6] = 0xb;

	fz_srand48(ctx, (uint32_t)time(NULL));
}

void
fz_drop_context_locks(fz_context *ctx)
{
	if (!ctx)
		return;

	ctx->locks = fz_locks_default;
}

static fz_context* global_ctx = NULL;

void
fz_drop_context(fz_context *ctx)
{
	fz_flush_all_std_logging_channels(ctx);

	if (!ctx)
		return;

#if FZ_ENABLE_PDF    // TODO: this is a rough cut condition; re-check when you need particular (minor) parts of the mupdf library in your application.
	/* Other finalisation calls go here (in reverse order) */
	fz_drop_document_handler_context(ctx);
	fz_drop_glyph_cache_context(ctx);
	fz_drop_store_context(ctx);
	fz_drop_style_context(ctx);
	fz_drop_tuning_context(ctx);
	fz_drop_colorspace_context(ctx);
	fz_drop_font_context(ctx);
#endif

	fz_flush_warnings(ctx);

	// WARNING: this assert fires when you run `mutool draw` (and probably other tools as well) and hit Ctrl+C
	// to ABORT/INTERRUPT the running application: the MSVC RTL calls this function in the atexit() handler
	// and the assert fires due to (ctx->error.top != ctx->error.stack).
	//
	// We are okay with that, as that scenario is an immediate abort anyway and the OS will be responsible
	// for cleaning up. That our fz_try/throw/catch exception stack hasn't been properly rewound at such times
	// is obvious, I suppose...
	ASSERT_AND_CONTINUE(ctx->error.top == ctx->error.stack_base);

	/* Free the context itself */
	ctx->alloc.free_(ctx->alloc.user, ctx);

	// fixup for when this happens to be the *global context*:
	if (global_ctx == ctx)
	{
		global_ctx = NULL;
	}
}

static void
fz_init_error_context(fz_context *ctx)
{
#define ALIGN(addr, align)  ((((intptr_t)(addr)) + ((align) - 1)) & ~((align) - 1))
	ctx->error.stack_base = (fz_error_stack_slot *)ALIGN(ctx->error.__stack, FZ_JMPBUF_ALIGN);
	ctx->error.top = ctx->error.stack_base;
	ctx->error.errcode = FZ_ERROR_NONE;
	ctx->error.message[0] = 0;

	ctx->warn.message[0] = 0;
	ctx->warn.count = 0;

	fz_clear_system_error(ctx);
}

fz_context *
fz_new_context_imp(const fz_alloc_context *alloc, const fz_locks_context *locks, size_t max_store, const char *version)
{
	fz_context *ctx;
	fz_context* global_default_ctx = NULL;

	if (strcmp(version, FZ_VERSION))
	{
		fz_error(NULL, "cannot create context: incompatible header (%s) and library (%s) versions", version, FZ_VERSION);
		return NULL;
	}

	if (fz_has_global_context())
	{
		global_default_ctx = fz_get_global_context();
	}

	if (!alloc)
		alloc = global_default_ctx ? &global_default_ctx->alloc : &fz_alloc_default;

	if (!locks)
		locks = global_default_ctx ? &global_default_ctx->locks : &fz_locks_default;

	ctx = (fz_context *)Memento_label(alloc->malloc_(alloc->user, sizeof(fz_context)   FZDBG_THIS_POS()), "fz_context");
	if (!ctx)
	{
		fz_error(NULL, "cannot create context (phase 1)");
		return NULL;
	}
	memset(ctx, 0, sizeof *ctx);

	ctx->user = NULL;
	ctx->alloc = *alloc;
	ctx->locks = *locks;

	if (global_default_ctx)
	{
		ctx->error = global_default_ctx->error;
		ctx->warn = global_default_ctx->warn;
		ctx->info = global_default_ctx->info;
		ctx->stddbg = global_default_ctx->stddbg;

		// and copy the randomness pool across for predictable semi-random behaviour in all *default* sub contexts
		memcpy(ctx->seed48, global_default_ctx->seed48, sizeof ctx->seed48);
	}
	else
	{
		ctx->error.print = fz_default_error_callback;
		ctx->warn.print = fz_default_warning_callback;
		ctx->info.print = fz_default_info_callback;

#if defined(_WIN32)
		/*
		* Check if STDERR and STDOUT are writable at all: Windows GUI applications, for example,
		* won't have those and errors and warnings would then be *silently lost*, unless we
		* set the Quiet Mode to log all errors, warnings and info messages to the system debug channel
		* as well, which is what `fz_enable_dbg_output_on_stdio_unreachable()` is for.
		*/
		{
			HANDLE h1 = GetStdHandle(STD_ERROR_HANDLE);
			int err1 = GetLastError();
			HANDLE h2 = GetStdHandle(STD_OUTPUT_HANDLE);
			int err2 = GetLastError();

			// https://docs.microsoft.com/en-us/windows/console/getstdhandle:
			//
			//> If the function fails, the return value is INVALID_HANDLE_VALUE.
			//> To get extended error information, call GetLastError.
			//>
			//> If an application does not have associated standard handles,
			//> such as a service running on an interactive desktop, and
			//> has not redirected them, the return value is NULL.
			if (h1 == NULL || h2 == NULL ||
				h1 == INVALID_HANDLE_VALUE || h2 == INVALID_HANDLE_VALUE ||
				err1 == ERROR_INVALID_HANDLE || err2 == ERROR_INVALID_HANDLE)
			{
				fz_enable_dbg_output_on_stdio_unreachable();

				// also shut up the stderr + stdio channels: those won't work anyway and will only cause trouble when used.
				// 
				// NOTE: these err/warn/info messages will still be shown via the system debug channel, thanks to
				//`fz_enable_dbg_output_on_stdio_unreachable()`.
				fz_default_error_warn_info_mode(1, 1, 1);
			}
		}
#endif

		// Get us a debug stream so we have a chance of seeing *independently* of stderr.
		ctx->stddbg = fz_stdods(ctx);

		fz_init_random_context(ctx);
	}

	fz_init_error_context(ctx);
#if FZ_ENABLE_PDF    // TODO: this is a rough cut condition; re-check when you need particular (minor) parts of the mupdf library in your application.
	fz_init_aa_context(ctx);

	/* Now initialise sections that are shared */
	fz_try(ctx)
	{
		fz_new_store_context(ctx, max_store);
		fz_new_glyph_cache_context(ctx);
		fz_new_colorspace_context(ctx);
		fz_new_font_context(ctx);
		fz_new_document_handler_context(ctx);
		fz_new_style_context(ctx);
		fz_new_tuning_context(ctx);
	}
	fz_catch(ctx)
	{
		fz_error(NULL, "cannot create context (phase 2)");
		fz_drop_context(ctx);
		return NULL;
	}
#endif
	return ctx;
}

fz_context *
fz_clone_context(fz_context *ctx)
{
	fz_context *new_ctx;

	/* We cannot safely clone the context without having locking/
	 * unlocking functions. */
	if (ctx == NULL || (ctx->locks.lock == fz_locks_default.lock && ctx->locks.unlock == fz_locks_default.unlock))
		return NULL;

	new_ctx = (fz_context *)ctx->alloc.malloc_(ctx->alloc.user, sizeof(fz_context)   FZDBG_THIS_POS());
	if (!new_ctx)
		return NULL;

	/* First copy old context, including pointers to shared contexts */
	memcpy(new_ctx, ctx, sizeof (fz_context));

	/* Reset error context to initial state. */
	fz_init_error_context(new_ctx);

#if FZ_ENABLE_PDF    // TODO: this is a rough cut condition; re-check when you need particular (minor) parts of the mupdf library in your application.
	/* Then keep lock checking happy by keeping shared contexts with new context */
	fz_keep_document_handler_context(new_ctx);
	fz_keep_style_context(new_ctx);
	fz_keep_tuning_context(new_ctx);
	fz_keep_font_context(new_ctx);
	fz_keep_colorspace_context(new_ctx);
	fz_keep_store_context(new_ctx);
	fz_keep_glyph_cache(new_ctx);
#endif

	fz_clear_system_error(new_ctx);

	return new_ctx;
}

void fz_set_user_context(fz_context *ctx, void *user)
{
	if (ctx != NULL)
		ctx->user = user;
}

void *fz_user_context(fz_context *ctx)
{
	if (ctx == NULL)
		return NULL;

	return ctx->user;
}

fz_context* fz_get_global_context(void)
{
	if (!global_ctx)
	{
		fz_set_global_context(fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED));
	}
	return global_ctx;
}

fz_context* __fz_get_RAW_global_context(void)
{
	return global_ctx;
}

void fz_set_global_context(fz_context *ctx)
{
	if (fz_has_global_context())
	{
		fz_error(ctx, "cannot (re)set global context after initial setup");
		exit(1);
	}
	if (!ctx)
	{
		fz_error(ctx, "cannot set global context to NULL (out of memory?)");
		exit(1);
	}
	global_ctx = ctx;
	atexit(fz_drop_global_context);
}

int fz_has_global_context(void)
{
	return global_ctx != NULL;
}

void __cdecl fz_drop_global_context(void)
{
	//fz_drop_context_locks(ctx);
	fz_drop_context(global_ctx);
	global_ctx = NULL;
}

int fz_has_locking_support(fz_context* ctx)
{
	return !(ctx == NULL || (ctx->locks.lock == fz_locks_default.lock && ctx->locks.unlock == fz_locks_default.unlock));
}
