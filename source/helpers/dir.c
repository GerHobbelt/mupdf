
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/system-header-files.h"
#include "utf.h"

#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <sys/stat.h>


/**
 * Normalize a given path, i.e. resolve the ./ and ../ directories that may be part of it.
 * Also UNIXify the path by replacing \ backslashes with / slashes, which work on all platforms.
 *
 * When path is NULL, the path is assumed to already be present in the dstpath buffer and
 * will be modified in place.
 *
 * Throws an exception when the path is buggy, e.g. contains a ../ which cannot be resolved,
 * e.g. C:\..\b0rk
 */
void fz_normalize_path(fz_context* ctx, char* dstpath, size_t dstpath_bufsize, const char* path)
{
	// as we normalize a path, it can only become *shorter* (or equal in length).
	// Thus we copy the source path to the buffer verbatim first.
	if (!dstpath)
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_normalize_path: dstpath cannot be NULL.");
	// copy source path, if it isn't already in the work buffer:
	size_t len;
	if (path) {
		len = strlen(path);
		if (dstpath_bufsize < len + 1)
			fz_throw(ctx, FZ_ERROR_GENERIC, "fz_normalize_path: buffer overrun.");
		if (path != dstpath)
			memmove(dstpath, path, len + 1);
	}
	else {
		len = strlen(dstpath);
	}

	// unixify MSDOS path:
	char* e = strchr(dstpath, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e, '\\');
	}

	int can_have_mswin_drive_letter = 1;

	// Sanitize the *entire* path, including the Windows Drive/Share part:
	e = dstpath;
	if (e[0] == '/' && e[1] == '/' && strchr(".?", e[2]) != NULL && e[3] == '/')
	{
		// skip //?/ and //./ UNC leaders
		e += 4;
	}
	else if (e[0] == '/' && e[1] == '/')
	{
		// skip //<server>... UNC path starter (which cannot contain Windows drive letters as-is)
		e += 2;
		can_have_mswin_drive_letter = 0;
	}

	if (can_have_mswin_drive_letter)
	{
		// See if we have a Windows drive as part of the path: keep that one intact!
		if (isalpha(e[0]) && e[1] == ':')
		{
			*e = toupper(e[0]);
			e += 2;
		}
	}

	char *start = e;

	// now find ./ and ../ directories and resolve each, if possible:
	char *p = start;
	e = strchr(*p ? p + 1 : p, '/');  // skip root /, if it (may) exist
	while (e)
	{
		if (e == p) {
			// matched an extra / (as in the second / of  '//'): that one can be killed
			//memmove(p, e + 1, len + 1 - (e + 1 - dstpath));  -->
			memmove(p, e + 1, len - (e - dstpath));
			len -= e + 1 - p;
		}
		else if (strncmp(p, ".", e - p) == 0) {
			// matched a ./ directory: that one can be killed
			//memmove(p, e + 1, len + 1 - (e + 1 - dstpath));  -->
			memmove(p, e + 1, len - (e - dstpath));
			len -= e + 1 - p;
		}
		else if (strncmp(p, "..", e - p) == 0) {
			// matched a ../ directory: that can only be killed when there's a parent available
			// which is not itself already a ../
			if (p - 2 >= start) {
				// scan back to previous /
				p -= 2;
				while (p > start && p[-1] != '/')
					p--;
				// make sure we have backtracked over a directory that is not itself named ../
				if (strncmp(p, "../", 3) != 0) {
					//memmove(p, e + 1, len + 1 - (e + 1 - dstpath));  -->
					memmove(p, e + 1, len - (e - dstpath));
					len -= e + 1 - p;
				}
				else {
					// otherwise, we're stuck with this ../ that we currently have.
					p = e + 1;
				}
			}
			else {
				// no parent available at all...
				// hence we're stuck with this ../ that we currently have.
				p = e + 1;
			}
		}
		else {
			// we now have a directory that's not ./ nor ../
			// hence keep as is
			p = e + 1;
		}
		e = strchr(p, '/');
	}

	// we now have the special case, where the path ends with a directory named . or .., which does not come with a trailing /
	if (strcmp(p, ".") == 0) {
		// matched a ./ directory: that one can be killed
		if (p - 1 > start) {
			// keep this behaviour of no / at end (unless we were processing the fringe case '/.'):
			p--;
		}
		*p = 0;
	}
	else if (strcmp(p, "..") == 0) {
		// matched a ../ directory: that can only be killed when there's a parent available
		// which is not itself already a ../
		if (p - 2 >= start) {
			// scan back to previous /
			p -= 2;
			while (p > start && p[-1] != '/')
				p--;
			// make sure we have backtracked over a directory that is not itself named ../
			if (strncmp(p, "../", 3) != 0) {
				if (p - 1 > start) {
					// keep this behaviour of no / at end (unless we were processing the fringe case '/..', which is illegal BTW):
					p--;
				}
				else if (p - 1 == start) {
					fz_throw(ctx, FZ_ERROR_GENERIC, "fz_normalize_path: illegal /.. path.");
				}
				*p = 0;
			}
			// otherwise, we're stuck with this ../ that we currently have.
		}
		// else: no parent available at all...
		// hence we're stuck with this ../ that we currently have.
	}
}

