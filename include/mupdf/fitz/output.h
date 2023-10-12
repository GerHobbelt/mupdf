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

#ifndef MUPDF_FITZ_OUTPUT_H
#define MUPDF_FITZ_OUTPUT_H

#include "mupdf/fitz/system.h"
#include "mupdf/fitz/config.h"
#include "mupdf/fitz/context.h"
#include "mupdf/fitz/buffer.h"
#include "mupdf/fitz/string-util.h"
#include "mupdf/fitz/stream.h"
#include "mupdf/helpers/mu-threads.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
	Generic output streams - generalise between outputting to a
	file, a buffer, etc.
*/

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	whenever data is written to the output.

	out: a reference to the output stream.

	data: a pointer to a buffer of data to write.

	n: The number of bytes of data to write.

	Returns TRUE(!0) on error, 0 on success.
*/
typedef int (fz_output_write_fn)(fz_context *ctx, fz_output *out, const void *data, size_t n);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called when
	fz_seek_output is requested.

	out: a reference to the output stream.

	offset, whence: as defined for fz_seek().

	Returns TRUE(!0) on error, 0 on success.
*/
typedef int (fz_output_seek_fn)(fz_context *ctx, fz_output *out, int64_t offset, int whence);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called when
	fz_tell_output is requested.

	out: a reference to the output stream.

	Returns the offset within the output stream.
	Returns a *negative* value when an error occurred.
*/
typedef int64_t (fz_output_tell_fn)(fz_context *ctx, fz_output *out);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when the output stream is closed, to flush any pending writes.

	It is assumed that everything went well up to now, for otherwise
	this function SHOULD NOT be called.

	The expected coding idiom for using any `fz_output`-type object
	goes like this:

	```
	fz_output *out = NULL;
	fz_try(ctx)
	{
		out = fz_new_output_XYZ(ctx, ...);	// create the output instance

		[...do preparatory work...]

		// write data to output using this API & friends:
		fz_write_output(ctx, out, ...);
		[...]

		// when finished, we close the output to announce our success at completion:
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		// call the cleanup crew now that we're done:
		fz_drop_output(ctx, out);
	}
	fz_catch(ctx)
	{
		// report the unexpected b0rk:
		report_our_failure_to_deliver(fz_caught_message(ctx));
	}
	```

	Note the placement of the `fz_close_output()` and `fz_drop_output()`
	calls in the try/always/catch blocks above.

	> As a side-effect of using this idiom consistently, we can safely code
	> mechanisms such as 'safe file creation', where files are only kept on
	> disk when they're actually carrying correct & complete data: the
	> 'close' signals successful completion, while a drop-without-preceding-close
	> signals failure to write the complete contents; in the latter case
	> we can then delete the scratch file to prevent broken output files
	> from remaining on the target file system.
	>
	> See also the `fz_set_safe_file_output_mode()` API.

	out: a reference to the output stream.
*/
typedef void (fz_output_close_fn)(fz_context *ctx, fz_output *out);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when the output stream is dropped, to release the stream
	specific state information.
*/
typedef void (fz_output_drop_fn)(fz_context *ctx, fz_output *out);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when the fz_stream_from_output is called.
*/
typedef fz_stream *(fz_stream_from_output_fn)(fz_context *ctx, fz_output *out);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when fz_truncate_output is called to truncate the file
	at that point.

	Returns TRUE on error, 0 on success.
*/
typedef int (fz_truncate_fn)(fz_context *ctx, fz_output *out);

/**
* Secondary outputs are files (or buffers, or ...) which are generated while
* the fz_output is written.
*
* This happens, for instance, when writing STEXT or HTML output and you want
* to write image data to a separate file (reference-images) instead of in-line
* as 'data URI'.
*
* Other scenarios include writing HTML pages, including their CSS, images and JavaScript
* files into an SHTML file (SMIME/HTML): the CSS, images, etc. should be *delayed*
* until the HTML content itself has been written completely: this can be accomplished
* by buffering these 'secondary' files until the end of the primary write action
* and then appending them to the output file (finalize).
*/

