/*
    This file is used on systems not using configure only and contains
    hardcoded definitions for MSVC, Xcode and MinGW when using makefiles.
 */

#ifndef _TIF_CONFIG_H_
#define _TIF_CONFIG_H_

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef __MINGW32__
#include <basetyps.h>
#endif
#ifdef _MSC_VER
#include <basetsd.h>
#endif

#include "tiffconf.h"


/* libtiff/tif_config.h.  Generated from tif_config.h.in by configure.  */
/* libtiff/tif_config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the declaration of `optarg', and to 0 if you don't. */
/* #undef HAVE_DECL_OPTARG */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `floor' function. */
#define HAVE_FLOOR 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
/* #undef HAVE_FSEEKO */

/* Define to 1 if you have the `getopt' function. */
/* #undef HAVE_GETOPT */

/* Define to 1 if you have the <GLUT/glut.h> header file. */
/* #undef HAVE_GLUT_GLUT_H */

/* Define to 1 if you have the <GL/glut.h> header file. */
/* #undef HAVE_GL_GLUT_H */

/* Define to 1 if you have the <GL/glu.h> header file. */
/* #undef HAVE_GL_GLU_H */

/* Define to 1 if you have the <GL/gl.h> header file. */
/* #undef HAVE_GL_GL_H */

/* Define as 0 or 1 according to the floating point format supported by the
   machine */
#define HAVE_IEEEFP 1

/* Define to 1 if the system has the type `int16'. */
/* #undef HAVE_INT16 */

/* Define to 1 if the system has the type `int32'. */
/* #undef HAVE_INT32 */

/* Define to 1 if the system has the type `int8'. */
/* #undef HAVE_INT8 */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <io.h> header file. */
#define HAVE_IO_H 1

/* Define to 1 if you have the `isascii' function. */
#define HAVE_ISASCII 1

/* Define to 1 if you have the `jbg_newlen' function. */
#define HAVE_JBG_NEWLEN 1

/* Define to 1 if you have the `lfind' function. */
#define HAVE_LFIND 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <malloc.h> header file. */
/* #undef HAVE_MALLOC_H */

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `mmap' function. */
/* #undef HAVE_MMAP */

/* Define to 1 if you have the <OpenGL/glu.h> header file. */
/* #undef HAVE_OPENGL_GLU_H */

/* Define to 1 if you have the <OpenGL/gl.h> header file. */
/* #undef HAVE_OPENGL_GL_H */

/* Define to 1 if you have the `pow' function. */
#define HAVE_POW 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Define to 1 if you have the <search.h> header file. */
#define HAVE_SEARCH_H 1

/* Define to 1 if you have the `setmode' function. */
#define HAVE_SETMODE 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the `sqrt' function. */
#define HAVE_SQRT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strrchr' function. */
#define HAVE_STRRCHR 1

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the `strtoull' function. */
#define HAVE_STRTOULL 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Use nonstandard varargs form for the GLU tesselator callback */
/* #undef HAVE_VARARGS_GLU_TESSCB */

/* Define to 1 if you have the <windows.h> header file. */
#define HAVE_WINDOWS_H 1

/* Native cpu byte order: 1 if big-endian (Motorola) or 0 if little-endian
   (Intel) */
#define HOST_BIGENDIAN 0

/* Set the native cpu bit order (FILLORDER_LSB2MSB or FILLORDER_MSB2LSB) */
#define HOST_FILLORDER FILLORDER_LSB2MSB

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Signed 8-bit type */
#define TIFF_INT8_T signed char

/* Unsigned 8-bit type */
#define TIFF_UINT8_T unsigned char

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `signed int', as computed by sizeof. */
#define SIZEOF_SIGNED_INT 4

/* The size of `signed long', as computed by sizeof. */
#define SIZEOF_SIGNED_LONG 4

/* The size of `signed long long', as computed by sizeof. */
#define SIZEOF_SIGNED_LONG_LONG 8

/* The size of `signed short', as computed by sizeof. */
#define SIZEOF_SIGNED_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#if defined(WIN64) || defined(_WIN64)
#define SIZEOF_SIZE_T 8
#else
#define SIZEOF_SIZE_T 4
#endif

