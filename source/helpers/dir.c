
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/system-header-files.h"
#include "utf.h"

#ifdef _MSC_VER
#include <direct.h> /* for mkdir */
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


static uint64_t calchash(const char* s) {
	uint64_t x = 0x33333333CCCCCCCCULL;
	const uint8_t *p = (const uint8_t *)s;

	// hash/reduction inspired by Xorshift64
	for ( ; *p; p++) {
		uint64_t c = *p;
		x ^= c << 26;
		x += c;

		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;
	}

	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;

	return x;
}


#include "../../scripts/legal_codepoints_bitmask.inc"


static int has_prefix(const char *s, const char *prefix)
{
	size_t len = strlen(prefix);
	return fz_strncasecmp(s, prefix, len) == 0;
}

static int has_postfix(const char *s, const char *postfix)
{
	size_t len = strlen(postfix);
	size_t slen = strlen(s);
	if (slen < len)
		return 0;
	return fz_strcasecmp(s + slen - len, postfix) == 0;
}

static int has_basename(const char *s, const char *prefix)
{
	size_t len = strlen(prefix);
	return (fz_strncasecmp(s, prefix, len) == 0) &&
		   (!s[len] || s[len] == '.');
}

static int has_infix(const char *s, const char *infix)
{
	size_t len = strlen(infix);
	size_t slen = strlen(s);
	if (slen < len)
		return 0;
	for (size_t i = 0; i < slen - len; i++)
	{
		if (has_prefix(s + i, infix))
			return 1;
	}
	return 0;
}



// Detect 'reserved' and other 'dangerous' file/directory names.
//
// DO NOT accept any of these file / directory *basenames*: 
//
// - CON, PRN, AUX, NUL, COM0, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, 
//   LPT0, LPT1, LPT2, LPT3, LPT4, LPT5, LPT6, LPT7, LPT8, and LPT9
//
// - while on UNIXes a directory named `dev` might be ill-advised. 
//   (Writing to a **device** such as `/dev/null` is perfectly okay there; we're interested 
//   and focusing on *regular files and directories* here though, so giving them names such 
//   as `dev`, `stdout`, `stdin`, `stderr` or `null` might be somewhat... *ill-advised*. 😉
//
// - of course, Mac/OSX has it's own set of reserved names and aliases, e.g. all filenames 
//   starting with `._` are "extended attributes": when you have filename `"xyz"`, OSX will 
//   create an additional file named `"._xyz"` when you set any *extended attributes*. 
//   There's also the `__MACOSX` directory that you never see -- unless you look at the same 
//   store by way of a MSWindows or Linux box.
//
// - it's ill-advised to start (or end!) any file or directory name with `"."` or `"~"`: 
//   the former is used to declare the file/directory "hidden" in UNIX, while the latter 
//   MAY be treated as a shorthand for the user home directory or signal the file is a 
//   "backup/temporary file" when appended at the end.
//
// - NTFS has a set of reserved names of its own, all of which start with `"$"`, while antique 
//   MSDOS reserved filenames *end* with a `"$"`, so you'ld do well to forego `"$"` anywhere 
//   it shows up in your naming. 
//   To put the scare into you, there's a MSWindows hack which will *crash and corrupt* the system 
//   by simply *addressing* a (non-existing) reserved filename `"$i30:$bitmap"` on any NTFS drive 
//   (CVE-2021-28312): https://www.bleepingcomputer.com/news/security/microsoft-fixes-windows-10-bug-that-can-corrupt-ntfs-drives/
//
// - OneDrive also [mentions several reserved filenames](https://support.microsoft.com/en-us/office/restrictions-and-limitations-in-onedrive-and-sharepoint-64883a5d-228e-48f5-b3d2-eb39e07630fa#invalidcharacters): 
//   "*These names aren't allowed for files or folders: `.lock`, `CON`, `PRN`, `AUX`, `NUL`, 
//   `COM0` - `COM9`, `LPT0` - `LPT9`, `_vti_`, `desktop.ini`, any filename starting with `~$`.*" 
//   It also rejects `".ds_store"`, which is a hidden file on Mac/OSX. 
//   [Elsewhere](https://support.microsoft.com/en-us/office/onedrive-can-rename-files-with-invalid-characters-99333564-c2ed-4e78-8936-7c773e958881) 
//   it mentions these again, but with a broader scope: 
//   "*These other characters have special meanings when used in file names in OneDrive, SharePoint, 
//   Windows and macOS, such as `"*"` for wildcards, `"\"` in file name paths, **and names containing** 
//   `.lock`, `CON`, or `_vti_`.*" They also strongly discourage the use of `#%&` characters in filenames.
//
// - Furthermore it's a **bad idea** to start any filename with `"-"` or `"--"` as this clashes 
//   very badly with standard UNIX commandline options; only a few tools "fix" this by allowing 
//   a special `"--"` commandline option.
//
// - [GoogleDrive](https://developers.google.com/style/filenames) doesn't mention any explicit 
//   restrictions, but the published *guidelines* rather suggest limiting oneself to plain ASCII only, 
//   with all the other restrictions mentioned in this larger list. 
//   Google is (as usual for those buggers) *extremely vague* on this subject, but [some hints](https://www.googlecloudcommunity.com/gc/Workspace-Q-A/File-and-restrictions-for-Google-Drive/td-p/509595) 
//   have been discovered: path length *probably* limited to 32767, directory depth max @ 21 is 
//   mentioned (probably gleaned from [here](https://support.google.com/a/users/answer/7338880?hl=en#shared_drives_file_folder_limits), 
//   however I tested it on my own GoogleDrive account and at the time of this writing (May 2023) 
//   Google didn't object to creating a tree depth >= 32, nor a filename <= 500 characters.)
//
static int is_reserved_filename(const char *s)
{
	if (has_infix(s, "_vti_"))
		return 1;
	if (has_postfix(s, "$"))
		return 1;
	if (has_postfix(s, "~"))
		return 1;
		
	switch (tolower(s[0]))
	{
	case 'a':
		return has_basename(s, "aux");
		
	case 'd':
		return has_basename(s, "dev") ||
		       has_prefix(s, "desktop.ini");
		
	case 'c':
		return has_basename(s, "con") ||
			(has_prefix(s, "com") && isdigit(s[3]));

	case 'l':
		return has_prefix(s, "lpt") && isdigit(s[3]);
		
	case 'n':
		return has_basename(s, "nul") ||
		       has_basename(s, "null");
		
	case 'p':
		return has_basename(s, "prn");
		
	case 's':
		return has_basename(s, "stdin") ||
		       has_basename(s, "stdout") ||
		       has_basename(s, "stderr");
		
	case '_':
		return has_basename(s, "__macosx");
		
	case '~':
		return 1;
		
	case '.':
		return has_basename(s + 1, "lock");
		       has_basename(s + 1, "ds_store");
		
	case '$':
		return 1;
		
	case '-':
		return 1;
	}
	
	return 0;
}

