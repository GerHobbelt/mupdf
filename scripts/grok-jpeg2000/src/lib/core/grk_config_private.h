#define GRK_PACKAGE_VERSION "10.0.5"

/* #undef _LARGEFILE_SOURCE */
/* #undef _LARGE_FILES */
/* #undef _FILE_OFFSET_BITS */
/* #undef GROK_HAVE_FSEEKO */

/* Byte order.  */
/* All compilers that support Mac OS X define either __BIG_ENDIAN__ or
__LITTLE_ENDIAN__ to match the endianness of the architecture being
compiled for. This is not necessarily the same as the architecture of the
machine doing the building. In order to support Universal Binaries on
Mac OS X, we prefer those defines to decide the endianness.
On other platforms we use the result of the TRY_RUN. */
#if !defined(__APPLE__)
/* #undef GROK_BIG_ENDIAN */
#elif defined(__BIG_ENDIAN__)
# define GROK_BIG_ENDIAN
#endif

#define GRK_BUILD_LIBTIFF ON
#define GROK_PLUGIN_NAME "grokj2k_plugin"
