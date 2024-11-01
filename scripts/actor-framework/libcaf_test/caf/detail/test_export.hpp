
#ifndef CAF_TEST_EXPORT_H
#define CAF_TEST_EXPORT_H

#ifdef LIBCAF_TEST_STATIC_DEFINE
#  define CAF_TEST_EXPORT
#  define LIBCAF_TEST_NO_EXPORT
#else
#  ifndef CAF_TEST_EXPORT
#    ifdef libcaf_test_EXPORTS
        /* We are building this library */
#      define CAF_TEST_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define CAF_TEST_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LIBCAF_TEST_NO_EXPORT
#    define LIBCAF_TEST_NO_EXPORT 
#  endif
#endif

#ifndef LIBCAF_TEST_DEPRECATED
#  define LIBCAF_TEST_DEPRECATED __declspec(deprecated)
#endif

#ifndef LIBCAF_TEST_DEPRECATED_EXPORT
#  define LIBCAF_TEST_DEPRECATED_EXPORT CAF_TEST_EXPORT LIBCAF_TEST_DEPRECATED
#endif

#ifndef LIBCAF_TEST_DEPRECATED_NO_EXPORT
#  define LIBCAF_TEST_DEPRECATED_NO_EXPORT LIBCAF_TEST_NO_EXPORT LIBCAF_TEST_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LIBCAF_TEST_NO_DEPRECATED
#    define LIBCAF_TEST_NO_DEPRECATED
#  endif
#endif

#endif /* CAF_TEST_EXPORT_H */
