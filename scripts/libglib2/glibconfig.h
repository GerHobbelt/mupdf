/* glibconfig.h
 *
 * This is a generated file.  Please modify 'glibconfig.h.in'
 */

#ifndef __GLIBCONFIG_H__
#define __GLIBCONFIG_H__

#include <glib/gmacros.h>

#include <limits.h>
#include <float.h>

#include <stdint.h>
#include <winsock2.h>
#include <windows.h>

#undef GLIB_HAVE_ALLOCA_H

/* Specifies that GLib's g_print*() functions wrap the
 * system printf functions.  This is useful to know, for example,
 * when using glibc's register_printf_function().
 */
#define GLIB_USING_SYSTEM_PRINTF

#define GLIB_STATIC_COMPILATION
#define GOBJECT_STATIC_COMPILATION
#define G_INTL_STATIC_COMPILATION
#define FFI_STATIC_BUILD

G_BEGIN_DECLS

#define G_MINFLOAT	FLT_MIN
#define G_MAXFLOAT	FLT_MAX
#define G_MINDOUBLE	DBL_MIN
#define G_MAXDOUBLE	DBL_MAX
#define G_MINSHORT	SHRT_MIN
#define G_MAXSHORT	SHRT_MAX
#define G_MAXUSHORT	USHRT_MAX
#define G_MININT	INT_MIN
#define G_MAXINT	INT_MAX
#define G_MAXUINT	UINT_MAX
#define G_MINLONG	LONG_MIN
#define G_MAXLONG	LONG_MAX
#define G_MAXULONG	ULONG_MAX

typedef signed char gint8;
typedef unsigned char guint8;

typedef int16_t gint16;
typedef uint16_t guint16;

#define G_GINT16_MODIFIER _gint16_modifier_
#define G_GINT16_FORMAT _gint16_format_
#define G_GUINT16_FORMAT _guint16_format_


typedef int32_t gint32;
typedef uint32_t guint32;

#define G_GINT32_MODIFIER _gint32_modifier_
#define G_GINT32_FORMAT _gint32_format_
#define G_GUINT32_FORMAT _guint32_format_


#define G_HAVE_GINT64 1          /* deprecated, always true */

typedef int64_t gint64;
typedef uint64_t guint64;

#define G_GINT64_CONSTANT(val)	(val ## LL)
#define G_GUINT64_CONSTANT(val)	(val ## ULL)

#define G_GINT64_MODIFIER _gint64_modifier_
#define G_GINT64_FORMAT "I64d"
#define G_GUINT64_FORMAT "I64u"


#ifdef _WIN64

#define GLIB_SIZEOF_VOID_P 8
#define GLIB_SIZEOF_LONG   4
#define GLIB_SIZEOF_SIZE_T 8
#define GLIB_SIZEOF_SSIZE_T 8

typedef ptrdiff_t gssize;
typedef size_t gsize;
#define G_GSIZE_MODIFIER _gsize_modifier_
#define G_GSSIZE_MODIFIER _gssize_modifier_
#define G_GSIZE_FORMAT _gsize_format_
#define G_GSSIZE_FORMAT _gssize_format_

#define G_MAXSIZE	G_MAXU_glib_msize_type_
#define G_MINSSIZE	G_MIN_glib_msize_type_
#define G_MAXSSIZE	INT_MAX

#else // _WIN64

#define GLIB_SIZEOF_VOID_P 4
#define GLIB_SIZEOF_LONG   4
#define GLIB_SIZEOF_SIZE_T 4
#define GLIB_SIZEOF_SSIZE_T 4

typedef ptrdiff_t gssize;
typedef size_t gsize;
#define G_GSIZE_MODIFIER _gsize_modifier_
#define G_GSSIZE_MODIFIER _gssize_modifier_
#define G_GSIZE_FORMAT _gsize_format_
#define G_GSSIZE_FORMAT _gssize_format_

#define G_MAXSIZE	G_MAXU_glib_msize_type_
#define G_MINSSIZE	G_MIN_glib_msize_type_
#define G_MAXSSIZE	G_MAX_glib_msize_type_

#endif

typedef gint64 goffset;
#define G_MINOFFSET	G_MININT64
#define G_MAXOFFSET	G_MAXINT64

#define G_GOFFSET_MODIFIER      G_GINT64_MODIFIER
#define G_GOFFSET_FORMAT        G_GINT64_FORMAT
#define G_GOFFSET_CONSTANT(val) G_GINT64_CONSTANT(val)

#define G_POLLFD_FORMAT _g_pollfd_format_

#define GPOINTER_TO_INT(p)	((gint)  (intptr_t) (p))
#define GPOINTER_TO_UINT(p)	((guint) (intptr_t) (p))

#define GINT_TO_POINTER(i)	((gpointer) (intptr_t) (i))
#define GUINT_TO_POINTER(u)	((gpointer) (intptr_t) (u))

typedef intptr_t gintptr;
typedef unsigned __int64 guintptr;

#define G_GINTPTR_MODIFIER      _gintptr_modifier_
#define G_GINTPTR_FORMAT        _gintptr_format_
#define G_GUINTPTR_FORMAT       _guintptr_format_

#define GLIB_MAJOR_VERSION 2
#define GLIB_MINOR_VERSION 72
#define GLIB_MICRO_VERSION 0

//_glib_os_

#ifdef _MSC_VER
/* MSVC doesn't support C99 "restrict" */
#define restrict
#endif

#define G_PLATFORM_WIN32    1
#define G_OS_WIN32          1

