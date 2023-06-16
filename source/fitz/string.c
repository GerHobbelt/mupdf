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
#include "utf.h"

#include <string.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h> /* for MultiByteToWideChar etc. */
#endif

#include "utfdata.h"

static const int *
fz_ucd_bsearch(int c, const int *t, int n, int ne)
{
	const int *p;
	int m;
	while (n > 1)
	{
		m = n/2;
		p = t + m*ne;
		if (c >= p[0])
		{
			t = p;
			n = n - m;
		}
		else
		{
			n = m;
		}
	}
	if (n && c >= t[0])
		return t;
	return 0;
}

int
fz_tolower(int c)
{
	const int *p;
	p = fz_ucd_bsearch(c, ucd_tolower2, nelem(ucd_tolower2) / 3, 3);
	if (p && c >= p[0] && c <= p[1])
		return c + p[2];
	p = fz_ucd_bsearch(c, ucd_tolower1, nelem(ucd_tolower1) / 2, 2);
	if (p && c == p[0])
		return c + p[1];
	return c;
}

int
fz_toupper(int c)
{
	const int *p;
	p = fz_ucd_bsearch(c, ucd_toupper2, nelem(ucd_toupper2) / 3, 3);
	if (p && c >= p[0] && c <= p[1])
		return c + p[2];
	p = fz_ucd_bsearch(c, ucd_toupper1, nelem(ucd_toupper1) / 2, 2);
	if (p && c == p[0])
		return c + p[1];
	return c;
}

size_t
fz_strnlen(const char *s, size_t n)
{
	const char *p = (const char *)memchr(s, 0, n);
	return p ? (size_t) (p - s) : n;
}

int
fz_strncasecmp(const char *a, const char *b, size_t n)
{
	if (!n--)
		return 0;
	for (; *a && *b && n && (*a == *b || fz_tolower(*a) == fz_tolower(*b)); a++, b++, n--)
		;
	return fz_tolower(*a) - fz_tolower(*b);
}

int
fz_strcasecmp(const char *a, const char *b)
{
	while (fz_tolower(*a) == fz_tolower(*b))
	{
		if (*a++ == 0)
			return 0;
		b++;
	}
	return fz_tolower(*a) - fz_tolower(*b);
}

char *
fz_strsep(char **stringp, const char *delim)
{
	char *ret = *stringp;
	if (!ret) return NULL;
	if ((*stringp = strpbrk(*stringp, delim)) != NULL)
		*((*stringp)++) = '\0';
	return ret;
}

size_t
fz_strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

size_t
fz_strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (*d != '\0' && n-- != 0)
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return dlen + strlen(s);
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return dlen + (s - src);	/* count does not include NUL */
}

void
fz_strncpy_s(fz_context* ctx, char* dst, const char* src, size_t dstsiz)
{
	if (dstsiz == 0)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_strncpy_s::dstsiz == 0: zero-length destination buffer specified.");
	}

	// use strnlen() as this function can legally be passed a non-NUL-terminated `src`!
	size_t srclen = strnlen(src, dstsiz);
	if (srclen == dstsiz)
	{
		memmove(dst, src, dstsiz - 1);
		dst[dstsiz - 1] = 0;
	}
	else
	{
		// len(src) < dstsiz: can safely use strcpy()
		memmove(dst, src, srclen);		// allow overlapping copy & include sentinel
		dst[srclen] = 0;
	}
}

void
fz_strncat_s(fz_context* ctx, char* dst, const char* src, size_t dstsiz)
{
	if (dstsiz == 0)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_strncat_s::dstsiz == 0: zero-length destination buffer specified.");
	}

	// use strnlen() as this function can legally be passed a non-NUL-terminated `dst`!
	size_t l = strnlen(dst, dstsiz);
	dst += l;
	dstsiz -= l;

	// use strnlen() as this function can legally be passed a non-NUL-terminated `src`!
	size_t srclen = strnlen(src, dstsiz);
	if (srclen == dstsiz)
	{
		memmove(dst, src, dstsiz - 1);
		dst[dstsiz - 1] = 0;
	}
	else
	{
		// len(src) < dstsiz: can safely use strcpy()
		memmove(dst, src, srclen);		// allow overlapping copy & include sentinel
		dst[srclen] = 0;
	}
}

