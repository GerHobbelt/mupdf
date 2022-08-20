
#if defined(_MSC_VER)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>
#endif

#include "mupdf/fitz.h"
#include "mupdf/assert.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


static fz_context* ctx = NULL;



static void mu_drop_context_at_exit(void)
{
	// we're aborting/exiting the application.
	fz_warn(ctx, "Landed in the atexit() handler as expected...\n");

	ASSERT_AND_CONTINUE(!ctx || (ctx->error.top == ctx->error.stack_base));

	//fz_drop_context_locks(ctx);
	fz_drop_context(ctx);

	ctx = NULL;

	fz_drop_global_context();

	// be nasty: throw inside the atexit handler:
	fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION: Testing %s when thrown in atexit() handler. Expects to abort the application.", "fz_throw()");
}



int
main(int argc, const char** argv)
{
	int rv = 0;

	fz_error(ctx, "Showcase fz_throw() exception throwing with a context, not even a global context.\nThis should exit the application in a controlled (yet abortive) fashion.\n\n");

	ASSERT(!fz_has_global_context());

	fz_enable_dbg_output();
	fz_enable_dbg_output_on_stdio_unreachable();

	ctx = NULL;

#ifdef _WIN32
	// Get us a debug stream so we have a chance of seeing *independently* of stderr.
	fz_set_stddbg(ctx, fz_stdods(ctx));
#endif

	atexit(mu_drop_context_at_exit);

	fz_warn(ctx, "Going to throw exception...\n");

	fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION: Testing %s.", "fz_throw()");

	fz_error(ctx, "Should NEVER get here!");
	return EXIT_FAILURE;
}

