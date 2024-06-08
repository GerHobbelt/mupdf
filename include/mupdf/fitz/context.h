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

#ifndef MUPDF_FITZ_CONTEXT_H
#define MUPDF_FITZ_CONTEXT_H

#include "mupdf/fitz/config.h"
#include "mupdf/fitz/version.h"
#include "mupdf/fitz/system.h"
#include "mupdf/fitz/geometry.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* Macros to assist location tracing of allocated heap memory in debug builds a la MSVC crtdbg API:
*/
#if defined(DEBUG) || defined(_DEBUG) || defined(FZ_HEAP_TRACING)

// for function prototypes:
#define FZDBG_DECL_ARGS				, const char *trace_srcfile, int trace_srcline
// for passing these through into function calls
#define FZDBG_PASS					, trace_srcfile, trace_srcline
// for those places where we wish to start the trace and invoke the heap alloc functions:
#define FZDBG_THIS_POS()			, __FILE__, __LINE__
// for the benefit of expanded macros which themselves don't accept our irregular approach:
#define FZDBG_DECL_THIS_POS()		static const char *trace_srcfile = __FILE__; static const int trace_srcline = __LINE__

#define FZDBG_HAS_TRACING 1

#else

#define FZDBG_DECL_ARGS     
#define FZDBG_PASS			
#define FZDBG_THIS_POS()		
#define FZDBG_DECL_THIS_POS()		(void)0

#endif

#ifndef FZ_VERBOSE_EXCEPTIONS
#define FZ_VERBOSE_EXCEPTIONS 0
#endif

typedef struct fz_font_context fz_font_context;
typedef struct fz_colorspace_context fz_colorspace_context;
typedef struct fz_style_context fz_style_context;
typedef struct fz_tuning_context fz_tuning_context;
typedef struct fz_store fz_store;
typedef struct fz_glyph_cache fz_glyph_cache;
typedef struct fz_document_handler_context fz_document_handler_context;
typedef struct fz_secondary_outputs fz_secondary_outputs;
typedef struct fz_archive_handler_context fz_archive_handler_context;
typedef struct fz_output fz_output;
typedef struct fz_context fz_context;
typedef struct fz_cookie fz_cookie;

/**
    Allocator structure; holds callbacks and private data pointer.
*/
typedef struct
{
    void *user;
    void *(*malloc_)(void *user, size_t size   FZDBG_DECL_ARGS);
    void *(*realloc_)(void *user, void *old_ptr, size_t size   FZDBG_DECL_ARGS);
    void (*free_)(void *user, void *old_ptr);
} fz_alloc_context;

/**
    Exception macro definitions. Just treat these as a black box -
    pay no attention to the man behind the curtain.
*/
#define fz_var(var) fz_var_imp((void *)&(var))
#define fz_try(ctx) if (!fz_setjmp(*fz_push_try(ctx))) if (fz_do_try(ctx)) do
#define fz_always(ctx) while (0); if (fz_do_always(ctx)) do
#define fz_catch(ctx) while (0); if (fz_do_catch(ctx))

/**
	These macros provide a simple exception handling system. Use them as
	follows:

	fz_try(ctx)
		...
	fz_catch(ctx)
		...

	or as:

	fz_try(ctx)
		...
	fz_always(ctx)
		...
	fz_catch(ctx)
		...

	Code within the fz_try() section can then throw exceptions using fz_throw()
	(or fz_vthrow()).

	They are implemented with setjmp/longjmp, which can have unfortunate
	consequences for 'losing' local variable values on a throw. To avoid this
	we recommend calling 'fz_var(variable)' before the fz_try() for any
	local variable whose value may change within the fz_try() block and whose
	value will be required afterwards.

	Do not call anything in the fz_always() section that can throw.

	Any exception can be rethrown from the fz_catch() section using fz_rethrow()
	as long as there has been no intervening use of fz_try/fz_catch.
*/

/**
	Throw an exception.

	This assumes an enclosing fz_try() block within the callstack.
*/
FZ_NORETURN void fz_vthrow(fz_context *ctx, int errcode, const char *fmt, va_list ap);
FZ_NORETURN void fz_throw(fz_context *ctx, int errcode, const char *fmt, ...);
FZ_NORETURN void fz_rethrow(fz_context *ctx);

/**
	Called within a catch block this modifies the current
	exception's code. If it's of type 'fromcode' it is
	modified to 'tocode'. Typically used for 'downgrading'
	exception severity.
*/
void fz_morph_error(fz_context *ctx, int fromcode, int tocode);

/**
	Log an error.

	This goes to the registered error stream (stderr by
	default).
*/
void fz_verror(fz_context* ctx, const char* fmt, va_list ap);
void fz_error(fz_context* ctx, const char* fmt, ...);
void fz_write_error_line(fz_context* ctx, const char* line);

/**
	Log a warning.

	This goes to the registered warning stream (stderr by
	default).
*/
void fz_vwarn(fz_context *ctx, const char *fmt, va_list ap);
void fz_warn(fz_context *ctx, const char *fmt, ...);
void fz_write_warn_line(fz_context* ctx, const char* line);

/**
	Log an informational notice.

	This goes to the registered information/notices stream (stderr by
	default).
*/
void fz_vinfo(fz_context* ctx, const char* fmt, va_list ap);
void fz_info(fz_context* ctx, const char* fmt, ...);
void fz_write_info_line(fz_context* ctx, const char* line);

/**
	Log a debug message.

	This goes to the registered debug stream (stderr by
	default).
*/
void fz_vdebug(fz_context* ctx, const char* fmt, va_list ap);
void fz_debug(fz_context* ctx, const char* fmt, ...);
void fz_write_debug_line(fz_context* ctx, const char* line);

/**
	Within an fz_catch() block, retrieve the formatted message
	string for the current exception.

	This assumes no intervening use of fz_try/fz_catch.

	See also fz_report_error() and fz_convert_error().
*/
const char *fz_caught_message(fz_context *ctx);

/**
	Within an fz_catch() block, retrieve the error code for
	the current exception.

	This assumes no intervening use of fz_try/fz_catch.
*/
int fz_caught(fz_context *ctx);

