/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEG_LIB_VERSION >= 60".
 */
#define JPEG_LIB_VERSION  94

/* libjpeg-turbo version */
#define LIBJPEG_TURBO_VERSION  2.0.91

/* libjpeg-turbo version in integer form */
#define LIBJPEG_TURBO_VERSION_NUMBER  2000091
//#define JPEG_LIB_VERSION_MAJOR 9
//#define JPEG_LIB_VERSION_MINOR 4
#define LIBJPEG_TURBO_VERSION_MAJOR 2
#define LIBJPEG_TURBO_VERSION_MINOR 0
#define LIBJPEG_TURBO_VERSION_PATCH 91


/* Support arithmetic encoding */
#define C_ARITH_CODING_SUPPORTED  1

/* Support arithmetic decoding */
#define D_ARITH_CODING_SUPPORTED  1

/* Support in-memory source/destination managers */
#define MEM_SRCDST_SUPPORTED  1

/* Use accelerated SIMD routines. */
#define WITH_SIMD  1

/* Disable GETENV */
#define NO_GETENV 1    // Disable getenv() function, i.e. don't listen to the "JPEGMEM" environment variable to adjust memory usage choices at run-time. See commit: f713b13593bfd323b3d56b0b7fd6e266e8c3cde4

/* Support 3DNow */
#undef WITH_3DNOW		// AMD dropped 3DNow support about 8 years ago; see commit 84ca5c34907870a98cdff3e9900fe241d452cf06

/* Enable full tracing */
#ifndef JPEGLIB_WITH_FULLTRACING
#define WITH_FULLTRACING 1
#endif

/* Enable compressor part */
#define JPEGLIB_ENABLE_COMPRESS 1

/*
 * Define BITS_IN_JSAMPLE as either
 *   8   for 8-bit sample values (the usual setting)
 *   12  for 12-bit sample values
 * Only 8 and 12 are legal data precisions for lossy JPEG according to the
 * JPEG standard, and the IJG code does not support anything else!
 * We do not support run-time selection of data precision, sorry.
 */

#ifndef BITS_IN_JSAMPLE  
#define BITS_IN_JSAMPLE  8      /* use 8 or 12 */
#endif

#undef HAVE_JPEGTURBO_DUAL_MODE_8_12 

/* Define to 1 if you have the <locale.h> header file. */
#undef HAVE_LOCALE_H

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H
#endif

/* Define if you need to include <sys/types.h> to get size_t. */
#undef NEED_SYS_TYPES_H

/* Define if you have BSD-like bzero and bcopy in <strings.h> rather than
   memset/memcpy in <string.h>. */
#undef NEED_BSD_STRINGS

/* Define to 1 if the system has the type `unsigned char'. */
#define HAVE_UNSIGNED_CHAR

/* Define to 1 if the system has the type `unsigned short'. */
#define HAVE_UNSIGNED_SHORT

/* Compiler does not support pointers to undefined structures. */
#undef INCOMPLETE_TYPES_BROKEN

/* Define if your (broken) compiler shifts signed values as if they were
   unsigned. */
#undef RIGHT_SHIFT_IS_UNSIGNED

/* Define "boolean" as unsigned char, not int, per Windows custom */
#ifndef __RPCNDR_H__            /* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#define HAVE_BOOLEAN            /* prevent jmorecfg.h from redefining it */

/* Define "INT32" as int, not long, per Windows custom */
#if !(defined(_BASETSD_H_) || defined(_BASETSD_H))   /* don't conflict if basetsd.h already read */
typedef short INT16;
typedef signed int INT32;
#endif
#define XMD_H                   /* prevent jmorecfg.h from redefining it */
