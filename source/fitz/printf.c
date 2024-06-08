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
#include "mupdf/pdf/object.h"
#include "utf.h"

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _MSC_VER
#if _MSC_VER < 1500 /* MSVC 2008 */
int snprintf(char *s, size_t n, const char *fmt, ...)
{
		int r;
		va_list ap;
		va_start(ap, fmt);
		r = vsprintf(s, fmt, ap);
		va_end(ap);
		return r;
}
#else if _MSC_VER < 1900 /* MSVC 2015 */
#define snprintf _snprintf
#endif
#endif

const char *fz_hex_digits = "0123456789ABCDEF";
static const char *fz_hex_digits_UC = "0123456789ABCDEF";

#define FMT_DEFAULT_FLOAT_PRECISION		6

struct fmtbuf
{
	fz_context *ctx;
	void *user;
	void (*emit)(fz_context *ctx, void *user, int c);
};

static inline void fmtputc(struct fmtbuf *out, int c)
{
	out->emit(out->ctx, out->user, c);
}

static inline void fmtputs(struct fmtbuf* out, const char *s)
{
	while (*s)
		fmtputc(out, *s++);
}

/*
 * Convert float to shortest possible string that won't lose precision, except:
 * NaN to 0, +Inf to FLT_MAX, -Inf to -FLT_MAX.
 */
static void fmtfloat(struct fmtbuf *out, float f)
{
	char digits[100];
	char* s = digits;
	int exp, ndigits, point;

	if (isnan(f)) f = 0;
	if (isinf(f)) f = f < 0 ? -FLT_MAX : FLT_MAX;

	if (signbit(f))
		fmtputc(out, '-');

	if (f == 0)
	{
		fmtputc(out, '0');
		return;
	}

	ndigits = fz_grisu(f, digits, &exp);
	point = exp + ndigits;

	if (point <= 0)
	{
		fmtputc(out, '0');
		fmtputc(out, '.');
		while (point++ < 0)
			fmtputc(out, '0');
		while (ndigits-- > 0)
			fmtputc(out, *s++);
	}
	else
	{
		while (ndigits-- > 0)
		{
			fmtputc(out, *s++);
			if (--point == 0 && ndigits > 0)
				fmtputc(out, '.');
		}
		while (point-- > 0)
			fmtputc(out, '0');
	}
}

static void fmtfloat_e(struct fmtbuf *out, double f, int w, int p, char fmt)
{
	char buf[100];
	char* s = buf;
	if (p == INT_MAX)
		p = FMT_DEFAULT_FLOAT_PRECISION;
	if (w < 0) // uninitialized value is INT_MIN but user may have passed bad negative width via '*' modifier either.
		w = 0;
#pragma warning(push)
#pragma warning(disable: 4774)  // warning C4774: '_snprintf' : format string expected in argument 3 is not a string literal; format specifiers in 'name' may pose a security issue
	char fmtstr[] = "%*.* ";
	fmtstr[4] = fmt;
	snprintf(buf, sizeof buf, fmtstr, w, p, f);
#pragma warning(pop)
	while (*s)
		fmtputc(out, *s++);
}

static void fmtuint32(struct fmtbuf *out, unsigned int a, int s, int z, int w, int base, int q)
{
	char buf[100];
	int i, qw;
	const char* hex_digits = fz_hex_digits;

	if (base < 0)
	{
		base = -base;
		hex_digits = fz_hex_digits_UC;
	}

	i = 0;
	if (a == 0)
		buf[i++] = '0';
	while (a) {
		buf[i++] = hex_digits[a % base];
		a /= base;
	}
	if (s) {
		if (z == '0')
			while (i < w - 1)
				buf[i++] = z;
		buf[i++] = s;
	}
	while (i < w)
		buf[i++] = z;
	if (!q) {
		qw = 0;
	}
	else {
		if (base != 10) {
			qw = 4;		// one separator per 4 (hex) nibbles or 4 binary bits
		}
		else {
			qw = 3;
		}
	}
	while (i > 0)
	{
		fmtputc(out, buf[--i]);
		if (q && i != 0 && i % qw == 0)
			fmtputc(out, q);
	}
}

static void fmtuint64(struct fmtbuf *out, uint64_t a, int s, int z, int w, int base, int q)
{
	char buf[100];
	int i, qw;
	const char *hex_digits = fz_hex_digits;

	if (base < 0)
	{
		base = -base;
		hex_digits = fz_hex_digits_UC;
	}

	i = 0;
	if (a == 0)
		buf[i++] = '0';
	while (a) {
		buf[i++] = hex_digits[a % base];
		a /= base;
	}
	if (s) {
		if (z == '0')
			while (i < w - 1)
				buf[i++] = z;
		buf[i++] = s;
	}
	while (i < w)
		buf[i++] = z;
	if (!q) {
		qw = 0;
	}
	else {
		if (base != 10) {
			qw = 4;		// one separator per 4 (hex) nibbles or 4 binary bits
		}
		else {
			qw = 3;
		}
	}
	while (i > 0)
	{
		fmtputc(out, buf[--i]);
		if (q && i != 0 && i % qw == 0)
			fmtputc(out, q);
	}
}

static void fmtint32(struct fmtbuf *out, int value, int s, int z, int w, int base, int q)
{
	unsigned int a;

	if (value < 0)
	{
		s = '-';
		a = -value;
	}
	else if (s)
	{
		s = '+';
		a = value;
	}
	else
	{
		s = 0;
		a = value;
	}
	fmtuint32(out, a, s, z, w, base, q);
}