/*
	Within an fz_catch() block, retrieve the errno code for
	the current SYSTEM exception.

	Is undefined for non-SYSTEM errors.
*/
int fz_caught_errno(fz_context *ctx);

/**
	Within an fz_catch() block, rethrow the current exception
	if the errcode of the current exception matches.

	This assumes no intervening use of fz_try/fz_catch.
*/
void fz_rethrow_if(fz_context *ctx, int errcode);
void fz_rethrow_unless(fz_context *ctx, int errcode);

/**
	Format an error message, and log it to the registered
	error stream (stderr by default).
*/
void fz_log_error_printf(fz_context *ctx, const char *fmt, ...) FZ_PRINTFLIKE(2,3);
void fz_vlog_error_printf(fz_context *ctx, const char *fmt, va_list ap);

/**
	Log a (preformatted) string to the registered
	error stream (stderr by default).
*/
void fz_log_error(fz_context *ctx, const char *str);

void fz_start_throw_on_repair(fz_context *ctx);
void fz_end_throw_on_repair(fz_context *ctx);

/**
	Now, a debugging feature. If FZ_VERBOSE_EXCEPTIONS is 1 then
	some of the above functions are replaced by versions that print
	FILE and LINE information.
*/
#if FZ_VERBOSE_EXCEPTIONS

#define fz_vthrow(CTX, ERRCODE, FMT, VA) fz_vthrowFL(CTX, __FILE__, __LINE__, ERRCODE, FMT, VA)
#define fz_throw(CTX, ERRCODE, ...) fz_throwFL(CTX, __FILE__, __LINE__, ERRCODE, __VA_ARGS__)
#define fz_rethrow(CTX) fz_rethrowFL(CTX, __FILE__, __LINE__)
#define fz_morph_error(CTX, FROM, TO) fz_morph_errorFL(CTX, __FILE__, __LINE__, FROM, TO)
#define fz_vwarn(CTX, FMT, VA) fz_vwarnFL(CTX, __FILE__, __LINE__, FMT, VA)
#define fz_warn(CTX, ...) fz_warnFL(CTX, __FILE__, __LINE__, __VA_ARGS__)
#define fz_rethrow_if(CTX, ERRCODE) fz_rethrow_ifFL(CTX, __FILE__, __LINE__, ERRCODE)
#define fz_rethrow_unless(CTX, ERRCODE) fz_rethrow_unlessFL(CTX, __FILE__, __LINE__, ERRCODE)
#define fz_log_error_printf(CTX, ...) fz_log_error_printfFL(CTX, __FILE__, __LINE__, __VA_ARGS__)
#define fz_vlog_error_printf(CTX, FMT, VA) fz_log_error_printfFL(CTX, __FILE__, __LINE__, FMT, VA)
#define fz_log_error(CTX, STR) fz_log_error_printfFL(CTX, __FILE__, __LINE__, STR)
#define fz_do_catch(CTX) fz_do_catchFL(CTX, __FILE__, __LINE__)
FZ_NORETURN void fz_vthrowFL(fz_context *ctx, const char *file, int line, int errcode, const char *fmt, va_list ap);
FZ_NORETURN void fz_throwFL(fz_context *ctx, const char *file, int line, int errcode, const char *fmt, ...) FZ_PRINTFLIKE(5,6);
FZ_NORETURN void fz_rethrowFL(fz_context *ctx, const char *file, int line);
void fz_morph_errorFL(fz_context *ctx, const char *file, int line, int fromcode, int tocode);
void fz_vwarnFL(fz_context *ctx, const char *file, int line, const char *fmt, va_list ap);
void fz_warnFL(fz_context *ctx, const char *file, int line, const char *fmt, ...) FZ_PRINTFLIKE(4,5);
void fz_rethrow_ifFL(fz_context *ctx, const char *file, int line, int errcode);
void fz_rethrow_unlessFL(fz_context *ctx, const char *file, int line, int errcode);
void fz_log_error_printfFL(fz_context *ctx, const char *file, int line, const char *fmt, ...) FZ_PRINTFLIKE(4,5);
void fz_vlog_error_printfFL(fz_context *ctx, const char *file, int line, const char *fmt, va_list ap);
void fz_log_errorFL(fz_context *ctx, const char *file, int line, const char *str);
int fz_do_catchFL(fz_context *ctx, const char *file, int line);
#endif

/*
* IMPORTANT NOTE:
* 
* The following calls do reset the error/exception error code and, as such, serve as
* 'handlers' of any fitz/pdf exception:
* 
* - fz_report_error()
* - fz_ignore_error()
* - fz_convert_error()
*
* After an exception is caught using `fz_catch()` you MUST call any one of the above.
* If you don't, the exception will remain marked as 'pending' and will be reported
* as UNHANDLED by the time the context `ctx` is dropped by way of `fz_drop_context()`.
*
* Also note that these calls DO NOT erase the last exception/error message, which can
* be accessed any time before or after by calling `fz_caught_message()` without
* changing the 'pending/handled' state of the current exception/error.
*/

/* Report an error to the registered error callback. */
void fz_report_error(fz_context *ctx);

/*
 * Swallow an error and ignore it completely.
 * This should only be called to signal that you've handled a TRYLATER or ABORT error,
 */
void fz_ignore_error(fz_context *ctx);

/* Convert an error into another runtime exception.
 * For use when converting an exception from Fitz to a language binding exception.
 */
const char *fz_convert_error(fz_context *ctx, int *code);

enum fz_error_type
{
	FZ_ERROR_NONE,
	FZ_ERROR_GENERIC,

	FZ_ERROR_SYSTEM, // fatal out of memory or syscall error
	FZ_ERROR_LIBRARY, // unclassified error from third-party library
	FZ_ERROR_ARGUMENT, // invalid or out-of-range arguments to functions
	FZ_ERROR_LIMIT, // failed because of resource or other hard limits
	FZ_ERROR_UNSUPPORTED, // tried to use an unsupported feature
	FZ_ERROR_FORMAT, // syntax or format errors that are unrecoverable
	FZ_ERROR_SYNTAX, // syntax errors that should be diagnosed and ignored

	// for internal use only
	FZ_ERROR_TRYLATER, // try-later progressive loading signal
	FZ_ERROR_ABORT, // user requested abort signal
	FZ_ERROR_REPAIRED, // internal flag used when repairing a PDF to avoid cycles
    FZ_ERROR_NOT_A_PDF,
	
