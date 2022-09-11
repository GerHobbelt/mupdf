
#include "mupdf/fitz.h"
#include "mupdf/assertions.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


static fz_context* ctx = NULL;


/*
	In the presence of pthreads or Windows threads, we can offer
	a multi-threaded option. In the absence of such we degrade
	nicely.
*/
#ifndef DISABLE_MUTHREADS

static mu_mutex mutexes[FZ_LOCK_MAX];
static mu_mutex heap_debug_mutex;
static int heap_debug_mutex_is_initialized;

static void mudraw_lock(void* user, int lock)
{
	mu_lock_mutex(&mutexes[lock]);
}

static void mudraw_unlock(void* user, int lock)
{
	mu_unlock_mutex(&mutexes[lock]);
}

static fz_locks_context mudraw_locks =
{
	NULL, mudraw_lock, mudraw_unlock
};

static void fin_mudraw_locks(void)
{
	int i;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		mu_destroy_mutex(&mutexes[i]);
	heap_debug_mutex_is_initialized = 0;
	mu_destroy_mutex(&heap_debug_mutex);
}

static fz_locks_context* init_mudraw_locks(void)
{
	int i;
	int failed = 0;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		failed |= mu_create_mutex(&mutexes[i]);
	failed |= mu_create_mutex(&heap_debug_mutex);
	heap_debug_mutex_is_initialized = !failed;

	if (failed)
	{
		fin_mudraw_locks();
		return NULL;
	}

	return &mudraw_locks;
}

#endif



static void mu_drop_context_at_exit(void)
{
	// we're aborting/exiting the application.
	fz_warn(ctx, "Landed in the atexit() handler as expected... You can expect a few 'soft assertions' to follow afterwards.\n");

	// WARNING: as we point `ctx` at the GLOBAL context in the app init phase, it MAY already be an INVALID
	// pointer reference by now!
	// 
	// WARNING: this assert fires when you run `mutool raster` (and probably other tools as well) and hit Ctrl+C
	// to ABORT/INTERRUPT the running application: the MSVC RTL calls this function in the atexit() handler
	// and the assert fires due to (ctx->error.top != ctx->error.stack).
	//
	// We are okay with that, as that scenario is an immediate abort anyway and the OS will be responsible
	// for cleaning up. That our fz_try/throw/catch exception stack hasn't been properly rewound at such times
	// is obvious, I suppose...
	ASSERT_AND_CONTINUE(!ctx || !fz_has_global_context() || (ctx->error.top == ctx->error.stack_base));

	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}
	if (ctx)
	{
		fz_drop_context_locks(ctx);
		fz_drop_context(ctx);
	}

	ctx = NULL;

	if (!ctx && fz_has_global_context())
	{
		ctx = fz_get_global_context();
	}

	fz_try(ctx)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION #6: Testing %s within a try/catch environment with the sole GLOBAL context remaining.", "fz_throw()");
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "TEST OK: Exception caught: %s\n", fz_caught_message(ctx));
	}

#if 01
	// be nasty: throw inside the atexit handler, as the contexts have been dropped:
	fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION: Testing %s when thrown in atexit() handler. Expects to abort the application.", "fz_throw()");

	ASSERT_AND_CONTINUE(!"Should never get here!");
#endif

	fz_drop_global_context();
	ctx = NULL;   // <-- important as we will use ctx, but it's an /ALIAS to the global context, which has just been nuked. So we need to NIL this alias too!

	// be nasty: throw inside the atexit handler, as the contexts have been dropped:

		ASSERT_AND_CONTINUE(!"Should never get here!");
	fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION: Testing %s when thrown in atexit() handler. Expects to abort the application.", "fz_throw()");

	ASSERT_AND_CONTINUE(!"Should never get here!");
}


static void f1(fz_context* global_ctx, fz_context* ctx)
{
	fz_try(ctx)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION #1: Testing %s within a try/catch environment.", "fz_throw()");

		ASSERT_AND_CONTINUE(!"Should never get here!");
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "TEST OK: Exception caught: %s\n", fz_caught_message(ctx));
	}
}


