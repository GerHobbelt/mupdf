#include "mupdf/fitz.h"

#include "context-imp.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

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
	return fz_keep_imp(ctx, ctx->style, &ctx->style->refs);
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
	return fz_keep_imp(ctx, ctx->tuning, &ctx->tuning->refs);
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

void
fz_drop_context(fz_context *ctx)
{
	if (!ctx)
		return;

	/* Other finalisation calls go here (in reverse order) */
	fz_drop_document_handler_context(ctx);
	fz_drop_glyph_cache_context(ctx);
	fz_drop_store_context(ctx);
	fz_drop_style_context(ctx);
	fz_drop_tuning_context(ctx);
	fz_drop_colorspace_context(ctx);
	fz_drop_font_context(ctx);

	fz_flush_warnings(ctx);

	assert(ctx->error.top == ctx->error.stack);

	/* Free the context itself */
	ctx->alloc.free_(ctx->alloc.user, ctx);
}

static void
fz_init_error_context(fz_context *ctx)
{
	ctx->error.top = ctx->error.stack;
	ctx->error.errcode = FZ_ERROR_NONE;
	ctx->error.message[0] = 0;

	ctx->warn.message[0] = 0;
	ctx->warn.count = 0;
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

	ctx = Memento_label(alloc->malloc_(alloc->user, sizeof(fz_context)), "fz_context");
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

		fz_init_random_context(ctx);
	}

	fz_init_error_context(ctx);
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

	new_ctx = ctx->alloc.malloc_(ctx->alloc.user, sizeof(fz_context));
	if (!new_ctx)
		return NULL;

	/* First copy old context, including pointers to shared contexts */
	memcpy(new_ctx, ctx, sizeof (fz_context));

	/* Reset error context to initial state. */
	fz_init_error_context(new_ctx);

	/* Then keep lock checking happy by keeping shared contexts with new context */
	fz_keep_document_handler_context(new_ctx);
	fz_keep_style_context(new_ctx);
	fz_keep_tuning_context(new_ctx);
	fz_keep_font_context(new_ctx);
	fz_keep_colorspace_context(new_ctx);
	fz_keep_store_context(new_ctx);
	fz_keep_glyph_cache(new_ctx);

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

static fz_context *global_ctx = NULL;

fz_context* fz_get_global_context(void)
{
	if (!global_ctx)
	{
		fz_set_global_context(fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED));
	}
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

void fz_drop_global_context(void)
{
	//fz_drop_context_locks(ctx);
	fz_drop_context(global_ctx);
	global_ctx = NULL;
}

int fz_has_locking_support(fz_context* ctx)
{
	return !(ctx == NULL || (ctx->locks.lock == fz_locks_default.lock && ctx->locks.unlock == fz_locks_default.unlock));
}
