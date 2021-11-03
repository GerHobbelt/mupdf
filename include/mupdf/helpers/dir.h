#ifndef __MUPDF_HELPERS_DIR_H__
#define __MUPDF_HELPERS_DIR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mupdf/fitz/version.h"
#include "mupdf/fitz/config.h"
#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"

#include <errno.h>

#ifndef E_OK
#define E_OK 0
#endif

int fz_chdir(fz_context* ctx, const char* path);

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
 */
void fz_sanitize_path(fz_context* ctx, char* dstpath, size_t dstpath_bufsize, const char* path);

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
char* fz_sanitize_path_ex(char* path, const char* set, const char* replace_single, char replace_sequence);

#ifdef __cplusplus
}
#endif

#endif