/* The size of `unsigned char *', as computed by sizeof. */
#if defined(WIN64) || defined(_WIN64)
#define SIZEOF_UNSIGNED_CHAR_P 8
#else
#define SIZEOF_UNSIGNED_CHAR_P 4
#endif

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 4

/* The size of `unsigned long long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* The size of `unsigned short', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Signed 16-bit type */
#define TIFF_INT16_T signed short

/* Unsigned 16-bit type */
#define TIFF_UINT16_T unsigned short

/* Signed 32-bit type formatter */
#define TIFF_INT32_FORMAT "%d"

/* Signed 32-bit type */
#define TIFF_INT32_T signed int

/* Signed 8-bit type */
#define TIFF_INT8_T signed char

/* Unsigned 8-bit type */
#define TIFF_UINT8_T unsigned char

/* Pointer difference type formatter */
#define TIFF_PTRDIFF_FORMAT "%ld"

/* Pointer difference type */
#define TIFF_PTRDIFF_T ptrdiff_t

/* Size type formatter */
#define TIFF_SIZE_FORMAT "%zd"

/* Unsigned size type */
#define TIFF_SIZE_T size_t

/* Unsigned 16-bit type */
#define TIFF_UINT16_T unsigned short

/* Unsigned 32-bit type formatter */
#define TIFF_UINT32_FORMAT "%u"

/* Unsigned 32-bit type */
#define TIFF_UINT32_T unsigned int

/* Signed 64-bit type formatter */
#define TIFF_INT64_FORMAT "%I64d"

/* Signed 64-bit type */
#define TIFF_INT64_T signed __int64

/* Unsigned 64-bit type formatter */
#define TIFF_UINT64_FORMAT "%I64u"

/* Unsigned 64-bit type */
#define TIFF_UINT64_T unsigned __int64

/* Signed size type */
#if defined(WIN64) || defined(_WIN64)
#define TIFF_SSIZE_T signed __int64
#else
#define TIFF_SSIZE_T signed long int
#endif

/* Signed size type formatter */
#if defined(WIN64) || defined(_WIN64)
#define TIFF_SSIZE_FORMAT "%I64d"
#else
#define TIFF_SSIZE_FORMAT "%ld"
#endif

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */





/* define to use win32 IO system */
#define USE_WIN32_FILEIO 1


/* Prevent clash with other libs loaded in monolithic builds... */
#if !defined(PACKAGE) && !defined(VERSION)


/* Name of package */
#define PACKAGE "tiff"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "tiff@lists.maptools.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "LibTIFF Software"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "LibTIFF Software 4.2.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "tiff"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.2.0"

/* Version number of package */
#define VERSION "4.2.0"


#endif


/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */


/*
  Please see associated settings in "nmake.opt" which configure porting
  settings. It should not be necessary to edit the following pre-processor
  logic.
*/
#if defined(_MSC_VER)
/* Visual Studio 2015 / VC 14 / MSVC 19.00 finally has snprintf() */
#  if _MSC_VER < 1900 /* Visual C++ 2015 */
#    define snprintf _snprintf
#  endif
#endif

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
# ifndef inline
#  define inline __inline
# endif
#endif

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

#if !defined (__BORLANDC__) && !defined (__WATCOMC__)
   #define lfind _lfind
#endif

#ifdef _WIN32_WCE
#   undef HAVE_FCNTL_H
#   undef HAVE_SYS_TYPES_H

    /*
       CE headers don't define these standard constants (not even underscored
       versions), provide our own replacements as they seem to be only used in
       libtiff own code anyhow.
     */
#   define   O_RDONLY    0x0000
#   define   O_WRONLY    0x0001
#   define   O_RDWR      0x0002
#   define   O_CREAT     0x0100
#   define   O_TRUNC     0x0200
#   define   O_EXCL      0x0400
#endif /* _WIN32_WCE */

#pragma warning(disable : 4996) /* function deprecation warnings */

#endif /* _TIF_CONFIG_H_ */
