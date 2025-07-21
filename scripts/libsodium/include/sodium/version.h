
#ifndef sodium_version_H
#define sodium_version_H

#include "export.h"

#define SODIUM_VERSION_STRING "3.0.0"

#define SODIUM_LIBRARY_VERSION_MAJOR 3
#define SODIUM_LIBRARY_VERSION_MINOR 0

// Only compile the minimum set of functions required for the high-level API
#if 0
// Define for a minimal build, without deprecated functions and functions that high-level APIs depend on
#define SODIUM_LIBRARY_MINIMAL 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
const char *sodium_version_string(void);

SODIUM_EXPORT
int         sodium_library_version_major(void);

SODIUM_EXPORT
int         sodium_library_version_minor(void);

SODIUM_EXPORT
int         sodium_library_minimal(void);

#ifdef __cplusplus
}
#endif

#endif