size_t
fz_strrcspn(const char* str, const char* set)
{
	if (!str)
		return 0;
	if (!set || !*set)
		return strlen(str);

	const char* p = str + strlen(str) - 1;
	for (; p >= str; p--)
	{
		if (strchr(set, *p))
		{
			return p - str;
		}
	}
	return strlen(str);
}

void
fz_dirname(char *dir, const char *path, size_t n)
{
	if (!path || !path[0])
	{
		fz_strncpy_s(NULL, dir, ".", n);
		return;
	}

	fz_strncpy_s(NULL, dir, path, n);
	char* p = (char *)fz_basename(dir);
	*p = 0;
	// trim off trailing '/', unless it's a root directory:
	if (p == dir)
	{
		fz_strncpy_s(NULL, dir, ".", n);
		return;
	}


	if (p[-1] == '/' || p[-1] == '\\')
	{
		p--;
		if (p > dir)
		{
			if (p[-1] == ':' && (dir + 2 == p || dir + 6 == p))
			{
				// root dir in classic MSwindows path, e.g. C:\ (or UNC prefixed: //?/C:/
				return;
			}
			if (strchr("/\\", dir[0]) != NULL && strchr("/\\", dir[1]) != NULL)
			{
				// UNC prefix: skip 'server' part and 'share' part to get to the root dir:
				char* e = dir + 2;
				e = strpbrk(e, "/\\");
				if (e)
				{
					e = strpbrk(e + 1, "/\\");
					if (e)
					{
						// we've observed a legal UNC path; if we don't get here, it's not a legal UNC path for sure!
						const char* root = e;
						if (p == root)
						{
							return;
						}
						*p = 0;
						return;
					}
				}
				// when we get here, it's in illegal/incomplete UNC path. Treat as a slightly b0rked UNIX path instead...
			}

			// not a UNIX root dir
			*p = 0;
			return;
		}
		// UNIX root dir
		return;
	}
	ASSERT0(!"Should never get here");
}

#ifdef _WIN32

char *fz_realpath(const char *path, char *buf)
{
	wchar_t wpath[PATH_MAX];
	wchar_t wbuf[PATH_MAX + 4];
	int i;
	if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, PATH_MAX))
		return NULL;
	if (!GetFullPathNameW(wpath, PATH_MAX, wbuf + 4, NULL))
		return NULL;
	const wchar_t* fp = wbuf + 4;
	// Is full path an UNC path already? If not, make it so:
	if (wbuf[4] != '\\' && wbuf[4] != '/')
	{
		wbuf[0] = '\\';
		wbuf[1] = '\\';
		wbuf[2] = '?';
		wbuf[3] = '\\';
		fp = wbuf;
	}
	if (!WideCharToMultiByte(CP_UTF8, 0, fp, -1, buf, PATH_MAX, NULL, NULL))
		return NULL;
	for (i=0; buf[i]; ++i)
		if (buf[i] == '\\')
			buf[i] = '/';
	// remove the trailing '/' when it is superfluous; GetFullPathNameW() doesn't do that.
	size_t len = strlen(buf);
	if (len > 1 && buf[len - 1] == '/')
	{
		// superfluous when it's not part of a UNC //?/ prefix, and not preceded by a drive letter.
		if (buf[len - 2] != ':' && buf[len - 2] != '?')
		{
			buf[len - 1] = 0;
		}
	}
	return buf;
}

#else