/**
 * Sanitize a given path, i.e. replace any "illegal" characters in the path, using generic
 * OS/filesystem heuristics. "Illegal" characters are replaced with an _ underscore.
 *
 * When path is NULL, the path is assumed to already be present in the dstpath buffer and
 * will be modified in place.
 *
 * The path is assumed to have been normalized already, hence little intermezzos like '/./'
 * shall not exist in the input. IFF they do, they will be sanitized to '/_/'.
 */
void fz_sanitize_path(fz_context* ctx, char* dstpath, size_t dstpath_bufsize, const char* path)
{
	// as we normalize a path, it can only become *shorter* (or equal in length).
	// Thus we copy the source path to the buffer verbatim first.
	if (!dstpath)
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_sanitize_path: dstpath cannot be NULL.");
	// copy source path, if it isn't already in the work buffer:
	size_t len;
	if (path) {
		len = strlen(path);
		if (dstpath_bufsize < len + 1)
			fz_throw(ctx, FZ_ERROR_GENERIC, "fz_sanitize_path: buffer overrun.");
		if (path != dstpath)
			memmove(dstpath, path, len + 1);
	}
	else {
		len = strlen(dstpath);
	}

	// unixify MSDOS/MSWIN/UNC path:
	char* e = strchr(dstpath, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e, '\\');
	}

	int can_have_mswin_drive_letter = 1;

	// Sanitize the *entire* path, including the Windows Drive/Share part:
	e = dstpath;
	if (e[0] == '/' && e[1] == '/' && strchr(".?", e[2]) != NULL && e[3] == '/')
	{
		// skip //?/ and //./ UNC leaders
		e += 4;
	}
	else if (e[0] == '/' && e[1] == '/')
	{
		// skip //<server>... UNC path starter (which cannot contain Windows drive letters as-is)
		e += 2;
		can_have_mswin_drive_letter = 0;
	}
	else if (e[0] == '.' && e[1] == '/')
	{
		// skip ./ relative path at start, replace it by NIL: './' is superfluous!
		len = strlen(e);
		memmove(e, e + 2, len + 1);
		can_have_mswin_drive_letter = 0;
	}
	else if (e[0] == '.' && e[1] == '.' && e[2] == '/')
	{
		// skip any chain of ../ relative path elements at start, as this may be a valid relative path
		e += 3;
		can_have_mswin_drive_letter = 0;

		while (e[0] == '.' && e[1] == '.' && e[2] == '/')
		{
			e += 3;
		}
	}

	if (can_have_mswin_drive_letter)
	{
		// See if we have a Windows drive as part of the path: keep that one intact!
		if (isalpha(e[0]) && e[1] == ':')
		{
			*e = toupper(e[0]);
			e += 2;
		}
	}

	for ( ; *e; e++) {
		int c = *e;
		if (c > 0x7F || c < 0) {
			// 0x80 and higher character codes: UTF8
			int u;
			int l = fz_chartorune_unsafe(&u, e);
			e += l - 1;
			if (l == 1 && u == Runeerror) {
				// bad UTF8 is to be discarded!
				*e = '_';
			}
		}
		else if (c < 32) {
			// control characters are generally not accepted in path or file names!
			*e = '_';
		}
		else if (c == '?' || c == '*') {
			// wildcard characters are generally not accepted in path or file names!
			*e = '_';
		}
		else if (c == ':') {
			// Windows drive delimiter is not allowed anywhere in the path (except at the start, which we checked already before)
			*e = '_';
		}
		else if (c == '.') {
			// a dot at the END of a path element is not accepted; neither do we tolerate ./ and ../ as we have
			// skipped any legal ones already before.
			//
			// we assume the path has been normalized (relative or absolute) before it was sent here, hence
			// we'll accept UNIX dotfiles (one leading '.' dot for an otherwise sane filename/dirname), but
			// otherwise dots can only sit right smack in the middle of an otherwise legal filename/dirname:
			if (!e[1] || e[1] == '.' || e[1] == '/')
				*e = '_';
		}
	}
}