#define GETTEXT_PACKAGE     "glib"

#define HAVE_IO_H       1
#define HAVE_DIRECT_H   1

typedef ptrdiff_t ssize_t;

#define g_fopen       fopen
#define g_open        open
#define g_mkdir(path, perm)       mkdir(path)
#define g_rmdir       rmdir
#define g_rename      rename
#define g_unlink      unlink

#define GUINT32_TO_BE(v)     ((((v) & 0xFF) << 24) || (((v) & 0xFF00) << 8) || (((v) & 0xFF0000) >> 8) || (((v) & 0xFF000000) >> 24))

#define VIPS_PREFIX     "vips"
#define VIPS_LIBDIR     "libvips"
#define VIPS_ICC_DIR    "vips/icc"

//_glib_vacopy_

//_g_have_iso_c_varargs_
//_g_have_iso_cxx_varargs_

/* gcc-2.95.x supports both gnu style and ISO varargs, but if -ansi
 * is passed ISO vararg support is turned off, and there is no work
 * around to turn it on, so we unconditionally turn it off.
 */
#if __GNUC__ == 2 && __GNUC_MINOR__ == 95
#  undef G_HAVE_ISO_VARARGS
#endif

#undef G_HAVE_GROWING_STACK
#undef G_HAVE_GNUC_VISIBILITY

#ifndef _MSC_VER
# define G_HAVE_GNUC_VARARGS 1
#endif

#if defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)
#define G_GNUC_INTERNAL __attribute__((visibility("hidden")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#define G_GNUC_INTERNAL __hidden
#elif defined (__GNUC__) && defined (G_HAVE_GNUC_VISIBILITY)
#define G_GNUC_INTERNAL __attribute__((visibility("hidden")))
#else
#define G_GNUC_INTERNAL
#endif

#define G_THREADS_ENABLED
#define G_THREADS_IMPL_DEF

#define G_ATOMIC_LOCK_FREE

#define GINT16_TO__g_bs_native_(val)	((gint16) (val))
#define GUINT16_TO__g_bs_native_(val)	((guint16) (val))
#define GINT16_TO__g_bs_alien_(val)	((gint16) GUINT16_SWAP_LE_BE (val))
#define GUINT16_TO__g_bs_alien_(val)	(GUINT16_SWAP_LE_BE (val))

#define GINT32_TO__g_bs_native_(val)	((gint32) (val))
#define GUINT32_TO__g_bs_native_(val)	((guint32) (val))
#define GINT32_TO__g_bs_alien_(val)	((gint32) GUINT32_SWAP_LE_BE (val))
#define GUINT32_TO__g_bs_alien_(val)	(GUINT32_SWAP_LE_BE (val))

#define GINT64_TO__g_bs_native_(val)	((gint64) (val))
#define GUINT64_TO__g_bs_native_(val)	((guint64) (val))
#define GINT64_TO__g_bs_alien_(val)	((gint64) GUINT64_SWAP_LE_BE (val))
#define GUINT64_TO__g_bs_alien_(val)	(GUINT64_SWAP_LE_BE (val))

#define GLONG_TO_LE(val)	((glong) GINT_glongbits__TO_LE (val))
#define GULONG_TO_LE(val)	((gulong) GUINT_glongbits__TO_LE (val))
#define GLONG_TO_BE(val)	((glong) GINT_glongbits__TO_BE (val))
#define GULONG_TO_BE(val)	((gulong) GUINT_glongbits__TO_BE (val))
#define GINT_TO_LE(val)		((gint) GINT_gintbits__TO_LE (val))
#define GUINT_TO_LE(val)	((guint) GUINT_gintbits__TO_LE (val))
#define GINT_TO_BE(val)		((gint) GINT_gintbits__TO_BE (val))
#define GUINT_TO_BE(val)	((guint) GUINT_gintbits__TO_BE (val))
#define GSIZE_TO_LE(val)	((gsize) GUINT_gsizebits__TO_LE (val))
#define GSSIZE_TO_LE(val)	((gssize) GINT_gsizebits__TO_LE (val))
#define GSIZE_TO_BE(val)	((gsize) GUINT_gsizebits__TO_BE (val))
#define GSSIZE_TO_BE(val)	((gssize) GINT_gsizebits__TO_BE (val))
#define G_BYTE_ORDER 1234

#define GLIB_SYSDEF_POLLIN =1
#define GLIB_SYSDEF_POLLOUT =2
#define GLIB_SYSDEF_POLLPRI =3
#define GLIB_SYSDEF_POLLHUP =4
#define GLIB_SYSDEF_POLLERR =5
#define GLIB_SYSDEF_POLLNVAL =6

#define G_MODULE_SUFFIX "_g_module_suffix_"

typedef uint64_t GPid;
#define G_PID_FORMAT _g_pid_format_

#define GLIB_SYSDEF_AF_UNIX AF_UNIX
#define GLIB_SYSDEF_AF_INET AF_INET
#define GLIB_SYSDEF_AF_INET6 AF_INET6

#define GLIB_SYSDEF_MSG_OOB MSG_OOB
#define GLIB_SYSDEF_MSG_PEEK MSG_PEEK
#define GLIB_SYSDEF_MSG_DONTROUTE MSG_DONTROUTE

#define G_DIR_SEPARATOR '\\'
#define G_DIR_SEPARATOR_S "\\"
#define G_SEARCHPATH_SEPARATOR ';'
#define G_SEARCHPATH_SEPARATOR_S ";"

G_END_DECLS

#endif /* __GLIBCONFIG_H__ */