char *fz_realpath(const char *path, char *buf)
{
	if (!realpath(path, buf))
		return NULL;

	// remove the trailing '/' when it is superfluous; realpath() doesn't do that.
	size_t len = strlen(buf);
	if (len > 1 && buf[len - 1] == '/')
	{
		// superfluous when it's not part of a UNC //?/ prefix, and not the root directory;
		// the latter is impossible because len >= 2 when we get here: .
		if (buf[len - 2] != ':' && buf[len - 2] != '?')
		{
			buf[len - 1] = 0;
		}
	}
	return buf;
}

#endif

static inline int ishex(int a)
{
	return (a >= 'A' && a <= 'F') ||
		(a >= 'a' && a <= 'f') ||
		(a >= '0' && a <= '9');
}

static inline int tohex(int c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 0xA;
	if (c >= 'A' && c <= 'F') return c - 'A' + 0xA;
	return 0;
}

char *
fz_urldecode(char *url)
{
	char *s = url;
	char *p = url;
	while (*s)
	{
		int c = (unsigned char) *s++;
		if (c == '%' && ishex(s[0]) && ishex(s[1]))
		{
			int a = tohex(*s++);
			int b = tohex(*s++);
			*p++ = a << 4 | b;
		}
		else
		{
			*p++ = c;
		}
	}
	*p = 0;
	return url;
}

static const char *check_percent_d(const char* s)
{
	/* find '%[+-][ ][0-9]*[.][0-9]*d', e.g. %04d or %+4.0d */
	if (*s++ == '%')
	{
		if (*s == '+' || *s == '-')
			++s;
		if (*s == ' ')
			++s;
		while (*s >= '0' && *s <= '9')
			++s;
		if (*s == '.')
			++s;
		while (*s >= '0' && *s <= '9')
			++s;
		if (*s == 'd')
			return s + 1;
	}
	return NULL;
}

int fz_has_page_format_marker(const char* s)
{
	return strpbrk(s, "%#$^!") != NULL;
}

// Skip directory/(Windows)drive separators.
//
// Returns pointer to basename part in the input string.
const char* fz_basename(const char* path)
{
	ASSERT0(path != NULL);

	size_t i;
	size_t len = strlen(path);
	for (i = strcspn(path, ":/\\"); i < len; i = strcspn(path, ":/\\"))
	{
		path = path + i + 1;
		len -= i + 1;
	}
	return path;
}

// include the leading '.' in the extension; point to the end of the input (NUL sentinel / empty string) when there's no extension to be had.
const char* fz_name_extension(const char* path)
{
	ASSERT0(path != NULL);

	path = fz_basename(path);

	// cope with 'dotfiles': ignore leading dots.
	while (*path == '.')
		path++;

	const char* x = path;

	path += strlen(path);
	for (x = strchr(x, '.'); x; x = strchr(x + 1, '.'))
	{
		path = x;
	}
	return path;
}

void
fz_format_output_path(fz_context *ctx, char *path, size_t size, const char *format, int page)
{
	fz_format_output_path_ex(ctx, path, size, format, 0, page, 0, NULL, NULL);
}

// a la strspn() but for a single-char set only
static inline size_t fz_strspn1(const char* s, char c)
{
	size_t i;
	for (i = 0; s[i] == c; i++)
		;
	return i;
}

