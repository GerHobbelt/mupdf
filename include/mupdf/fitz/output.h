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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

#ifndef MUPDF_FITZ_OUTPUT_H
#define MUPDF_FITZ_OUTPUT_H

#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"
#include "mupdf/fitz/buffer.h"
#include "mupdf/fitz/string-util.h"
#include "mupdf/fitz/stream.h"

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

	state: The state for the output stream.

	data: a pointer to a buffer of data to write.

	n: The number of bytes of data to write.
*/
typedef void (fz_output_write_fn)(fz_context *ctx, void *state, const void *data, size_t n);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called when
	fz_seek_output is requested.

	state: The output stream state to seek within.

	offset, whence: as defined for fs_seek_output.
*/
typedef void (fz_output_seek_fn)(fz_context *ctx, void *state, int64_t offset, int whence);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called when
	fz_tell_output is requested.

	state: The output stream state to report on.

	Returns the offset within the output stream.
*/
typedef int64_t (fz_output_tell_fn)(fz_context *ctx, void *state);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when the output stream is closed, to flush any pending writes.
*/
typedef void (fz_output_close_fn)(fz_context *ctx, void *state);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when the output stream is dropped, to release the stream
	specific state information.
*/
typedef void (fz_output_drop_fn)(fz_context *ctx, void *state);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when the fz_stream_from_output is called.
*/
typedef fz_stream *(fz_stream_from_output_fn)(fz_context *ctx, void *state);

/**
	A function type for use when implementing
	fz_outputs. The supplied function of this type is called
	when fz_truncate_output is called to truncate the file
	at that point.
*/
typedef void (fz_truncate_fn)(fz_context *ctx, void *state);

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
	struct fz_secondary_outputs secondary;
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

#ifdef _WIN32
/**
	Retrieve an fz_output that directs to OutputDebugString.

	Optionally may be fz_dropped when finished with.
*/
fz_output *fz_stdods(fz_context *ctx);
#endif

/**
	Set the output stream to be used for fz_stddbg. Set to NULL to
	reset to default (stderr).
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
	HEX nibble conversion lookup table. Used internally by fz_printf() et al.
*/
extern const char* fz_hex_digits;