typedef void (fz_mk_filename_fn)(fz_context* ctx, char* pathbuf, size_t pathbufsize, fz_secondary_outputs* sec_out);
typedef fz_output* (fz_mk_output_fn)(fz_context* ctx, fz_secondary_outputs* sec_out);
typedef void (fz_finalize_secondary_fn)(fz_context* ctx, fz_secondary_outputs* sec_out, fz_output *primary_out);
typedef void (fz_drop_secondary_fn)(fz_context* ctx, fz_secondary_outputs* sec_out);

struct fz_secondary_outputs
{
	void *secondary_state;
	const char* path_template;
	int counter;					// used to produce filename sequences
	int unique_counter;	        	// used to produce unique filenames
	fz_mk_filename_fn* mk_filename;
	fz_mk_output_fn* mk_output;
	fz_finalize_secondary_fn* finalize;
	fz_drop_secondary_fn* drop;
};

/*
	fz_output::flags bits:

	- FZOF_IS_INSIDE_PRINTF_LOCK: this flag is used internally to ensure fz_write_printf() et al do NOT cause a deadlock while they invoke fz_write_byte() et al as part of their process.
	  Meanwhile, the user will observe a lock granularity **per written message** this way, instead of merely **per character / message-segment**, which makes for garbled
	  log output, for example.
	- FZOF_IS_INSIDE_LOCK: set when inside the critical section protecting the `fz_output` buffer (`bp`, `ep`, `wp` members)

*/
typedef enum fz_output_flags
{
	FZOF_NONE = 0,
	FZOF_HAS_LOCKS = 0x0001,
	FZOF_IS_INSIDE_LOCK = 0x0002,
	FZOF_IS_INSIDE_PRINTF_LOCK = 0x0004
} fz_output_flags_t;

struct fz_output
{
	void *state;
	fz_output_write_fn *write;
	fz_output_seek_fn *seek;
	fz_output_tell_fn *tell;
	fz_output_close_fn *close;
	fz_output_drop_fn *drop;
	fz_stream_from_output_fn *as_stream;
	fz_truncate_fn *truncate;
	char *bp, *wp, *ep;
	char *filepath;
#if !defined(DISABLE_MUTHREADS)
	mu_mutex buf_mutex;
	mu_mutex printf_mutex;
#endif
	fz_output_flags_t flags;
    int severity_level;			// side channel for custom dispatchers
	struct fz_secondary_outputs secondary;
	/* If buffered is non-zero, then we have that many
	 * bits (1-7) waiting to be written in bits. */
	int buffered;
	int bits;
};

/**
	Create a new output object with the given
	internal state and function pointers.

	state: Internal state (opaque to everything but implementation).

	write: Function to output a given buffer.

	close: Cleanup function to destroy state when output closed.
	May permissibly be null.
*/
fz_output *fz_new_output(fz_context *ctx, int bufsiz, void *state, fz_output_write_fn *write, fz_output_close_fn *close, fz_output_drop_fn *drop);

/**
	Open an output stream that writes to a
	given path.

	filename: The filename to write to (specified in UTF-8).

	append: non-zero if we should append to the file, rather than
	overwriting it.
*/
fz_output *fz_new_output_with_path(fz_context *, const char *filename, int append);

/**
	Open an output stream that appends
	to a buffer.

	buf: The buffer to append to.
*/
fz_output *fz_new_output_with_buffer(fz_context *ctx, fz_buffer *buf);

/**
	Set a buffer for an output which hasn't got any yet. This is usually the case
	with predefined outputs: fz_stderr and fz_stdods.

	Return 0 on success, 1 on failure to (re)set the buffer.
*/
int fz_set_output_buffer(fz_context* ctx, fz_output* out, int bufsiz);

enum {
    FZO_SEVERITY_NONE = 0,
    FZO_SEVERITY_ERROR = 1,
    FZO_SEVERITY_WARNING,
    FZO_SEVERITY_INFO,
    FZO_SEVERITY_DEBUG,
    FZO_SEVERITY_DENUG2,
};