void
fz_format_output_path_ex(fz_context* ctx, char* dstpath, size_t dstsize, const char* fmt, int chapter, int page, int sequence_number, const char* label, const char* extension)
{
	char* dstbase = dstpath;

	assert(dstsize >= 1);
	*dstpath = 0;

	int did_chapter = 0;
	int did_page = 0;
	int did_sequence_number = 0;
	int did_label = 0;

	const char* old_ext = fz_name_extension(fmt);
	const char* nuke_old_ext = NULL;
	if (extension)
	{
		const char* d = old_ext;
		while (*d == '.')
			d++;
		while (*extension == '.')
			extension++;
		if (!*extension)
		{
			extension = NULL;
		}
		else if (strcmp(d, extension) == 0)
		{
			// DEFERRED: nuke old extension as it matches exactly with the new one:
			nuke_old_ext = old_ext;
		}
		else
		{
			// NOT a matching extension: replace '.' with '-' as per spec.
			/* DEFERRED */;
		}
	}

	if (label && !*label)
	{
		// empty label == no label at all
		label = NULL;
	}

	if (!extension)
	{
		// No extension or EMPTY extension specified: use the extension from the template itself
		extension = fz_name_extension(fmt);
		nuke_old_ext = extension;

		while (*extension == '.')
			extension++;
	}
	assert(extension != NULL);
	assert(*extension != '.');

	const char* fmt_end = nuke_old_ext;
	if (!fmt_end)
		fmt_end = old_ext + strlen(old_ext);

	while (*fmt && fmt < fmt_end && dstsize > 1)
	{
		size_t pos = strcspn(fmt, "^!$%#.");

		if (pos >= dstsize)
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "fz_format_output_path_ex: out of bounds; path too long");
		}
		memcpy(dstpath, fmt, pos);
		dstpath += pos;
		dstsize -= pos;
		fmt += pos;

		switch (fmt[0])
		{
		case 0:
			continue;

		case '.':
			*dstpath++ = '.';
			dstsize--;
			fmt++;
			continue;

		case '%':
			{
				const char* p2 = check_percent_d(fmt);
				if (p2)
				{
					// cut after the 'd' of '%d':
					char fstr[20];
					size_t l = p2 - fmt;
					if (l >= sizeof(fmt))
						fz_throw(ctx, FZ_ERROR_GENERIC, "illegal (too large) marker in path name template: %.*q in %q", (int)l, fmt, fmt);
					memcpy(fstr, fmt, l);
					fstr[l] = 0;

					// bingo: workable/legal %d particle:
					l = fz_snprintf(dstpath, dstsize, fstr, page);
					if (l >= dstsize)
						fz_throw(ctx, FZ_ERROR_GENERIC, "path name buffer overflow");
					dstpath += l;
					dstsize -= l;

					fmt = p2;

					did_page++;
					continue;
				}
				// else: not a legal pagenum marker: copy as-is:
			}
			break;

		case '#':
			{
				size_t vl = fz_strspn1(fmt, fmt[0]);

				size_t l = fz_snprintf(dstpath, dstsize, "%0*d", (int)vl, page);
				if (l >= dstsize)
					fz_throw(ctx, FZ_ERROR_GENERIC, "path name buffer overflow");
				dstpath += l;
				dstsize -= l;

				fmt += l;

				did_page++;
				continue;
			}

		case '^':
			{
				size_t vl = fz_strspn1(fmt, fmt[0]);

				size_t l = fz_snprintf(dstpath, dstsize, "%0*d", (int)vl, sequence_number);
				if (l >= dstsize)
					fz_throw(ctx, FZ_ERROR_GENERIC, "path name buffer overflow");
				dstpath += l;
				dstsize -= l;

				fmt += l;

				did_sequence_number++;
				continue;
			}

		case '!':
			if (label)
			{
				size_t vl = fz_strspn1(fmt, fmt[0]);

				size_t l = fz_snprintf(dstpath, dstsize, "%s", label);
				if (l >= dstsize)
					fz_throw(ctx, FZ_ERROR_GENERIC, "path name buffer overflow");
				dstpath += l;
				dstsize -= l;

				fmt += l;

				did_label++;
				continue;
			}
			break;

		case '$':
			{
				size_t vl = fz_strspn1(fmt, fmt[0]);

				size_t l = fz_snprintf(dstpath, dstsize, "%0*d", (int)vl, chapter);
				if (l >= dstsize)
					fz_throw(ctx, FZ_ERROR_GENERIC, "path name buffer overflow");
				dstpath += l;
				dstsize -= l;

				fmt += l;

				did_chapter++;
				continue;
			}
		}

		// not a desirable formatter code --> copy character verbatim
		*dstpath++ = *fmt++;
		dstsize--;
		fmt++;
	}

	// backpedal when we last wrote '.' as we hit the end/extension boundary above:
	while (dstbase < dstpath && strchr("-.", dstpath[-1]))
	{
		dstpath--;
		dstsize++;
	}

	// edge case checks:
	if (dstbase == dstpath)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_format_output_path_ex: invalid or empty path template specified; path name buffer underflow");
	}
	if (strchr("\\/:", dstpath[-1]))
	{
		static const char* fname = "NO-NAME";
		size_t l = strlen(fname);
		if (l + 1 >= dstsize)
			fz_throw(ctx, FZ_ERROR_GENERIC, "path name buffer overflow");
		memcpy(dstpath, fname, l);

		dstpath += l;
		dstsize -= l;
	}

	if (!dstsize)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_format_output_path_ex: out of bounds; path too long");
	}

