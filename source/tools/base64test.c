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

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"

#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>


static int tool_is_toplevel_ctx = 0;

static fz_context *ctx = NULL;

static void mu_drop_context(void)
{
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

	if (ctx != __fz_get_RAW_global_context())
	{
		fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
		ctx = NULL;
	}

	// nuke the locks last as they are still used by the heap free ('drop') calls in the lines just above!
	if (tool_is_toplevel_ctx)
	{
		ctx = NULL;

		fz_drop_global_context();

		tool_is_toplevel_ctx = 0;
	}
}



#if defined(BUILD_MONOLITHIC)
#define main(cnt, arr)      mupdf_base64_test_main(cnt, arr)
#endif

int main(int argc, const char** argv)
{
	// reset global vars: this tool MAY be re-invoked via bulktest or others and should RESET completely between runs:
	//ctx = NULL;

	if (!fz_has_global_context())
	{
		ASSERT(ctx == NULL);
		
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);

		tool_is_toplevel_ctx = 1;
	}
	atexit(mu_drop_context);

	if (ctx != __fz_get_RAW_global_context())
	{
		fz_drop_context(ctx); // drop our previous context IFF this happens to be a re-run in monolithic mode.
		ctx = NULL;
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	{
		const char *input[] = {
			"",
			"Zg==",
			"Zm8=",
			"Zm9v",
			"Zm9vYg==",
			"Zm9vYmE=",
			"Zm9vYmFy",
			"Z",
			"Zg",
			"Zg=",
			"Zg======",
			"Zg==Zg==",
			"Zg==%",
			"Z'g=",
		};
		size_t k;

		for (k = 0; k < nelem(input); k++)
		{
			fz_buffer *buf = NULL;
			size_t size, i;
			unsigned char *data = NULL;
			fz_buffer *out = fz_new_buffer(ctx, 0);
			unsigned char *s;

			fz_var(buf);

			fz_warn(ctx, "<<<");

			fz_append_printf(ctx, out, "input = '");
			for (i = 0; i < strlen(input[k]); i++)
				fz_append_byte(ctx, out, input[k][i]);
			fz_append_printf(ctx, out, "' -> ");

			fz_try(ctx)
			{
				buf = fz_new_buffer_from_base64(ctx, input[k], strlen(input[k]));

				size = fz_buffer_storage(ctx, buf, &data);
				fz_warn(ctx, "data = %p size = %zd", data, size);

				fz_append_printf(ctx, out, "'");
				for (i = 0; i < size; i++)
					if (data[i] >= ' ' && data[i] <= '~')
						fz_append_byte(ctx, out, data[i]);
					else
						fz_append_byte(ctx, out, '?');
				fz_append_printf(ctx, out, "' == ");
				for (i = 0; i < size; i++)
				{
					int m = data[i];
					fz_append_printf(ctx, out, "0x%02x ", m);
				}
			}
			fz_always(ctx)
				fz_drop_buffer(ctx, buf);
			fz_catch(ctx)
				fz_append_printf(ctx, out, "ERROR");

			fz_terminate_buffer(ctx, out);
			(void) fz_buffer_storage(ctx, out, &s);
			fz_warn(ctx, "%s", s);
			fz_drop_buffer(ctx, out);

			fz_warn(ctx, ">>>");
		}
	}

	fz_flush_warnings(ctx);
	mu_drop_context();

	return 0;
}