static void fmtint64(struct fmtbuf *out, int64_t value, int s, int z, int w, int base, int q)
{
	uint64_t a;

	if (value < 0)
	{
		s = '-';
		a = -value;
	}
	else if (s)
	{
		s = '+';
		a = value;
	}
	else
	{
		s = 0;
		a = value;
	}
	fmtuint64(out, a, s, z, w, base, q);
}

static void fmtquote(struct fmtbuf *out, const char *s, size_t slen, int sq, int eq, int verbatim, int no_hex_unicode_only)
{
	int i, n, c;
	fmtputc(out, sq);
	while (slen > 0) {
		n = fz_chartorune(&c, s, slen);
		switch (c) {
		default:
			if (c < 32) {
				if (!no_hex_unicode_only) {
					fmtputc(out, '\\');
					fmtputc(out, 'x');
					fmtputc(out, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c) & 0x0F]);
				}
				else {
					// some JSON parsers don't accept \xNN encodings, only \u00XX
					fmtputs(out, "\\u00");
					fmtputc(out, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c) & 0x0F]);
				}
			} else if (c >= 127) {
				if (n == 1 && c == Runeerror)
				{
					// output \uFFFD + hex-encoded bad char:
					char buf[10];
					int l = fz_runetochar(buf, c);
					for (i = 0; i < l; ++i)
						fmtputc(out, buf[i]);

					c = (unsigned char)s[0];
					if (!no_hex_unicode_only) {
						fmtputc(out, '\\');
						fmtputc(out, 'x');
						fmtputc(out, fz_hex_digits[(c >> 4) & 0x0F]);
						fmtputc(out, fz_hex_digits[(c) & 0x0F]);
					}
					else {
						// some JSON parsers don't accept \xNN encodings, only \u00XX
						fmtputs(out, "\\u00");
						fmtputc(out, fz_hex_digits[(c >> 4) & 0x0F]);
						fmtputc(out, fz_hex_digits[(c) & 0x0F]);
					}
				}
				if (verbatim)
				{
					for (i = 0; i < n; ++i)
						fmtputc(out, (unsigned char)s[i]);
				}
				else if (c < 0x10000)
				{
					fmtputc(out, '\\');
					fmtputc(out, 'u');
					fmtputc(out, fz_hex_digits[(c >> 12) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c >> 8) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c) & 0x0F]);
				}
				else
				{
					fmtputc(out, '\\');
					fmtputc(out, 'u');
					fmtputc(out, fz_hex_digits[(c >> 20) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c >> 16) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c >> 12) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c >> 8) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c) & 0x0F]);
				}
			} else {
				if (c == sq || c == eq)
					fmtputc(out, '\\');
				fmtputc(out, c);
			}
			break;
		case '\\': fmtputc(out, '\\'); fmtputc(out, '\\'); break;
		case '\b': fmtputc(out, '\\'); fmtputc(out, 'b'); break;
		case '\f': fmtputc(out, '\\'); fmtputc(out, 'f'); break;
		case '\n': fmtputc(out, '\\'); fmtputc(out, 'n'); break;
		case '\r': fmtputc(out, '\\'); fmtputc(out, 'r'); break;
		case '\t': fmtputc(out, '\\'); fmtputc(out, 't'); break;
		}
		slen -= n;
		s += n;
	}
	fmtputc(out, eq);
}

static void fmtquote_pdf(struct fmtbuf *out, const char *s, int sq, int eq)
{
	int c;
	fmtputc(out, sq);
	while ((c = (unsigned char)*s++) != 0) {
		switch (c) {
		default:
			if (c < 32 || c >= 127) {
				fmtputc(out, '\\');
				if (sq == '(')
				{
					fmtputc(out, '0' + ((c >> 6) & 7));
					fmtputc(out, '0' + ((c >> 3) & 7));
					fmtputc(out, '0' + ((c) & 7));
				}
				else
				{
					fmtputc(out, 'x');
					fmtputc(out, fz_hex_digits[(c>>4) & 0x0F]);
					fmtputc(out, fz_hex_digits[(c) & 0x0F]);
				}
			} else {
				if (c == sq || c == eq)
					fmtputc(out, '\\');
				fmtputc(out, c);
			}
			break;
		case '\\': fmtputc(out, '\\'); fmtputc(out, '\\'); break;
		case '\b': fmtputc(out, '\\'); fmtputc(out, 'b'); break;
		case '\f': fmtputc(out, '\\'); fmtputc(out, 'f'); break;
		case '\n': fmtputc(out, '\\'); fmtputc(out, 'n'); break;
		case '\r': fmtputc(out, '\\'); fmtputc(out, 'r'); break;
		case '\t': fmtputc(out, '\\'); fmtputc(out, 't'); break;
		}
	}
	fmtputc(out, eq);
}

static void fmtname(struct fmtbuf *out, const char *s)
{
	int c;
	fmtputc(out, '/');
	while ((c = *s++) != 0) {
		if (c <= 32 || c == '/' || c == '#') {
			fmtputc(out, '#');
			fmtputc(out, fz_hex_digits[(c>>4) & 0x0F]);
			fmtputc(out, fz_hex_digits[(c) & 0x0F]);
		} else {
			fmtputc(out, c);
		}
	}
}

#define FPBO_JSON_MODE				0x0001
#define FPBO_VERBATIM_UNICODE		0x0002
#define FPBO_NO_CONTROL_ESCAPES		0x0004

