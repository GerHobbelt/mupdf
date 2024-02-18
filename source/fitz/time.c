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
#include <direct.h> /* for mkdir */


void *
fz_fopen_utf8(const char *name, const char *mode)
{
	wchar_t *wname, *wmode;
	FILE *file;

	wname = wchar_from_utf8(name);
	if (wname == NULL)
	{
		return NULL;
	}

	wmode = wchar_from_utf8(mode);
	if (wmode == NULL)
	{
		free(wname);
		return NULL;
	}

	file = _wfopen(wname, wmode);

	free(wname);
	free(wmode);
	return file;
}

int
fz_remove_utf8(const char *name)
{
	wchar_t *wname;
	int n;

	wname = wchar_from_utf8(name);
	if (wname == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	n = _wremove(wname);

	free(wname);
	return n;
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

int64_t
fz_stat_ctime(const char *path)
{
	struct _stat info;
	wchar_t *wpath;

	wpath = wchar_from_utf8(path);
	if (wpath == NULL)
		return 0;

	if (_wstat(wpath, &info) < 0) {
		free(wpath);
		return 0;
	}

	free(wpath);
	return info.st_ctime;
}

int64_t
fz_stat_mtime(const char *path)
{
	struct _stat info;
	wchar_t *wpath;

	wpath = wchar_from_utf8(path);
	if (wpath == NULL)
		return 0;

	if (_wstat(wpath, &info) < 0) {
		free(wpath);
		return 0;
	}

	free(wpath);
	return info.st_mtime;
}

int
fz_mkdir(char *path)
{
	int ret;
	wchar_t *wpath = wchar_from_utf8(path);

	if (wpath == NULL)
		return -1;

	ret = _wmkdir(wpath);

	free(wpath);

	return ret;
}

#else

int64_t
fz_stat_ctime(const char *path)
{
	struct stat info;
	if (stat(path, &info) < 0)
		return 0;
	return info.st_ctime;
}

int64_t
fz_stat_mtime(const char *path)
{
	struct stat info;
	if (stat(path, &info) < 0)
		return 0;
	return info.st_mtime;
}

int
fz_mkdir(char *path)
{
	return mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
}

#endif /* _WIN32 */
