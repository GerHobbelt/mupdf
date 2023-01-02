/* zconf-ng.h -- configuration of the zlib-ng compression library
 * Copyright (C) 1995-2022 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef ZCONFNG_H
#define ZCONFNG_H

#include "zlib_name_mangling-ng.h"

#if !defined(_WIN32) && defined(__WIN32__)
#  define _WIN32
#endif

#ifdef __STDC_VERSION__
#  if __STDC_VERSION__ >= 199901L
#    ifndef STDC99
#      define STDC99
#    endif
#  endif
#endif

/* Clang macro for detecting declspec support
 * https://clang.llvm.org/docs/LanguageExtensions.html#has-declspec-attribute
 */
#ifndef __has_declspec_attribute
#  define __has_declspec_attribute(x) 0
#endif

/* Always define z_const as const */
#define z_const const

/* Maximum value for memLevel in deflateInit2 */
#ifndef MAX_MEM_LEVEL
#  define MAX_MEM_LEVEL 9
#endif

/* Maximum value for windowBits in deflateInit2 and inflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */
#ifndef MAX_WBITS
#  define MAX_WBITS   15 /* 32K LZ77 window */
#endif

/* The memory requirements for deflate are (in bytes):
            (1 << (windowBits+2)) +  (1 << (memLevel+9))
 that is: 128K for windowBits=15  +  128K for memLevel = 8  (default values)
 plus a few kilobytes for small objects. For example, if you want to reduce
 the default memory requirements from 256K to 128K, compile with
     make CFLAGS="-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7"
 Of course this will generally degrade compression (there's no free lunch).

   The memory requirements for inflate are (in bytes) 1 << windowBits
 that is, 32K for windowBits=15 (default value) plus about 7 kilobytes
 for small objects.
*/

/* Type declarations */

#if defined(ZLIB_INTERNAL) && !defined(Z_INTERNAL)
#  define Z_INTERNAL ZLIB_INTERNAL
#endif

/* If building or using zlib as a DLL, define ZLIB_DLL.
 * This is not mandatory, but it offers a little performance increase.
 */
#if defined(ZLIB_DLL) && (defined(_WIN32) || (__has_declspec_attribute(dllexport) && __has_declspec_attribute(dllimport)))
#  ifdef Z_INTERNAL
#    define Z_EXTERN extern __declspec(dllexport)
#  else
#    define Z_EXTERN extern __declspec(dllimport)
#  endif
#endif

/* If building or using zlib with the WINAPI/WINAPIV calling convention,
 * define ZLIB_WINAPI.
 * Caution: the standard ZLIB1.DLL is NOT compiled using ZLIB_WINAPI.
 */
#if defined(ZLIB_WINAPI) && defined(_WIN32)
#  include <windows.h>
   /* No need for _export, use ZLIB.DEF instead. */
   /* For complete Windows compatibility, use WINAPI, not __stdcall. */
#  define Z_EXPORT WINAPI
#  define Z_EXPORTVA WINAPIV
#endif

#ifndef Z_EXTERN
#  define Z_EXTERN extern
#endif
#ifndef Z_EXPORT
#  define Z_EXPORT
#endif
#ifndef Z_EXPORTVA
#  define Z_EXPORTVA
#endif

/* Conditional exports */
#define ZNG_CONDEXPORT Z_EXPORT

/* Fallback for something that includes us. */
typedef unsigned char Byte;
typedef Byte Bytef;

typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

typedef char  charf;
typedef int   intf;
typedef uInt  uIntf;
typedef uLong uLongf;

typedef void const *voidpc;
typedef void       *voidpf;
typedef void       *voidp;

#ifdef HAVE_UNISTD_H    /* may be set to #if 1 by configure/cmake/etc */
#  define Z_HAVE_UNISTD_H
#endif

#ifdef NEED_PTRDIFF_T    /* may be set to #if 1 by configure/cmake/etc */
typedef PTRDIFF_TYPE ptrdiff_t;
#endif

#include <sys/types.h>      /* for off_t */
#include <stdarg.h>         /* for va_list */

#include <stddef.h>         /* for wchar_t and NULL */