static void fmt_print_buffer_as_hex(struct fmtbuf* out, const char* data, size_t datalen, int p, int mode)
{
	if (!data)
		fmtputs(out, "(null)");
	if (mode & FPBO_JSON_MODE)
		fmtputc(out, '"');
	else if (!datalen)
		fmtputs(out, "(empty)");

	// precision determines the chunking: byte, word, ...
	//
	// as *uninitialized* precision was INT_MAX (we're 're-using' a floating point modifier here)
	// we make that 1: byte segmentation by default.
	//
	// Note: %.0H would mean NO segmentation what-so-ever!
	if (p == INT_MAX)
		p = 1;
	int segging = p;
	while (datalen-- > 0) {
		int c = (unsigned char)*data++;
		fmtputc(out, fz_hex_digits[(c >> 4) & 0x0F]);
		fmtputc(out, fz_hex_digits[(c) & 0x0F]);
		--segging;
		if (segging == 0 && datalen > 0) {
			fmtputc(out, ' ');
			segging = p;
		}
	}
	if (mode & FPBO_JSON_MODE)
		fmtputc(out, '"');
}

/*
	When this function is called (inside printf()) we assume that the incoming data has already been converted to the UTF8 'codepage'
	if any such conversion was applicable. We assume it was, and has been done, as we are treating the data as STRING data here,
	unless proven otherwise by the analysis which will be performed first.

	The results from that analysis will determine how the data is printed exactly: as a HEX DUMP or a more-or-less sane STRING.
*/
static void fmt_print_buffer_optimally(fz_context* ctx, struct fmtbuf* fmt, const char* data, size_t datalen, int flags, int mode)
{
	// Step 1: Content Analysis
	//
	// Do not write the string/data, just observe what happens and report so sane choices for final output can be made.
	int c;
	size_t i;
	int bad_unicodes = 0;
	int control_chars = 0;
	int has_C_control_escapes = 0;		// \b, \v, \f
	int irregular_control_chars = 0;
	int unicodes = 0;
	int has_nuls = 0;
	int has_linebreaks = 0;
	int has_misc_escapes = 0;		// \t, \\, \"
	int has_astral_unicodes = 0;

	if (!data)
		datalen = 0;

	for (i = 0; i < datalen; i++)
	{
		c = (unsigned char)data[i];
		switch (c)
		{
		case 0:
			has_nuls++;
			break;
		case '\n':
			has_linebreaks++;
			break;
		case '\r':
			has_linebreaks++;
			break;
		case '\t':
			has_misc_escapes++;
			break;
		case '\b':
			control_chars++;
			has_C_control_escapes++;
			break;
		case '\f':
			has_linebreaks++;
			control_chars++;
			has_C_control_escapes++;
			break;
//		case '\v':
//			has_linebreaks++;
//			control_chars++;
//			has_C_control_escapes++;
//			break;
		case '"':
			has_misc_escapes++;
			break;
		case '\\':
			has_misc_escapes++;
			break;
		default:
			if (c < 32)
			{
				control_chars++;
				irregular_control_chars++;
			}
			else if (c >= 127)
			{
				// decode UTF8 to Unicode
				int u;
				int l = fz_chartorune(&u, data + i, datalen - i);
				i += l - 1;
				if (l == 1 && u == Runeerror)
				{
					bad_unicodes++;
				}
				else
				{
					unicodes++;
					if (u > 0xFFFF)
					{
						has_astral_unicodes++;
					}
				}
			}
			break;
		}
	}

	// correct flags: must have control escape codes in 'JSON mode' (quoted string)
	if (mode & FPBO_JSON_MODE) {
		mode &= ~FPBO_NO_CONTROL_ESCAPES;
	}

	static const char* controls_display_escaped_mode[] =
	{
		"\\\"",
		"\\\\",
		"\\b",
		"\\f",
		"\\n",
		"\\r",
		"\\t",
//		"\\v"
	};
	static const char* controls_display_semi_escaped_mode[] =
	{
		"\"",
		"\\\\",
		"\b",
		"\f",
		"\n",
		"\r",
		"\t",
//		"\v"
	};
	static const char* controls_display_direct_mode[] =
	{
		"\"",
		"\\",
		"\b",
		"\f",
		"\n",
		"\r",
		"\t",
//		"\v"
	};

	const char** ctrl_tbl = controls_display_escaped_mode;
	if (!(mode & FPBO_JSON_MODE)) {
		if ((mode & FPBO_NO_CONTROL_ESCAPES) && (mode & FPBO_VERBATIM_UNICODE)) {
			ctrl_tbl = controls_display_direct_mode;
		}
		else {
			ctrl_tbl = controls_display_semi_escaped_mode;
		}
	}

	if (mode & FPBO_JSON_MODE) {
		fmtputc(fmt, '"');
	}

	// Now check if we consider this a "sane" string value:
	if (bad_unicodes ||
		(unicodes && control_chars) ||
		has_nuls ||
		irregular_control_chars)
	{
		// clearly NOT...
		//
		// decide whether to hex dump or to print with some hex, depending on the number
		// of items which are irregular:
		int bad_count_sans_nuls = bad_unicodes + control_chars;
		int bad_count = bad_count_sans_nuls + has_nuls;
		int non_ascii_count = bad_count + has_misc_escapes + unicodes;

		if (flags & PDF_PRINT_JSON_BINARY_DATA_AS_HEX_PLUS_RAW) {
			if (flags & PDF_PRINT_JSON_BINARY_DATA_AS_PURE_HEX) {
				int chunking = PDF_PRINT_JSON_DEPTH_LEVEL(flags);
				if (!chunking)
					chunking = 1;
				fmt_print_buffer_as_hex(fmt, data, datalen, chunking, mode & ~FPBO_JSON_MODE);
				// mark data as all done
				datalen = 0;
			}
			else {
				fmtputs(fmt, "HEX:");
				int chunking = PDF_PRINT_JSON_DEPTH_LEVEL(flags);
				if (!chunking)
					chunking = 1;
				fmt_print_buffer_as_hex(fmt, data, datalen, chunking, mode & ~FPBO_JSON_MODE);
				fmtputs(fmt, ";MASSAGED:");

				while (datalen) {
					int n = fz_chartorune(&c, data, datalen);
					switch (c) {
					default:
						if (c < 32) {
							fmtputc(fmt, ' ');
						}
						else if (c >= 127) {
							if (n == 1 && c == Runeerror)
							{
								fmtputc(fmt, ' ');
							}
							else if (mode & FPBO_VERBATIM_UNICODE)
							{
								for (int i = 0; i < n; ++i)
									fmtputc(fmt, (unsigned char)data[i]);
							}
							else if (c < 0x10000)
							{
								fmtputc(fmt, '\\');
								fmtputc(fmt, 'u');
								fmtputc(fmt, fz_hex_digits[(c >> 12) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 8) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
							}
							else
							{
								fmtputc(fmt, '\\');
								fmtputc(fmt, 'u');
								fmtputc(fmt, fz_hex_digits[(c >> 20) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 16) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 12) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 8) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
							}
						}
						else
						{
							fmtputc(fmt, c);
						}
						break;
					case '"':
						fmtputs(fmt, ctrl_tbl[0]);
						break;
					case '\\':
						fmtputs(fmt, ctrl_tbl[1]);
						break;
					case '\b':
						fmtputs(fmt, ctrl_tbl[2]);
						break;
					case '\f':
						fmtputs(fmt, ctrl_tbl[3]);
						break;
					case '\n':
						fmtputs(fmt, ctrl_tbl[4]);
						break;
					case '\r':
						fmtputs(fmt, ctrl_tbl[5]);
						break;
					case '\t':
						fmtputs(fmt, ctrl_tbl[6]);
						break;
//					case '\v':
//						fmtputs(fmt, ctrl_tbl[7]);
//						break;
					}
					datalen -= n;
					data += n;
				}
			}
		}
		else if (flags & PDF_PRINT_JSON_ILLEGAL_UNICODE_AS_HEX) {
			// heuristics: when enough is bad or non-ASCII, we dump hex.
			// (In larger files we trigger earlier and harder; when there's more than 30 bad entries, we assume it's pure binary all the way.)
			if (bad_count * 100 / datalen >= 15 || non_ascii_count * 100 / datalen >= 40 || bad_count >= 50) {
				if (flags & PDF_PRINT_JSON_BINARY_DATA_AS_PURE_HEX || bad_count_sans_nuls >= 30) {
					int chunking = PDF_PRINT_JSON_DEPTH_LEVEL(flags);
					if (!chunking)
						chunking = 1;
					fmt_print_buffer_as_hex(fmt, data, datalen, chunking, mode & ~FPBO_JSON_MODE);
					// mark data as all done
					datalen = 0;
				}
				else {
					while (datalen) {
						int n = fz_chartorune(&c, data, datalen);
						if (n == 1) {
							// grab byte/character again for hexdumping, as `c` can be RuneError and we don't wanna see that one.
							int b = (unsigned char)data[0];
							fmtputc(fmt, fz_hex_digits[(b >> 4) & 0x0F]);
							fmtputc(fmt, fz_hex_digits[(b) & 0x0F]);

							// show character itself when it's in ASCII printable range
							if (b >= 32 && b < 127) {
								fmtputc(fmt, '(');
								switch (b)
								{
								default:
									fmtputc(fmt, b);
									break;
								case '"':
									fmtputs(fmt, ctrl_tbl[0]);
									break;
								case '\\':
									fmtputs(fmt, ctrl_tbl[1]);
									break;
								case '\b':
									fmtputs(fmt, ctrl_tbl[2]);
									break;
								case '\f':
									fmtputs(fmt, ctrl_tbl[3]);
									break;
								case '\n':
									fmtputs(fmt, ctrl_tbl[4]);
									break;
								case '\r':
									fmtputs(fmt, ctrl_tbl[5]);
									break;
								case '\t':
									fmtputs(fmt, ctrl_tbl[6]);
									break;
//								case '\v':
//									fmtputs(fmt, ctrl_tbl[7]);
//									break;
								}
								fmtputc(fmt, ')');
							}
							fmtputc(fmt, ' ');
						}
						else {
							for (int i = 0; i < n; ++i) {
								int b = (unsigned char)data[i];
								if (i > 0)
									fmtputc(fmt, '.');
								fmtputc(fmt, fz_hex_digits[(b >> 4) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(b) & 0x0F]);
							}
							fmtputc(fmt, '(');
							if (c < 0x10000)
							{
								fmtputc(fmt, '\\');
								fmtputc(fmt, 'u');
								fmtputc(fmt, fz_hex_digits[(c >> 12) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 8) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
							}
							else
							{
								fmtputc(fmt, '\\');
								fmtputc(fmt, 'u');
								fmtputc(fmt, fz_hex_digits[(c >> 20) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 16) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 12) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 8) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
								fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
							}
							fmtputc(fmt, ')');
						}
						datalen -= n;
						data += n;
					}
				}
			}
		}
	}

	// is the string "non-simple", i.e. contains ANY escapes?
	// we don't care. Simply print the bugger.
	//
	// Also note that this code is the fallback for the "insane content",
	// so better make sure we can deal with that too!
	while (datalen) {
		int n = fz_chartorune(&c, data, datalen);
		switch (c) {
		default:
			if (c < 32) {
				if (mode & FPBO_JSON_MODE) {
					// some JSON parsers don't accept \xNN encodings, only \u00XX
					fmtputs(fmt, "\\u00");
					fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
				}
				else {
					fmtputc(fmt, '\\');
					fmtputc(fmt, 'x');
					fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
				}
			}
			else if (c >= 127) {
				if (n == 1 && c == Runeerror)
				{
					// output \uFFFD + hex-encoded bad char:
					char buf[10];
					int l = fz_runetochar(buf, c);
					for (int i = 0; i < l; ++i)
						fmtputc(fmt, buf[i]);

					c = (unsigned char)data[0];
					if ((mode & FPBO_JSON_MODE) || !(flags & PDF_PRINT_JSON_ILLEGAL_UNICODE_AS_HEX)) {
						// some JSON parsers don't accept \xNN encodings, only \u00XX
						fmtputs(fmt, "\\u00");
						fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
						fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
					}
					else {
						fmtputc(fmt, '\\');
						fmtputc(fmt, 'x');
						fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
						fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
					}
				}
				else if (mode & FPBO_VERBATIM_UNICODE)
				{
					for (int i = 0; i < n; ++i)
						fmtputc(fmt, (unsigned char)data[i]);
				}
				else if (c < 0x10000)
				{
					fmtputc(fmt, '\\');
					fmtputc(fmt, 'u');
					fmtputc(fmt, fz_hex_digits[(c >> 12) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c >> 8) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
				}
				else
				{
					fmtputc(fmt, '\\');
					fmtputc(fmt, 'u');
					fmtputc(fmt, fz_hex_digits[(c >> 20) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c >> 16) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c >> 12) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c >> 8) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c >> 4) & 0x0F]);
					fmtputc(fmt, fz_hex_digits[(c) & 0x0F]);
				}
			}
			else
			{
				fmtputc(fmt, c);
			}
			break;
		case '"':
			fmtputs(fmt, ctrl_tbl[0]);
			break;
		case '\\':
			fmtputs(fmt, ctrl_tbl[1]);
			break;
		case '\b':
			fmtputs(fmt, ctrl_tbl[2]);
			break;
		case '\f':
			fmtputs(fmt, ctrl_tbl[3]);
			break;
		case '\n':
			fmtputs(fmt, ctrl_tbl[4]);
			break;
		case '\r':
			fmtputs(fmt, ctrl_tbl[5]);
			break;
		case '\t':
			fmtputs(fmt, ctrl_tbl[6]);
			break;
