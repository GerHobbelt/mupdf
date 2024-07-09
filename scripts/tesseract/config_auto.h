
#pragma once

// undef: use double (64bit),
// defined: use float (32bit) for matrix and dot product calculations
#define FAST_FLOAT                      1

// defined: support image sizes > 32K*32K pixels.
#undef LARGE_IMAGES                    


// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?redirectedfrom=MSDN&view=msvc-160
//
//#define __AVX2__                         1
//#define __AVX__                          1
#if defined(_M_IX86_FP) && (_M_IX86_FP > 0)
#define __FMA__                          1
#define __SSE4_1__                       1
#endif

#ifdef __AVX2__
#define HAVE_AVX2                        1
#endif
#ifdef __AVX__
#define HAVE_AVX                         1
#endif
#ifdef __FMA__
#define HAVE_FMA                         1
#endif
#define HAVE_LEPTONICA                   1
#define HAVE_LIBCURL                     1
#define HAVE_LIBARCHIVE                  1
#define LIBARCHIVE_STATIC                1
#define CURL_STATICLIB                   1
#define HAVE_MUPDF                       1
#if defined(__SSE4_1__) || defined(__AVX2__) // anyone with AVX2 surely has SSE4.1 as well
#define HAVE_SSE4_1                      1
#endif
#define HAVE_TESSERACT                   1
#define HAVE_TIFFIO_H                    1

#if defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
#define HAVE_NEON                        1
#endif

#define TESSERACT_DISABLE_DEBUG_FONTS    1   /* 0: debug code will try to print caption at the bottom of the debug pixes; won't work well for tiny image snippets, so use with caution; 1: caption ends up in the HTML output and all is fine */
#define TESSERACT_IMAGEDATA_AS_PIX       1

#define GRAPHICS_DISABLED                0
#define DISABLED_LEGACY_ENGINE           0

#define PANGO_ENABLE_ENGINE              1

#define HAS_LIBICU                       1

// icu defines required to produce a matching compile/build:
#define U_CHARSET_IS_UTF8                1
#define U_STATIC_IMPLEMENTATION
#define U_IMPORT /**/

#if !defined(DISABLED_LEGACY_ENGINE)
#define DISABLED_LEGACY_ENGINE           0
#endif

