#ifdef _WIN32

#include "mupdf/fitz.h"

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <windows.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif


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
		s += fz_chartorune(&c, s);
		/* Truncating c to a wchar_t can be problematic if c
		 * is 0x10000. */
		if (c >= 0x10000)
			c = FZ_REPLACEMENT_CHARACTER;
		*d++ = c;
	}
	*d = 0;
	return r;
}

void *
fz_fopen_utf8(const char *name, const char *mode)
{
	wchar_t *wname, *wmode;
	FILE *file;

	wname = fz_wchar_from_utf8(name);
	if (wname == NULL)
	{
		return NULL;
	}

	wmode = fz_wchar_from_utf8(mode);
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

	wname = fz_wchar_from_utf8(name);
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
fz_free_argv(int argc, const char **argv)
{
	int i;
	for (i = 0; i < argc; i++)
		free((void *)argv[i]);
	free((void *)argv);
}

#else

int fz_time_dummy;

#endif /* _WIN32 */