/**
	Replace any path-invalid characters from the input path.
	This API assumes a rather strict rule set for file naming to ensure resulting paths
	are valid on UNIX, Windows and Mac OSX platforms' default file systems (ext2, HFS, NTFS).

	You may additionally specify a set of characters, which should be replaced as well,
	and a replacement string for each occurrence or run of occurrences.

	When `f` is specified as part of the replacement sequence, this implies any `printf`/`fz_format_output_path`
	format string, e.g. `%[+-][0-9]*d`, in its entirety. You may want to specify `%` in the set if you only
	wish to replace/'nuke' the leading `%` in each such format specifier. Use `f` to strip out all `printf`-like
	format parts from a filename/path to make it generically safe to use.

	These additional arguments, `set` and `replace_single` and `replace_sequence`, may be NULL
	or NUL(0), in which case they are not active.

	The `replacement_single` string is used as a map: each character in the `set` is replaced by its corresponding
	character in `replacement_single`. When `replacement_single` is shorter than `set`, any of the later `set` members
	will be replaced by the last `replacement_single` character.

	When `start_at_offset` is not zero (i.e. start-of-string), only the part of the path at & following the
	`start_at_offset` position is sanitized. It is assumed that the non-zero offset ALWAYS points past any critical
	UNC or MSWindows Drive designation parts of the path.

	Overwrites the `path` string in place.
*/
char*
fz_sanitize_path_ex(char* path, const char* set, const char* replace_single, char replace_sequence, size_t start_at_offset)
{
	if (!path)
		return NULL;
	if (!replace_single || !*replace_single)
		replace_single = "_";
	if (!set)
		set = "";

	size_t repl_map_len = strlen(replace_single);
	int has_printf_format_repl_idx1;
	{
		const char* m = strchr(set, 'f');
		if (m)
		{
			has_printf_format_repl_idx1 = m - set;
			// pick last in map when we're out-of-bounds:
			if (has_printf_format_repl_idx1 >= repl_map_len)
				has_printf_format_repl_idx1 = repl_map_len - 1;
			has_printf_format_repl_idx1++;
		}
		else
		{
			has_printf_format_repl_idx1 = 0;
		}
	}

	char* rv_path = path;

	if (start_at_offset)
	{
		size_t l = strlen(path);
		if (l <= start_at_offset)
		{
			// nothing to process!
			return path;
		}

		path += start_at_offset;
	}

	// UNIXify:
	char* e = strchr(path, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e, '\\');
	}

	char* p;
	char* d;

	d = p = path;

	// check if path is a UNC path. It may legally start with `\\.\` or `\\?\` before a Windows drive/share+COLON:
	if (start_at_offset == 0)
	{
		if (*p == '/' && p[1] == '/')
		{
			p += 2;
			// scan legal domain name:
			if (strchr(".?", *p) && p[1] == '/' && p[2] != '/')
			{
				p += 2;    // skip the legal UNC `//./` or `//?/` path starter.
			}
			else
			{
				d = p;
				while (isalnum(*p) || strchr("_-$", *p))
					p++;
				if (p > d && *p == '/' && p[1] != '/')
					p++;
				else
				{
					// no legal UNC domain name, hence no UNC at all:
					p = path;
				}
			}
		}
		// check if path is a (possibly UNC'd) Windows Drive/Share designator: letter(s)+COLON:
		d = p;
		while (isalnum(*p))
			p++;
		if (p > d && *p == ':')
		{
			p++;
			// while users can specify relative paths within drives, e.g. `C:path`, they CANNOT do so when this is a UNC path.
			if (*p != '/' && d > path)
			{
				// not a legal UNC path after all
				p = path;
			}
		}
	}

	// now go and scan/clean the rest of the path spec:
	int repl_seq_count = 0;
	for (d = p; *p; p++)
	{
		if (replace_sequence && repl_seq_count > 1)
		{
			// replace series of replacements with `replace_sequence` char:
			d -= repl_seq_count;
			*d++ = replace_sequence;
			repl_seq_count = 1;
			// ^^^ note that we do this step-by-step this way, slowly eating the
			// replacement series. The benefit of this approach, however, is that
			// we don't have to bother the rest of the code/loop with this,
			// making the code easier to review. Performance is still good enough.
		}

		char c = *p;
		if (c == '/')
		{
			// keep path separators intact at all times.
			*d++ = c;
			repl_seq_count = 0;
			continue;
		}

		// custom set replacements for printf-style format bits, anyone?
		if (has_printf_format_repl_idx1 && c == '%')
		{
			// replace any %[+/-/ ][0-9.*][dlfgespuizxc] with a single replacement character
			p++;
			while (*p && strchr("+- .0123456789*", *p))
				p++;
			if (*p && strchr("lz", *p))
				p++;
			if (*p && strchr("dlfgespuizxc", *p))
				p++;
			p--;

			// custom 1:1 replacement: set -> replace_single.
			*d++ = replace_single[has_printf_format_repl_idx1 - 1];
			repl_seq_count++;
			continue;
		}

		// custom set replacements, anyone?
		const char* m = strchr(set, c);
		if (m)
		{
			// custom replacement for fz_format / printf formatters:
			if (c == '#')
			{
				// replace ream of '#'s with a single replacement character
				p++;
				while (*p == '#')
					p++;
				p--;

				// custom 1:1 replacement: set -> replace_single.
				int idx = m - set;
				// pick last in map when we're out-of-bounds:
				if (idx >= repl_map_len)
					idx = repl_map_len - 1;
				*d++ = replace_single[idx];
				repl_seq_count++;
			}
			else
			{
				// custom 1:1 replacement: set -> replace_single.
				int idx = m - set;
				// pick last in map when we're out-of-bounds:
				if (idx >= repl_map_len)
					idx = repl_map_len - 1;
				*d++ = replace_single[idx];
				repl_seq_count++;
			}
			continue;
		}

		// regular sanitation of the path follows
		//
		// reject ASCII control chars.
		// also reject any non-UTF8-legal byte (the red slots in https://en.wikipedia.org/wiki/UTF-8#Codepage_layout)
		//    if (c < ' ' || c == 0x7F /* DEL */ || strchr("\xC0\xC1\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF", c))
		// in fact, check for *proper UTF8 encoding* and replace all illegal code points:
		if (c > 0x7F || c < 0) {
			// 0x80 and higher character codes: UTF8
			int u;
			int l = fz_chartorune_unsafe(&u, p);
			if (l == 1 && u == Runeerror) {
				// bad UTF8 is to be discarded!
				*d++ = '_';
				repl_seq_count++;
				continue;
			}
			// otherwise keep Unicode UTF8 codepoint:
			for (; l > 0; l--)
				*d++ = *p++;
			p--;
			repl_seq_count = 0;
		}
		else if (c < ' ' || c == 0x7F /* DEL */)
		{
			*d++ = '_';
			repl_seq_count++;
		}
		else if (strchr("`\"*@=|;?:", c))
		{
			// replace NTFS-illegal character path characters.
			// replace some shell-scripting-risky character path characters.
			// replace the usual *wildcards* as well.
			*d++ = '_';
			repl_seq_count++;
		}
		else
		{
			static const char* braces = "{}[]<>";
			const char* b = strchr(braces, c);

			if (b)
			{
				// replace some shell-scripting-risky brace types as well: all braces are transmuted to `()` for safety.
				int idx = b - braces;
				*d++ = "()"[idx % 2];
				repl_seq_count++;
			}
			else
			{
				// we're looking at a plain-as-Jim vanilla character here: pass as-is:
				*d++ = c;
				repl_seq_count = 0;
			}
		}
	}

	// and print the sentinel
	*d = 0;

	return rv_path;
}

