/* libjpeg-turbo build number */
#define BUILD  "20210423"

/* Compiler's inline keyword */
#undef inline

/* How to obtain function inlining. */
#define INLINE  __forceinline

/* How to obtain thread-local storage */
#define THREAD_LOCAL  __declspec(thread)

/* Define to the full name of this package. */
#define PACKAGE_NAME  "libjpeg-turbo"

/* Version number of package */
#define VERSION  "2.0.91"

/* The size of `size_t', as computed by sizeof. */
#if defined(_MSC_VER)
#if defined(_WIN64)
#  define SIZEOF_SIZE_T 8
#else
#  define SIZEOF_SIZE_T 4
#endif
#endif

/* Define if your compiler has __builtin_ctzl() and sizeof(unsigned long) == sizeof(size_t). */
/* #undef HAVE_BUILTIN_CTZL */

/* Define to 1 if you have the <intrin.h> header file. */
#define HAVE_INTRIN_H

#if defined(_MSC_VER) && defined(HAVE_INTRIN_H)
#if (SIZEOF_SIZE_T == 8)
#define HAVE_BITSCANFORWARD64
#elif (SIZEOF_SIZE_T == 4)
#define HAVE_BITSCANFORWARD
#endif
#endif

#if defined(__clang__) && __cplusplus >= 201103L
   /* clang's fallthrough annotations are only available starting in C++11. */
#  define FALLTHROUGH [[clang::fallthrough]]
#elif defined(__GNUC__) && (__GNUC__ >= 7)
   /* GNU fallthrough attribute is available from GCC7 */
#  define FALLTHROUGH __attribute__((fallthrough))
#elif defined(_MSC_VER)
   /*
    * MSVC's __fallthrough annotations are checked by /analyze (Code Analysis):
    * https://msdn.microsoft.com/en-us/library/ms235402%28VS.80%29.aspx
    */
#  include <sal.h>
#  define FALLTHROUGH __fallthrough
#else
#  define FALLTHROUGH /* FALLTHROUGH */
#endif