#if 0
	// only for the preliminary filename should we convert dots to dashes:
	*dstpath = 0;
	for (char* fn = (char*)fz_basename(dstbase); fn < dstpath; fn++)
	{
		if (*fn == '.')
			*fn = '-';
	}
#endif

	// now append all the items which have not been incorporated in the template themselves:

	if (chapter && !did_chapter)
	{
		fz_snprintf(dstpath, dstsize, "-%02d", chapter);
		size_t len = strlen(dstpath);
		dstsize -= len;
		dstpath += len;
	}
	if ((page && !did_page) || (sequence_number && !did_sequence_number))
	{
		fz_snprintf(dstpath, dstsize, "-%04d-%02d", page, sequence_number);
		size_t len = strlen(dstpath);
		dstsize -= len;
		dstpath += len;
	}
	if (label && !did_label)
	{
		fz_snprintf(dstpath, dstsize, "-%s", label);
		size_t len = strlen(dstpath);
		dstsize -= len;
		dstpath += len;
	}
	if (*extension)
	{
		fz_snprintf(dstpath, dstsize, ".%s", extension);
		size_t len = strlen(dstpath);
		dstsize -= len;
		dstpath += len;
	}

	if (!dstsize)
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_format_output_path_ex: out of bounds; path too long");
	}

	*dstpath = 0;
}

#define SEP(x) ((x)=='/' || (x) == 0)

char *
fz_cleanname(char *name)
{
	char *p, *q, *dotdot;
	int rooted;

	rooted = name[0] == '/';

	/*
	 * invariants:
	 *		p points at beginning of path element we're considering.
	 *		q points just past the last path element we wrote (no slash).
	 *		dotdot points just past the point where .. cannot backtrack
	 *				any further (no slash).
	 */
	p = q = dotdot = name + rooted;
	while (*p)
	{
		if(p[0] == '/') /* null element */
			p++;
		else if (p[0] == '.' && SEP(p[1]))
			p += 1; /* don't count the separator in case it is nul */
		else if (p[0] == '.' && p[1] == '.' && SEP(p[2]))
		{
			p += 2;
			if (q > dotdot) /* can backtrack */
			{
				while(--q > dotdot && *q != '/')
					;
			}
			else if (!rooted) /* /.. is / but ./../ is .. */
			{
				if (q != name)
					*q++ = '/';
				*q++ = '.';
				*q++ = '.';
				dotdot = q;
			}
		}
		else /* real path element */
		{
			if (q != name+rooted)
				*q++ = '/';
			while ((*q = *p) != '/' && *q != 0)
				p++, q++;
		}
	}

	if (q == name) /* empty string is really "." */
		*q++ = '.';
	*q = '\0';
	return name;
}

// NOTE: this next part is a near-duplicate of the code in thirdparty/mujs/utf.c

enum
{
	Bit1 = 7,
	Bitx = 6,
	Bit2 = 5,
	Bit3 = 4,
	Bit4 = 3,
	Bit5 = 2,

