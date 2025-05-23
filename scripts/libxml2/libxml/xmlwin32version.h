/*
 * Summary: compile-time version information on Windows
 * Description: compile-time version information for the XML library
 *              when compiled on the Windows platform
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef __XML_WIN_VERSION_H__
#define __XML_WIN_VERSION_H__

#include <libxml/xmlexports.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * use those to be sure nothing nasty will happen if
 * your library and includes mismatch
 */
XMLPUBFUN void XMLCALL xmlCheckVersion(int version);

/**
 * LIBXML_DOTTED_VERSION:
 *
 * the version string like "1.2.3"
 */
#define LIBXML_DOTTED_VERSION "2.15.0"

/**
 * LIBXML_VERSION:
 *
 * the version number: 1.2.3 value is 10203
 */
#define LIBXML_VERSION 21500

/**
 * LIBXML_VERSION_STRING:
 *
 * the version number string, 1.2.3 value is "10203"
 */
#define LIBXML_VERSION_STRING "21500"

/**
 * LIBXML_VERSION_EXTRA:
 *
 * extra version information, used to show a git commit description
 */
#define LIBXML_VERSION_EXTRA "-win32"

/**
 * LIBXML_TEST_VERSION:
 *
 * Macro to check that the libxml version in use is compatible with
 * the version the software has been compiled against
 */
#define LIBXML_TEST_VERSION() xmlCheckVersion(21500)

/**
 * LIBXML_THREAD_ENABLED:
 *
 * Whether the thread support is configured in
 */
#if 1
#define LIBXML_THREAD_ENABLED
#endif

/**
 * LIBXML_THREAD_ALLOC_ENABLED:
 *
 * Whether the allocation hooks are per-thread
 */
#if 0
#define LIBXML_THREAD_ALLOC_ENABLED
#endif

/**
 * LIBXML_TREE_ENABLED:
 *
 * Always enabled since 2.14.0
 */
#define LIBXML_TREE_ENABLED

/**
 * LIBXML_OUTPUT_ENABLED:
 *
 * Whether the serialization/saving support is configured in
 */
#if 1
#define LIBXML_OUTPUT_ENABLED
#endif

/**
 * LIBXML_PUSH_ENABLED:
 *
 * Whether the push parsing interfaces are configured in
 */
#if 1
#define LIBXML_PUSH_ENABLED
#endif

/**
 * LIBXML_READER_ENABLED:
 *
 * Whether the xmlReader parsing interface is configured in
 */
#if 1
#define LIBXML_READER_ENABLED
#endif

/**
 * LIBXML_PATTERN_ENABLED:
 *
 * Whether the xmlPattern node selection interface is configured in
 */
#if 1
#define LIBXML_PATTERN_ENABLED
#endif

/**
 * LIBXML_WRITER_ENABLED:
 *
 * Whether the xmlWriter saving interface is configured in
 */
#if 1
#define LIBXML_WRITER_ENABLED
#endif

/**
 * LIBXML_SAX1_ENABLED:
 *
 * Whether the older SAX1 interface is configured in
 */
#if 1
#define LIBXML_SAX1_ENABLED
#endif

/**
 * LIBXML_FTP_ENABLED:
 *
 * Whether the FTP support is configured in
 */
#if 1
#define LIBXML_FTP_ENABLED
#endif

/**
 * LIBXML_HTTP_ENABLED:
 *
 * Whether the HTTP support is configured in
 */
#if 1
#define LIBXML_HTTP_ENABLED
#endif

/**
 * LIBXML_VALID_ENABLED:
 *
 * Whether the DTD validation support is configured in
 */
#if 1
#define LIBXML_VALID_ENABLED
#endif

/**
 * LIBXML_HTML_ENABLED:
 *
 * Whether the HTML support is configured in
 */
#if 1
#define LIBXML_HTML_ENABLED
#endif

/**
 * LIBXML_LEGACY_ENABLED:
 *
 * Removed in 2.14
 */