const char *rigorously_clean_fname(char *s)
{
	// - keep up to two 'extension' dots.
	// - nuke anything that's not an ASCII alphanumeric
	// - modify in place
	char *d = s;
	while (*s)
	{
		char c = *s;
		if (isalnum(c))
		{
			*d++ = c;
		}
		else if (c == '.' && d > s)
		{
			*d++ = c;
		}
		// else: discard!
	}

	// trim off trailing dots:
	while (d > s && d[1] == '.')
		d--;

	*d = 0;

	// now nuke all but the last two dots:
	int dot_count = 0;	
	while (d > s)
	{
		char c = *--d;
		if (c == '.')
		{
			dot_count++;
			if (dot_count > 2)
				*d = '_';
		}
	}
	
	return d;
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
int fz_sanitize_path(fz_context* ctx, char* dstpath, size_t dstpath_bufsize, const char* path)
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

	return fz_sanitize_path_ex(dstpath, NULL, NULL, 0, dstpath_bufsize);
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

	These additional arguments, `set` and `replace_single`, may be NULL	or NUL(0), in which case they are not active.

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
int
fz_sanitize_path_ex(char* path, const char* set, const char* replace_single, size_t start_at_offset, size_t maximum_path_length)
{
	if (!path)
		return 0;
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

	if (start_at_offset)
	{
		size_t l = strlen(path);
		if (l <= start_at_offset)
		{
			// nothing to process!
			return 0;
		}

		path += start_at_offset;
	}

	// unixify MSDOS/MSWIN/UNC path:
	char* e = strchr(path, '\\');
	while (e)
	{
		*e = '/';
		e = strchr(e + 1, '\\');
	}

	e = path;

	if (start_at_offset == 0)
	{
		int can_have_mswin_drive_letter = 1;

		// Sanitize the *entire* path, including the Windows Drive/Share part:
		//
		// check if path is a UNC path. It may legally start with `\\.\` or `\\?\` before a Windows drive/share+COLON:
		if (e[0] == '/' && e[1] == '/')
		{
			if (strchr(".?", e[2]) != NULL && e[3] == '/')
			{
				// skip //?/ and //./ UNC path leaders
				e += 4;
				can_have_mswin_drive_letter = -1;
			}
			else
			{
				// skip //<server>... UNC path starter (which cannot contain Windows drive letters as-is)
				char *p = e + 2;
				while (isalnum(*p) || strchr("_-$", *p))
					p++;
				if (p > e && *p == '/' && p[1] != '/')
					p++;
				else
				{
					// no legal UNC domain name, hence no UNC at all: assume it's a simple UNIX '/' root directory
					p = e + 1;
				}
				e = p;
				can_have_mswin_drive_letter = 0;
			}
		}
		else if (e[0] == '.') 
		{
			can_have_mswin_drive_letter = 0;
			
			if (e[1] == '/')
			{
				// skip ./ relative path at start, replace it by NIL: './' is superfluous!
				size_t len = strlen(e + 2);
				memmove(e, e + 2, len + 1);
			}
			else if (e[1] == '.' && e[2] == '/')
			{
				// skip any chain of ../ relative path elements at start, as this may be a valid relative path
				e += 3;

				while (e[0] == '.' && e[1] == '.' && e[2] == '/')
				{
					e += 3;
				}
			}
		}
		else if (e[0] == '/')
		{
			// basic UNIX '/' root path:
			e++;
		}

		if (can_have_mswin_drive_letter)
		{
			// See if we have a Windows drive as part of the path: keep that one intact!
			if (isalpha(e[0]) && e[1] == ':')
			{
				// while users can specify relative paths within drives, e.g. `C:path`, they CANNOT do so when this is a UNC path.
				if (e[2] != '/' && can_have_mswin_drive_letter < 0)
				{
					// not a legal UNC path after all
				}
				else
				{
					e[0] = toupper(e[0]);
					e += 2;
				}
			}
		}
		
		// when we get here, `e` will point PAST the previous '/', so we can ditch any leading '/' below, before we sanitize the remainder.
	}
	else
	{
		// when the caller specified an OFFSET, we assume we're pointing PAST the previous '/', but we better make sure
		// as we MAY have been pointed AT the separating '/' instead...
		if (e[0] == '/')
		{
			if (e[-1] != '/') 
			{
				// skip this '/' directory separator before we start for real:
				e++;
			}
		}
	}

	char *e_start = e;
	char *cur_segment_start = e;
	char *d = e;

	// skip leading surplus '/'
	while (e[0] == '/')
		e++;

	// calculate a simple & fast hash of the remaining path:
	uint32_t hash = calchash(e);

	char *p = e;

	// now go and scan/clean the rest of the path spec:
	int repl_seq_count = 0;
	
	while (*p)
	{
		while (repl_seq_count > 1)
		{
			if (d[-1] != d[-2])
				break;
			d--;
			repl_seq_count--;
		}
		// ^^^ note that we do this step-by-step this way, slowly eating the
		// replacement series. The benefit of this approach, however, is that
		// we don't have to bother the rest of the code/loop with this,
		// making the code easier to review. Performance is still good enough.

		char c = *p++;
		if (c == '/')
		{
			*d = 0;
			if (is_reserved_filename(cur_segment_start))
			{
				// previous part of the path isn't allowed: replace by a hash-based name instead.
				char buf[32];
				size_t max_width = p - cur_segment_start - 1;
				
				const char *old_cleaned_fname = rigorously_clean_fname(cur_segment_start);
				size_t fnlen = strlen(old_cleaned_fname);
				
				int rslen = max_width;
				rslen -= 4 + 8;
				if (rslen > fnlen)
					rslen = fnlen;
				
				if (rslen >= 0)
				{
					snprintf(buf, sizeof(buf), "_H%08X_%s", (unsigned int)hash, old_cleaned_fname + fnlen - rslen);
					buf[sizeof(buf) - 1] = 0;
				}
				else
				{
					snprintf(buf, sizeof(buf), "H%08X", (unsigned int)hash);
					buf[max_width] = 0;
				}
				strcpy(cur_segment_start, buf);
				d = cur_segment_start + strlen(cur_segment_start);
			}
			else
			{
				size_t sslen = strlen(cur_segment_start);
				
				// limit to UNIX fname length limit:
				if (sslen > 255) 
				{
					char buf[256];
				
					const char *old_cleaned_fname = rigorously_clean_fname(cur_segment_start);
					size_t fnlen = strlen(old_cleaned_fname);
				
					int rslen = 255 - 10;
					if (rslen > fnlen)
						rslen = fnlen;
					
					snprintf(buf, sizeof(buf), "_H%08X_%s", (unsigned int)hash, old_cleaned_fname + fnlen - rslen);
					buf[sizeof(buf) - 1] = 0;
					strcpy(cur_segment_start, buf);
					d = cur_segment_start + strlen(cur_segment_start);
				}
			}
			
			// keep path separators intact at all times.
			*d++ = c;

			cur_segment_start = d;
			
			// skip superfluous addititional '/' separators:
			while (*p == '/')
				p++;
		
			repl_seq_count = 0;
			continue;
		}

		// custom set replacements for printf-style format bits, anyone?
		if (has_printf_format_repl_idx1 && c == '%')
		{
			// replace any %[+/-/ ][0-9.*][dlfgespuizxc] with a single replacement character
			while (*p && strchr("+- .0123456789*", *p))
				p++;
			if (*p && strchr("lz", *p))
				p++;
			if (*p && strchr("dlfgespuizxc", *p))
				p++;

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
				while (*p == '#')
					p++;
			}

			// custom 1:1 replacement: set -> replace_single.
			int idx = m - set;
			// pick last in map when we're out-of-bounds:
			if (idx >= repl_map_len)
				idx = repl_map_len - 1;
			*d++ = replace_single[idx];
			repl_seq_count++;
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
			int l = fz_chartorune_unsafe(&u, p - 1);
			if (u == Runeerror) {
				// bad UTF8 is to be discarded!
				*d++ = '_';
				repl_seq_count++;
				continue;
			}
			
			// Only accept Letters and Numbers in the BMP:
			if (u <= 65535)
			{
				int pos = u / 32;
				int bit = u % 32;
				uint32_t mask = 1U << bit;
				if (legal_codepoints_bitmask[pos] & mask) 
				{
					// Unicode BMP: L (Letter) or N (Number)
					// --> keep Unicode UTF8 codepoint:
					p--;
					for (; l > 0; l--)
						*d++ = *p++;
					repl_seq_count = 0;
					continue;
				}
			}

			// undesirable UTF8 codepoint is to be discarded!
			*d++ = '_';
			p += l - 1;
			repl_seq_count++;
			continue;
		}
		else if (c < ' ' || c == 0x7F /* DEL */)
		{
			*d++ = '_';
			repl_seq_count++;
			continue;
		}
		else if (strchr("`\"*@=|;?:", c))
		{
			// replace NTFS-illegal character path characters.
			// replace some shell-scripting-risky character path characters.
			// replace the usual *wildcards* as well.
			*d++ = '_';
			repl_seq_count++;
			continue;
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
				continue;
			}
			else if (c == '.') 
			{
				// a dot at the END of a path element is not accepted; neither do we tolerate ./ and ../ as we have
				// skipped any legal ones already before.
				//
				// we assume the path has been normalized (relative or absolute) before it was sent here, hence
				// we'll accept UNIX dotfiles (one leading '.' dot for an otherwise sane filename/dirname), but
				// otherwise dots can only sit right smack in the middle of an otherwise legal filename/dirname:
				if (!p[0] || p[0] == '.' || p[0] == '/') 
				{
					*d++ = '_';
					repl_seq_count++;
					continue;
				}
				// we're looking at a plain-as-Jim vanilla '.' dot here: pass as-is:
				*d++ = c;
				repl_seq_count = 0;
				continue;
			}
			else if (c == '$') 
			{
				// a dollar is only accepted in the middle of an element in order to prevent NTFS special filename attacks.
				if (!p[0] || p[0] == '/') 
				{
					*d++ = '_';
					repl_seq_count++;
					continue;
				}
				else if (d == e_start || strchr(":/", d[-1])) 
				{
					*d++ = '_';
					repl_seq_count++;
					continue;
				}
				
				// we're looking at a plain-as-Jim vanilla '$' dollar here: pass as-is:
				*d++ = c;
				repl_seq_count = 0;
				continue;
			}
			else if (isalnum(c))
			{
				// we're looking at a plain-as-Jim vanilla character here: pass as-is:
				*d++ = c;
				repl_seq_count = 0;
				continue;
			}
			else if (c == '-')
			{
				// Furthermore it's a **bad idea** to start any filename with `"-"` or `"--"` 
				// as this clashes very badly with standard UNIX commandline options; 
				// only a few tools "fix" this by allowing a special `"--"` commandline option.
				//
				// We also don't like filenames which end with '-', but that's more an aesthetical thing...
				//
				// Hence we nuke such filenames:
				if (!p[0] || p[0] == '/') 
				{
					*d++ = '_';
					repl_seq_count++;
					continue;
				}
				else if (d == cur_segment_start)
				{
					*d++ = '_';
					repl_seq_count++;
					continue;
				}

				// we're looking at a plain-as-Jim vanilla '-' dash character here: pass as-is:
				*d++ = c;
				repl_seq_count = 0;
				continue;
			}
			else if (strchr("_()", c))
			{
				// we're looking at a plain-as-Jim vanilla character here: pass as-is:
				*d++ = c;
				repl_seq_count = 0;
				continue;
			}
			else 
			{
				// anything else is considered illegal / bad for our FS health:
				*d++ = '_';
				repl_seq_count++;
				continue;
			}
		}
	}

	// and print the sentinel
	*d = 0;

	return 0;
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

