
#include "mupdf/fitz.h"
#include "mupdf/assertions.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


static fz_context* ctx = NULL;



static void mu_drop_context_at_exit(void)
{
	// we're aborting/exiting the application.
	fz_warn(ctx, "Landed in the atexit() handler as expected...\n");

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

	atexit(mu_drop_context_at_exit);

	fz_warn(ctx, "Going to throw exception...\n");

	fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION: Testing %s.", "fz_throw()");

	fz_error(ctx, "Should NEVER get here!");
	return EXIT_FAILURE;
}