#undef LIBXML_LEGACY_ENABLED

/**
 * LIBXML_C14N_ENABLED:
 *
 * Whether the Canonicalization support is configured in
 */
#if 1
#define LIBXML_C14N_ENABLED
#endif

/**
 * LIBXML_CATALOG_ENABLED:
 *
 * Whether the Catalog support is configured in
 */
#if 1
#define LIBXML_CATALOG_ENABLED
#endif

/**
 * LIBXML_DOCB_ENABLED:
 *
 * Whether the SGML Docbook support is configured in     (support has been dropped in libxml2)
 */
#if 0
#define LIBXML_DOCB_ENABLED
#endif

/**
 * LIBXML_XPATH_ENABLED:
 *
 * Whether XPath is configured in
 */
#if 1
#define LIBXML_XPATH_ENABLED
#endif

/**
 * LIBXML_XPTR_ENABLED:
 *
 * Whether XPointer is configured in
 */
#if 1
#define LIBXML_XPTR_ENABLED
#endif

/**
 * LIBXML_XPTR_LOCS_ENABLED:
 *
 * Whether support for XPointer locations is configured in
 */
#if 1
#define LIBXML_XPTR_LOCS_ENABLED
#endif

/**
 * LIBXML_XINCLUDE_ENABLED:
 *
 * Whether XInclude is configured in
 */
#if 1
#define LIBXML_XINCLUDE_ENABLED
#endif

/**
 * LIBXML_ICONV_ENABLED:
 *
 * Whether iconv support is available
 */
#if 1
#define LIBXML_ICONV_ENABLED
#endif

/**
 * LIBXML_ICU_ENABLED:
 *
 * Whether icu support is available
 */
#if 1
#define LIBXML_ICU_ENABLED
#endif

/**
 * LIBXML_ISO8859X_ENABLED:
 *
 * Whether ISO-8859-* support is made available in case iconv is not
 */
#if 0
#define LIBXML_ISO8859X_ENABLED
#endif

/**
 * LIBXML_DEBUG_ENABLED:
 *
 * Whether Debugging module is configured in
 */
#if 1
#define LIBXML_DEBUG_ENABLED
#endif

/**
 * DEBUG_MEMORY_LOCATION:
 *
 * Whether the memory debugging is configured in
 */
#if 0
#define DEBUG_MEMORY_LOCATION
#endif

/**
 * LIBXML_DEBUG_RUNTIME:
 *
 * Whether the runtime debugging is configured in
 */
#if 0
#define LIBXML_DEBUG_RUNTIME
#endif

/**
 * LIBXML_UNICODE_ENABLED:
 *
 * Whether the Unicode related interfaces are compiled in
 * Removed in 2.14
 */
#if 1
#define LIBXML_UNICODE_ENABLED
#endif

/**
 * LIBXML_REGEXP_ENABLED:
 *
 * Whether the regular expressions interfaces are compiled in
 */
#if 1
#define LIBXML_REGEXP_ENABLED
#endif

/**
 * LIBXML_AUTOMATA_ENABLED:
 *
 * Whether the automata interfaces are compiled in
 */
#if 1
#define LIBXML_AUTOMATA_ENABLED
#endif

/**
 * LIBXML_EXPR_ENABLED:
 *
 * Whether the formal expressions interfaces are compiled in
 *
 * This code is unused and disabled unconditionally for now.
 */
#if 0
#define LIBXML_EXPR_ENABLED
#endif

/**
 * LIBXML_RELAXNG_ENABLED:
 *
 * Whether the RelaxNG validation interfaces are compiled in
 */
#if 1
#define LIBXML_RELAXNG_ENABLED
#endif

/**
 * LIBXML_SCHEMAS_ENABLED:
 *
 * Whether the Schemas validation interfaces are compiled in
 */
#if 1
#define LIBXML_SCHEMAS_ENABLED
#endif

/**
 * LIBXML_SCHEMATRON_ENABLED:
 *
 * Whether the Schematron validation interfaces are compiled in
 */