// Note:
// both abspaths are assumed to be FILES (not directories): the filename
// at the end of `relative_to_abspath` therefor is not considered; only the
// directory part of that path is compared against.
char* fz_mk_relative_path(fz_context* ctx, char* dst, size_t dstsiz, const char* abspath, const char* relative_to_abspath)
{
	// tactic:
	// - determine common prefix
	// - see what's left and walk up that dirchain, producing '..' elements.
	// - construct the relative path by merging all.
	// When the prefix length is zero or down to drive-root directory (MS Windows)
	// there's not much 'relative' to walk and the result therefor is the abs.path
	// itself.
	size_t len_of_cmp_dirpath = fz_strrcspn(relative_to_abspath, ":/\\");
	size_t len_of_cmp_drivespec = fz_strrcspn(relative_to_abspath, "?:") + 1;   // Match UNC and classic paths both
	size_t common_prefix_len = 0;

	for (size_t pos = 0; pos < len_of_cmp_dirpath + 1; pos++)
	{
		char c1 = abspath[pos];
		char c2 = relative_to_abspath[pos];

		// check for MSWin vs. UNIXy directory separator style mashups before ringing the bell:
		if (c1 == '\\')
			c1 = '/';
		if (c2 == '\\')
			c2 = '/';
		if (c1 != c2)
			break;

		// when we have a match and it's a directory '/', we have a preliminary common_prefix:
		if (c1 == '/')
		{
			common_prefix_len = pos;
		}
	}

	char dotdot[PATH_MAX] = "";
	char* d = dotdot;
	// find out how many segments we have to walk up the dirtree:
	if (relative_to_abspath[common_prefix_len] && len_of_cmp_dirpath > common_prefix_len)
	{
		const char* sep = relative_to_abspath + common_prefix_len;
		do
		{
			*d++ = '.';
			*d++ = '.';
			*d++ = '/';
			sep = strpbrk(sep + 1, "\\/");
		} while (sep);
		*d++ = '.';
		*d++ = '.';
		*d++ = '/';
		*d = 0;
		ASSERT((d - dotdot) < sizeof(dotdot));
	}
	// is this a 'sensible' common prefix, i.e. one where both paths are on the same drive?
	// (We're okay with having to walk the chain all the way back to the drive/root-directory, though...)
	if (common_prefix_len >= len_of_cmp_drivespec)
	{
		const char* remainder = abspath + common_prefix_len;
		if (remainder[0])
		{
			// skip initial '/'
			remainder++;
		}
		// when, despite assumptions, we happen to be looking at the fringe case where `abspath` was
		// a subpath (parent) of `relative_to_abspath`, then we wish to output without a trailing '/'
		// anyway, i.e. a path that only consists of '..' directory elements:
		if (!remainder[0])
		{
			if (d != dotdot)
			{
				// end on a '.', not a '/':
				d[-1] = 0;
			}
		}
		fz_snprintf(dst, dstsiz, "%s%s", dotdot, remainder);
	}
	else
	{
		fz_strncpy_s(ctx, dst, abspath, dstsiz);
	}
	return dst;
}

