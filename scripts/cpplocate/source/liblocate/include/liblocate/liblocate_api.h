
#ifndef LIBLOCATE_TEMPLATE_API_H
#define LIBLOCATE_TEMPLATE_API_H

#include <liblocate/liblocate_export.h>

#ifdef LIBLOCATE_STATIC_DEFINE
#  define LIBLOCATE_TEMPLATE_API
#else
#  ifndef LIBLOCATE_TEMPLATE_API
#    ifdef LIBLOCATE_EXPORTS
        /* We are building this library */
#      define LIBLOCATE_TEMPLATE_API
#    else
        /* We are using this library */
#      define LIBLOCATE_TEMPLATE_API
#    endif
#  endif

#endif

#endif