static int fz_is_UNC_path(const wchar_t* path)
{
	// anything with prefix '//yadayada'
	return (path && (
		(path[0] == '/') ||
		(path[0] == '\\')
	) && (
		(path[1] == '/') ||
		(path[1] == '\\')
	) && (
		isalnum(path[2]) ||
		path[2] == '$' ||
		path[2] == '.' ||
		path[2] == '?'
	));
}

static wchar_t* fz_UNC_wfullpath_from_name(fz_context* ctx, const char* path)
{
	if (!path) {
		errno = EINVAL;
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return NULL;
	}

	// don't use PATH_MAX fixed-sized arrays as the input path MAY be larger than this!
	size_t slen = strlen(path);
	size_t wlen = slen + 5;
	wchar_t* wpath = fz_malloc(ctx, wlen * sizeof(wpath[0]));
	wchar_t* wbuf = fz_malloc(ctx, wlen * sizeof(wbuf[0]));

	if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, wlen))
	{
err:
		fz_copy_ephemeral_system_error(ctx, GetLastError(), NULL);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_free(ctx, wbuf);
		fz_free(ctx, wpath);
		return NULL;
	}
	for (;;) {
		size_t reqd_len = GetFullPathNameW(wpath, wlen - 4, wbuf + 4, NULL);

		if (!reqd_len)
		{
			fz_copy_ephemeral_system_error(ctx, GetLastError(), NULL);
			ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
			goto err;
		}
		if (reqd_len < wlen - 4) {
			// buffer was large enough; path has been rewritten in wbuf[4]+
			break;
		}
		wlen = reqd_len + 7;
		wbuf = fz_realloc(ctx, wbuf, wlen * sizeof(wbuf[0]));
	}

	const wchar_t* fp = wbuf + 4;
	// Is full path an UNC path already? If not, make it so:
	if (!fz_is_UNC_path(wbuf + 4))
	{
		wbuf[0] = '\\';
		wbuf[1] = '\\';
		wbuf[2] = '?';
		wbuf[3] = '\\';
	}
	else
	{
		size_t dlen = wcslen(wbuf + 4) + 1;
		memmove(wbuf, wbuf + 4, dlen * sizeof(wbuf[0]));
	}
	fz_free(ctx, wpath);
	return wbuf;
}

