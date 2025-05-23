/* Macros for the header version.
 */

#ifndef VIPS_VERSION_H
#define VIPS_VERSION_H

#define VIPS_VERSION		"8.15"
#define VIPS_VERSION_STRING	"8.15.0"
#define VIPS_MAJOR_VERSION	(8)
#define VIPS_MINOR_VERSION	(15)
#define VIPS_MICRO_VERSION	(0)

/* The ABI version, as used for library versioning.
 */
#define VIPS_LIBRARY_CURRENT	(0x0801500)
#define VIPS_LIBRARY_REVISION	(0x20240329)
#define VIPS_LIBRARY_AGE	(0x0801200)

#define VIPS_CONFIG		"---"

/** 
 * VIPS_SONAME:
 *
 * The name of the shared object containing the vips library, for example
 * "libvips.so.42", or "libvips-42.dll".
 */

#if !defined(BUILD_MONOLITHIC)
#include "soname.h"
#endif

/* Not really anything to do with versions, but this is a handy place to put
 * it.
 */
#define VIPS_EXEEXT ".exe"
#define VIPS_ENABLE_DEPRECATED    0

#endif /*VIPS_VERSION_H*/