static void f2sub(fz_context* global_ctx, fz_context* ctx)
{
	// throwing in a multi-level context:
	fz_try(ctx)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION #2: Testing %s within a 2-layer try/catch environment.", "fz_throw()");
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "TEST OK: Exception caught: %s\n", fz_caught_message(ctx));
	}

	fz_throw(global_ctx, FZ_ERROR_GENERIC, "EXCEPTION #3: Testing %s within a try/catch environment.", "fz_throw()");
}

static void f2(fz_context* global_ctx, fz_context* ctx)
{
	// throwing in a multi-level context:
	fz_try(global_ctx)
	{
		f2sub(global_ctx, ctx);

		ASSERT_AND_CONTINUE(!"Should never get here!");
	}
	fz_catch(global_ctx)
	{
		fz_error(global_ctx, "TEST OK: Exception caught: %s\n", fz_caught_message(ctx));
	}
}


static void f3sub(fz_context* global_ctx, fz_context* ctx)
{
	fz_try(ctx)
	{
		// !!! global_ctx used !!!
		fz_throw(global_ctx, FZ_ERROR_GENERIC, "EXCEPTION #4: Testing %s within a 2-layer try/catch environment.", "fz_throw()");
	}
	fz_catch(ctx)
	{
		ASSERT_AND_CONTINUE(!"Should never get here!");
		fz_error(ctx, "TEST FAIL: Exception caught at inner layer: %s\n", fz_caught_message(ctx));
	}
}

static void f3(fz_context* global_ctx, fz_context* ctx)
{
	// throwing in a multi-level context with awareness of only the outer layer:
	fz_try(global_ctx)
	{
		f3sub(global_ctx, ctx);

		ASSERT_AND_CONTINUE(!"Should never get here!");
	}
	fz_catch(global_ctx)
	{
		fz_error(global_ctx, "TEST OK: Exception caught at outer layer: %s\n", fz_caught_message(ctx));
	}
}


static void f4(fz_context* global_ctx, fz_context* ctx)
{
	// throwing in a multi-level context with awareness of only the global layer:
	fz_try(ctx)
	{
		// !!! global_ctx used !!!
		fz_throw(global_ctx, FZ_ERROR_GENERIC, "EXCEPTION #5: Testing %s within a different context, which should be deaf to our exception.", "fz_throw()");
	}
	fz_catch(ctx)
	{
		ASSERT_AND_CONTINUE(!"Should never get here!");
		fz_error(ctx, "TEST FAIL: Exception caught: %s\n", fz_caught_message(ctx));
	}
}


int
main(int argc, const char** argv)
{
	int rv = 0;
	fz_locks_context* locks = NULL;
	size_t max_store = FZ_STORE_DEFAULT;

	fz_error(ctx, "Showcase fz_throw() exception throwing with a context, or a chained context.\nThis should exit the application in a controlled (yet abortive) fashion.\n\n");

	ASSERT(!fz_has_global_context());

	fz_enable_dbg_output();
	fz_enable_dbg_output_on_stdio_unreachable();

	ctx = NULL;

	atexit(mu_drop_context_at_exit);

#ifndef DISABLE_MUTHREADS
	locks = init_mudraw_locks();
	if (locks == NULL)
	{
		fz_error(ctx, "mutex initialisation failed");
		return EXIT_FAILURE;
	}
#endif

	// setting up a context:
	// first we produce a global context (IMPLICIT creation in next call):
	ctx = fz_get_global_context();
	ASSERT(ctx != NULL);

	fz_context* global_ctx = ctx;

	ctx = fz_new_context(NULL, locks, max_store);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_warn(ctx, "Going to throw exceptions...\n");

	f1(global_ctx, ctx);
	f2(global_ctx, ctx);
	f3(global_ctx, ctx);
	f4(global_ctx, ctx);

	fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION: Testing %s OUTSIDE a try/catch environment, with a still-alive context.", "fz_throw()");

	fz_error(ctx, "Should NEVER get here!");
	return EXIT_FAILURE;
}

