/*
    RawSpeed - RAW file decoder.

    Copyright (C) 2016-2018 Roman Lebedev

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#pragma once

#include <assert.h>

#if defined(__SSE2__)
#define WITH_SSE2
#else
/* #undef WITH_SSE2 */
#endif

// NOLINTNEXTLINE(google-runtime-int)
static constexpr unsigned long long RAWSPEED_CACHELINESIZE = 64;
static_assert(RAWSPEED_CACHELINESIZE > 0 &&
                  ((RAWSPEED_CACHELINESIZE & (RAWSPEED_CACHELINESIZE - 1)) ==
                   0),
              "Expected to know CPU L1d cache line size.");

// NOLINTNEXTLINE(google-runtime-int)
static constexpr unsigned long long RAWSPEED_PAGESIZE = 4096;
static_assert(RAWSPEED_PAGESIZE > 0 &&
                  ((RAWSPEED_PAGESIZE & (RAWSPEED_PAGESIZE - 1)) == 0),
              "Expected to know (minimal) CPU page size.");
static_assert(RAWSPEED_PAGESIZE >= RAWSPEED_CACHELINESIZE,
              "Expected that (minimal) CPU page size is not smaller than "
              "(minimal) CPU page size.");

// NOLINTNEXTLINE(google-runtime-int)
static constexpr unsigned long long RAWSPEED_LARGEPAGESIZE =
    4096;
static_assert(RAWSPEED_LARGEPAGESIZE > 0 &&
                  ((RAWSPEED_LARGEPAGESIZE & (RAWSPEED_LARGEPAGESIZE - 1)) ==
                   0),
              "Expected to know CPU page size.");
static_assert(RAWSPEED_LARGEPAGESIZE >= RAWSPEED_PAGESIZE,
              "Expected that CPU large page size is not smaller than (minimal) "
              "CPU page size.");

#define HAVE_PUGIXML 

#ifndef NO_OPENMP
#define HAVE_OPENMP
#endif

#define HAVE_ZLIB 

#define HAVE_JPEG 
#define HAVE_JPEG_MEM_SRC 

#define HAVE_CXX_THREAD_LOCAL
/* #undef HAVE_GCC_THREAD_LOCAL */

#undef RAWSPEED_STANDALONE_BUILD
#ifdef RAWSPEED_STANDALONE_BUILD
#define RAWSPEED_SOURCE_DIR "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/rawspeed"
#else
// If rawspeed is being built as part of some larger build, we can not retain
// the RAWSPEED_SOURCE_DIR, because that would affect the reproducible builds.
#endif

// see http://clang.llvm.org/docs/LanguageExtensions.html
#ifndef __has_feature      // Optional of course.
#define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
#define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if !defined(_MSC_VER) && ( ( defined(__has_builtin) && __has_builtin(__attribute__) ) || defined(__attribute__) || defined(__GNUC__) )

#define RAWSPEED_UNLIKELY_FUNCTION __attribute__((cold))
#define RAWSPEED_NOINLINE __attribute__((noinline))
#define RAWSPEED_ALWAYS_INLINE __attribute__((always_inline))
#define RAWSPEED_READONLY __attribute__((pure))
#define RAWSPEED_READNONE __attribute__((const))

#elif defined(_MSC_VER)

#ifndef __attribute__
#define __attribute__(x)  /**/
#endif

#define RAWSPEED_UNLIKELY_FUNCTION 
#define RAWSPEED_NOINLINE        __declspec(noinline)
#define RAWSPEED_ALWAYS_INLINE   __forceinline
#define RAWSPEED_READONLY 
#define RAWSPEED_READNONE 

// https://stackoverflow.com/questions/60802864/emulating-gccs-builtin-unreachable-in-visual-studio
#define __builtin_unreachable()    __assume(0)

#else

#define RAWSPEED_UNLIKELY_FUNCTION 
#define RAWSPEED_NOINLINE 
#define RAWSPEED_ALWAYS_INLINE 
#define RAWSPEED_READONLY 
#define RAWSPEED_READNONE 

#define __builtin_unreachable()     std::unreachable()    // only available in C++23, though...

#endif

// https://stackoverflow.com/questions/48857887/pretty-function-in-visual-c
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__     __FUNCSIG__
#endif
