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

#include <sys/stat.h>

#ifdef _WIN32

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <windows.h>

#ifdef _MSC_VER
#ifndef _WINRT

#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;

	if (tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		tmpres /= 10; /*convert into microseconds*/
		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	return 0;
}

#endif /* !_WINRT */
#endif /* _MSC_VER */

/* Static version of fz_utf8_from_wchar that can
 * operate without an fz_context * - only used for
 * argc/argv handling. */
static char *
utf8_from_wchar(const wchar_t *s)
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

void *
fz_fopen_utf8(fz_context *ctx, const char *name, const char *mode)
{
	wchar_t *wname, *wmode;
	FILE *file;

	wname = fz_wchar_from_utf8(ctx, name);
	if (wname == NULL)
	{
		return NULL;
	}

	wmode = fz_wchar_from_utf8(ctx, mode);
	if (wmode == NULL)
	{
		fz_free(ctx, wname);
		return NULL;
	}

	file = _wfopen(wname, wmode);

	fz_free(ctx, wname);
	fz_free(ctx, wmode);
	return file;
}

int
fz_remove_utf8(fz_context *ctx, const char *name)
{
	wchar_t *wname;
	int n;

	wname = fz_wchar_from_utf8(ctx, name);
	if (wname == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	n = _wremove(wname);

	fz_free(ctx, wname);
	return n;
}

char **
fz_argv_from_wargv(int argc, wchar_t **wargv)
{
	char **argv;
	int i;

	argv = Memento_label(calloc(argc, sizeof(char *)), "fz_argv");
	if (argv == NULL)
	{
		fprintf(stderr, "Out of memory while processing command line args!\n");
		exit(1);
	}

	for (i = 0; i < argc; i++)
	{
		argv[i] = Memento_label(utf8_from_wchar(wargv[i]), "fz_arg");
		if (argv[i] == NULL)
		{
			fprintf(stderr, "Out of memory while processing command line args!\n");
			exit(1);
		}
	}

	return argv;
}

void
fz_free_argv(int argc, char **argv)
{
	int i;
	for (i = 0; i < argc; i++)
		free(argv[i]);
	free(argv);
}

int64_t
fz_stat_ctime(fz_context *ctx, const char *path)
{
	struct _stat info;
	wchar_t *wpath;

	wpath = fz_wchar_from_utf8(ctx, path);
	if (wpath == NULL)
		return 0;

	if (_wstat(wpath, &info) < 0) {
		fz_free(ctx, wpath);
		return 0;
	}

	fz_free(ctx, wpath);
	return info.st_ctime;
}

int64_t
fz_stat_mtime(fz_context *ctx, const char *path)
{
	struct _stat info;
	wchar_t *wpath;

	wpath = fz_wchar_from_utf8(ctx, path);
	if (wpath == NULL)
		return 0;

	if (_wstat(wpath, &info) < 0) {
		fz_free(ctx, wpath);
		return 0;
	}

	fz_free(ctx, wpath);
	return info.st_mtime;
}

#else

int64_t
fz_stat_ctime(fz_context *ctx, const char *path)
{
	struct stat info;
	if (stat(path, &info) < 0)
		return 0;
	return info.st_ctime;
}

int64_t
fz_stat_mtime(fz_context *ctx, const char *path)
{
	struct stat info;
	if (stat(path, &info) < 0)
		return 0;
	return info.st_mtime;
}

#endif /* _WIN32 */