void fz_mk_absolute_path_using_absolute_base(fz_context* ctx, char* dst, size_t dstsiz, const char* source_path, const char* abs_optional_base_file_path)
{
	ASSERT(source_path);
	ASSERT(abs_optional_base_file_path);

	if (fz_is_absolute_path(source_path))
	{
		fz_strncpy_s(ctx, dst, source_path, dstsiz);
		fz_normalize_path(ctx, dst, dstsiz, dst);
		fz_sanitize_path(ctx, dst, dstsiz, dst);
		return;
	}

	// relative paths are assumed relative to the *base file path*:
	//
	// NOTE: the 'base file path' is assumed to be a FILE path, NOT a DIRECTORY, so we need to get rid of that
	// 'useless' base filename: we do that by appending a '../' element and have the sanitizer deal with it
	// afterwards: the '../' will eat the preceding element, which, in this case, will be the 'base file name'.
	fz_snprintf(dst, dstsiz, "%s/../%s", abs_optional_base_file_path, source_path);
	fz_normalize_path(ctx, dst, dstsiz, dst);
	fz_sanitize_path(ctx, dst, dstsiz, dst);

#if 0 // as abs_optional_base_path is already to be absolute and sanitized, we don't need fz_realpath() in here
	char abspath[PATH_MAX];
	if (!fz_realpath(dst, abspath))
	{
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot process relative (based) file path to a sane absolute path: rel.path: %q, abs.base: %q, dst: %q", source_path, abs_optional_base_file_path, dst);
	}
	fz_normalize_path(ctx, abspath, sizeof(abspath), abspath);
	fz_sanitize_path(ctx, abspath, sizeof(abspath), abspath);
	fz_strncpy_s(ctx, dst, abspath, dstsiz);
#endif
}

