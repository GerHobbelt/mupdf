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

#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>


char *
fz_utf8_from_wchar(const wchar_t *s)
{
	const wchar_t *src = s;
	char *d;
	char *dst;
	int len = 1;

	while (*src)
	{
		len += fz_runelen(*src++);
	}

	d = Memento_label(malloc(len), "utf8_from_wchar");
	if (d != NULL)
	{
		dst = d;
		src = s;
		while (*src)
		{
			dst += fz_runetochar(dst, *src++);
		}
		*dst = 0;
	}
	return d;
}

wchar_t *
fz_wchar_from_utf8(const char *s)
{
	wchar_t *d, *r;
	int c;
	r = d = malloc((strlen(s) + 1) * sizeof(wchar_t));
	if (!r)
		return NULL;
	while (*s) {
		s += fz_chartorune_unsafe(&c, s);
		/* Truncating c to a wchar_t can be problematic if c
		 * is 0x10000. */
		if (c >= 0x10000)
			c = FZ_REPLACEMENT_CHARACTER;
		*d++ = c;
	}
	*d = 0;
	return r;
}

/*
Return NULL on (out of memory) error.
*/
char **
fz_argv_from_wargv(int argc, const wchar_t **wargv)
{
	char **argv;
	int i;

	argv = Memento_label(calloc(argc, sizeof(char *)), "fz_argv");
	if (argv == NULL)
	{
		fz_error(NULL, "Out of memory while processing command line args!");
		return NULL;
	}

	for (i = 0; i < argc; i++)
	{
		argv[i] = Memento_label(fz_utf8_from_wchar(wargv[i]), "fz_arg");
		if (argv[i] == NULL)
		{
			fz_error(NULL, "Out of memory while processing command line args!");
			return NULL;
		}
	}

	return argv;
}

void
fz_free_argv(int argc, const char** argv)
{
	int i;
	for (i = 0; i < argc; i++)
		free((void *)argv[i]);
	free((void *)argv);
}

#endif /* _WIN32 */
