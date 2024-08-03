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

#ifndef MUPDF_FITZ_STRING_H
#define MUPDF_FITZ_STRING_H

#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The Unicode character used to incoming character whose value is
 * unknown or unrepresentable. */
#define FZ_REPLACEMENT_CHARACTER 0xFFFD

/**
	Safe string functions
*/

/**
	Return strlen(s), if that is less than maxlen, or maxlen if
	there is no null byte ('\0') among the first maxlen bytes.
*/
size_t fz_strnlen(const char *s, size_t maxlen);

/**
	Given a pointer to a C string (or a pointer to NULL) break
	it at the first occurrence of a delimiter char (from a given
	set).

	stringp: Pointer to a C string pointer (or NULL). Updated on
	exit to point to the first char of the string after the
	delimiter that was found. The string pointed to by stringp will
	be corrupted by this call (as the found delimiter will be
	overwritten by 0).

	delim: A C string of acceptable delimiter characters.

	Returns a pointer to a C string containing the chars of stringp
	up to the first delimiter char (or the end of the string), or
	NULL.
*/
char *fz_strsep(char **stringp, const char *delim);

/**
	Copy at most n-1 chars of a string into a destination
	buffer with null termination, returning the real length of the
	initial string (excluding terminator).

	dst: Destination buffer, at least n bytes long.

	src: C string (non-NULL).

	n: Size of dst buffer in bytes.

	Returns the length (excluding terminator) of src.
*/
size_t fz_strlcpy(char *dst, const char *src, size_t n);

/**
	Concatenate 2 strings, with a maximum length.

	dst: pointer to first string in a buffer of n bytes.

	src: pointer to string to concatenate.

	n: Size (in bytes) of buffer that dst is in.

	Returns the real length that a concatenated dst + src would have
	been (not including terminator).
*/
size_t fz_strlcat(char *dst, const char *src, size_t n);

/**
	Copy `src` string to limited-length `dst`. "Safe string copy to limited-length buffer."

	This is similar to regular RTL strncpy() with these differences:

	- `fz_strncpy_s()` DOES NOT pad `dst` with surplus NUL bytes
	  after the string-sentinel (NUL).
	- no more than `dstsiz` characters in `src` will be accessed
	  at any time, hence you can legally feed this function a
	  non-NUL-terminated `src` string, IFF the legal-filled
	  length of `src` is AT LEAST `dstsiz` characters.
	  This feature implies that this function can also be used
	  to extract strings from known-width/fixed-width fields
	  in otherwise arbitrary binary input.
	- `src` and `dst` may overlap.
	- `dst` is guaranteed to be NUL-terminated.
	- a zero-length `dstsiz` will throw an exception as that is
	  considered an illegal input parameter value.
	  If your code chances feeding this function a zero-length
	  `dstsiz`, check for that condition beforehand if you do
	  not wish the exception to be thrown.

	Almost no-one needs this, but in case you need to detect
	whether this function DID limit the copy size to `dstsiz-1`,
	you can achieve this by first setting `dst[dstsiz - 1]` to
	an arbitrary non-zero value before you invoke this function,
	then on return from this call check if that byte value
	has been altered to become NUL: that means the function used
	all of `dstsiz` and can be construed as 'clipped copy' IFF
	you chose your `dstsiz` appropriately, e.g.:

	```
	// input is expected to never be longer than 20 chars:
	char buf[20 + 2];          // !!! +2: +1 for regular NUL sentinel, +1 for 'overflow detect'

	// mark the end byte:
	buf[sizeof(buf)-1] = 1;    

	fz_strncpy_s(buf, src, sizeof(buf));

	// check the end byte
	if (!buf[sizeof(buf)-1])   
		B0RK_B0RK_B0RK_report_clipped_src();
	```
*/
void fz_strncpy_s(fz_context* ctx, char* dst, const char* src, size_t dstsiz);

/**
	Concatenate `src` string to limited-length `dst`. "Safe string concatenation to limited-length buffer."

	This is similar to regular RTL strncat() with the same differences as mentioned for `fz_strncpy_s()`.
*/
void fz_strncat_s(fz_context* ctx, char* dst, const char* src, size_t dstsiz);

/**
	Find the start of the first occurrence of the substring needle in haystack.
*/
void *fz_memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen);

/**
	extract the directory component from a path.
*/
void fz_dirname(char *dir, const char *path, size_t dirsize);

/**
    Skip directory/(Windows)drive separators.

    Returns pointer to basename part in the input string.
*/
const char* fz_basename(const char* path);