/**
    Set the 'severity level' of the `fz_output` instance. This is used
    as a side channel to pass 'severity level' metadata to the underlying
    output handler, which MAY use this datum to dispatch/route the data being
    written.

    This is used, f.e., by the fz_error()/fz_warn()/fz_info() functions in
    conjunction with the `fz_stdods()` (`fz_stddbg()`) debug output channel.
*/
static inline void fz_output_set_severity_level(fz_context *ctx, fz_output *out, int level)
{
    ASSERT0(out);
    out->severity_level = level;
}

/**
    Get the 'severity level' of the `fz_output` instance. This is used
    as a side channel to pass 'severity level' metadata to the underlying
    output handler, which MAY use this datum to dispatch/route the data being
    written.

    This is used, f.e., by the fz_error()/fz_warn()/fz_info() functions in
    conjunction with the `fz_stdods()` (`fz_stddbg()`) debug output channel.
*/
static inline int fz_output_get_severity_level(fz_context *ctx, fz_output *out)
{
    ASSERT0(out);
    return out->severity_level;
}


/**
	Retrieve an fz_output that directs to stdout.

	Optionally may be fz_dropped when finished with.
*/
fz_output *fz_stdout(fz_context *ctx);

/**
	Retrieve an fz_output that directs to stdout.

	Optionally may be fz_dropped when finished with.
*/
fz_output *fz_stderr(fz_context *ctx);

/**
    Retrieve an fz_output that directs to OutputDebugString (Win32) or your OS's native debug channel.

    Returns NULL when no debug channel is available.

	Optionally may be fz_dropped when finished with.
*/
fz_output *fz_stdods(fz_context *ctx);

/**
    Set the output stream to be used for fz_stddbg. Set to NULL to
    reset to default (stdods / stderr).
*/
void fz_set_stddbg(fz_context *ctx, fz_output *out);

/**
	Retrieve an fz_output for the default debugging stream. On
	Windows this will be OutputDebugString for non-console apps.
	Otherwise, it is always fz_stderr.

	Optionally may be fz_dropped when finished with.
*/
fz_output *fz_stddbg(fz_context *ctx);

/**
	Return TRUE when the specified `fz_output` stream is one of the global stdout/stderr/stddbg.
*/
int fz_channel_is_any_stdout(fz_context *ctx, fz_output *stream);


/**
	Format and write data to an output stream.
	See fz_format_string for formatting details.
	Does not write zero terminator.
*/
void fz_write_printf(fz_context *ctx, fz_output *out, const char* fmt, ...);

/**
	va_list version of fz_write_printf.
*/
void fz_write_vprintf(fz_context *ctx, fz_output *out, const char *fmt, va_list ap);

/**
	Write a series of strings (terminated by a NULL pointer) to the output.

	Extra: when one of the strings starts or ends with a `\n` NEWLINE, the written
	string data will be flushed immediately at the end of this call. This is very
	useful for logging systems, which can use a bit of I/O buffering for improved
	performance but where you might want to see the data ASAP on a line-by-line
	basis.
*/
void fz_write_strings(fz_context* ctx, fz_output* out, ...);

/**
	va_list version of fz_write_printf.
*/
void fz_vwrite_strings(fz_context* ctx, fz_output* out, va_list ap);

/**
	Seek to the specified file position.
	See fseek for arguments.

	Throw an error on unseekable outputs.
*/
void fz_seek_output(fz_context *ctx, fz_output *out, int64_t off, int whence);

/**
	Return the current file position.

	Throw an error on untellable outputs.
*/
int64_t fz_tell_output(fz_context *ctx, fz_output *out);

/**
	Flush unwritten data.
*/
void fz_flush_output(fz_context *ctx, fz_output *out);
void fz_flush_output_no_lock(fz_context *ctx, fz_output *out);

/**
	Flush pending output and close an output stream.
*/
void fz_close_output(fz_context *, fz_output *);

/**
	Free an output stream. Don't forget to close it first!
*/
void fz_drop_output(fz_context *, fz_output *);

