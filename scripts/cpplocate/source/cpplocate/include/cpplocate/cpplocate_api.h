
#ifndef CPPLOCATE_TEMPLATE_API_H
#define CPPLOCATE_TEMPLATE_API_H

#include <cpplocate/cpplocate_export.h>

#ifdef CPPLOCATE_STATIC_DEFINE
#  define CPPLOCATE_TEMPLATE_API
#else
#  ifndef CPPLOCATE_TEMPLATE_API
#    ifdef CPPLOCATE_EXPORTS
        /* We are building this library */
#      define CPPLOCATE_TEMPLATE_API
#    else
        /* We are using this library */
#      define CPPLOCATE_TEMPLATE_API
#    endif
#  endif

#endif

#endif
