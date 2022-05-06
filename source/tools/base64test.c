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

#if defined(_MSC_VER)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>
#endif

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"

#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>


static fz_context *ctx = NULL;


#if defined(BUILD_MONOLITHIC)
#define main(cnt, arr)      mupdf_base64_test_main(cnt, arr)
#endif

int main(int argc, const char** argv)
{
	// reset global vars: this tool MAY be re-invoked via bulktest or others and should RESET completely between runs:
	ctx = NULL;

	if (!fz_has_global_context())
	{
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
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
	fz_drop_context(ctx);

	return 0;
}