	T1 = ((1<<(Bit1+1))-1) ^ 0xFF, /* 0000 0000 */
	Tx = ((1<<(Bitx+1))-1) ^ 0xFF, /* 1000 0000 */
	T2 = ((1<<(Bit2+1))-1) ^ 0xFF, /* 1100 0000 */
	T3 = ((1<<(Bit3+1))-1) ^ 0xFF, /* 1110 0000 */
	T4 = ((1<<(Bit4+1))-1) ^ 0xFF, /* 1111 0000 */
	T5 = ((1<<(Bit5+1))-1) ^ 0xFF, /* 1111 1000 */

	Rune1 = (1<<(Bit1+0*Bitx))-1, /* 0000 0000 0111 1111 */
	Rune2 = (1<<(Bit2+1*Bitx))-1, /* 0000 0111 1111 1111 */
	Rune3 = (1<<(Bit3+2*Bitx))-1, /* 1111 1111 1111 1111 */
	Rune4 = (1<<(Bit4+3*Bitx))-1, /* 0001 1111 1111 1111 1111 1111 */

	Maskx = (1<<Bitx)-1,	/* 0011 1111 */
	Testx = Maskx ^ 0xFF,	/* 1100 0000 */

	Bad = Runeerror,
};

int
fz_chartorune(int *rune, const char *str, size_t n)
{
	int c, c1, c2, c3;
	int l;

	/*
	 * one character sequence
	 *	00000-0007F => T1
	 */
	if (n < 1)
		goto bad;
	c = *(const unsigned char*)str;
	if(c < Tx) {
		*rune = c;
		return 1;
	}

	/*
	 * two character sequence
	 *	0080-07FF => T2 Tx
	 */
	if (n < 2)
		goto bad;
	c1 = *(const unsigned char*)(str+1) ^ Tx;
	if(c1 & Testx)
		goto bad;
	if(c < T3) {
		if(c < T2)
			goto bad;
		l = ((c << Bitx) | c1) & Rune2;
		if(l <= Rune1)
			goto bad;
		*rune = l;
		return 2;
	}

	/*
	 * three character sequence
	 *	0800-FFFF => T3 Tx Tx
	 */
	if (n < 3)
		goto bad;
	c2 = *(const unsigned char*)(str+2) ^ Tx;
	if(c2 & Testx)
		goto bad;
	if(c < T4) {
		l = ((((c << Bitx) | c1) << Bitx) | c2) & Rune3;
		if(l <= Rune2)
			goto bad;
		*rune = l;
		return 3;
	}

	/*
	 * four character sequence (21-bit value)
	 *	10000-1FFFFF => T4 Tx Tx Tx
	 */
	if (n < 4)
		goto bad;
	c3 = *(const unsigned char*)(str+3) ^ Tx;
	if (c3 & Testx)
		goto bad;
	if (c < T5) {
		l = ((((((c << Bitx) | c1) << Bitx) | c2) << Bitx) | c3) & Rune4;
		if (l <= Rune3)
			goto bad;
		*rune = l;
		return 4;
	}
	/*
	 * Support for 5-byte or longer UTF-8 would go here, but
	 * since we don't have that, we'll just fall through to bad.
	 */

	/*
	 * bad decoding
	 */
bad:
	*rune = Bad;
	return 1;
}

