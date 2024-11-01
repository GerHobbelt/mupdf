
#ifndef CAF_NET_EXPORT_H
#define CAF_NET_EXPORT_H

#ifdef LIBCAF_NET_STATIC_DEFINE
#  define CAF_NET_EXPORT
#  define LIBCAF_NET_NO_EXPORT
#else
#  ifndef CAF_NET_EXPORT
#    ifdef libcaf_net_EXPORTS
        /* We are building this library */
#      define CAF_NET_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define CAF_NET_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef LIBCAF_NET_NO_EXPORT
#    define LIBCAF_NET_NO_EXPORT 
#  endif
#endif

#ifndef LIBCAF_NET_DEPRECATED
#  define LIBCAF_NET_DEPRECATED __declspec(deprecated)
#endif

#ifndef LIBCAF_NET_DEPRECATED_EXPORT
#  define LIBCAF_NET_DEPRECATED_EXPORT CAF_NET_EXPORT LIBCAF_NET_DEPRECATED
#endif

#ifndef LIBCAF_NET_DEPRECATED_NO_EXPORT
#  define LIBCAF_NET_DEPRECATED_NO_EXPORT LIBCAF_NET_NO_EXPORT LIBCAF_NET_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LIBCAF_NET_NO_DEPRECATED
#    define LIBCAF_NET_NO_DEPRECATED
#  endif
#endif

#endif /* CAF_NET_EXPORT_H */
