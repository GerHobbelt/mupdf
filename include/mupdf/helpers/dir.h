#ifndef __MUPDF_HELPERS_DIR_H__
#define __MUPDF_HELPERS_DIR_H__

#include "mupdf/fitz/version.h"
#include "mupdf/fitz/config.h"
#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef E_OK
#define E_OK 0
#endif

int fz_chdir(fz_context* ctx, const char* path);

FILE *fz_fopen_utf8(fz_context *ctx, const char *name, const char *mode);
int fz_remove_utf8(fz_context *ctx, const char *name);
int fz_mkdirp_utf8(fz_context *ctx, const char* name);

/**
* Return the file's Creation Timestamp.
*
* Return 0 on error.
*/
int64_t fz_stat_ctime(fz_context *ctx, const char *path);

/**
* Return the file's Last-Modification Timestamp.
*
* Return 0 on error.
*/
int64_t fz_stat_mtime(fz_context *ctx, const char *path);

/**
* Return the file's Last-Accessed Timestamp.
*
* NOTE: some operating systems, file systems and/or user-configured security/privacy seettings
* may preveent the OS from tracking this timestamp for each file. In that case, expect bogus
* timestamps to be produced; this function does not check whether the OS-produced `atime` is
* indeed valid or bogus after all, hence YMMV.
* 
* Return 0 on error.
*/
int64_t fz_stat_mtime(fz_context *ctx, const char *path);

/**
* Return the file's size (in bytes).
*
* Return -1 on error -- 0 is a valid file size, after all.
*/
int64_t fz_stat_size(fz_context *ctx, const char *path);

/**
* Return TRUE when the path is a regular file (or a link to a regular file).
*
* Return FALSE when it is not, or when the path does not exist at all / an error was reported by the OS.
*/
int fz_path_is_regular_file(fz_context *ctx, const char *path);

/**
* Return TRUE when the given path is a directory (or a link to a directory).
*
* Return FALSE when it is not, or when the path does not exist at all / an error was reported by the OS.
*/
int fz_path_is_directory(fz_context *ctx, const char *path);


/**
  Create directory for given *file* path, so that a subsequent file-create action will not fail due to the given path not existing.

  Throws exception when errors occur.
*/
void fz_mkdir_for_file(fz_context* ctx, const char* path);

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
void fz_normalize_path(fz_context* ctx, char* dstpath, size_t dstpath_bufsize, const char* path);

/**
 * Sanitize a given path, i.e. replace any "illegal" characters in the path, using generic
 * OS/filesystem heuristics. "Illegal" characters are replaced with an _ underscore.
 *
 * When path is NULL, the path is assumed to already be present in the dstpath buffer and
 * will be modified in place.
 *
 * The path is assumed to have been normalized already, hence little intermezzos like '/./'
 * shall not exist in the input. IFF they do, they will be sanitized to '/_/'.
 *
 * Path elements (filename, directory names) are restricted to a maximum length of 255 characters
 * *each* (Linux FS limit).
 *
 * The entire path will be reduced to the given `dstpath_bufsize`;
 * reductions are done by first reducing the filename length, until it is less
 * or equal to 13 characters, after which directory elements are reduced, one after the other,
 * until we've reached the desired total path length.
 *
 * Returns 0 when no sanitization has taken place. Returns 1 when only the filename part
 * of the path has been sanitized. Returns 2 or higher when any of the directory parts have
 * been sanitized: this is useful when calling code wishes to speed up their FS I/O by
 * selectively executing `mkdirp` only when the directory elements are sanitized and thus
 * "may be new".
 */
int fz_sanitize_path(fz_context* ctx, char* dstpath, size_t dstpath_bufsize, const char* path);

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

	Modifies the `path` string in place.
 
    Returns 0 when no sanitization has taken place. Returns 1 when only the filename part
    of the path has been sanitized. Returns 2 or higher when any of the directory parts have
    been sanitized: this is useful when calling code wishes to speed up their FS I/O by
    selectively executing `mkdirp` only when the directory elements are sanitized and thus
    "may be new".
*/
int fz_sanitize_path_ex(char* path, const char* set, const char* replace_single, size_t start_at_offset, size_t maximum_path_length);

/**
	Produce a relative path in `dst` for the given absolute path `abspath`, when considered relative to absolute path `relative_to_abspath`.

	Relative paths MAY include leading '../' elements, but DOES NOT include a (superfluous) './' element.

	When the `abspath` is not sensibly relative (e.g. when it points to a different drive on MSWindows), then the result of this call
	will be a copy of the absolute path itself.

	CAVEATS:
	- both input paths are assumed to be normalized.
	- both input paths are assumed to be absolute.
	- both input paths are either both in UNC form ('//?/Drive:/path/file') or both in classic form ('Drive:/path/file')
	- both input paths are assumed to point to **files** and are treated as such: the last element (the filename itself)
	  is never considered as part of the 'common prefix'.
	- path comparison is done in case-sensitive fashion, even on MSWindows/NTFS, which is a case-insensitive file system.
	  Usually this is not a problem, IFF the sources of both path originate from the same or very similar processing routes
	  through the code OR when normalization includes ensuring each path element is in **canonical case**: that will work
	  for *any* file-system, including case-insensitive file systems such as NTFS or VFAT/FAT/FAT32.

	Returns `dst` when done.
*/
char* fz_mk_relative_path(fz_context* ctx, char* dst, size_t dstsiz, const char* abspath, const char* relative_to_abspath);

/**
	Produce an absolute path in `dst`, based on the (relative or absolute) `source_path`. When `source_path` is a
	*relative path*, it is assumed to be relative to the 'base', represented by the 'base file path' `abs_optional_base_file_path'.

	Use this function to produce an absolute path for any relative pathed file, relative to **another file**:
	the 'base file path' `abs_optional_base_file_path` is assumed to be a FILE path, NOT a DIRECTORY, so we need to get rid of that
	'useless' base filename: we do that by appending a '../' element and have the internal path sanitizer deal with it
	afterwards: the '../' will eat the preceding element, which, in this case, will be the 'base file name'.

	When `source_path` is an absolute path, it remains unchanged (apart from sanitization and normalization).

	ALL produced paths have been normalized and sanitized.

	Examples:

	source_path = a/b/c.fil
	abs_optional_base_file_path = C:/d/e/f.txt
	fz_mk_absolute_path_using_absolute_base --> C:/d/e/a/b/c.fil

	source_path = C:/a/b/c.fil
	abs_optional_base_file_path = C:/d/e/f.txt
	fz_mk_absolute_path_using_absolute_base --> C:/a/b/c.fil

	source_path = ../.b0rk.//fil...
	abs_optional_base_file_path = C:/d/e/f.txt
	fz_mk_absolute_path_using_absolute_base --> C:/d/e/../.b0rk.//fil... -> C:/d/.b0rk_/fil___
*/
void fz_mk_absolute_path_using_absolute_base(fz_context* ctx, char* dst, size_t dstsiz, const char* source_path, const char* abs_optional_base_file_path);

/**
	Return TRUE when the given path is an absolute path.

	Anything else will return FALSE.
*/
int fz_is_absolute_path(const char* path);

#ifdef __cplusplus
}
#endif

#endif