	FZ_ERROR_COUNT,

	FZ_ERROR_C_RTL_SERIES       = 0x10000000,	/// marks the error value as actually an `errno` error value stored in the context error state
	FZ_ERROR_C_RTL_SERIES_MASK  = 0x0FFFFFFF,	/// the mask for the part of the integer value where the actual `errno` value is stored.
	FZ_ERROR_SYS_SERIES         = 0x80000000,	/// marks the error value as actually a system error value stored in the context error state; e.g. a Win32 system error as may be produced by Win32 API `GetLastError()` (see also <winerror.h> ), stored in the context error state.
	FZ_ERROR_SYS_SERIES_MASK    = 0x7FFFFFFF,	/// the mask for the part of the integer value where the actual `sys_errno` value is stored.
};

/**
    Flush any repeated warnings.

    Repeated warnings are buffered, counted and eventually printed
    along with the number of repetitions. Call fz_flush_warnings
    to force printing of the latest buffered warning and the
    number of repetitions, for example to make sure that all
    warnings are printed before exiting an application.
*/
void fz_flush_warnings(fz_context *ctx);

/**
    Flush all standard logging channels: fz_stdout(), fz_stderr() and fz_stdods()

    This is invoked when an exception is thrown and when a fz_context is dropped:
    those are considered important enough events to merit flushing all buffers
    and ensuring the latest intel has been sent/saved.
*/
void fz_flush_all_std_logging_channels(fz_context* ctx);

/**
    Locking functions

    MuPDF is kept deliberately free of any knowledge of particular
    threading systems. As such, in order for safe multi-threaded
    operation, we rely on callbacks to client provided functions.

    A client is expected to provide FZ_LOCK_MAX number of mutexes,
    and a function to lock/unlock each of them. These may be
    recursive mutexes, but do not have to be.

    If a client does not intend to use multiple threads, then it
    may pass NULL instead of a lock structure.

    In order to avoid deadlocks, we have one simple rule
    internally as to how we use locks: We can never take lock n
    when we already hold any lock i, where 0 <= i <= n. In order
    to verify this, we have some debugging code, that can be
    enabled by defining FITZ_DEBUG_LOCKING.
*/

typedef struct
{
    void *user;
    void (*lock)(void *user, int lock);
    void (*unlock)(void *user, int lock);
} fz_locks_context;

enum {
    FZ_LOCK_ALLOC = 0,
    FZ_LOCK_FREETYPE,
    FZ_LOCK_GLYPHCACHE,
    FZ_LOCK_JPX,
    FZ_LOCK_JBIG2,
    FZ_LOCK_JBIG2_INTERNAL,
    FZ_LOCK_MAX
};

#if defined(MEMENTO) || !defined(NDEBUG)
#define FITZ_DEBUG_LOCKING
#endif

#ifdef FITZ_DEBUG_LOCKING

void fz_assert_lock_held(fz_context *ctx, int lock);
void fz_assert_lock_not_held(fz_context *ctx, int lock);
void fz_lock_debug_attempt_lock(fz_context *ctx, int lock);
void fz_lock_debug_lock_obtained(fz_context *ctx, int lock);
void fz_lock_debug_unlock(fz_context *ctx, int lock);
void fz_lock_debug_lock_start_timer_assist(fz_context *ctx, int lock);
void fz_dump_lock_times(fz_context* ctx, int total_program_time_ms);

#else

#define fz_assert_lock_held(A,B) do { } while (0)
#define fz_assert_lock_not_held(A,B) do { } while (0)
#define fz_lock_debug_attempt_lock(A,B) do { } while (0)
#define fz_lock_debug_lock_obtained(A,B) do { } while (0)
#define fz_lock_debug_unlock(A,B) do { } while (0)
#define fz_lock_debug_lock_start_timer_assist(A,B) do { } while (0)
#define fz_dump_lock_times(C,T) do { } while (0)

#endif /* !FITZ_DEBUG_LOCKING */

/**
    Specifies the maximum size in bytes of the resource store in
    fz_context. Given as argument to fz_new_context.

    FZ_STORE_UNLIMITED: Let resource store grow unbounded.

    FZ_STORE_DEFAULT: A reasonable upper bound on the size, for
    devices that are not memory constrained.
*/
enum {
    FZ_STORE_UNLIMITED = 0,
    FZ_STORE_DEFAULT = 256 << 20,
};

/**
    Allocate context containing global state.

    The global state contains an exception stack, resource store,
    etc. Most functions in MuPDF take a context argument to be
    able to reference the global state. See fz_drop_context for
    freeing an allocated context.

    alloc: Supply a custom memory allocator through a set of
    function pointers. Set to NULL for the standard library
    allocator. The context will keep the allocator pointer, so the
    data it points to must not be modified or freed during the
    lifetime of the context.

    locks: Supply a set of locks and functions to lock/unlock
    them, intended for multi-threaded applications. Set to NULL
    when using MuPDF in a single-threaded applications. The
    context will keep the locks pointer, so the data it points to
    must not be modified or freed during the lifetime of the
    context.

    max_store: Maximum size in bytes of the resource store, before
    it will start evicting cached resources such as fonts and
    images. FZ_STORE_UNLIMITED can be used if a hard limit is not
    desired. Use FZ_STORE_DEFAULT to get a reasonable size.

    May return NULL.
*/
fz_context *fz_new_context_imp(const fz_alloc_context *alloc, const fz_locks_context *locks, size_t max_store, const char *version);
static inline fz_context *fz_new_context(const fz_alloc_context *alloc, const fz_locks_context *locks, size_t max_store)
{
    return fz_new_context_imp(alloc, locks, max_store, FZ_VERSION);
}

/**
    Make a clone of an existing context.

    This function is meant to be used in multi-threaded
    applications where each thread requires its own context, yet
    parts of the global state, for example caching, are shared.

    ctx: Context obtained from fz_new_context to make a copy of.
    ctx must have had locks and lock/functions setup when created.
    The two contexts will share the memory allocator, resource
    store, locks and lock/unlock functions. They will each have
    their own exception stacks though.

    May return NULL.
*/
fz_context *fz_clone_context(fz_context *ctx);