int
fz_runetochar(char *str, int rune)
{
	/* Runes are signed, so convert to unsigned for range check. */
	unsigned int c = (unsigned int)rune;

	/*
	 * one character sequence
	 *	00000-0007F => 00-7F
	 */
	if(c <= Rune1) {
		str[0] = c;
		return 1;
	}

	/*
	 * two character sequence
	 *	0080-07FF => T2 Tx
	 */
	if(c <= Rune2) {
		str[0] = T2 | (c >> 1*Bitx);
		str[1] = Tx | (c & Maskx);
		return 2;
	}

	/*
	 * If the Rune is out of range, convert it to the error rune.
	 * Do this test here because the error rune encodes to three bytes.
	 * Doing it earlier would duplicate work, since an out of range
	 * Rune wouldn't have fit in one or two bytes.
	 */
	if (c > Runemax)
		c = Runeerror;

	/*
	 * three character sequence
	 *	0800-FFFF => T3 Tx Tx
	 */
	if (c <= Rune3) {
		str[0] = T3 | (c >> 2*Bitx);
		str[1] = Tx | ((c >> 1*Bitx) & Maskx);
		str[2] = Tx | (c & Maskx);
		return 3;
	}

	/*
	 * four character sequence (21-bit value)
	 *	10000-1FFFFF => T4 Tx Tx Tx
	 */
	str[0] = T4 | (c >> 3*Bitx);
	str[1] = Tx | ((c >> 2*Bitx) & Maskx);
	str[2] = Tx | ((c >> 1*Bitx) & Maskx);
	str[3] = Tx | (c & Maskx);
	return 4;
}

int
fz_runelen(int c)
{
	char str[10];
	return fz_runetochar(str, c);
}

int
fz_runeidx(const char *s, const char *p)
{
	int rune;
	int i = 0;
	while (s < p) {
		if (*(unsigned char *)s < Runeself)
			++s;
		else
			s += fz_chartorune_unsafe(&rune, s);
		++i;
	}
	return i;
}

const char *
fz_runeptr(const char *s, int i)
{
	int rune;
	while (i-- > 0) {
		rune = *(unsigned char*)s;
		if (rune < Runeself) {
			if (rune == 0)
				return NULL;
			++s;
		} else
			s += fz_chartorune_unsafe(&rune, s);
	}
	return s;
}

int
fz_utflen(const char *s)
{
	int c, n, rune;
	n = 0;
	for(;;) {
		c = *(const unsigned char*)s;
		if(c < Runeself) {
			if(c == 0)
				return n;
			s++;
		} else
			s += fz_chartorune_unsafe(&rune, s);
		n++;
	}
	return 0;
}

float fz_atof(const char *s)
{
	float result;

	if (s == NULL)
		return 0;

	errno = 0;
	result = fz_strtof(s, NULL);
	if ((errno == ERANGE && result == 0) || isnan(result))
		/* Return 1.0 on  underflow, as it's a small known value that won't cause a divide by 0.  */
		return 1;
	result = fz_clamp(result, -FLT_MAX, FLT_MAX);
	return result;
}

int fz_atoi(const char *s)
{
	if (s == NULL)
		return 0;
	return atoi(s);
}

int64_t fz_atoi64(const char *s)
{
	if (s == NULL)
		return 0;
	return atoll(s);
}

int fz_is_page_range(fz_context *ctx, const char *s)
{
	/* TODO: check the actual syntax... */
	while (*s)
	{
		if ((*s < '0' || *s > '9') && *s != 'N' && *s != '-' && *s != ',')
			return 0;
		s++;
	}
	return 1;
}

const char *fz_parse_page_range(fz_context *ctx, const char *s, int *a, int *b, int n)
{
	int rev = 0;

	if (!s || !s[0])
		return NULL;

	if (s[0] == ',')
		s += 1;

	if (s[0] == '~')
		rev = 1, s++;
	if (s[0] == 'N')
	{
		*a = n;
		s += 1;
	}
	else
		*a = strtol(s, (char**)&s, 10);
	if (rev)
		*a = n+1-*a;

	if (s[0] == '-')
	{
		rev = 0;
		if (s[1] == '~')
			rev = 1, s++;
		if (s[1] == 'N')
		{
			*b = n;
			s += 2;
		}
		else
			*b = strtol(s+1, (char**)&s, 10);
		if (rev)
			*b = n+1-*b;
	}
	else
		*b = *a;

	if (*a < 0) *a = n + 1 + *a;
	if (*b < 0) *b = n + 1 + *b;

	*a = fz_clampi(*a, 1, n);
	*b = fz_clampi(*b, 1, n);

	return s;
}