/**
	Our customised 'printf'-like string formatter.
	Takes `%c`, `%d`, `%i`, `%s`, `%u`, `%x` as usual.

	Most usual modifiers are supported:
	zero-padding integers (e.g.	`%02d`, `%03u`, `%04x`, etc),
	integer sign formatting flags (e.g.	`%+d`, `% d`),
	left justification (e.g. `%-5s`, `%-8d`) and
	width, both static width (e.g. `%5d`) and dynamic width (e.g. `%*d`).
	The `*` modifier expects an extra `int` type argument, as usual.

	The 'precision' modifier is also supported, both static precision
	(e.g. `%.2f`) and dynamic precision (e.g. `%.*f`), where the latter
	expects an extra `int` type argument, as usual.

	Precision is applied to floating point values (`%e` and `%f`), strings (`%s`)
	(e.g. `%.3s`, `%.*s`) and `%H`, `%Q`, `%q`.

	*Negative* values for precision can be specified using the dynamic modifier (e.g. `%.*s`)
	and have special meaning for `%s` and when using the `j` (JSON) modifier:
	when the `j` (JSON) modifier is used, then a *negative precision* will be interpreted
	as the *negate* of the `PDF_PRINT_JSON_***` flags, including `PDF_PRINT_JSON_DEPTH_LEVEL`.
	This allows for a powerful shorthand where we use `fz_format_string()` as a powerful
	means to produce JSON-compliant output. (See also:
	`PDF_PRINT_JSON_BINARY_DATA_AS_HEX_PLUS_RAW`, `PDF_PRINT_JSON_ILLEGAL_UNICODE_AS_HEX`,
	`PDF_PRINT_JSON_BINARY_DATA_AS_PURE_HEX`, `PDF_PRINT_JSON_DEPTH_LEVEL(n)`.)

	These modifiers may be mixed (e.g. `%-+*.*f`).

	`%g` output in "as short as possible hopefully lossless
	non-exponent" form, see `fz_ftoa` for specifics.
	`%g` ignores all size/sign/precision modifiers.

	`%f` and `%e` output as usual.

	`%x` prints an integer number as hexadecimal value, as usual.

	`%d` and `%i` print a signed integer number as decimal value, as usual.

	`%u` prints an unsigned integer number as decimal value, as usual.

	`%B` prints an unsigned integer number as *binary* value, e.g. `%B` of `13` will print `1101`.

	`%C` outputs a utf8 encoded int, i.e. output a Unicode codepoint verbatim.

	`%c` outputs a character encoded in a single byte. (Argument type is `int`, though).

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
	a length limiting ("clipping") value.
	A note about the `j` modifier: when the precision is not negative, i.e. we're using `%s`
	as usual and *not* as a fancy (hex)dumper of arbitrary string content, then the `j`
	modifier is *ignored*: you should use `%q` and `%Q` instead to print JSON strings, as those
	will include the required escaping of some characters. Do note that printing a `NULL`
	string value using `%q` or `%Q` will render as an empty string instead of `null`: if you need
	to print `null` values, you must do so explicitly.

	`%M` outputs a `fz_matrix*` as a series of `%g` values.

	`%R` outputs a `fz_rect*` as a series of `%g` values.

	`%P` outputs a `fz_point*` as a series of `%g` values.

	`%Z` outputs a `fz_quad*` as a series of `%g` values.

	The `,` comma modifier for `%M`/`%R`/`%P`/`%Z` will print a comma+space separator
	between the values instead of only a space.

	`%T` outputs an `in64_t` as time (`time_t`, UTC).
	Invalid/unparseable timestamps will print as `(invalid)`.

	`%H` outputs a byte buffer in hex. (argument passed as `void*` pointer + `size_t` length).
	When precision has been specified, but is NEGATIVE, than this is a special mode:
	the code will discover how to best print the data buffer, using the `-p` negated value
	as a `PDF_PRINT_JSON_***` flags value (see above), while Unicode Codepoints in the byte buffer
	will be output verbatim (i.e. no `\uXXXX` Unicode escapes, not even in JSON mode!).
	When non-negative, the precision value is treated as a `PDF_PRINT_JSON_***` flags value
	WITHOUT the verbatim copying of any Unicode Codepoints in the byte buffer.

	`%n` outputs a `const char *` string value as a PDF name (with appropriate escaping).

	`%(` outputs escaped strings in C/PDF syntax.
	Printing a `NULL` string value using `%(` will render as an empty string instead of `(null)`.

	`%Q` outputs a string value as a loosely JSON-compliant quoted string
	with all Unicode codepoints output verbatim (UTF8 encoded).
	Do note that printing a `NULL` string value using `%Q` will render as an empty string instead of `null`:
	if you need	to print `null` values, you must do so explicitly.
	Illegal Unicode UTF8 sequences in the input string value will be output as a set of the INVALID CHARACTER `\uFFFD` plus a
	single illegal byte escaped as hex `\xNN` or Unicode `\u00NN` value.
	The `j` modifier specifies that `%Q` will never output `\xNN` hex escape codes but always use `\u00NN` Unicode escapes
	instead for full JSON-compliance as some JSON parsers don't accept `\xNN` encodings, only `\u00NN`.

	`%q` outputs a string value as a fully JSON-compliant quoted string
	with all non-ASCII-printable Unicode codepoints output as `\uNNNN`/`\uNNNNNN` escapes.
	Do note that printing a `NULL` string value using `%q` will render as an empty string instead of `null`:
	if you need	to print `null` values, you must do so explicitly.
	Illegal Unicode UTF8 sequences in the input string value will be output as a set of the INVALID CHARACTER `\uFFFD` plus a
	single illegal byte escaped as hex `\xNN` or Unicode `\u00NN` value.
	The `j` modifier specifies that `%q` will never output `\xNN` hex escape codes but always use `\u00NN` Unicode escapes
	instead for full JSON-compliance as some JSON parsers don't accept `\xNN` encodings, only `\u00NN`.

	The `j` modifier ("JSON mode") signals to print control characters
	in JSON-compliant format as \u00NN escapes instead of \xNN
	hex escapes.
	The `j` modifier signals all commands (exceptions listed below) to print
	quotes surrounding the output (thus producing a JSON-compliant
	string). This includes the numeric and array types `%e`, `%f`, `%g`, `%d`, `%i`, `%u`, `%x`, `%B`, `%R`, `%Z`, `%P`, `%M`.
	E.g. `%jd` of `5` will produce the quoted string `"5"`, `%,jP` will produce the string value `"12, 42"` for value `{12, 42}`.
	Exceptions to this rule are `%(`, which ignores the `j` modifier entirely, and `%s`, which only uses
	the `j` modifier when printing non-`NULL`-input under *negative* precision conditions: see `%s` above.

	`%l{d,i,u,x,B}` indicates that the values are `int64_t`.
	`%ll{d,i,u,x,B}` is treated as synonymous to %l{d,i,u,x,B}.
	`%t{d,i,u,x,B}` indicates that the value is a `ptrdiff_t`.
	`%z{d,i,u,x,B}` indicates that the value is a `size_t`.

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
