/*
    This file is used on systems not using configure only and contains
    hardcoded definitions for MSVC, Xcode and MinGW when using makefiles.
 */

/*
  Configuration defines for installed libtiff.
  This file maintained for backward compatibility. Do not use definitions
  from this file in your programs.
*/

#ifndef _TIFFCONF_
#define _TIFFCONF_

#include "tif_config.h"


/* Compatibility stuff. */

/* Support Deflate compression */
#define ZIP_SUPPORT 1

/* Support zstd compression */
#define ZSTD_SUPPORT 1

/* Support CCITT Group 3 & 4 algorithms */
#define CCITT_SUPPORT 1

/* Pick up YCbCr subsampling info from the JPEG data stream to support files
   lacking the tag (default enabled). */
#define CHECK_JPEG_YCBCR_SUBSAMPLING 1

/* enable partial strip reading for large strips (experimental) */
/* #undef CHUNKY_STRIP_READ_SUPPORT */

/* Support C++ stream API (requires C++ compiler) */
#define CXX_SUPPORT 1

/* Treat extra sample as alpha (default enabled). The RGBA interface will
   treat a fourth sample with no EXTRASAMPLE_ value as being ASSOCALPHA. Many
   packages produce RGBA files but don't mark the alpha properly. */
#define DEFAULT_EXTRASAMPLE_AS_ALPHA 1

/* enable deferred strip/tile offset/size loading */
/* #undef DEFER_STRILE_LOAD */

/* Support WEBP compression */
#define WEBP_SUPPORT 1

/* 8/12 bit libjpeg dual mode enabled */
#undef JPEG_DUAL_MODE_8_12

/* Support JPEG compression (requires IJG JPEG library) */
#define JPEG_SUPPORT 1

/* Support ISO JBIG compression (requires JBIG-KIT library) */
#undef JBIG_SUPPORT

/* Support LERC compression */
/* #undef LERC_SUPPORT */

/* Support libdeflate enhanced compression */
#undef LIBDEFLATE_SUPPORT

/* 12bit libjpeg primary include file with path */
#undef LIBJPEG_12_PATH 

/* Support LogLuv high dynamic range encoding */
#define LOGLUV_SUPPORT 1

/* Support LZMA2 compression */
/* #undef LZMA_SUPPORT */

/* Support LZW algorithm */
#define LZW_SUPPORT 1

/* Support Microsoft Document Imaging format */
#define MDI_SUPPORT 1

/* Support NeXT 2-bit RLE algorithm */
#define NEXT_SUPPORT 1

/* Support Old JPEG compression (read contrib/ojpeg/README first! Compilation
   fails with unpatched IJG JPEG library) */
/* #undef OJPEG_SUPPORT */

/* Support Macintosh PackBits algorithm */
#define PACKBITS_SUPPORT 1

/* Support Pixar log-format algorithm (requires Zlib) */
/* #undef PIXARLOG_SUPPORT */

/* Support ThunderScan 4-bit RLE algorithm */
#define THUNDER_SUPPORT 1

/* Support strip chopping (whether or not to convert single-strip uncompressed
   images to multiple strips of specified size to reduce memory usage) */
#define STRIPCHOP_DEFAULT TIFF_STRIPCHOP

/* Default size of the strip in bytes (when strip chopping enabled) */
#define STRIP_SIZE_DEFAULT 8192

/* Enable SubIFD tag (330) support */
#define SUBIFD_SUPPORT 1

/* Treat extra sample as alpha (default enabled). The RGBA interface will
   treat a fourth sample with no EXTRASAMPLE_ value as being ASSOCALPHA. Many
   packages produce RGBA files but don't mark the alpha properly. */
#define DEFAULT_EXTRASAMPLE_AS_ALPHA 1

/* Pick up YCbCr subsampling info from the JPEG data stream to support files
   lacking the tag (default enabled). */
#define CHECK_JPEG_YCBCR_SUBSAMPLING 1

/* Support MS MDI magic number files as TIFF */
#define MDI_SUPPORT 1

/*
 * Feature support definitions.
 * XXX: These macros are obsoleted. Don't use them in your apps!
 * Macros stays here for backward compatibility and should be always defined.
 */
#define COLORIMETRY_SUPPORT
#define YCBCR_SUPPORT
#define CMYK_SUPPORT
#define ICC_SUPPORT
#define PHOTOSHOP_SUPPORT
#define IPTC_SUPPORT

#endif /* _TIFFCONF_ */