/**
    Free a context and its global state.

    The context and all of its global state is freed, and any
    buffered warnings are flushed (see fz_flush_warnings). If NULL
    is passed in nothing will happen.

    Must not be called for a context that is being used in an active
    fz_try(), fz_always() or fz_catch() block.
*/
void fz_drop_context(fz_context *ctx);

/**
    Set the user field in the context.

    NULL initially, this field can be set to any opaque value
    required by the user. It is copied on clones.
*/
void fz_set_user_context(fz_context *ctx, void *user);

/**
    Read the user field from the context.
*/
void *fz_user_context(fz_context *ctx);

/**
    Drop the locks registered with this context.
    You may use this to remove all locks before you call fz_drop_context()
    on the context in any final application crash/cleanup code.
*/
void fz_drop_context_locks(fz_context* ctx);

/**
    Get a reference to the global context.
    Use this for access to error, warning and info log channels, for example.
*/
fz_context* fz_get_global_context(void);

fz_context* __fz_get_RAW_global_context(void);

/**
    Set the global context to a given context.

    Will abort the application when a global context has been set up already.
    Use has_global_context() check function to check for this condition before
    calling this function.
*/
void fz_set_global_context(fz_context* ctx);

/**
    Return TRUE when there is an initialized global context available.
*/
int fz_has_global_context(void);

void __cdecl fz_drop_global_context(void);

/**
    Return TRUE when the provided context includes locking support.
*/
int fz_has_locking_support(fz_context* ctx);

/**
    FIXME: Better not to expose fz_default_error_callback, and
    fz_default_warning callback and to allow 'NULL' to be used
    int fz_set_xxxx_callback to mean "defaults".

    FIXME: Do we need/want functions like
    fz_error_callback(ctx, message) to allow callers to inject
    stuff into the error/warning streams?
*/

/**
    The default error callback. Declared publicly just so that the
    error callback can be set back to this after it has been
    overridden.
*/
void fz_default_error_callback(fz_context* ctx, void *user, const char *message);

/**
    The default warning callback. Declared publicly just so that
    the warning callback can be set back to this after it has been
    overridden.
*/
void fz_default_warning_callback(fz_context* ctx, void *user, const char *message);

/**
    The default info callback. Declared publicly just so that
    the info callback can be set back to this after it has been
    overridden.
*/
void fz_default_info_callback(fz_context* ctx, void* user, const char* message);

/**
    Sets default error/warn/info log callbacks to quiet mode.
    Useful when binary data is written to stdout or stderr, where such output
    might clash with output written by these callbacks.

    A non-zero value (TRUE) sets the channel to quiet mode.
*/
void fz_default_error_warn_info_mode(int quiet_error, int quiet_warn, int quiet_info);

void fz_disable_dbg_output(void);
void fz_enable_dbg_output(void);

void fz_enable_dbg_output_on_stdio_unreachable(void);

/**
    The prototype of the error/warning/info callback.
    
    A callback called whenever an error/warning/info message is generated.
    The user pointer passed to fz_set_error_callback() / fz_set_warning_callback() / fz_set_info_callback() is passed
    along with the error message.
*/
typedef void fz_error_print_callback(fz_context* ctx, void* user, const char* message);

/**
    Set the error callback. This will be called as part of the
    exception handling.

    The callback must not throw exceptions!

    `print`: passing NULL means use the default handler.
    
    Returns the previously set error callback.
*/
fz_error_print_callback* fz_set_error_callback(fz_context *ctx, fz_error_print_callback* print, void *user);

/**
    Retrieve the currently set error callback, or NULL if none
    has been set. Optionally, if user is non-NULL, the user pointer
    given when the warning callback was set is also passed back to
    the caller.
    
    Both `print` and/or `user` reference arguments may be safely set to NULL
    if you don't want to retrieve that particular value.
*/
void fz_get_error_callback(fz_context* ctx, fz_error_print_callback** print, void** user);

/**
    Set the warning callback. This will be called as part of the
    exception handling.

    The callback must not throw exceptions!

    `print`: passing NULL means use the default handler.
    
    Returns the previously set error callback.
*/
fz_error_print_callback* fz_set_warning_callback(fz_context *ctx, fz_error_print_callback* print, void *user);

/**
    Retrieve the currently set warning callback, or NULL if none
    has been set. Optionally, if user is non-NULL, the user pointer
    given when the warning callback was set is also passed back to
    the caller.
    
    Both `print` and/or `user` reference arguments may be safely set to NULL
    if you don't want to retrieve that particular value.
*/
void fz_get_warning_callback(fz_context* ctx, fz_error_print_callback** print, void** user);

/**
    Set the info callback. This will be called by any invocation of fz_info() et al.

    The callback must not throw exceptions!

    `print`: passing NULL means use the default handler.
    
    Returns the previously set error callback.
*/
fz_error_print_callback* fz_set_info_callback(fz_context* ctx, fz_error_print_callback* print, void* user);

/**
    Retrieve the currently set info callback, or NULL if none
    has been set. Optionally, if user is non-NULL, the user pointer
    given when the warning callback was set is also passed back to
    the caller.
    
    Both `print` and/or `user` reference arguments may be safely set to NULL
    if you don't want to retrieve that particular value.
*/
void fz_get_info_callback(fz_context* ctx, fz_error_print_callback** print, void** user);

/**
    In order to tune MuPDF's behaviour, certain functions can
    (optionally) be provided by callers.
*/

#if FZ_ENABLE_RENDER_CORE 

/**
    Given the width and height of an image,
    the subsample factor, and the subarea of the image actually
    required, the caller can decide whether to decode the whole
    image or just a subarea.

    arg: The caller supplied opaque argument.

    w, h: The width/height of the complete image.

    l2factor: The log2 factor for subsampling (i.e. image will be
    decoded to (w>>l2factor, h>>l2factor)).

    subarea: The actual subarea required for the current operation.
    The tuning function is allowed to increase this in size if
    required.
*/
typedef void (fz_tune_image_decode_fn)(void *arg, int w, int h, int l2factor, fz_irect *subarea);