#if 1
#define LIBXML_SCHEMATRON_ENABLED
#endif

/**
 * LIBXML_MODULES_ENABLED:
 *
 * Whether the module interfaces are compiled in
 */
#if 1
#define LIBXML_MODULES_ENABLED
/**
 * LIBXML_MODULE_EXTENSION:
 *
 * the string suffix used by dynamic modules (usually shared libraries)
 */
#define LIBXML_MODULE_EXTENSION ".dll" 
#endif

/**
 * LIBXML_ZLIB_ENABLED:
 *
 * Whether the Zlib support is compiled in
 */
#if 1
#define LIBXML_ZLIB_NG_ENABLED
#define LIBXML_ZLIB_ENABLED
#endif

/**
 * LIBXML_ZLIB_NG_ENABLED:
 *
 * Whether the Zlib-NG support is compiled in
 */
#if 1
#define LIBXML_ZLIB_NG_ENABLED
#endif

/**
 * LIBXML_LZMA_ENABLED:
 *
 * Whether the Lzma support is compiled in
 */
#if 1
#if defined(_MSC_VER) && _MSC_VER >= 1800 // VS2013 or later
#define LIBXML_LZMA_ENABLED
#endif
#endif


/**
 * LIBXML_DLL_IMPORT:
 *
 * Used on Windows (MS C compiler only) to declare a variable as
 * imported from the library. This macro should be empty when compiling
 * libxml itself. It should expand to __declspec(dllimport)
 * when the client code includes this header, and that only if the client
 * links dynamically against libxml.
 * For this to work, we need three macros. One tells us which compiler is
 * being used and luckily the compiler defines such a thing: _MSC_VER. The
 * second macro tells us if we are compiling libxml or the client code and
 * we define the macro IN_LIBXML on the compiler's command line for this
 * purpose. The third macro, LIBXML_STATIC, must be defined by any client
 * code which links against libxml statically.
 */
#ifndef LIBXML_DLL_IMPORT
#if defined(_MSC_VER) && !defined(IN_LIBXML) && !defined(LIBXML_STATIC)
#define LIBXML_DLL_IMPORT __declspec(dllimport)
#else
#define LIBXML_DLL_IMPORT
#endif
#endif

#ifdef __GNUC__

/**
 * ATTRIBUTE_UNUSED:
 *
 * Macro used to signal to GCC unused function parameters
 */

#ifndef ATTRIBUTE_UNUSED
# if ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 7)))
#  define ATTRIBUTE_UNUSED __attribute__((unused))
# else
#  define ATTRIBUTE_UNUSED
# endif
#endif

/**
 * LIBXML_ATTR_ALLOC_SIZE:
 *
 * Macro used to indicate to GCC this is an allocator function
 */

#ifndef LIBXML_ATTR_ALLOC_SIZE
# if (!defined(__clang__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))))
#  define LIBXML_ATTR_ALLOC_SIZE(x) __attribute__((alloc_size(x)))
# else
#  define LIBXML_ATTR_ALLOC_SIZE(x)
# endif
#else
# define LIBXML_ATTR_ALLOC_SIZE(x)
#endif

/**
 * LIBXML_ATTR_FORMAT:
 *
 * Macro used to indicate to GCC the parameter are printf like
 */

#ifndef LIBXML_ATTR_FORMAT
# if ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3)))
#  define LIBXML_ATTR_FORMAT(fmt,args) __attribute__((__format__(__printf__,fmt,args)))
# else
#  define LIBXML_ATTR_FORMAT(fmt,args)
# endif
#else
# define LIBXML_ATTR_FORMAT(fmt,args)
#endif

#ifndef XML_DEPRECATED
#  if defined (IN_LIBXML) || (__GNUC__ * 100 + __GNUC_MINOR__ < 301)
#    define XML_DEPRECATED
#  elif defined (_MSC_VER) && (_MSC_VER >= 1400)
#    define XML_DEPRECATED __declspec(deprecated)
/* Available since at least GCC 3.1 */
#  else
#    define XML_DEPRECATED __attribute__((deprecated))
#  endif
#endif