/**
	Returns pointer to the filename extension, *including the leading '.' dot*, in the input string.

	Returns the empty string (reference to the *end* of the input string, in fact) when
	the filename does not have a file name extension.

	Note: this routine handles 'dotfiles', a.k.a. 'hidden files' for UNIX systems, across
	all platforms: those files won't report a file extension -- unless they have one, e.g.
	'.dotfile.org' --> extension = '.org'.
*/
const char* fz_name_extension(const char* path);

/**
	Like fz_decode_uri_component but in-place.
*/
char *fz_urldecode(char *url);

/**
 * Return a new string representing the unencoded version of the given URI.
 * This decodes all escape sequences except those that would result in a reserved
 * character that are part of the URI syntax (; / ? : @ & = + $ , #).
 */
char *fz_decode_uri(fz_context *ctx, const char *s);

/**
 * Return a new string representing the unencoded version of the given URI component.
 * This decodes all escape sequences!
 */
char *fz_decode_uri_component(fz_context *ctx, const char *s);

/**
 * Return a new string representing the provided string encoded as a URI.
 */
char *fz_encode_uri(fz_context *ctx, const char *s);

/**
 * Return a new string representing the provided string encoded as an URI component.
 * This also encodes the special reserved characters (; / ? : @ & = + $ , #).
 */
char *fz_encode_uri_component(fz_context *ctx, const char *s);

/**
 * Return a new string representing the provided string encoded as an URI path name.
 * This also encodes the special reserved characters except /.
 */
char *fz_encode_uri_pathname(fz_context *ctx, const char *s);

/**
	create output file name using a template.

	If the path contains %[+-][0-9]*d, every such pattern will be
	replaced with the page number. If the template does not contain
	such a pattern, the page number will be inserted before the
	filename extension. If the template does not have a filename
	extension, the page number will be added to the end.
*/
void fz_format_output_path(fz_context *ctx, char *path, size_t size, const char *fmt, int page);

/**
	create output file name using a template.

	Same as `fz_format_output_path()`, with these additions:

	(IFF the specified chapter number is non-zero, otherwise the next rule is skipped!)

	- if the path contains `$`, every such pattern will be
	  replaced with the chapter number. If the template does not
	  contain any `$`, the chapter number will be inserted before the
	  filename extension, preceded by a `-` dash.
	  If the template does not have a filename extension,
	  the chapter number will be added to the end.

	  When a chapter number is inserted like that, i.e. when not replacing a `$` marker,
	  then it will be separated from the page number by an extra `-` dash.

	(IFF the specified sequence number is non-zero, otherwise the next rule is skipped!)

	- if the path contains `^`, every such pattern will be
	  replaced with the sequence number. If the template does not
	  contain any `^`, the sequence number will be inserted before the
	  filename extension, preceded by a `-` dash.
	  If the template does not have a filename extension,
	  the sequence number will be added to the end.

	(IFF the specified label is non-empty/NULL, otherwise the next rule is skipped!)

	- if the path contains `!`, every such pattern will be
	  replaced with the label text. If the template does not
	  contain any `!`, the label text will be inserted before the
	  filename extension, preceded by a `-` dash.
	  If the template does not have a filename extension,
	  the label text will be added to the end.

	(IFF the specified extension is non-empty/NULL, otherwise the next rule is skipped!)

	- The specified extension will be appended to the template.
	  The code will make sure the extension is always preceded by a `.`, unless
	  the template ends with a directory separator, in which case the filename `____`
	  will be assumed and appended first: this is done to prevent creating
	  'UNIX dot files' *inadvertently*.

	  If the template already includes a file extension AND that extension matches
	  the specified extension *exactly* (case-sensitive!), then the extension in the
	  template is stripped off, and the appended at the tail end again, i.e. the
	  extension is kept intact when it matches exactly.

	  When it DOES NOT (or does not exist) the next rule applies:

	  If the template already includes a file extension, than that extension is
	  kept as-is, but its leading dot will be replaced by a `-` dash.
	  We keep the existing template extension like that to help applications which
	  output many templated files for various source file templates which *may*
	  only differ in their source extension part.

	  The given extension is then appended at the end to complete the generated file name/path.

	(OTHERWISE:)

	- The filename extension (if any) is stripped off the template and appended
	  at the end of the generated filename/path: this ensures the template dictates
	  the file extension as you'ld expect when no override has been specified.

	Also note that a zero(0) page number will be treated as yet another part to skip.
	This allows application code to re-use this API to apply arbitrary non-zero numbers,
	e.g. coordinates, to the filename template instead.

	Hence, when appending all these parts, the filename will look like this:

	    <TEMPLATEFILENAME>-<CHAPTER>-<PAGE>-<SEQUENCENUMBER>-<LABEL>.<EXTENSION>
*/
void fz_format_output_path_ex(fz_context* ctx, char* path, size_t size, const char* fmt, int chapter, int page, int sequence_number, const char *label, const char *extension);

