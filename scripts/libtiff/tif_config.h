#ifndef _TIF_CONFIG_H_
#define _TIF_CONFIG_H_


#include "tiffconf.h"

/* Support CCITT Group 3 & 4 algorithms */
#define CCITT_SUPPORT 1

/* Pick up YCbCr subsampling info from the JPEG data stream to support files
   lacking the tag (default enabled). */
#define CHECK_JPEG_YCBCR_SUBSAMPLING 1

/* enable partial strip reading for large strips (experimental) */
/* #undef CHUNKY_STRIP_READ_SUPPORT */

/* Support C++ stream API (requires C++ compiler) */
#define CXX_SUPPORT 1

/* enable deferred strip/tile offset/size loading (experimental) */
/* #undef DEFER_STRILE_LOAD */

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the declaration of `optarg', and to 0 if you don't. */
/* #undef HAVE_DECL_OPTARG */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define as 0 or 1 according to the floating point format suported by the
   machine */
#define HAVE_IEEEFP 1

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

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <io.h> header file. */
#define HAVE_IO_H 1

/* Define to 1 if you have the `jbg_newlen' function. */
#define HAVE_JBG_NEWLEN 1

/* Define to 1 if you have the `mmap' function. */
/* #undef HAVE_MMAP */

/* Define to 1 if you have the <OpenGL/glu.h> header file. */
/* #undef HAVE_OPENGL_GLU_H */

/* Define to 1 if you have the <OpenGL/gl.h> header file. */
/* #undef HAVE_OPENGL_GL_H */

/* Define to 1 if you have the `setmode' function. */
#define HAVE_SETMODE 1

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <search.h> header file. */
#define HAVE_SEARCH_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* 8/12 bit libjpeg dual mode enabled */
/* #undef JPEG_DUAL_MODE_8_12 */

/* Support LERC compression */
/* #undef LERC_SUPPORT */

/* 12bit libjpeg primary include file with path */
#define LIBJPEG_12_PATH 

/* Define to 1 if you have the `setmode' function. */
#define HAVE_SETMODE 1

/* Support LZMA2 compression */
/* #undef LZMA_SUPPORT */

/* Name of package */
#define PACKAGE "LibTIFF Software"

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

/* Define to 1 if you have the declaration of `optarg', and to 0 if you don't. */
#define HAVE_DECL_OPTARG 0

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* Signed 64-bit type formatter */
#define TIFF_INT64_FORMAT "%I64d"

/* Signed 64-bit type */
#define TIFF_INT64_T signed __int64

/* Unsigned 64-bit type formatter */
#define TIFF_UINT64_FORMAT "%I64u"

/* Unsigned 64-bit type */
#define TIFF_UINT64_T unsigned __int64

#if _WIN64
/*
  Windows 64-bit build
*/

/* Pointer difference type */
#  define TIFF_PTRDIFF_T TIFF_INT64_T

/* The size of `size_t', as computed by sizeof. */
#  define SIZEOF_SIZE_T 8

/* Size type formatter */
#  define TIFF_SIZE_FORMAT TIFF_INT64_FORMAT

/* Unsigned size type */
#  define TIFF_SIZE_T TIFF_UINT64_T

/* Signed size type formatter */
#  define TIFF_SSIZE_FORMAT TIFF_INT64_FORMAT

/* Signed size type */
#  define TIFF_SSIZE_T TIFF_INT64_T

#else
/*
  Windows 32-bit build
*/

/* Pointer difference type */
#  define TIFF_PTRDIFF_T signed int

/* The size of `size_t', as computed by sizeof. */
#  define SIZEOF_SIZE_T 4

/* Size type formatter */
#  define TIFF_SIZE_FORMAT "%u"

/* Size type formatter */
#  define TIFF_SIZE_FORMAT "%u"

/* Unsigned size type */
#  define TIFF_SIZE_T unsigned int

/* Signed size type formatter */
#  define TIFF_SSIZE_FORMAT "%d"

/* Signed size type */
#  define TIFF_SSIZE_T signed int

#endif


/* Default size of the strip in bytes (when strip chopping enabled) */
#define STRIP_SIZE_DEFAULT 8192

/* define to use win32 IO system */
#define USE_WIN32_FILEIO 1

/* Version number of package */
#define VERSION "4.2.0"

/* Support WEBP compression */
/* #undef WEBP_SUPPORT */

/* Support ZSTD compression */
/* #undef ZSTD_SUPPORT */


/* Set the native cpu bit order */
#define HOST_FILLORDER FILLORDER_LSB2MSB

/*
  Please see associated settings in "nmake.opt" which configure porting
  settings. It should not be necessary to edit the following pre-processor
  logic.
*/
#if defined(_MSC_VER)
/* Visual Studio 2015 / VC 14 / MSVC 19.00 finally has snprintf() */
#  if _MSC_VER < 1900 /* Visual C++ 2015 */
#    define snprintf _snprintf
#  else
#    define HAVE_SNPRINTF 1
#  endif
#  define HAVE_STRTOL 1
#  define HAVE_STRTOUL 1
#  if _MSC_VER >= 1900 /* Visual Studio 2015 added strtoll/strtoull */
#    define HAVE_STRTOLL 1
#    define HAVE_STRTOULL 1
#  endif
#endif

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
# ifndef inline
#  define inline __inline
# endif
#endif

#define lfind _lfind

#pragma warning(disable : 4996) /* function deprecation warnings */

#endif /* _TIF_CONFIG_H_ */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * fill-column: 78
 * End:
 */