int fz_is_absolute_path(const char* path)
{
	// anything absolute is either a UNIX root directory '/', an UNC path or prefix '//yadayada' or a classic MSWindows drive letter.
	return (path && (
		(path[0] == '/') ||
		(path[0] == '\\') ||
		(isalpha(path[0]) && path[1] == ':' && strchr("/\\", path[2]) != NULL)
		));
}

#if defined(_WIN32)

static int fz_UNC_wfullpath_from_name(fz_context* ctx, wchar_t dstbuf[PATH_MAX], const char* path)
{
	wchar_t wpath[PATH_MAX];
	wchar_t wbuf[PATH_MAX + 4];

	if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, PATH_MAX))
	{
		fz_copy_ephemeral_system_error(ctx, GetLastError(), NULL);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}
	if (!GetFullPathNameW(wpath, PATH_MAX, wbuf + 4, NULL))
	{
		fz_copy_ephemeral_system_error(ctx, GetLastError(), NULL);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}
	const wchar_t* fp = wbuf + 4;
	// Is full path an UNC path already? If not, make it so:
	if (wbuf[4] != '\\' && wbuf[4] != '/')
	{
		wbuf[0] = '\\';
		wbuf[1] = '\\';
		wbuf[2] = '?';
		wbuf[3] = '\\';
		wcsncpy(dstbuf, wbuf, PATH_MAX);
	}
	else
	{
		wcsncpy(dstbuf, wbuf + 4, PATH_MAX);
	}
	return 0;
}

#endif