/**
	Query whether a given fz_output supports fz_stream_from_output.
*/
int fz_output_supports_stream(fz_context *ctx, fz_output *out);

/**
	Obtain the fz_output in the form of a fz_stream.

	This allows data to be read back from some forms of fz_output
	object. When finished reading, the fz_stream should be released
	by calling fz_drop_stream. Until the fz_stream is dropped, no
	further operations should be performed on the fz_output object.
*/
fz_stream *fz_stream_from_output(fz_context *, fz_output *);

/**
	Truncate the output at the current position.

	This allows output streams which have seeked back from the end
	of their storage to be truncated at the current point.
*/
void fz_truncate_output(fz_context *, fz_output *);

/**
	Set the file write/output mode for any file created/written to
	disk from now on.

	enable: when non-zero (TRUE), files are created using 'safe mode'
	(see below), while zero (FALSE) instructs to use classic
	'direct write' mode instead.

	## Safe Mode

	'Safe mode' works as long as your file system (and target directory
	access permissions) allow file name **rename**. Only rarely is
	this right not granted.

	'Safe mode' will create any target file using the following procedure:

	- any file is opened for create/write mode, with the extra extension
	  '.tmp' appended to the given filename.
	- data is written as usual.
	- when `close` is invoked (signaling a successful completion of the
	  create/write process), the '.tmp' file is closed as usual.

	  **Then** the '.tmp'-extended filename is **renamed** to the
	  original specified target filename.

	  When the original specified target filename already exists at
	  the time rename, this is considered a collision failure and will
	  be reported by throwing an exception, so the userland code can
	  deal with this particular situation.

	- when `drop` is invoked without a preceding `close` call, this signals
	  the file create/write process did not succeed entirely, hence the
	  output file is surely broken/incomplete.

	  In this case, the '.tmp' file will be closed as usual, but rather
	  rather then renaming to the target filename, the '.tmp' file is
	  **deleted** subsequently, which we consider a file system cleanup action.

	Note/Aside:

	`fz_new_output_with_path(..., append)` signals the library
	you want to **append**, rather than **create/overwrite**, the target file.
	Here 'safe mode' is slightly less 'safe' in extreme conditions, but
	in order to spare the HDD/SSD and keep our I/O bandwidth low, we DO NOT
	copy the existing target file to the '.tmp' file on creation (as one
	might expect from a naive implementation), but instead the '.tmp' file
	only writes the appended material (effectively making 'append mode'
	a 'create/write' mode while writing to the output), which is finally
	*appended* to the original target file when 'close' is invoked: this
	reduces the total I/O bytecount to O(3N): 1 write + (1 read + 1 append)_on_close.

	The naive implementation would have cost O(M+N) instead: copy M old/existing + 1 write (+ 1 rename).
	As we expect 'M' to be large/huge, compared to the amount of data we expect to append,
	the append-on-close scheme was chosen to reduce I/O costs (assuming 3N < M+N for the average case).

	Note:

	Userland code can register custom `on_create`, `on_create_append`, `on_success`, `on_collision` and
	`on_fail` handlers to tweak the behaviour of the code during those phases
	described above, thus giving userland code extensive control over
	custom 'safe mode' behaviours, via `fz_set_custom_safe_file_output_mode_handlers()`.
*/
void fz_set_safe_file_output_mode(int enable);
/**
    See the `fz_set_safe_file_output_mode()` notes.
 */
typedef int (fz_output_safe_mode_on_event_fn)(fz_context *ctx, fz_output *out);
/**
	See the `fz_set_safe_file_output_mode()` notes.
 */
void fz_set_custom_safe_file_output_mode_handlers(fz_output_safe_mode_on_event_fn *on_create, fz_output_safe_mode_on_event_fn *on_create_append, fz_output_safe_mode_on_event_fn *on_success, fz_output_safe_mode_on_event_fn *on_rename_collision, fz_output_safe_mode_on_event_fn *on_fail);