#if defined(__LCC__)
  #define XML_IGNORE_FPTR_CAST_WARNINGS
  #define XML_POP_WARNINGS \
    _Pragma("diag_default 1215")
#elif defined(__clang__) || (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 406))
  #if defined(__clang__) || (__GNUC__ * 100 + __GNUC_MINOR__ >= 800)
    #define XML_IGNORE_FPTR_CAST_WARNINGS \
      _Pragma("GCC diagnostic push") \
      _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
      _Pragma("GCC diagnostic ignored \"-Wcast-function-type\"")
  #else
    #define XML_IGNORE_FPTR_CAST_WARNINGS \
      _Pragma("GCC diagnostic push") \
      _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
  #endif
  #define XML_POP_WARNINGS \
    _Pragma("GCC diagnostic pop")
#else
  #define XML_IGNORE_FPTR_CAST_WARNINGS
  #define XML_POP_WARNINGS
#endif

#else /* ! __GNUC__ */

/**
 * ATTRIBUTE_UNUSED:
 *
 * Macro used to signal to GCC unused function parameters
 */
#define ATTRIBUTE_UNUSED
/**
 * LIBXML_ATTR_ALLOC_SIZE:
 *
 * Macro used to indicate to GCC this is an allocator function
 */
#define LIBXML_ATTR_ALLOC_SIZE(x)
/**
 * LIBXML_ATTR_FORMAT:
 *
 * Macro used to indicate to GCC the parameter are printf like
 */
#define LIBXML_ATTR_FORMAT(fmt,args)
/**
 * XML_DEPRECATED:
 *
 * Macro used to indicate that a function, variable, type or struct member
 * is deprecated.
 */
#ifndef XML_DEPRECATED
#  if defined (IN_LIBXML) || !defined (_MSC_VER)
#    define XML_DEPRECATED
/* Available since Visual Studio 2005 */
#  elif defined (_MSC_VER) && (_MSC_VER >= 1400)
#    define XML_DEPRECATED __declspec(deprecated)
#  endif
#endif

/**
 * pragma understood my MS compiler which enables a conditional link with
 * iconv.
 */
#ifdef _MSC_VER
#if defined LIBXML_ICONV_ENABLED && !defined LIBXML2_COMPILING_MSCCDEF
//#pragma comment(lib, "iconv.lib")
#endif
#endif

/*
 * #pragma comment(lib, "kernel32.lib")
 *
 * pragma understood by MS compiler which enables a conditional link with
 * kernel32.
 */
#ifdef _MSC_VER
#if defined LIBXML_MODULES_ENABLED
#pragma comment(lib, "kernel32.lib")
#endif
#endif

/**
 * LIBXML_IGNORE_FPTR_CAST_WARNINGS:
 *
 * Macro used to ignore pointer cast warnings that can't be worked around.
 */
#ifndef XML_IGNORE_FPTR_CAST_WARNINGS
#define XML_IGNORE_FPTR_CAST_WARNINGS
#endif

/**
 * LIBXML_POP_WARNINGS:
 *
 * Macro used to restore warnings state.
 */
#ifndef XML_POP_WARNINGS
#define XML_POP_WARNINGS
#endif

#endif /* __GNUC__ */

#define XML_NO_ATTR

#ifndef XML_DECLARE_GLOBAL
#ifdef LIBXML_THREAD_ENABLED
  #define XML_DECLARE_GLOBAL(name, type, attrs) \
    attrs XMLPUBFUN type *__##name(void);
  #define XML_GLOBAL_MACRO(name) (*__##name())
#else
  #define XML_DECLARE_GLOBAL(name, type, attrs) \
    attrs XMLPUBVAR type name;
  #define XML_GLOBAL_MACRO(name) (name)
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#include <libxml/xmlexports.h>

#endif