/**
    Given the source width and height of
    image, together with the actual required width and height,
    decide whether we should use mitchell scaling.

    arg: The caller supplied opaque argument.

    dst_w, dst_h: The actual width/height required on the target
    device.

    src_w, src_h: The source width/height of the image.

    Return 0 not to use the Mitchell scaler, 1 to use the Mitchell
    scaler. All other values reserved.
*/
typedef int (fz_tune_image_scale_fn)(void *arg, int dst_w, int dst_h, int src_w, int src_h);

/**
    Set the tuning function to use for
    image decode.

    image_decode: Function to use.

    arg: Opaque argument to be passed to tuning function.
*/
void fz_tune_image_decode(fz_context *ctx, fz_tune_image_decode_fn *image_decode, void *arg);

/**
    Set the tuning function to use for
    image scaling.

    image_scale: Function to use.

    arg: Opaque argument to be passed to tuning function.
*/
void fz_tune_image_scale(fz_context *ctx, fz_tune_image_scale_fn *image_scale, void *arg);

/**
    Get the number of bits of antialiasing we are
    using (for graphics). Between 0 and 8.
*/
int fz_aa_level(fz_context *ctx);

/**
    Set the number of bits of antialiasing we should
    use (for both text and graphics).

    bits: The number of bits of antialiasing to use (values are
    clamped to within the 0 to 8 range).
*/
void fz_set_aa_level(fz_context *ctx, int bits);

/**
    Get the number of bits of antialiasing we are
    using for text. Between 0 and 8.
*/
int fz_text_aa_level(fz_context *ctx);

/**
    Set the number of bits of antialiasing we
    should use for text.

    bits: The number of bits of antialiasing to use (values are
    clamped to within the 0 to 8 range).
*/
void fz_set_text_aa_level(fz_context *ctx, int bits);

/**
    Get the number of bits of antialiasing we are
    using for graphics. Between 0 and 8.
*/
int fz_graphics_aa_level(fz_context *ctx);

/**
    Set the number of bits of antialiasing we
    should use for graphics.

    bits: The number of bits of antialiasing to use (values are
    clamped to within the 0 to 8 range).
*/
void fz_set_graphics_aa_level(fz_context *ctx, int bits);

/**
    Get the minimum line width to be
    used for stroked lines.

    min_line_width: The minimum line width to use (in pixels).
*/
float fz_graphics_min_line_width(fz_context *ctx);

/**
    Set the minimum line width to be
    used for stroked lines.

    min_line_width: The minimum line width to use (in pixels).
*/
void fz_set_graphics_min_line_width(fz_context *ctx, float min_line_width);

/**
    Type for a sub pix quantizer function. Given a size,
    return the quantisation factors to use for major (normally x)
    and minor (normally y) axes.

    size: Size of the glyph in points.

    x: Should be filled with the quantization factor for the major
    axis on return.

    y: Should be filled with the quantization factor for the minor
    axis on return.

    The quantisation factor gives the number of possible positions a
    glyph may occupy within a pixel. Thus 1 will give less accurate
    results, and (for example) 4 will give more accurate results.
*/
typedef void (fz_aa_sub_pix_quantizer)(float size, int *x, int *y);

/**
    Set a function to be used for sub pixel quantisation of text.
    This allows users of the library to tune the tradeoff between
    appearance of text and the number of distinct cached glyphs
    generated.

    fn: The quantizer function. This will be called with size as the
    size of the font in points. The two int * parameters should be filled
    with the quantisation factor to use; one for the 'axis of motion'
    (normally x) and one for the 'axis perpendicular to motion' (normally
    y).

    Passing NULL will revert MuPDF to its default function. For the
    major axis, this uses 1 for glyphs >= 48points, 2 for glyphs >=
    24 points, and 4 for all others. For the minor axis, this uses
    1 for glyphs >= 8 points, 2 for glyphs >= 4 points, and 4 for all
    others.
*/
void fz_set_graphics_sub_pix_quantizer(fz_context *ctx, fz_aa_sub_pix_quantizer *fn);

#endif // FZ_ENABLE_RENDER_CORE 

/**
    Get the user stylesheet source text.
*/
const char *fz_user_css(fz_context *ctx);

/**
    Set the user stylesheet source text for use with HTML and EPUB.
*/
void fz_set_user_css(fz_context *ctx, const char *text);

/**
    Return whether to respect document styles in HTML and EPUB.
*/
int fz_use_document_css(fz_context *ctx);

/**
    Toggle whether to respect document styles in HTML and EPUB.
*/
void fz_set_use_document_css(fz_context *ctx, int use);

/**
    Enable icc profile based operation.
*/
void fz_enable_icc(fz_context *ctx);

/**
    Disable icc profile based operation.
*/
void fz_disable_icc(fz_context *ctx);

/**
    Memory Allocation and Scavenging:

    All calls to MuPDF's allocator functions pass through to the
    underlying allocators passed in when the initial context is
    created, after locks are taken (using the supplied locking
    function) to ensure that only one thread at a time calls
    through.

    If the underlying allocator fails, MuPDF attempts to make room
    for the allocation by evicting elements from the store, then
    retrying.

    Any call to allocate may then result in several calls to the
    underlying allocator, and result in elements that are only
    referred to by the store being freed.
*/