/**
	Write data to output.

	data: Pointer to data to write.
	size: Size of data to write in bytes.
*/
void fz_write_data(fz_context *ctx, fz_output *out, const void *data, size_t size);
void fz_write_buffer(fz_context *ctx, fz_output *out, fz_buffer *data);

/**
	Write a string. Does not write zero terminator.
*/
void fz_write_string(fz_context *ctx, fz_output *out, const char *s);

/**
	Write different sized data to an output stream.
*/
void fz_write_int64_be(fz_context *ctx, fz_output *out, int64_t x);
void fz_write_int64_le(fz_context *ctx, fz_output *out, int64_t x);
void fz_write_uint64_be(fz_context *ctx, fz_output *out, uint64_t x);
void fz_write_uint64_le(fz_context *ctx, fz_output *out, uint64_t x);
void fz_write_int32_be(fz_context *ctx, fz_output *out, int x);
void fz_write_int32_le(fz_context *ctx, fz_output *out, int x);
void fz_write_uint32_be(fz_context *ctx, fz_output *out, unsigned int x);
void fz_write_uint32_le(fz_context *ctx, fz_output *out, unsigned int x);
void fz_write_int16_be(fz_context *ctx, fz_output *out, int x);
void fz_write_int16_le(fz_context *ctx, fz_output *out, int x);
void fz_write_uint16_be(fz_context *ctx, fz_output *out, unsigned int x);
void fz_write_uint16_le(fz_context *ctx, fz_output *out, unsigned int x);
void fz_write_char(fz_context *ctx, fz_output *out, char x);
void fz_write_byte(fz_context *ctx, fz_output *out, unsigned char x);
void fz_write_byte_no_lock(fz_context *ctx, fz_output *out, unsigned char x);
void fz_write_float_be(fz_context *ctx, fz_output *out, float f);
void fz_write_float_le(fz_context *ctx, fz_output *out, float f);

/**
	Write a UTF-8 encoded unicode character.
*/
void fz_write_rune(fz_context *ctx, fz_output *out, int rune);

/**
	Write a base64 encoded data block, optionally with periodic
	newlines.
*/
void fz_write_base64(fz_context *ctx, fz_output *out, const unsigned char *data, size_t size, int newline);

/**
	Write a base64 encoded fz_buffer, optionally with periodic
	newlines.
*/
void fz_write_base64_buffer(fz_context *ctx, fz_output *out, fz_buffer *data, int newline);

/**
	Write num_bits of data to the end of the output stream, assumed to be packed
	most significant bits first.
*/
void fz_write_bits(fz_context *ctx, fz_output *out, unsigned int data, int num_bits);

/**
	Sync to byte boundary after writing bits.
*/
void fz_write_bits_sync(fz_context *ctx, fz_output *out);

/**
	HEX nibble conversion lookup table. Used internally by fz_printf() et al.
*/
extern const char* fz_hex_digits;