int fz_chdir(fz_context* ctx, const char *path)
{
#ifdef _MSC_VER
	wchar_t wname[PATH_MAX];

	if (fz_UNC_wfullpath_from_name(ctx, wname, path))
	{
		return -1;
	}

	// remove trailing / dir separator, if any...
	size_t len = wcslen(wname);
	if (wname[len - 1] == '\\')
	{
		if (wname[len - 2] != ':')
			wname[len - 1] = 0;
	}

	if (_wchdir(wname))
#else
	if (chdir(path))
#endif
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);

		if (fz_ctx_get_rtl_errno(ctx) == ENOENT)
		{
			fz_freplace_ephemeral_system_error(ctx, 0, "chdir: Unable to locate the directory: %s", path);
		}
		return -1;
	}

	return 0;
}

#if defined(_WIN32)

void fz_mkdir_for_file(fz_context* ctx, const char* path)
{
	char* buf = fz_strdup(ctx, path);
	ASSERT(buf);

	// unixify MSDOS path:
	char* e = strchr(buf, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e, '\\');
	}
	e = strrchr(buf, '/');

	if (e)
	{
		// strip off the *filename*: keep the (nested?) path intact for recursive mkdir:
		*e = 0;

		int rv = fz_mkdirp_utf8(ctx, buf);
		if (rv)
		{
			rv = fz_ctx_get_rtl_errno(ctx);
			if (rv != EEXIST)
			{
				const char* errmsg = fz_ctx_get_system_errormsg(ctx);
				fz_info(ctx, "mkdirp --> mkdir(%s) --> (%d) %s\n", buf, rv, errmsg);
			}
		}
	}

	fz_free(ctx, buf);
}

#else

void fz_mkdir_for_file(fz_context* ctx, const char* path)
{
	char* buf = fz_strdup(ctx, path);
	ASSERT(buf);

	// unixify MSDOS path:
	char *e = strchr(buf, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e, '\\');
	}
	e = strrchr(buf, '/');

	if (e)
	{
		// strip off the *filename*: keep the (nested?) path intact for recursive mkdir:
		*e = 0;

		// construct the path:
		for (e = strchr(buf, '/'); e; e = strchr(e + 1, '/'))
		{
			*e = 0;
			// do not mkdir UNIX or MSDOS/WIN/Network root directory:
			if (e == buf || e[-1] == ':' || e[-1] == '/')
			{
				*e = '/';
				continue;
			}
			// just bluntly attempt to create the directory: we don't care if it fails.
			int rv = mkdir(buf);
			if (rv)
			{
				fz_copy_ephemeral_errno(ctx);
				ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
				rv = fz_ctx_get_rtl_errno(ctx);
				if (rv != EEXIST)
				{
					const char* errmsg = fz_ctx_get_system_errormsg(ctx);
					fz_info(ctx, "mkdirp --> mkdir(%s) --> (%d) %s\n", buf, rv, errmsg);
				}
			}
			*e = '/';
		}
		{
			int rv = mkdir(buf);
			if (rv)
			{
				fz_copy_ephemeral_errno(ctx);
				ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
				rv = fz_ctx_get_rtl_errno(ctx);
				if (rv != EEXIST)
				{
					const char* errmsg = fz_ctx_get_system_errormsg(ctx);
					fz_info(ctx, "mkdirp --> mkdir(%s) --> (%d) %s\n", buf, rv, errmsg);
				}
			}
		}
	}
	fz_free(ctx, buf);
}

#endif



#if defined(_WIN32)

int64_t
fz_stat_ctime(fz_context *ctx, const char* path)
{
	struct _stat info;
	wchar_t wpath[PATH_MAX];

	if (fz_UNC_wfullpath_from_name(ctx, wpath, path))
		return 0;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return 0;
	}

	return info.st_ctime;
}

int64_t
fz_stat_mtime(fz_context* ctx, const char* path)
{
	struct _stat info;
	wchar_t wpath[PATH_MAX];

	if (fz_UNC_wfullpath_from_name(ctx, wpath, path))
		return 0;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return 0;
	}

	return info.st_mtime;
}

#else

int64_t
fz_stat_ctime(fz_context* ctx, const char* path)
{
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return 0;
	}
	return info.st_ctime;
}

int64_t
fz_stat_mtime(fz_context* ctx, const char* path)
{
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return 0;
	}
	return info.st_mtime;
}

