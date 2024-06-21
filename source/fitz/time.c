// Copyright (C) 2004-2024 Artifex Software, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif


/*
Return NULL on (out of memory) error.
*/
char **
fz_argv_from_wargv(fz_context* ctx, int argc, const wchar_t **wargv)
{
	char **argv;
	int i;

	argv = Memento_label(fz_calloc(ctx, argc, sizeof(char *)), "fz_argv");
	if (argv == NULL)
	{
		fz_error(NULL, "Out of memory while processing command line args!");
		return NULL;
	}

	for (i = 0; i < argc; i++)
	{
		argv[i] = Memento_label(fz_utf8_from_wchar(ctx, wargv[i]), "fz_arg");
		if (argv[i] == NULL)
		{
			fz_error(NULL, "Out of memory while processing command line args!");
			fz_free_argv(ctx, argc, argv);
			return NULL;
		}
	}

	return argv;
}

void
fz_free_argv(fz_context* ctx, int argc, const char** argv)
{
	int i;
	for (i = 0; i < argc; i++)
		fz_free(ctx, (void *)argv[i]);
	fz_free(ctx, (void *)argv);
}