//		case '\v':
//			fmtputs(fmt, ctrl_tbl[7]);
//			break;
		}
		datalen -= n;
		data += n;
	}

	if (mode & FPBO_JSON_MODE)
		fmtputc(fmt, '"');
}

void
fz_format_string(fz_context *ctx, void *user, void (*emit)(fz_context *ctx, void *user, int c), const char *fmt, va_list args)
{
	struct fmtbuf out;
	int c, s, z, p, w, l, j, q, hexprefix;
	const char *comma;
	size_t bits;

	out.ctx = ctx;
	out.user = user;
	out.emit = emit;

	while ((c = *fmt++) != 0)
	{
		if (c == '%')
		{
			const char* start = fmt;
			q = 0;
			s = 0;
			l = 0;
			z = ' ';

			/* sign/justification flags */
			while ((c = *fmt++) != 0)
			{
				/* plus sign */
				if (c == '+')
					s = 1;
				/* space sign */
				else if (c == ' ')
					s = ' ';
				/* zero padding */
				else if (c == '0')
					z = '0';
				/* comma separators */
				else if (c == '\'')
					q = '\'';
				else if (c == ',')
					q = ',';
				else if (c == '_')
					q = '_';
				/* '-' to left justify */
				else if (c == '-')
					l = 1;
				else
					break;
			}
			if (c == 0)
				goto not_a_decent_format_spec;

			/* width */
			w = INT_MIN;		// flag as 'uninitialized'; different default for floats and strings will be applied later.
			if (c == '*') {
				c = *fmt++;
				w = va_arg(args, int);
			} else {
				while (c >= '0' && c <= '9') {
					w = w * 10 + c - '0';
					c = *fmt++;
				}
			}
			if (c == 0)
				goto not_a_decent_format_spec;

			/* precision */
			p = INT_MAX;		// flag as 'uninitialized'; different default for floats and strings' precision will be applied later.
			if (c == '.') {
				c = *fmt++;
				if (c == 0)
					goto not_a_decent_format_spec;
				if (c == '*') {
					c = *fmt++;
					p = va_arg(args, int);
				} else {
					if (c >= '0' && c <= '9')
						p = 0;
					while (c >= '0' && c <= '9') {
						p = p * 10 + c - '0';
						c = *fmt++;
					}
				}
			}
			if (c == 0)
				goto not_a_decent_format_spec;

			/* misc flags */
			j = 0;
			comma = " ";
			/* ',' for comma separator in R,M,... */
			if (c == ',') {
				c = *fmt++;
				comma = ", ";
			}
			/* JSON-only */
			if (c == 'j') {
				c = *fmt++;
				j = 1;
			}

			/* lengths */
			bits = 0;
			if (c == 'l') {
				c = *fmt++;
				bits = sizeof(int64_t) * 8;
				if (c == 'l') {
					c = *fmt++;
				}
				if (c == 0)
					goto not_a_decent_format_spec;
			}
			else if (c == 't') {
				c = *fmt++;
				bits = sizeof(ptrdiff_t) * 8;
				if (c == 0)
					goto not_a_decent_format_spec;
			}
			else if (c == 'z') {
				c = *fmt++;
				bits = sizeof(size_t) * 8;
				if (c == 0)
					goto not_a_decent_format_spec;
			}
			// I64d, I32d, I16d
			else if (c == 'I') {
				c = *fmt++;
				while (c >= '0' && c <= '9') {
					bits = bits * 10 + c - '0';
					c = *fmt++;
				}
				if (bits != 64 && bits != 32 && bits != 16)
					goto not_a_decent_format_spec;
			}

			hexprefix = 0;

			switch (c) {
			default:
				goto not_a_decent_format_spec;

			case '%':
				fmtputc(&out, '%');
				break;

			case 'M':
				if (j)
					fmtputc(&out, '"');
				{
					fz_matrix* matrix = va_arg(args, fz_matrix*);
					fmtfloat(&out, matrix->a); fmtputs(&out, comma);
					fmtfloat(&out, matrix->b); fmtputs(&out, comma);
					fmtfloat(&out, matrix->c); fmtputs(&out, comma);
					fmtfloat(&out, matrix->d); fmtputs(&out, comma);
					fmtfloat(&out, matrix->e); fmtputs(&out, comma);
					fmtfloat(&out, matrix->f);
				}
				if (j)
					fmtputc(&out, '"');
				break;

			case 'R':
				if (j)
					fmtputc(&out, '"');
				{
					fz_rect* rect = va_arg(args, fz_rect*);
					fmtfloat(&out, rect->x0); fmtputs(&out, comma);
					fmtfloat(&out, rect->y0); fmtputs(&out, comma);
					fmtfloat(&out, rect->x1); fmtputs(&out, comma);
					fmtfloat(&out, rect->y1);
				}
				if (j)
					fmtputc(&out, '"');
				break;

			case 'P':
				if (j)
					fmtputc(&out, '"');
				{
					fz_point* point = va_arg(args, fz_point*);
					fmtfloat(&out, point->x); fmtputs(&out, comma);
					fmtfloat(&out, point->y);
				}
				if (j)
					fmtputc(&out, '"');
				break;

			case 'Z':
				if (j)
					fmtputc(&out, '"');
				{
					fz_quad* quad = va_arg(args, fz_quad*);
					fmtfloat(&out, quad->ul.x); fmtputs(&out, comma);
					fmtfloat(&out, quad->ul.y); fmtputs(&out, comma);
					fmtfloat(&out, quad->ur.x); fmtputs(&out, comma);
					fmtfloat(&out, quad->ur.y); fmtputs(&out, comma);
					fmtfloat(&out, quad->ll.x); fmtputs(&out, comma);
					fmtfloat(&out, quad->ll.y); fmtputs(&out, comma);
					fmtfloat(&out, quad->lr.x); fmtputs(&out, comma);
					fmtfloat(&out, quad->lr.y);
				}
				if (j)
					fmtputc(&out, '"');
				break;

			case 'T':
			{
				int64_t tv = va_arg(args, int64_t);
				char sbuf[40];
				time_t secs = tv;
#ifdef _POSIX_SOURCE
				struct tm tmbuf, * tm = gmtime_r(&secs, &tmbuf);
#else
				struct tm* tm = gmtime(&secs);
#endif

				const char* str = sbuf;
				if (tv < 0 || !tm || !strftime(sbuf, nelem(sbuf), "D:%Y-%m-%d %H:%M:%S UTC", tm))
					str = "(invalid)";
				if (j)
					fmtquote(&out, str, strlen(str), '"', '"', 0, 0);
				else
					while ((c = *str++) != 0)
						fmtputc(&out, c);
			}
			break;

			case 'H':
			{
				const char* ptr = (const char*)va_arg(args, void*);
				size_t seglen = va_arg(args, size_t);
				// when precision has been specified, but is NEGATIVE, than this is a special mode:
				// discover how to best print the data buffer:
				if (p < 0) {
					fmt_print_buffer_optimally(ctx, &out, ptr, seglen, -p, (j ? FPBO_JSON_MODE : 0) | FPBO_VERBATIM_UNICODE);
				}
				else {
					fmt_print_buffer_as_hex(&out, ptr, seglen, p, (j ? FPBO_JSON_MODE : 0));
				}
			}
			break;

			case 'C': /* unicode char */
				c = va_arg(args, int);
				if (j)
					fmtputc(&out, '"');

				// when precision has been specified, but is NEGATIVE, than this is a special mode:
				// repeat the character ABS(p) times:
				if (p < 0)
					p = -p;
				else
					p = 1;

				if (c >= 0 && c < 128)
				{
					while (p-- > 0)
					{
						fmtputc(&out, c);
					}
				}
				else
				{
					char buf[10];
					int i, n = fz_runetochar(buf, c);
					while (p-- > 0)
					{
						for (i = 0; i < n; ++i)
							fmtputc(&out, buf[i]);
					}
				}

				if (j)
					fmtputc(&out, '"');
				break;

			case 'c':
				c = va_arg(args, int);
				if (j)
					fmtputc(&out, '"');

				// when precision has been specified, but is NEGATIVE, than this is a special mode:
				// repeat the character ABS(p) times:
				if (p < 0)
					p = -p;
				else
					p = 1;

				while (p-- > 0)
				{
					fmtputc(&out, c);
				}

				if (j)
					fmtputc(&out, '"');
				break;

			case 'e':
			case 'E':
			case 'f':
			case 'F':
				if (j)
					fmtputc(&out, '"');
				fmtfloat_e(&out, va_arg(args, double), w, p, c);
				if (j)
					fmtputc(&out, '"');
				break;

			case 'g':
			case 'G':
				if (j)
					fmtputc(&out, '"');
				fmtfloat(&out, va_arg(args, double));
				if (j)
					fmtputc(&out, '"');
				break;

			case 'p':
				bits = 8 * sizeof(void *);
				z = '0';
				// q = 0;
				hexprefix = 1;
				/* fallthrough */
			case 'x':
			case 'X':
				if (j)
					fmtputc(&out, '"');
				if (hexprefix)
				{
					fmtputc(&out, '0');
					fmtputc(&out, 'x');
				}
				if (bits == 64)
				{
					uint64_t i64 = va_arg(args, uint64_t);
					fmtuint64(&out, i64, 0, z, w, c == 'X' ? -16 : 16, q);
				}
				else
				{
					unsigned int iv;
					ASSERT0(sizeof(unsigned int) == sizeof(uint32_t));

					if (bits == 16)
					{
						iv = va_arg(args, uint16_t);
					}
					else
					{
						iv = va_arg(args, unsigned int);
					}

					fmtuint32(&out, iv, 0, z, w, c == 'X' ? -16 : 16, q);
				}
				if (j)
					fmtputc(&out, '"');
				break;

			case 'd':
			case 'i':
				if (j)
					fmtputc(&out, '"');
				if (bits == 64)
				{
					int64_t i64 = va_arg(args, int64_t);
					fmtint64(&out, i64, s, z, w, 10, q);
				}
				else
				{
					int iv;
					ASSERT0(sizeof(int) == sizeof(int32_t));

					if (bits == 16)
					{
						iv = va_arg(args, int16_t);
					}
					else
					{
						iv = va_arg(args, int);
					}

					fmtint32(&out, iv, s, z, w, 10, q);
				}
				if (j)
					fmtputc(&out, '"');
				break;

			case 'u':
				if (j)
					fmtputc(&out, '"');
				if (bits == 64)
				{
					uint64_t u64 = va_arg(args, uint64_t);
					fmtuint64(&out, u64, 0, z, w, 10, q);
				}
				else
				{
					unsigned int iv;
					ASSERT0(sizeof(unsigned int) == sizeof(uint32_t));

					if (bits == 16)
					{
						iv = va_arg(args, uint16_t);
					}
					else
					{
						iv = va_arg(args, unsigned int);
					}

					fmtuint32(&out, iv, 0, z, w, 10, q);
				}
				if (j)
					fmtputc(&out, '"');
				break;

			case 'B':
				if (j)
					fmtputc(&out, '"');
				if (bits == 64)
				{
					uint64_t u64 = va_arg(args, uint64_t);
					fmtuint64(&out, u64, 0, z, w, 2, q);
				}
				else
				{
					unsigned int iv;
					ASSERT0(sizeof(unsigned int) == sizeof(uint32_t));

					if (bits == 16)
					{
						iv = va_arg(args, uint16_t);
					}
					else
					{
						iv = va_arg(args, unsigned int);
					}

					fmtuint32(&out, iv, 0, z, w, 2, q);
				}
				if (j)
					fmtputc(&out, '"');
				break;

#if FZ_ENABLE_PDF

			case 'O':
				// print pdf_object a.k.a. 'node'
				{
					size_t len = 0;
					pdf_obj* node = va_arg(args, pdf_obj*);
					fz_context* ctx2 = ctx ? ctx : fz_get_global_context();
					const char* str = pdf_sprint_obj_to_json(ctx2, NULL, 1024, &len, node, PDF_PRINT_RESOLVE_ALL_INDIRECT | PDF_PRINT_LIMITED_ARRAY_DUMP | PDF_PRINT_JSON_BINARY_DATA_AS_HEX_PLUS_RAW | PDF_DO_NOT_THROW_ON_CONTENT_PARSE_FAULTS | PDF_PRINT_JSON_DEPTH_LEVEL(4));

					int linecount = w > 1 ? w : 12;
					int truncated = 0;

					const char *nlp = strchr(str, '\n');
					while (--linecount > 0 && nlp)
					{
						nlp = strchr(nlp + 1, '\n');
					}
					if (nlp)
					{
						((char*)nlp)[0] = 0;
						truncated = 1;
					}
					char buf[400];
					fz_strncpy_s(ctx2, buf, str, sizeof(buf));
					// push the truncated marker: if the already trancated output is long enough, it will 'connect':
					size_t offset = sizeof(buf) - sizeof("(...truncated...)");
					size_t slen = strlen(buf);
					if (truncated && slen < offset)
						offset = slen;
					strcpy(buf + offset, "(...truncated...)");

					fz_free(ctx2, str);
					str = buf;

					if (0)
					{
			case 's':
						str = va_arg(args, const char*);
					}

#else

			case 's':
				{
					const char* str = va_arg(args, const char*);

#endif

					// when precision has been specified, but is NEGATIVE, than this is a special mode:
					// discover how to best print the data buffer:
					if (p < 0 && str && *str) {
						fmt_print_buffer_optimally(ctx, &out, str, (w >= 0 ? w : strlen(str)), -p, (j ? FPBO_JSON_MODE : 0) | FPBO_VERBATIM_UNICODE | FPBO_NO_CONTROL_ESCAPES);
					}
					else {
						if (!str) {
							str = "(null)";
							// override the clipping length, a.k.a. 'precision', now: make sure we always print the full "(null)".
							if (p < 6) {
								p = 6;
							}
						}
						size_t slen = strnlen(str, p);
						size_t cliplen = (p != INT_MAX ? p : slen);
						while (cliplen > slen) {
							fmtputc(&out, ' ');
							cliplen--;
						}
						// only need to print string when it's not an empty string.
						// Edge case example:   fz_printf("%.*s", 7, "")
						while (cliplen-- > 0) {
							fmtputc(&out, *str++);
						}
					}
				}
				break;

			case 'S':
				{
					const wchar_t* str = va_arg(args, const wchar_t*);
					// when precision has been specified, but is NEGATIVE, than this is a special mode:
					// discover how to best print the data buffer:
					if (p < 0 && str && *str) {
						fmt_print_buffer_optimally(ctx, &out, (const char*)(void*)str, (w >= 0 ? w : strlen((const char *)(void *)str)), -p, (j ? FPBO_JSON_MODE : 0) | FPBO_VERBATIM_UNICODE | FPBO_NO_CONTROL_ESCAPES);
					}
					else {
						if (!str) {
							str = L"(null)";
							// override the clipping length, a.k.a. 'precision', now: make sure we always print the full "(null)".
							if (p < 6) {
								p = 6;
							}
						}
						size_t slen = wcsnlen(str, p);
						size_t cliplen = (p != INT_MAX ? p : slen);
						while (cliplen > slen) {
							fmtputc(&out, ' ');
							cliplen--;
						}
						// only need to print string when it's not an empty string.
						// Edge case example:   fz_printf("%.*s", 7, "")
						while (cliplen-- > 0) {
							wchar_t c = *str++;
							if (c >= 0 && c < 128)
								fmtputc(&out, c);
							else {
								char buf[10];
								int i, n = fz_runetochar(buf, c);
								for (i = 0; i < n; ++i)
									fmtputc(&out, buf[i]);
							}
						}
					}
				}
				break;

			case 'Q': /* quoted string (with verbatim unicode in UTF8) */
				{
					const char* str = va_arg(args, const char*);
					if (!str) str = "";
					// when precision has been specified, but is NEGATIVE, than this is a special mode:
					// discover how to best print the data buffer:
					if (p < 0) {
						fmt_print_buffer_optimally(ctx, &out, str, (w >= 0 ? w : strlen(str)), -p, FPBO_JSON_MODE | FPBO_VERBATIM_UNICODE);
					}
					else {
						fmtquote(&out, str, (p != INT_MAX ? p : strlen(str)), '"', '"', 1, j);
					}
				}
				break;

			case 'q': /* quoted string */
				{
					const char* str = va_arg(args, const char*);
					if (!str) str = "";
					// when precision has been specified, but is NEGATIVE, than this is a special mode:
					// discover how to best print the data buffer:
					if (p < 0) {
						fmt_print_buffer_optimally(ctx, &out, str, (w >= 0 ? w : strlen(str)), -p, FPBO_JSON_MODE);
					}
					else {
						if (p == INT_MAX)
							p = -1;
						fmtquote(&out, str, (p > 0 ? p : strlen(str)), '"', '"', 0, j);
					}
				}
				break;

			case '(': /* pdf string */
				{
					const char* str = va_arg(args, const char*);
					if (!str) str = "";
					fmtquote_pdf(&out, str, '(', ')');
				}
				break;

			case 'n': /* pdf name */
				{
					const char* str = va_arg(args, const char*);
					if (j)
						fmtputc(&out, '"');
					if (!str)
						str = "";
					fmtname(&out, str);
					if (j)
						fmtputc(&out, '"');
				}
				break;
			}
			continue;

not_a_decent_format_spec:
			fmt = start;
			fmtputc(&out, '%');
		}
		else
		{
			fmtputc(&out, c);
		}
	}
}

struct snprintf_buffer
{
	char *p;
	size_t s, n;
};

static void snprintf_emit(fz_context *ctx, void *out_, int c)
{
	struct snprintf_buffer *out = (struct snprintf_buffer *)out_;
	if (out->n < out->s)
		out->p[out->n] = c;
	++(out->n);
}

size_t
fz_vsnprintf(char *buffer, size_t space, const char *fmt, va_list args)
{
	struct snprintf_buffer out;
	out.p = buffer;
	out.s = space > 0 ? space - 1 : 0;
	out.n = 0;

	/* Note: using a NULL context is safe here */
	fz_format_string(NULL, &out, snprintf_emit, fmt, args);
	if (space > 0)
		out.p[out.n < space ? out.n : space - 1] = '\0';

	return out.n;
}

size_t
fz_snprintf(char *buffer, size_t space, const char *fmt, ...)
{
	va_list ap;
	struct snprintf_buffer out;
	out.p = buffer;
	out.s = space > 0 ? space - 1 : 0;
	out.n = 0;

	va_start(ap, fmt);
	/* Note: using a NULL context is safe here */
	fz_format_string(NULL, &out, snprintf_emit, fmt, ap);
	if (space > 0)
		out.p[out.n < space ? out.n : space - 1] = '\0';
	va_end(ap);

	return out.n;
}

char *
fz_asprintf(fz_context *ctx, const char *fmt, ...)
{
	size_t len;
	char *mem;
	va_list ap;
	va_start(ap, fmt);
	len = fz_vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	mem = Memento_label(fz_malloc(ctx, len+1), "asprintf");
	va_start(ap, fmt);
	fz_vsnprintf(mem, len+1, fmt, ap);
	va_end(ap);
	return mem;
}