/* memmem from musl */

#define MAX(a,b) ((a)>(b)?(a):(b))

#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

static char *twobyte_memmem(const unsigned char *h, size_t k, const unsigned char *n)
{
	uint16_t nw = n[0]<<8 | n[1], hw = h[0]<<8 | h[1];
	for (h++, k--; k; k--, hw = hw<<8 | *++h)
		if (hw == nw) return (char *)h-1;
	return 0;
}

static char *threebyte_memmem(const unsigned char *h, size_t k, const unsigned char *n)
{
	uint32_t nw = n[0]<<24 | n[1]<<16 | n[2]<<8;
	uint32_t hw = h[0]<<24 | h[1]<<16 | h[2]<<8;
	for (h+=2, k-=2; k; k--, hw = (hw|*++h)<<8)
		if (hw == nw) return (char *)h-2;
	return 0;
}

static char *fourbyte_memmem(const unsigned char *h, size_t k, const unsigned char *n)
{
	uint32_t nw = n[0]<<24 | n[1]<<16 | n[2]<<8 | n[3];
	uint32_t hw = h[0]<<24 | h[1]<<16 | h[2]<<8 | h[3];
	for (h+=3, k-=3; k; k--, hw = hw<<8 | *++h)
		if (hw == nw) return (char *)h-3;
	return 0;
}

static char *twoway_memmem(const unsigned char *h, const unsigned char *z, const unsigned char *n, size_t l)
{
	size_t i, ip, jp, k, p, ms, p0, mem, mem0;
	size_t byteset[32 / sizeof(size_t)] = { 0 };
	size_t shift[256];

	/* Computing length of needle and fill shift table */
	for (i=0; i<l; i++)
		BITOP(byteset, n[i], |=), shift[n[i]] = i+1;

	/* Compute maximal suffix */
	ip = -1; jp = 0; k = p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else k++;
		} else if (n[ip+k] > n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	ms = ip;
	p0 = p;

	/* And with the opposite comparison */
	ip = -1; jp = 0; k = p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else k++;
		} else if (n[ip+k] < n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip+1 > ms+1) ms = ip;
	else p = p0;

	/* Periodic needle? */
	if (memcmp(n, n+p, ms+1)) {
		mem0 = 0;
		p = MAX(ms, l-ms-1) + 1;
	} else mem0 = l-p;
	mem = 0;

	/* Search loop */
	for (;;) {
		/* If remainder of haystack is shorter than needle, done */
		if ((size_t)(z-h) < l) return 0;

		/* Check last byte first; advance by shift on mismatch */
		if (BITOP(byteset, h[l-1], &)) {
			k = l-shift[h[l-1]];
			if (k) {
				if (mem0 && mem && k < p) k = l-p;
				h += k;
				mem = 0;
				continue;
			}
		} else {
			h += l;
			mem = 0;
			continue;
		}

		/* Compare right half */
		for (k=MAX(ms+1,mem); k<l && n[k] == h[k]; k++);
		if (k < l) {
			h += k-ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k=ms+1; k>mem && n[k-1] == h[k-1]; k--);
		if (k <= mem) return (char *)h;
		h += p;
		mem = mem0;
	}
}

void *fz_memmem(const void *h0, size_t k, const void *n0, size_t l)
{
	const unsigned char *h = h0, *n = n0;

	/* Return immediately on empty needle */
	if (!l) return (void *)h;

	/* Return immediately when needle is longer than haystack */
	if (k<l) return 0;

	/* Use faster algorithms for short needles */
	h = memchr(h0, *n, k);
	if (!h || l==1) return (void *)h;
	k -= h - (const unsigned char *)h0;
	if (k<l) return 0;
	if (l==2) return twobyte_memmem(h, k, n);
	if (l==3) return threebyte_memmem(h, k, n);
	if (l==4) return fourbyte_memmem(h, k, n);

	return twoway_memmem(h, h+k, n, l);
}