#endif

int fz_chdir(fz_context* ctx, const char *path)
{
#if defined(_WIN32)
	wchar_t* wname = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wname)
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
#if defined(_WIN32)
		fz_free(ctx, wname);
#endif
		return -1;
	}

#if defined(_WIN32)
	fz_free(ctx, wname);
#endif
	return 0;
}

void fz_mkdir_for_file(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
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
#else
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
	}
	fz_free(ctx, buf);
#endif
}


int64_t
fz_stat_ctime(fz_context *ctx, const char* path)
{
#if defined(_WIN32)
	struct _stat info;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
		return 0;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_free(ctx, wpath);
		return 0;
	}

	fz_free(ctx, wpath);
	return info.st_ctime;
#else
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return 0;
	}
	return info.st_ctime;
#endif
}

int64_t
fz_stat_mtime(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
	struct _stat info;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
		return 0;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_free(ctx, wpath);
		return 0;
	}

	fz_free(ctx, wpath);
	return info.st_mtime;
#else
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return 0;
	}
	return info.st_mtime;
#endif
}

int64_t
fz_stat_atime(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
	struct _stat info;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
		return 0;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_free(ctx, wpath);
		return 0;
	}

	fz_free(ctx, wpath);
	return info.st_atime;
#else
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return 0;
	}
	return info.st_atime;