/**
    Allocate memory for a structure, clear it, and tag the pointer
    for Memento.

    Throws exception in the event of failure to allocate.
*/
#define fz_malloc_struct(CTX, TYPE) \
    ((TYPE*)Memento_label(fz_calloc(CTX, 1, sizeof(TYPE)), #TYPE))

/**
    Allocate memory for an array of structures, clear it, and tag
    the pointer for Memento.

    Throws exception in the event of failure to allocate.
*/
#define fz_malloc_struct_array(CTX, N, TYPE) \
    ((TYPE*)Memento_label(fz_calloc(CTX, N, sizeof(TYPE)), #TYPE "[]"))

/**
    Allocate uninitialized memory for an array of structures, and
    tag the pointer for Memento. Does NOT clear the memory!

    Throws exception in the event of failure to allocate.
*/
#define fz_malloc_array(CTX, COUNT, TYPE) \
    ((TYPE*)Memento_label(fz_malloc(CTX, (COUNT) * sizeof(TYPE)), #TYPE "[]"))
#define fz_realloc_array(CTX, OLD, COUNT, TYPE) \
    ((TYPE*)Memento_label(fz_realloc(CTX, OLD, (COUNT) * sizeof(TYPE)), #TYPE "[]"))

/**
    Allocate uninitialized memory of a given size.
    Does NOT clear the memory!

    May return NULL for size = 0.

    Throws exception in the event of failure to allocate.
*/
void *fz_malloc(fz_context *ctx, size_t size   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_malloc(ctx, size)					\
    fz_malloc(ctx, size, __FILE__, __LINE__)
#endif

/**
    Allocate array of memory of count entries of size bytes.
    Clears the memory to zero.

    Throws exception in the event of failure to allocate.
*/
void *fz_calloc(fz_context *ctx, size_t count, size_t size   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_calloc(ctx, count, size)					\
    fz_calloc(ctx, count, size, __FILE__, __LINE__)
#endif

/**
    Reallocates a block of memory to given size. Existing contents
    up to min(old_size,new_size) are maintained. The rest of the
    block is uninitialised.

    fz_realloc(ctx, NULL, size) behaves like fz_malloc(ctx, size).

    fz_realloc(ctx, p, 0); behaves like fz_free(ctx, p).

    Throws exception in the event of failure to allocate.
*/
void *fz_realloc(fz_context *ctx, void *p, size_t size   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_realloc(ctx, old, size)					\
    fz_realloc(ctx, old, size, __FILE__, __LINE__)
#endif

/**
    Free a previously allocated block of memory.

    fz_free(ctx, NULL) does nothing.

    Never throws exceptions.
*/
void fz_free(fz_context *ctx, const void *p);

/**
    fz_malloc equivalent that returns NULL rather than throwing
    exceptions.
*/
void *fz_malloc_no_throw(fz_context *ctx, size_t size   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_malloc_no_throw(ctx, size)					\
    fz_malloc_no_throw(ctx, size, __FILE__, __LINE__)
#endif

/**
    fz_calloc equivalent that returns NULL rather than throwing
    exceptions.
*/
void *fz_calloc_no_throw(fz_context *ctx, size_t count, size_t size   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_calloc_no_throw(ctx, count, size)					\
    fz_calloc_no_throw(ctx, count, size, __FILE__, __LINE__)
#endif

/**
    fz_realloc equivalent that returns NULL rather than throwing
    exceptions.
*/
void *fz_realloc_no_throw(fz_context *ctx, void *p, size_t size   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_realloc_no_throw(ctx, old, size)					\
    fz_realloc_no_throw(ctx, old, size, __FILE__, __LINE__)
#endif

/**
	fz_malloc equivalent, except that the block is guaranteed aligned.
	Block must be freed later using fz_free_aligned.
*/
void *fz_malloc_aligned(fz_context *ctx, size_t size, int align   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_malloc_aligned(ctx, size, align)					\
    fz_malloc_aligned(ctx, size, align, __FILE__, __LINE__)
#endif

/**
	fz_free equivalent, for blocks allocated via fz_malloc_aligned.
*/
void fz_free_aligned(fz_context *ctx, void *p);

/**
    Portable strdup implementation, using fz allocators.
*/
char *fz_strdup(fz_context *ctx, const char *s   FZDBG_DECL_ARGS);
#if defined(FZDBG_HAS_TRACING)
#define fz_strdup(ctx, str)					\
    fz_strdup(ctx, str, __FILE__, __LINE__)
#endif

/**
    Fill block with len bytes of pseudo-randomness.
*/
void fz_memrnd(fz_context *ctx, uint8_t *block, int len);

/*
	Reference counted malloced C strings.
*/
typedef struct
{
	int refs;
	char str[1];
} fz_string;

/*
	Allocate a new string to hold a copy of str.

	Returns with a refcount of 1.
*/
fz_string *fz_new_string(fz_context *ctx, const char *str   FZDBG_DECL_ARGS);

/*
	Take another reference to a string.
*/
fz_string *fz_keep_string(fz_context *ctx, fz_string *str);

/*
	Drop a reference to a string, freeing if the refcount
	reaches 0.
*/
void fz_drop_string(fz_context *ctx, fz_string *str);

#define fz_cstring_from_string(A) ((A) == NULL ? NULL : (A)->str)

/* Implementation details: subject to change. */

/* Implementations exposed for speed, but considered private. */

void fz_var_imp(void *);
fz_jmp_buf *fz_push_try(fz_context *ctx);
int fz_do_try(fz_context *ctx);
int fz_do_always(fz_context *ctx);
int (fz_do_catch)(fz_context *ctx);

#ifndef FZ_JMPBUF_ALIGN
#define FZ_JMPBUF_ALIGN 32
#endif

typedef struct
{
    fz_jmp_buf buffer;
    int state, code;
    char padding[FZ_JMPBUF_ALIGN-sizeof(int)*2];
} fz_error_stack_slot;

typedef struct
{
    fz_error_stack_slot *top;
    fz_error_stack_slot __stack[512];
    fz_error_stack_slot padding;
    fz_error_stack_slot *stack_base;
	// Stores the last (exception) error code. See the FZ_ERROR_XXX enum, e.g. FZ_ERROR_GENERIC.
    int errcode;
    // See fz_rethrow() code comments for the complete story:
    int last_nonzero_errcode;

	// This (together with the `system_error_message` member) stores the last **system error**
	// which was encountered in the fz_xxxxxxx() functions.
    int system_errcode[3];   // first/nested/last
    int system_errdepth;

	void *print_user;
    fz_error_print_callback *print;
    char message[LONGLINE];
    char system_error_message[3][LONGLINE];
} fz_error_context;

typedef struct
{
    void *print_user;
    fz_error_print_callback *print;
    int count;
    char message[LONGLINE];
} fz_warn_context;

typedef struct
{
    void* print_user;
    fz_error_print_callback *print;
} fz_info_context;

#if FZ_ENABLE_RENDER_CORE 

typedef struct
{
    int hscale;
    int vscale;
    int scale;
    int bits;
    int text_bits;
    float min_line_width;
    fz_aa_sub_pix_quantizer *sub_pix_quantizer;
} fz_aa_context;

#endif

struct fz_context
{
    void *user;
    fz_alloc_context alloc;
    fz_locks_context locks;
    fz_error_context error;
    fz_warn_context warn;
    fz_info_context info;

    /* unshared contexts */
#if FZ_ENABLE_RENDER_CORE 
	fz_aa_context aa;
#endif
	uint16_t seed48[7];
#if FZ_ENABLE_ICC
    int icc_enabled;
#endif
    int throw_on_repair;
    int within_throw_call;

    /* TODO: should these be unshared? */
    fz_document_handler_context *handler;
	fz_archive_handler_context *archive;
    fz_style_context *style;
    fz_tuning_context *tuning;

    /* shared contexts */
    fz_output *stddbg;
    fz_font_context *font;
    fz_colorspace_context *colorspace;
    fz_store *store;
    fz_glyph_cache *glyph_cache;
	
	const void *jbig2encoder;

	fz_cookie* cookie;
};

fz_context *fz_new_context_imp(const fz_alloc_context *alloc, const fz_locks_context *locks, size_t max_store, const char *version);

#ifdef __cplusplus
}
#endif


/**
	At least in the mupdf library internally we store ephemeral run-time and system errors in the context `ctx` to ensure they
	don't disappear before we've been able to handle and/or report them.

	This intentionally DOES NOT use the same scratch space as used by the exception handling system: throwing exceptions can include
	formatting messages, which desire to include the last system error message using `%s`, which would break in very destructive ways
	when we were to re-use `ctx->error.message[]` for our system error message as well!

	This is why the accompanying system error message is kept in a separate scratch space and can be accessed via the `fz_ctx_get_system_errormsg()` API call.

    NOTE: once you have copied an ephemeral error (and its message), any subsequent attempts to copy an ephemeral error will
    be silently ignored until you signal the `ctx` that you have observed and handled the error by calling `fz_clear_system_error()`.
    This way you get desirable 'first error is the only one relevant/persisted' behaviour.

    You MAY replace any ephemeral error code + message by combining the calls `fz_clear_system_error()`+`fz_copy_ephemeral_system_error_explicit()`-->`fz_replace_ephemeral_system_error()`
*/

void fz_copy_ephemeral_system_error_explicit(fz_context *ctx, int errorcode, const char *errormessage, int category_code, int errorcode_mask);

static inline void fz_copy_ephemeral_errno(fz_context *ctx)
{
	int ec = errno;
	fz_copy_ephemeral_system_error_explicit(ctx, ec, strerror(ec), FZ_ERROR_C_RTL_SERIES, FZ_ERROR_C_RTL_SERIES_MASK);
}

static inline void fz_copy_ephemeral_errno_and_message(fz_context *ctx, int sys_errorcode, const char *errormessage)
{
	fz_copy_ephemeral_system_error_explicit(ctx, sys_errorcode, errormessage, FZ_ERROR_C_RTL_SERIES, FZ_ERROR_C_RTL_SERIES_MASK);
}

static inline void fz_copy_ephemeral_system_error(fz_context *ctx, int sys_errorcode, const char *errormessage)
{
	fz_copy_ephemeral_system_error_explicit(ctx, sys_errorcode, errormessage, FZ_ERROR_SYS_SERIES, FZ_ERROR_SYS_SERIES_MASK);
}

// 0/NULL arg means: don't replace this part!
void fz_replace_ephemeral_system_error(fz_context *ctx, int errorcode, const char *errormessage);
void fz_freplace_ephemeral_system_error(fz_context *ctx, int errorcode, const char *errormessage_fmt, ...);
void fz_vreplace_ephemeral_system_error(fz_context *ctx, int errorcode, const char *errormessage_fmt, va_list ap);


static inline void fz_clear_system_error(fz_context *ctx)
{
    ctx->error.system_errcode[0] = 0;
    ctx->error.system_errcode[1] = 0;
    ctx->error.system_errcode[2] = 0;
    ctx->error.system_error_message[0][0] = 0;
    ctx->error.system_error_message[1][0] = 0;
    ctx->error.system_error_message[2][0] = 0;
    ctx->error.system_errdepth = 0;
}

static inline void fz_pop_system_error(fz_context *ctx)
{
    int idx = ctx->error.system_errdepth;
	ASSERT0(idx >= 0 && idx < 3);

    // DO NOT clear a message what isn't used any more: this is the secret sauce that helps make
    // `fz_ctx_pop_system_errormsg()` work!
    // 
    ctx->error.system_errcode[idx] = 0;
    //ctx->error.system_error_message[idx][0] = 0;

    --idx;
    if (idx < 0)
        idx = 0;
	ASSERT0(idx >= 0 && idx < 3);
    ctx->error.system_errdepth = idx;
}

static inline void fz_push_system_error(fz_context *ctx)
{
    int idx = ctx->error.system_errdepth + 1;
	ASSERT0(idx > 0 && idx <= 3);
	if (idx >= 3)
        idx = 2;
	ASSERT0(idx >= 0 && idx < 3);
	ctx->error.system_errdepth = idx;

    ctx->error.system_errcode[idx] = 0;
    ctx->error.system_error_message[idx][0] = 0;
}


static inline int fz_is_generic_system_error(int errorcode)
{
	return (errorcode < 0 || errorcode >= FZ_ERROR_C_RTL_SERIES);
}

static inline int fz_is_os_system_error(int errorcode)
{
	return (errorcode & FZ_ERROR_SYS_SERIES);
}

static inline int fz_is_rtl_error(int errorcode)
{
	return errorcode >= FZ_ERROR_C_RTL_SERIES && errorcode < (FZ_ERROR_C_RTL_SERIES | FZ_ERROR_C_RTL_SERIES_MASK);
}


///  Return the ephemeral run-time-library error code stored in the `ctx` context.
static inline int fz_ctx_get_generic_system_error(fz_context *ctx)
{
    int idx = ctx->error.system_errdepth;
	ASSERT0(idx >= 0 && idx < 3);
	return ctx->error.system_errcode[idx];
}

///  Return 1 when the `ctx` stores an ephemeral system/run-time-library error code (& accompanying message)
static inline int fz_ctx_has_system_error(fz_context *ctx)
{
    int ec = fz_ctx_get_generic_system_error(ctx);
	return fz_is_generic_system_error(ec);
}

///  Return non-zero `errno`-equivalent error code IFF the `ctx` stores the ephemeral run-time-library error code
/// Return -1 when the error stored in the 'ctx' is of another kind.
/// Return 0 when there's no system error stored in the `ctx` at all.
static inline int fz_ctx_get_rtl_errno(fz_context *ctx)
{
    int ec = fz_ctx_get_generic_system_error(ctx);
	if (fz_is_rtl_error(ec))
		return ec & FZ_ERROR_C_RTL_SERIES_MASK;
	return ec ? -1 : 0;
}

///  Return non-zero system error code IFF the `ctx` stores the ephemeral system error code
static inline int fz_ctx_get_os_system_error(fz_context *ctx)
{
    int ec = fz_ctx_get_generic_system_error(ctx);
#if defined(_WIN32)
	// all Win32 system errors (and warnings!) have their high bit set, i.e. bitmask FZ_ERROR_SYS_SERIES ! (this is intentional!) 
	if (fz_is_os_system_error(ec))
		return ec;
#else
	if (fz_is_os_system_error(ec))
		return ec & FZ_ERROR_SYS_SERIES_MASK;
#endif
	return 0;
}

static inline const char *fz_ctx_get_system_errormsg(fz_context *ctx)
{
    if (fz_ctx_has_system_error(ctx))
    {
        int idx = ctx->error.system_errdepth;
		ASSERT0(idx >= 0 && idx < 3);
		return ctx->error.system_error_message[idx];
    }
    return NULL;
}

// use this to mark a system error as handled, by feeding this message into an exception message, f.e.
static inline const char *fz_ctx_pop_system_errormsg(fz_context *ctx)
{
    const char *msg = fz_ctx_get_system_errormsg(ctx);
    fz_pop_system_error(ctx);
    // while this might look positively fruity to you (fetch, then pop, then *still access previous fetch*,
    // this is perfectly safe. This WILL, however, be the very last time you'll be able to look at that
    // system error message; the accompanying system error *code* has already been popped = cleared!
    return msg;
}


/**
    Lock one of the user supplied mutexes.
*/
static inline void
fz_lock(fz_context *ctx, int lock)
{
    fz_lock_debug_lock_start_timer_assist(ctx, lock);
    fz_lock_debug_attempt_lock(ctx, lock);
    ctx->locks.lock(ctx->locks.user, lock);
    fz_lock_debug_lock_obtained(ctx, lock);
}

/**
    Unlock one of the user supplied mutexes.
*/
static inline void
fz_unlock(fz_context *ctx, int lock)
{
    fz_lock_debug_unlock(ctx, lock);
    ctx->locks.unlock(ctx->locks.user, lock);
}

/* Lock-safe reference counting functions */

static inline void *
fz_keep_imp(fz_context *ctx, void *p, int *refs)
{
    if (p)
    {
        (void)Memento_checkIntPointerOrNull(refs);
        fz_lock(ctx, FZ_LOCK_ALLOC);
        if (*refs > 0)
        {
            (void)Memento_takeRef(p);
            ++*refs;
        }
        fz_unlock(ctx, FZ_LOCK_ALLOC);
    }
    return p;
}

static inline void *
fz_keep_imp_locked(fz_context *ctx FZ_UNUSED, void *p, int *refs)
{
    if (p)
    {
        (void)Memento_checkIntPointerOrNull(refs);
        if (*refs > 0)
        {
            (void)Memento_takeRef(p);
            ++*refs;
        }
    }
    return p;
}

static inline void *
fz_keep_imp8_locked(fz_context *ctx FZ_UNUSED, void *p, int8_t *refs)
{
	if (p)
	{
		(void)Memento_checkIntPointerOrNull(refs);
		if (*refs > 0)
		{
			(void)Memento_takeRef(p);
			++*refs;
		}
	}
	return p;
}

static inline void *
fz_keep_imp8(fz_context *ctx, void *p, int8_t *refs)
{
    if (p)
    {
        (void)Memento_checkBytePointerOrNull(refs);
        fz_lock(ctx, FZ_LOCK_ALLOC);
        if (*refs > 0)
        {
            (void)Memento_takeRef(p);
            ++*refs;
        }
        fz_unlock(ctx, FZ_LOCK_ALLOC);
    }
    return p;
}

static inline void *
fz_keep_imp16(fz_context *ctx, void *p, int16_t *refs)
{
    if (p)
    {
        (void)Memento_checkShortPointerOrNull(refs);
        fz_lock(ctx, FZ_LOCK_ALLOC);
        if (*refs > 0)
        {
            (void)Memento_takeRef(p);
            ++*refs;
        }
        fz_unlock(ctx, FZ_LOCK_ALLOC);
    }
    return p;
}

static inline int
fz_drop_imp(fz_context *ctx, void *p, int *refs)
{
    if (p)
    {
        int drop;
        (void)Memento_checkIntPointerOrNull(refs);
        fz_lock(ctx, FZ_LOCK_ALLOC);
        if (*refs > 0)
        {
            (void)Memento_dropIntRef(p);
            drop = --*refs == 0;
        }
        else
            drop = 0;
        fz_unlock(ctx, FZ_LOCK_ALLOC);
        return drop;
    }
    return 0;
}

static inline int
fz_drop_imp8(fz_context *ctx, void *p, int8_t *refs)
{
    if (p)
    {
        int drop;
        (void)Memento_checkBytePointerOrNull(refs);
        fz_lock(ctx, FZ_LOCK_ALLOC);
        if (*refs > 0)
        {
            (void)Memento_dropByteRef(p);
            drop = --*refs == 0;
        }
        else
            drop = 0;
        fz_unlock(ctx, FZ_LOCK_ALLOC);
        return drop;
    }
    return 0;
}

static inline int
fz_drop_imp16(fz_context *ctx, void *p, int16_t *refs)
{
    if (p)
    {
        int drop;
        (void)Memento_checkShortPointerOrNull(refs);
        fz_lock(ctx, FZ_LOCK_ALLOC);
        if (*refs > 0)
        {
            (void)Memento_dropShortRef(p);
            drop = --*refs == 0;
        }
        else
            drop = 0;
        fz_unlock(ctx, FZ_LOCK_ALLOC);
        return drop;
    }
    return 0;
}

#endif