#endif /* _WIN32 */


#if defined(_WIN32)

FILE *
fz_fopen_utf8(fz_context* ctx, const char* name, const char* mode)
{
	wchar_t wname[PATH_MAX];
	wchar_t *wmode;
	FILE* file;

	if (fz_UNC_wfullpath_from_name(ctx, wname, name))
	{
		return NULL;
	}

	wmode = fz_wchar_from_utf8(mode);
	if (wmode == NULL)
	{
		return NULL;
	}

	file = _wfopen(wname, wmode);
	if (!file)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
	}

	free(wmode);

	return file;
}

int
fz_remove_utf8(fz_context* ctx, const char* name)
{
	wchar_t wname[PATH_MAX];
	int n;

	if (fz_UNC_wfullpath_from_name(ctx, wname, name))
	{
		return -1;
	}

	n = _wremove(wname);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
	}

	return n;
}

int
fz_mkdirp_utf8(fz_context* ctx, const char* name)
{
	wchar_t wname[PATH_MAX];

	if (fz_UNC_wfullpath_from_name(ctx, wname, name))
	{
		return -1;
	}

	// as this is now an UNC path, we can only mkdir beyond the drive letter:
	wchar_t* p = wname + 4;
	wchar_t* q = wcschr(p, ':');
	if (!q)
		q = p;
	wchar_t* d = wcschr(q, '\\'); // drive rootdir
	if (d)
		d = wcschr(d + 1, '\\'); // first path level
	if (d == NULL)
	{
		// apparently, there's only one directory name above the drive letter specified. We still need to mkdir that one, though.
		d = q + wcslen(q);
	}

	int rv = 0;

	for(;;)
	{
		wchar_t c = *d;
		*d = 0;

		int n = _wmkdir(wname);
		int e = errno;
		if (n && e != EEXIST)
		{
			fz_copy_ephemeral_errno(ctx);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			rv = -1;
		}

		*d = c;
		// did we reach the end of the *original* path spec?
		if (!c)
			break;
		q = wcschr(d + 1, '\\');
		if (q)
			d = q;
		else
			d += wcslen(d);  // make sure the sentinel-patching doesn't damage the last part of the original path spec
	}

	return rv;
}

#else

// TODO: code review re ephemeral errorcode handling

FILE*
fz_fopen_utf8(fz_context* ctx, const char* name, const char* mode)
{
	if (name == NULL)
	{
		errno = EINVAL;
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return NULL;
	}

	if (mode == NULL)
	{
		errno = EINVAL;
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return NULL;
	}

	FILE *rv = fopen(name, mode);
	if (!rv)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return NULL;
	}
	return rv;
}

int
fz_remove_utf8(fz_context* ctx, const char* name)
{
	if (name == NULL)
	{
		errno = EINVAL;
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}

	int rv = remove(name);
	if (rv)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}
	return 0;
}

int
fz_mkdirp_utf8(fz_context* ctx, const char* name)
{
	char* pname;

	pname = fz_strdup_no_throw(ctx, name);
	if (pname == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	// we can only mkdir beyond the (optional) root directory:
	char* p = pname + 1;
	char* d = strchr(p, '/'); // drive rootdir
	if (d == NULL)
	{
		// apparently, there's only one directory name above the drive letter specified. We still need to mkdir that one, though.
		d = p + strlen(p);
	}

	for (;;)
	{
		char c = *d;
		*d = 0;

		int n = mkdir(pname);
		int e = errno;
		if (n && e != EEXIST && e != EACCES)
		{
			fz_copy_ephemeral_errno(ctx);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			free(pname);
			return -1;
		}
		*d = c;
		// did we reach the end of the *original* path spec?
		if (!c)
			break;
		q = strchr(d + 1, '/');
		if (q)
			d = q;
		else
			d += strlen(d);  // make sure the sentinel-patching doesn't damage the last part of the original path spec
	}

	free(wname);
	return 0;
}

#endif
