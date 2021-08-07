
#pragma once

// undef: use double (64bit),
// defined: use float (32bit) for matrix and dot product calculations
#define FAST_FLOAT   1

// defined: support image sizes > 32K*32K pixels.
#define LARGE_IMAGES 1


#define __AVX2__                         1
#define __AVX__                          1
#define __FMA__                          1
#define __SSE4_1__                       1

#define HAVE_AVX2                        1
#define HAVE_AVX                         1
#define HAVE_FMA                         1
#define HAVE_LEPTONICA                   1
#define HAVE_LIBCURL                     1
#define CURL_STATICLIB                   1
#define HAVE_MUPDF                       1
#define HAVE_SSE4_1                      1
#define HAVE_TESSERACT                   1
#define HAVE_TIFFIO_H                    1

#if defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
#define HAVE_NEON                        1
#endif

#define TESSERACT_DISABLE_DEBUG_FONTS    1
#define TESSERACT_IMAGEDATA_AS_PIX       1

#undef GRAPHICS_DISABLED
#undef DISABLED_LEGACY_ENGINE