#endif
}

int64_t
fz_stat_size(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
	struct _stat info;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
		return 0;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_free(ctx, wpath);
		return -1;
	}

	fz_free(ctx, wpath);

	// directories should be reported as size=0 or 1...

	if (info.st_mode & _S_IFDIR)
		return 0;
	if (info.st_mode & _S_IFREG)
		return info.st_size;
	return 0;
#else
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}
	if (info.st_mode & _S_IFDIR)
		return 0;
	if (info.st_mode & _S_IFREG)
		return info.st_size;
	return 0;
#endif
}

int
fz_path_is_regular_file(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
	struct _stat info;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
		return FALSE;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_free(ctx, wpath);
		return FALSE;
	}

	fz_free(ctx, wpath);

	// directories should be reported as size=0 or 1...

	return (info.st_mode & _S_IFREG) && !(info.st_mode & _S_IFDIR);
#else
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return FALSE;
	}

	return (info.st_mode & _S_IFREG) && !(info.st_mode & _S_IFDIR);
#endif
}

int
fz_path_is_directory(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
	struct _stat info;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
		return FALSE;

	int n = _wstat(wpath, &info);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		fz_free(ctx, wpath);
		return FALSE;
	}

	fz_free(ctx, wpath);

	// directories should be reported as size=0 or 1...

	return !!(info.st_mode & _S_IFDIR);