/**
	rewrite path to the shortest string that names the same path.

	Eliminates multiple and trailing slashes, interprets "." and
	"..". Overwrites the string in place.
*/
char *fz_cleanname(char *name);

/**
	rewrite path to the shortest string that names the same path.

	Eliminates multiple and trailing slashes, interprets "." and
	"..". Allocates a new string that the caller must free.
*/
char *fz_cleanname_strdup(fz_context *ctx, const char *name);

/**
	Resolve a path to an absolute file name.
	The resolved path buffer must be of at least PATH_MAX size.
*/
char *fz_realpath(const char *path, char resolved_path[PATH_MAX]);

/**
	Case insensitive (ASCII only) string comparison.
*/
int fz_strcasecmp(const char *a, const char *b);
int fz_strncasecmp(const char *a, const char *b, size_t n);

/**
	Case insensitive (ASCII only) string-in-string search.
*/
char *fz_strcasestr(char *str, const char *substr);

/**
	Return the length of the string until the last occurrence of any of the characters in the set.
	Consequently, this API returns the length of the string when none of the characters in the set were found.

	Hence this is the `strr*` scan-from-the-right-towards-the-left variant of `strcspn()`.
*/
size_t fz_strrcspn(const char *str, const char *set);

/**
	Return TRUE when the given string contains a `%d`, '#', '!', '$' or '^' page/sequence format specifier for use with `fz_format_output_path_ex()`.
*/
int fz_has_page_format_marker(const char* s);

/**
	FZ_UTFMAX: Maximum number of bytes in a decoded rune (maximum
	length returned by fz_chartorune).
*/
enum { FZ_UTFMAX = 4 };

/**
	UTF8 decode a single rune from a sequence of chars.

	rune: Pointer to an int to assign the decoded 'rune' to.

	str: Pointer to a UTF8 encoded string.

	Returns the number of bytes consumed.
*/
int fz_chartorune(int *rune, const char *str, size_t n);

/*
	'Unsafe' version of chars->unicode point: this one assumes your
	input string is properly NUL terminated, which will help it detect
	invalid UTF8 inputs.
*/
static inline int
fz_chartorune_unsafe(int* rune, const char* str)
{
	return fz_chartorune(rune, str, 7 /* arbitrary */);
}

/**
	UTF8 encode a rune to a sequence of chars.

	str: Pointer to a place to put the UTF8 encoded character.

	rune: Pointer to a 'rune'.

	Returns the number of bytes the rune took to output.
*/
int fz_runetochar(char *str, int rune);

/**
	Count how many chars are required to represent a rune.

	rune: The rune to encode.

	Returns the number of bytes required to represent this run in
	UTF8.
*/
int fz_runelen(int rune);

/**
	Compute the index of a rune in a string.

	str: Pointer to beginning of a string.

	p: Pointer to a char in str.

	Returns the index of the rune pointed to by p in str.
*/
int fz_runeidx(const char *str, const char *p);

/**
	Obtain a pointer to the char representing the rune
	at a given index.

	str: Pointer to beginning of a string.

	idx: Index of a rune to return a char pointer to.

	Returns a pointer to the char where the desired rune starts,
	or NULL if the string ends before the index is reached.
*/
const char *fz_runeptr(const char *str, int idx);

/**
	Count how many runes the UTF-8 encoded string
	consists of.

	s: The UTF-8 encoded, NUL-terminated text string.

	Returns the number of runes in the string.
*/
int fz_utflen(const char *s);

/**
	Convert a wchar string into a new heap allocated utf8 one.
*/
char *fz_utf8_from_wchar(fz_context *ctx, const wchar_t *s);

/*
	Convert a utf8 string into a new heap allocated wchar one.
*/
wchar_t *fz_wchar_from_utf8(fz_context *ctx, const char *path);


/**
	Locale-independent decimal to binary conversion. On overflow
	return (-)INFINITY and set errno to ERANGE. On underflow return
	0 and set errno to ERANGE. Special inputs (case insensitive):
	"NAN", "INF" or "INFINITY".
*/
float fz_strtof(const char *s, char **es);

/**
	Compute decimal integer m, exp such that:
		f = m * 10^exp
		m is as short as possible without losing exactness
	Assumes special cases (0, NaN, +Inf, -Inf) have been handled.
*/
int fz_grisu(float f, char s[100], int *exp);

/**
	Check and parse string into page ranges:
		/,?(-?\d+|N)(-(-?\d+|N))?/
*/
int fz_is_page_range(fz_context *ctx, const char *s);
const char *fz_parse_page_range(fz_context *ctx, const char *s, int *a, int *b, int n);

/**
	Unicode aware tolower and toupper functions.
*/
int fz_tolower(int c);
int fz_toupper(int c);

#ifdef __cplusplus
}
#endif

#endif