/**
	Our customised 'printf'-like string formatter.
	Takes `%c`, `%d`, `%i`, `%s`, `%u`, `%x`, `%X` as usual and supports most POSIX printf()
	features. Where it differs is with the extra formatters (e.g. `%H`), the extra
	modifiers (e.g. `j`) and the handling of *negative* precision values.

	Most usual modifiers are supported:
	zero-padding integers (e.g.	`%02d`, `%03u`, `%04x`, etc),
	integer sign formatting flags (e.g.	`%+d`, `% d`),
	left justification (e.g. `%-5s`, `%-8d`) and
	width, both static width (e.g. `%5d`) and dynamic width (e.g. `%*d`).
	The `*` modifier expects an extra `int` type argument, as usual.

	The 'precision' modifier is also supported, both static precision
	(e.g. `%.2f`) and dynamic precision (e.g. `%.*f`), where the latter
	expects an extra `int` type argument, as usual.

	Precision is applied to floating point values (`%e` and `%f`), strings (`%s` and `%S`)
	(e.g. `%.3s`, `%.*s`) and `%H`, `%Q`, `%q`.

	Also note that `%c` and `%C` have a special	'repeat mode' which is controlled through
	using negative precision: see further below.

	*Negative* values for precision can be specified using the dynamic modifier (e.g. `%.*s`)
	and have special meaning for both `%s` and `%S` and whether or not you use the `j` (JSON) modifier:
	when the `j` (JSON) modifier is used, then a *negative precision* will be interpreted
	as the *negate* of the `PDF_PRINT_JSON_***` flags, including `PDF_PRINT_JSON_DEPTH_LEVEL`.
	This allows for a powerful shorthand where we use `fz_format_string()` as a powerful
	means to produce JSON-compliant output. (See also:
	`PDF_PRINT_JSON_BINARY_DATA_AS_HEX_PLUS_RAW`, `PDF_PRINT_JSON_ILLEGAL_UNICODE_AS_HEX`,
	`PDF_PRINT_JSON_BINARY_DATA_AS_PURE_HEX`, `PDF_PRINT_JSON_DEPTH_LEVEL(n)`.)

	These modifiers may be mixed (e.g. `%-+*.*f`).

	`%g` outputs in "as short as possible hopefully lossless non-exponent" form, see `fz_ftoa` for specifics.
	`%g` ignores all size/sign/precision modifiers.

	`%f` and `%e` output as usual.

	`%x` and `%X` print an integer number as hexadecimal value, as usual. (lowercase and uppercase, resp.)

	`%d` and `%i` print a signed integer number as decimal value, as usual.

	`%u` prints an unsigned integer number as decimal value, as usual.

	`%B` prints an unsigned integer number as *binary* value, e.g. `%B` of `13` will print `1101`.

	`%C` outputs a utf8 encoded int, i.e. output a Unicode codepoint verbatim.
	When precision has been specified, but is NEGATIVE, than this is a special mode:
	the Unicode codepoint is copied verbatim `ABS(p)` times into the output; otherwise
	it is -- of course -- copied only once.

	`%c` outputs a character encoded in a single byte. (Argument type is `int`, though).
	When precision has been specified, but is NEGATIVE, than this is a special mode:
	the character (byte) is copied verbatim `ABS(p)` times into the output; otherwise
	it is -- of course -- copied only once.

	`%p` output as usual: a '0x'-prefixed hex representation of the `void*` pointer.

	`%s` prints a `const char*` string value, as usual.
	When the string value is `NULL`, it is printed as `(null)`, while any clipping precision
	value (smaller than 6) will be disabled to ensure the entire string `(null)` makes it
	into the output, e.g. `%2.2s` of `"foobar"` will print `"fo"`, but given the value `NULL`,
	it will print `"(null)"`, disregarding the size & precision `2` values in that format spec.

	When precision has been specified, but is NEGATIVE, than this is a special mode:
	the code will discover how to best print the data buffer, using the `-p` negated value
	as a `PDF_PRINT_JSON_***` flags value (see above), while Unicode codepoints in the byte buffer
	will be output verbatim (i.e. no `\uXXXX` Unicode escapes, not even in JSON mode!). Also
	the 'usual whitespace' (`\r`, `\n`, `\t`, `\f`, `\b`) will be printed verbatim.
	When non-negative, the precision value is treated as usual for `%s`, hence it serves as
	a length limiting ("clipping") value, as usual.

	A note about the `j` modifier: when the precision is not negative, i.e. we're using `%s`
	as usual and *not* as a fancy (hex)dumper of arbitrary string content, then the `j`
	modifier is *ignored*: you should use `%q` and `%Q` instead to print JSON strings, as those
	will include the required escaping of some characters (quotes, most importantly). Do note that printing a `NULL`
	string value using `%q` or `%Q` will render as an empty string instead of `null`: if you need
	to print `null` values, you must do so explicitly.

	`%M` outputs a `fz_matrix *` as a series of 6 `%g` values: `.a .b .c .d .e .f`.

	`%R` outputs a `fz_rect *` as a series of 4 `%g` values: `.x0 .y0 .x1 .y1`.

	`%P` outputs a `fz_point *` as a series of 2 `%g` values: `.x .y`.

	`%Z` outputs a `fz_quad *` as a series of 8 `%g` values: `.ul.x .ul.y .ur.x .ur.y .ll.x .ll.y .lr.x .lr.y`.

	The `,` comma modifier for `%M`/`%R`/`%P`/`%Z` will print a comma+space separator
	between the values instead of only the default single space.

	`%T` outputs an `in64_t` as time (`time_t`, UTC) using the strftime() format: "D:%Y-%m-%d %H:%M:%S UTC".
	Invalid/unparseable timestamps will print as `(invalid)`.

	`%H` outputs a byte buffer in hex. (argument passed as `void*` pointer + `size_t` length).
	When precision has been specified, but is NEGATIVE, than this is a special mode:
	the code will discover how to best print the data buffer, using the `-p` negated value
	as a `PDF_PRINT_JSON_***` flags value (see above), while Unicode Codepoints in the byte buffer
	will be output verbatim (i.e. no `\uXXXX` Unicode escapes, not even in JSON mode!).

	When precision is non-negative, its value is treated as a `PDF_PRINT_JSON_***` flags value
	WITHOUT the verbatim copying of any Unicode Codepoints in the byte buffer, hence Unicode
	characters outside the ASCII printable range will then be `\uXXXX` escaped in the output.

	`%n` outputs a `const char *` string value as a PDF name (with appropriate escaping).

	`%(` outputs escaped strings in C/PDF syntax.
	Printing a `NULL` string value using `%(` will render as an empty string instead of `(null)`.

	`%Q` outputs a string value as a loosely JSON-compliant quoted string
	with all Unicode codepoints output verbatim (UTF8 encoded).
	Do note that printing a `NULL` string value using `%Q` will render as an empty string instead of `null`:
	if you need	to print `null` values, you must do so explicitly or use `%s` instead.
	Illegal Unicode UTF8 sequences in the input string value will be output as a set of the INVALID CHARACTER `\uFFFD` plus a
	single illegal byte escaped as hex `\xNN` or Unicode `\u00NN` value.
	The `j` modifier specifies that `%Q` will never output `\xNN` hex escape codes but always use `\u00NN` Unicode escapes
	instead for full JSON-compliance as some JSON parsers don't accept `\xNN` encodings, only `\u00NN`.

	`%q` outputs a string value as a fully JSON-compliant quoted string
	with all non-ASCII-printable Unicode codepoints output as `\uNNNN`/`\uNNNNNN` escapes.
	Do note that printing a `NULL` string value using `%q` will render as an empty string instead of `null`:
	if you need	to print `null` values, you must do so explicitly or use `%s` instead.
	Illegal Unicode UTF8 sequences in the input string value will be output as a set of the INVALID CHARACTER `\uFFFD` plus a
	single illegal byte escaped as hex `\xNN` or Unicode `\u00NN` value.
	The `j` modifier specifies that `%q` will never output `\xNN` hex escape codes but always use `\u00NN` Unicode escapes
	instead for full JSON-compliance as some JSON parsers don't accept `\xNN` encodings, only `\u00NN`.

	The `j` modifier ("JSON mode") signals to print control characters
	in JSON-compliant format as \u00NN escapes instead of \xNN
	hex escapes.
	The `j` modifier signals all commands (exceptions listed below) to print
	quotes surrounding the output (thus producing a JSON-compliant
	string). This includes the numeric and array types `%e`, `%f`, `%g`, `%d`, `%i`, `%u`, `%x`, `%X`, `%B`, `%R`, `%Z`, `%P`, `%M`.
	E.g. `%jd` of `5` will produce the quoted string `"5"`, `%,jP` will produce the string value `"12, 42"` for value `{12, 42}`.
	Exceptions to this rule are `%(`, which ignores the `j` modifier entirely, and `%s`, which only uses
	the `j` modifier when printing non-`NULL`-input under *negative* precision conditions: see `%s` above.

	`%l{d,i,u,x,X,B}` indicates that the values are `int64_t`.
	`%ll{d,i,u,x,X,B}` is treated as synonymous to %l{d,i,u,x,B}.
	`%t{d,i,u,x,X,B}` indicates that the value is a `ptrdiff_t`.
	`%z{d,i,u,x,X,B}` indicates that the value is a `size_t`.
	`%I64{d,i,u,x,X,B}` indicates that the value is a `uint64_t/int64_t`.
	`%I32{d,i,u,x,X,B}` indicates that the value is a `uint32_t/int32_t`.
	`%I16{d,i,u,x,X,B}` indicates that the value is a `uint16_t/int16_t`.

	The expected order of the modifiers is:
	- any of the { '-', '+', ' ', '0' } modifiers, in any order and any number of occurrences
	  (f.e., while the second `+` is useless, `%++d` is considered a valid format spec)
	- '<width>': either '*' or a non-negative decimal number, e.g. `5` in `%5d`
	- '.<precision>': a literal '.' dot, followed by either '*' or a non-negative decimal number, e.g. `5` in `%1.5f`
	- ',' for comma-separated values, where applicable (`%R`, `%M`, `%P`, `%Z` formats)
	- 'j' for JSON-compliant output.
	  DO NOTE that numeric formats are quoted too when the `j` modifier was specified, e.g. `%jd` will print `"125"`.
	- '<size>': one of 'l' , 'll', 't', 'z', 'I64', 'I32', 'I16'. Both 'l' and 'll' expect an `int64_t` compatible-sized type, 't' maps
	  to your system's `ptrdiff_t` type and `z` maps to your system's `size_t` type.

	Unrecognized `%` commands will be copied verbatim, *but without
	any recognized modifiers*. E.g. `%5K` will print `%K`.

	user: An opaque pointer that is passed to the emit function.

	emit: A function pointer called to emit output bytes as the
	string is being formatted.
*/
void fz_format_string(fz_context *ctx, void *user, void (*emit)(fz_context *ctx, void *user, int c), const char *fmt, va_list args);

