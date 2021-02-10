#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"

#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>

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

enum
{
	Runeerror = 0xFFFD, /* decoding error in UTF */
	Runemax = 0x10FFFF, /* maximum rune value */
};

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
		if (c > 0x7F) {
			int u;
			int l = fz_chartorune(&u, e);
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