/* a little trick to accommodate both "#define _LARGEFILE64_SOURCE" and
 * "#define _LARGEFILE64_SOURCE 1" as requesting 64-bit operations, (even
 * though the former does not conform to the LFS document), but considering
 * both "#undef _LARGEFILE64_SOURCE" and "#define _LARGEFILE64_SOURCE 0" as
 * equivalently requesting no 64-bit operations
 */
#if defined(_LARGEFILE64_SOURCE) && -_LARGEFILE64_SOURCE - -1 == 1
#  undef _LARGEFILE64_SOURCE
#endif

#if !defined(_WIN32) && (defined(Z_HAVE_UNISTD_H) || defined(_LARGEFILE64_SOURCE))
#  include <unistd.h>         /* for SEEK_*, off_t, and _LFS64_LARGEFILE */
#  ifndef z_off_t
#    define z_off_t off_t
#  endif
#endif

#if defined(_LFS64_LARGEFILE) && _LFS64_LARGEFILE-0
#  define Z_LFS64
#endif

#if defined(_LARGEFILE64_SOURCE) && defined(Z_LFS64)
#  define Z_LARGE64
#endif

#if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS-0 == 64 && defined(Z_LFS64)
#  define Z_WANT64
#endif

#if !defined(SEEK_SET) && defined(WITH_GZFILEOP)
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif

#ifndef z_off_t
#  define z_off_t long
#endif

#if !defined(_WIN32) && defined(Z_LARGE64)
#  define z_off64_t off64_t
#else
#  if defined(__MSYS__)
#    define z_off64_t _off64_t
#  elif defined(_WIN32) && !defined(__GNUC__)
#    define z_off64_t __int64
#  else
#    define z_off64_t z_off_t
#  endif
#endif



// build configuration:

#undef DUMP_BL_TREE

#undef NO_GZIP
//GUNZIP
//GZIP
#undef NO_GZCOMPRESS

//#define HAVE_BUILTIN_CTZ    1    // via fallback_builtins.h

#undef HAVE_POSIX_MEMALIGN
#undef HAVE_SYS_SDT_H      // s390
#undef HAVE_UNISTD_H       // UNIX

#undef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR

#define INFLATE_STRICT      1


#undef NO_MEDIUM_STRATEGY
#undef NO_QUICK_STRATEGY

#define NOT_TWEAK_COMPILER   1    // gcc (4.8.*) tweak

#define PKZIP_BUG_WORKAROUND   1


//POWER8_VSX_ADLER32
//POWER8_VSX_CHUNKSET
//POWER8_VSX_SLIDEHASH
//POWER_FEATURES
//PPC_FEATURES
//PPC_VMX_ADLER32
//PPC_VMX_SLIDEHASH
//S390_CRC32_VX
//S390_DFLTCC_DEFLATE
//S390_DFLTCC_INFLATE
//ARM_NEON_ADLER32
//ARM_NEON_CHUNKSET
//ARM_ACLE_CRC_HASH


//#define UNALIGNED64_OK   1
//#define UNALIGNED_OK     1

#define UNROLL_MORE    1

#undef USE_MMAP

#define WITH_GZFILEOP    1

#ifdef __AVX2__
#define X86_AVX2            1
#define X86_AVX2_ADLER32    1
#endif
#ifdef __AVX__
#define X86_AVX_CHUNKSET    1
#endif

#define X86_FEATURES        1

#define X86_PCLMULQDQ_CRC   1

#define X86_VPCLMULQDQ_CRC  1

#if defined(_M_IX86_FP) && (_M_IX86_FP > 0)
#define X86_SSE2            1
#define X86_SSE2_CHUNKSET   1
#define X86_SSE42_CMP_STR   1
#define X86_SSE42_CRC_HASH  1
#define X86_SSE42_CRC_INTRIN 1
#define X86_SSSE3_ADLER32  1
#endif

#undef ZLIB_COMPAT

#undef ZLIB_DEBUG              // define this one to collect run-time statistics for analysis/debugging



#define __vector /* no-op */



// provide the ctz intrinsics a la GCC:
#include "fallback_builtins.h"



#endif /* ZCONFNG_H */
