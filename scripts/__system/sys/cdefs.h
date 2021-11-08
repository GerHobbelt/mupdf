
#ifndef __SYS_CDEFS_H__
#define __SYS_CDEFS_H__

#if defined(_MSC_VER)
#include "./msvc/cdefs.h"
#else
#error "You should already have a sys/cdefs.h available on your build platfomr."
#endif

#endif

