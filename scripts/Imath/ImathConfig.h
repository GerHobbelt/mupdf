// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenEXR Project.

// This file is auto-generated by the cmake configure step

#ifndef INCLUDED_IMATH_CONFIG_H
#define INCLUDED_IMATH_CONFIG_H 1

#pragma once

//
// Options / configuration based on O.S. / compiler
/////////////////////

//
// Define if the target system has support for large
// stack sizes.
//
/* #undef IMATH_HAVE_LARGE_STACK */

//////////////////////
//
// C++ namespace configuration / options

// Current (internal) library namepace name and corresponding public
// client namespaces.
#define IMATH_INTERNAL_NAMESPACE_CUSTOM 0
#define IMATH_INTERNAL_NAMESPACE Imath_3_1


#define IMATH_NAMESPACE_CUSTOM 0
#define IMATH_NAMESPACE Imath


//
// Version information
//
#define IMATH_VERSION_STRING "3.1.0"
#define IMATH_PACKAGE_STRING "Imath 3.1.0-dev"

#define IMATH_VERSION_MAJOR 3
#define IMATH_VERSION_MINOR 1
#define IMATH_VERSION_PATCH 0

#define IMATH_VERSION_HEX ((uint32_t(IMATH_VERSION_MAJOR) << 24) | \
                             (uint32_t(IMATH_VERSION_MINOR) << 16) | \
                             (uint32_t(IMATH_VERSION_PATCH) <<  8))

// IMATH_LIB_VERSION is the library API version: SOCURRENT.SOAGE.SOREVISION
#define IMATH_LIB_VERSION_STRING "29.0.0"


//
// By default, opt into the interoparability constructors and assignments.
// If this causes problems, it can be disabled by defining this symbol to
// be 0 prior to including any Imath headers.
//
// If no such definition is found, we enable automatically unless we are
// using gcc 4.x, which appears to have bugs that prevent the interop
// templates from compiling correctly.
//
#ifndef IMATH_FOREIGN_VECTOR_INTEROP
#  if defined(__GNUC__) && __GNUC__ == 4 && !defined(__clang__)
#    define IMATH_FOREIGN_VECTOR_INTEROP 0
#  else
#    define IMATH_FOREIGN_VECTOR_INTEROP 1
#  endif
#endif


//
// Decorator that makes a function available for both CPU and GPU, when
// compiling for Cuda.
//
#ifdef __CUDACC__
  #define IMATH_HOSTDEVICE __host__ __device__
#else
  #define IMATH_HOSTDEVICE
#endif


//
// Some compilers define a special intrinsic to use in conditionals that can
// speed up extremely performance-critical spots if the conditional is
// usually (or rarely) is true.  You use it by replacing
//     if (x) ...
// with
//     if (IMATH_LIKELY(x)) ...     // if you think x will usually be true
// or
//     if (IMATH_UNLIKELY(x)) ...   // if you think x will rarely be true
//
// Caveat: Programmers are notoriously bad at guessing this, so it should be
// used only with thorough benchmarking.
//
#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
#    define IMATH_LIKELY(x) (__builtin_expect(bool(x), true))
#    define IMATH_UNLIKELY(x) (__builtin_expect(bool(x), false))
#else
#    define IMATH_LIKELY(x) (x)
#    define IMATH_UNLIKELY(x) (x)
#endif


//
// Simple way to mark things as deprecated.
// When we are sure that C++14 is our true minimum, then we can just
// directly use [[deprecated(msg)]].
//
#if defined(_MSC_VER)
# define IMATH_DEPRECATED(msg) __declspec(deprecated(msg))
#elif __cplusplus >= 201402L
# define IMATH_DEPRECATED(msg) [[deprecated(msg)]]
#elif defined(__GNUC__) || defined(__clang__)
# define IMATH_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
# define IMATH_DEPRECATED(msg) /* unsupported on this platform */
#endif

// Whether the user configured the library to have symbol visibility
// tagged
#define IMATH_ENABLE_API_VISIBILITY

// MSVC does not do the same visibility attributes, and when we are
// compiling a static library we won't be in DLL mode, but just don't
// define these and the export headers will work out
#if ! defined(_MSC_VER) && defined(IMATH_ENABLE_API_VISIBILITY)
#  define IMATH_PUBLIC_SYMBOL_ATTRIBUTE __attribute__ ((__visibility__ ("default")))
#  define IMATH_PRIVATE_SYMBOL_ATTRIBUTE __attribute__ ((__visibility__ ("hidden")))
// clang differs from gcc and has type visibility which is needed for enums and templates
#  if __has_attribute(__type_visibility__)
#    define IMATH_PUBLIC_TYPE_VISIBILITY_ATTRIBUTE __attribute__ ((__type_visibility__ ("default")))
#  endif
#endif

#endif // INCLUDED_IMATH_CONFIG_H