/**
	A vsnprintf work-alike, using our custom formatter.

	fz_vsnprintf is guaranteed to NUL-terminate the output buffer.
	fz_vsnprintf can cope with a zero-length output buffer.
*/
size_t fz_vsnprintf(char *buffer, size_t space, const char *fmt, va_list args);

/**
	The non va_list equivalent of fz_vsnprintf.

	fz_snprintf is guaranteed to NUL-terminate the output buffer.
	fz_snprintf can cope with a zero-length output buffer.
*/
size_t fz_snprintf(char *buffer, size_t space, const char* fmt, ...);

/**
	Allocated sprintf.

	Returns a null terminated allocated block containing the
	formatted version of the format string/args.
*/
char *fz_asprintf(fz_context *ctx, const char* fmt, ...);

/**
	Save the contents of a buffer to a file.
*/
void fz_save_buffer(fz_context *ctx, fz_buffer *buf, const char *filename);

/**
	Compression and other filtering outputs.

	These outputs write encoded data to another output. Create a
	filter output with the destination, write to the filter, then
	close and drop it when you're done. These can also be chained
	together, for example to write ASCII Hex encoded, Deflate
	compressed, and RC4 encrypted data to a buffer output.

	Output streams don't use reference counting, so make sure to
	close all of the filters in the reverse order of creation so
	that data is flushed properly.

	Accordingly, ownership of 'chain' is never passed into the
	following functions, but remains with the caller, whose
	responsibility it is to ensure they exist at least until
	the returned fz_output is dropped.
*/

fz_output *fz_new_asciihex_output(fz_context *ctx, fz_output *chain);
fz_output *fz_new_ascii85_output(fz_context *ctx, fz_output *chain);
fz_output *fz_new_rle_output(fz_context *ctx, fz_output *chain);
fz_output *fz_new_arc4_output(fz_context *ctx, fz_output *chain, unsigned char *key, size_t keylen);
fz_output *fz_new_deflate_output(fz_context *ctx, fz_output *chain, int effort, int raw);

#ifdef __cplusplus
}
#endif

#endif
