#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "utf.h"

#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include <ctype.h>

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

	// See if we have a Windows drive/share as part of the path: keep that one intact!
	char* start = dstpath;
	char* p = strchr(dstpath, ':');
	if (p) {
		start = p + 1;
	}

	// unixify MSDOS path:
	char* e = strchr(start, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e, '\\');
	}

	// now find ./ and ../ directories and resolve each, if possible:
	p = start;
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

	// See if we have a Windows drive/share as part of the path: keep that one intact!
	char* start = dstpath;
	char* p = strchr(dstpath, ':');
	if (p) {
		start = p + 1;
	}

	// unixify MSDOS path:
	char* e = strchr(start, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e, '\\');
	}

	// Sanitize the *entire* path, including the Windows Drive/Share part:
	for (e = dstpath; *e; e++) {
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
		else if (c == '.' && e[1] == '/' && (e == dstpath || e[-1] == '.')) {
			// a dot at the END of a path name is not accepted; tolerate ./ and ../ at the start of the path
			*e = '_';
		}
		else if (c == '.' && e[1] == 0 && (e == dstpath || e[-1] == '.')) {
			// a dot at the END of a file/path name is not accepted; tolerate ./ and ../ at the start of the path
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

	Overwrites the `path` string in place.
*/
char*
fz_sanitize_path_ex(char* path, const char* set, const char* replace_single, char replace_sequence)
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

	char* p;
	char* d;

	// UNIXify:
	for (p = path; *p; ++p)
		if (*p == '\\')
			*p = '/';

	d = p = path;

	// check if path is a UNC path. It may legally start with `\\.\` or `\\?\` before a Windows drive/share+COLON:
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
		else if (strchr("`\"*@=|;?", c))
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

	return path;
}


int fz_chdir(fz_context* ctx, const char *path)
{
#ifdef _MSC_VER
	if (_chdir(path))
#else
	if (chdir(path))
#endif
	{
		if (ctx)
		{
			switch (errno)
			{
			case ENOENT:
				fz_throw(ctx, FZ_ERROR_GENERIC, "chdir: Unable to locate the directory: %s", path);
				break;
			case EINVAL:
				fz_throw(ctx, FZ_ERROR_GENERIC, "chdir: Invalid buffer.");
				break;
			default:
				fz_throw(ctx, FZ_ERROR_GENERIC, "chdir: Unknown error %d.", (int)errno);
				break;
			}
		}
		return errno;
	}
	return E_OK;
}

void fz_mkdir_for_file(fz_context* ctx, const char* path)
{
	char* buf = fz_strdup(ctx, path);

	if (!buf)
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_mkdir_for_file: out of memory.");

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
			mkdir(buf);
			*e = '/';
		}
		mkdir(buf);
	}
	fz_free(ctx, buf);
}