#else
	struct stat info;
	if (stat(path, &info))
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return FALSE;
	}

	return !!(info.st_mode & _S_IFDIR);
#endif
}


FILE *
fz_fopen_utf8(fz_context* ctx, const char* path, const char* mode)
{
#if defined(_WIN32)
	wchar_t *wmode;
	FILE* file;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
	{
		return NULL;
	}

	wmode = fz_wchar_from_utf8(ctx, mode);
	if (wmode == NULL)
	{
		fz_free(ctx, wpath);
		return NULL;
	}

	file = _wfopen(wpath, wmode);
	if (!file)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
	}

	fz_free(ctx, wmode);
	fz_free(ctx, wpath);

	return file;
#else
	if (path == NULL)
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

	FILE* rv = fopen(path, mode);
	if (!rv)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return NULL;
	}
	return rv;
#endif
}

int
fz_remove_utf8(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
	int n;
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
	{
		return -1;
	}

	n = _wremove(wpath);
	if (n)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		n = -1;
	}

	return n;
#else
	if (path == NULL)
	{
		errno = EINVAL;
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}

	int rv = remove(path);
	if (rv)
	{
		fz_copy_ephemeral_errno(ctx);
		ASSERT(fz_ctx_get_system_errormsg(ctx) != NULL);
		return -1;
	}
	return 0;
#endif
}

int
fz_mkdirp_utf8(fz_context* ctx, const char* path)
{
#if defined(_WIN32)
	wchar_t* wpath = fz_UNC_wfullpath_from_name(ctx, path);

	if (!wpath)
	{
		return -1;
	}

	// as this is now an UNC path, we can only mkdir beyond the drive letter:
	wchar_t* p = wpath + 4;
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

		int n = _wmkdir(wpath);
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
#else
	char* pname;

	pname = fz_strdup_no_throw(ctx, path);
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
			fz_free(ctx, pname);
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

	fz_free(ctx, pname);
	return 0;
#endif
}


int
fz_mkdir(fz_context* ctx, const char* path)
{
#ifdef _WIN32
	int ret;
	wchar_t* wpath = fz_wchar_from_utf8(ctx, path);

	if (wpath == NULL)
		return -1;

	ret = _wmkdir(wpath);

	fz_free(ctx, wpath);

	return ret;
#else
	return mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
}


#pragma message("TODO: code review re ephemeral errorcode handling")

