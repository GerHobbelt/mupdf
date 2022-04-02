#ifndef HAD_CONFIG_H
#define HAD_CONFIG_H
#include "zipconf.h"
/* BEGIN DEFINES */
/* #undef HAVE___PROGNAME */
#define HAVE__CLOSE
#define HAVE__DUP
#undef HAVE_DLSYM
#define HAVE__FDOPEN
#define HAVE__FILENO
#define HAVE__SETMODE
#define HAVE__SNPRINTF
#define HAVE__STRDUP
#define HAVE__STRICMP
#define HAVE__STRTOI64
#define HAVE__STRTOUI64
/* #undef HAVE__UMASK */
#define HAVE__UNLINK
#undef HAVE_SRANDOM
/* #undef HAVE_ARC4RANDOM */
/* #undef HAVE_CLONEFILE */
/* #undef HAVE_COMMONCRYPTO */
#define HAVE_CRYPTO
/* #undef HAVE_FICLONERANGE */
#define HAVE_FILENO
/* #undef HAVE_FSEEKO */
/* #undef HAVE_FTELLO */
/* #undef HAVE_GETPROGNAME */
/* #undef HAVE_GNUTLS */
//#define HAVE_LIBBZ2 
//#define HAVE_LIBLZMA 
#define HAVE_LIBZSTD 
/* #undef HAVE_LOCALTIME_R */
/* #undef HAVE_MBEDTLS */
/* #undef HAVE_MKSTEMP */
/* #undef HAVE_NULLABLE */
#undef HAVE_OPENSSL 
#define HAVE_SETMODE
#define HAVE_SNPRINTF
/* #undef HAVE_STRCASECMP */
#define HAVE_STRDUP
#define HAVE_STRICMP
#define HAVE_STRTOLL
#define HAVE_STRTOULL
/* #undef HAVE_STRUCT_TM_TM_ZONE */
#define HAVE_STDBOOL_H
/* #undef HAVE_STRINGS_H */
/* #undef HAVE_UNISTD_H */
#define HAVE_WINDOWS_CRYPTO
#define SIZEOF_OFF_T 4
#define SIZEOF_SIZE_T 8
/* #undef HAVE_DIRENT_H */
/* #undef HAVE_FTS_H */
/* #undef HAVE_NDIR_H */
/* #undef HAVE_SYS_DIR_H */
/* #undef HAVE_SYS_NDIR_H */
#undef HAVE_DLFCN_H
#undef HAVE_ZLIB_H
#define HAVE_ZLIB_NG_H
//#define HAVE_LZMA_H
/* #undef WORDS_BIGENDIAN */
#define HAVE_SHARED
/* END DEFINES */
#define PACKAGE "libzip"
#define VERSION "1.8.1"

#endif /* HAD_CONFIG_H */
