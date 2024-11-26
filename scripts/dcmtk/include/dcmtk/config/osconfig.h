#ifndef OSCONFIG_H
#define OSCONFIG_H

/*
** Define enclosures for include files with C linkage (mostly system headers)
*/
#ifdef __cplusplus
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }
#else
#define BEGIN_EXTERN_C
#define END_EXTERN_C
#endif

/* Define when building a shared library */
/* #undef DCMTK_SHARED */

/* Define when building the whole toolkit as a single shared library */
/* #undef DCMTK_BUILD_SINGLE_SHARED_LIBRARY */

/* Define when the compiler supports hidden visibility */
/* #undef HAVE_HIDDEN_VISIBILITY */

/* Define when building with wide char file I/O functions */
/* #undef WIDE_CHAR_FILE_IO_FUNCTIONS */

/* Define when building command line tools with wide char main function */
/* #undef WIDE_CHAR_MAIN_FUNCTION */

#ifdef _WIN32
  /* Define if you have the <windows.h> header file. */
#define HAVE_WINDOWS_H 1
  /* Define if you have the <winsock.h> header file. */
#define HAVE_WINSOCK_H 1
#endif

/* Define the canonical host system type as a string constant */
#define CANONICAL_HOST_TYPE "x64-Windows"

/* Define if char is unsigned on the C compiler */
/* #undef C_CHAR_UNSIGNED */

/* Define if >> is unsigned on the C compiler */
/* #undef C_RIGHTSHIFT_UNSIGNED */

/* Define the DCMTK default path */
#define DCMTK_PREFIX "C:\\Program Files (x86)\\DCMTK"

/* Define the default data dictionary path for the dcmdata library package */
#define DCM_DICT_DEFAULT_PATH ""

/* Define the type of standard dictionary that we want to use:
   0 - Do not load any default dictionary on startup
   1 - Load builtin dictionary on startup
   2 - Load external (i.e. file-based) dictionary on startup
*/
#define DCM_DICT_DEFAULT 1

/* Define whether dictionaries defined through DCMDICTPATH variable should be loaded */
#define DCM_DICT_USE_DCMDICTPATH 1

/* Define the environment variable path separator */
#define ENVIRONMENT_PATH_SEPARATOR ';'

/* Define to 1 if you have the `access' function. */
#define HAVE_ACCESS 1

/* Define to 1 if you have the <alloca.h> header file. */
/* #undef HAVE_ALLOCA_H */

/* Define to 1 if you have the <arpa/inet.h> header file. */
/* #undef HAVE_ARPA_INET_H */

/* Define to 1 if you have the `bcmp' function. */
/* #undef HAVE_BCMP */

/* Define to 1 if you have the `cuserid' function. */
/* #undef HAVE_CUSERID */

/* Define to 1 if you have the `fgetln' function. */
/* #undef HAVE_FGETLN */

/* Define if your system has a declaration for fp_except_t in ieeefp.h */
/* #undef HAVE_DECLARATION_FP_EXCEPT_T */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.*/
#define HAVE_DIRENT_H 1     /* we have a dirent emulation library... */

/* Define to 1 if you have the <err.h> header file. */
/* #undef HAVE_ERR_H */

/* Define to 1 if <errno.h> defined ENAMETOOLONG. */
#define HAVE_ENAMETOOLONG 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `finite' function. */
/* #undef HAVE_FINITE */

/* Define to 1 if you have the `flock' function. */
/* #undef HAVE_FLOCK */

/* Define to 1 if you have the <fnmatch.h> header file. */
#define HAVE_FNMATCH_H 1

/* Define to 1 if you have the `fork' function. */
/* #undef HAVE_FORK */

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
/* #undef HAVE_FSEEKO */

/* Define to 1 if you have the `ftime' function. */
#define HAVE_FTIME 1

/* Define to 1 if you have the `geteuid' function. */
/* #undef HAVE_GETEUID */

/* Define to 1 if you have the `getgrnam' function. */
/* #undef HAVE_GETGRNAM */

/* Define to 1 if you have the `gethostbyname_r' function. */
/* #undef HAVE_GETHOSTBYNAME_R */

/* Define to 1 if you have the `gethostbyaddr_r' function. */
/* #undef HAVE_GETHOSTBYADDR_R */

/* Define to 1 if you have the `getgrnam_r' function. */
/* #undef HAVE_GETGRNAM_R */

/* Define to 1 if you have the `getpwnam_r' function. */
/* #undef HAVE_GETPWNAM_R */

/* Define to 1 if you have the `gethostid' function. */
/* #undef HAVE_GETHOSTID */

/* Define to 1 if you have the `getlogin' function. */
/* #undef HAVE_GETLOGIN */

/* Define to 1 if you have the `getlogin_r' function. */
/* #undef HAVE_GETLOGIN_R */

/* Define to 1 if you have the `getpid' function. */
#define HAVE_GETPID 1

/* Define to 1 if you have the `getpwnam' function. */
/* #undef HAVE_GETPWNAM */

/* Define to 1 if you have the `getrusage' function. */
/* #undef HAVE_GETRUSAGE */

/* Define to 1 if you have the `gettimeofday' function. */
/* #undef HAVE_GETTIMEOFDAY */

/* Define to 1 if you have the `getuid' function. */
/* #undef HAVE_GETUID */

/* Define to 1 if you have the `gmtime_r' function. */
/* #undef HAVE_GMTIME_R */

/* Define to 1 if you have the <grp.h> header file. */
/* #undef HAVE_GRP_H */

/* Define to 1 if you have the <ieeefp.h> header file. */
/* #undef HAVE_IEEEFP_H */

/* Define to 1 if you have the `index' function. */
/* #undef HAVE_INDEX */

/* Define if your system declares argument 3 of accept() as int * instead of
   size_t * or socklen_t * */
#define HAVE_INTP_ACCEPT 1

/* Define if your system declares argument 5 of getsockopt() as int * instead
   of size_t * or socklen_t */
#define HAVE_INTP_GETSOCKOPT 1

/* Define to 1 if you have the <io.h> header file. */
#define HAVE_IO_H 1

/* Define to 1 if you have the `itoa' function. */
#define HAVE_ITOA 1

/* Define to 1 if you have the <langinfo.h> header file. */
/* #undef HAVE_LANGINFO_H */

/* Define to 1 if you have the <libc.h> header file. */
/* #undef HAVE_LIBC_H */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if the <libpng/png.h> header shall be used instead of <png.h>. */
/* #undef HAVE_LIBPNG_PNG_H */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the `listen' function. */
#define HAVE_LISTEN 1

/* Define to 1 if you have the `localtime_r' function. */
/* #undef HAVE_LOCALTIME_R */

/* Define to 1 if you have the `lockf' function. */
/* #undef HAVE_LOCKF */

/* Define to 1 if you have the `lstat' function. */
/* #undef HAVE_LSTAT */

/* Define to 1 if you have the `malloc_debug' function. */
/* #undef HAVE_MALLOC_DEBUG */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `mkstemp' function. */
/* #undef HAVE_MKSTEMP */

/* Define to 1 if you have the `mktemp' function. */
#define HAVE_MKTEMP 1

/* Define to 1 if you have the <mqueue.h> header file. */
/* #undef HAVE_MQUEUE_H */

/* Define to 1 if you have the <netdb.h> header file. */
/* #undef HAVE_NETDB_H */

/* Define to 1 if you have the <netinet/in.h> header file. */
/* #undef HAVE_NETINET_IN_H */

/* Define to 1 if you have the <netinet/in_systm.h> header file. */
/* #undef HAVE_NETINET_IN_SYSTM_H */

/* Define to 1 if you have the <netinet/tcp.h> header file. */
/* #undef HAVE_NETINET_TCP_H */

/* Define to 1 if you have the <sys/systeminfo.h> header file. */
/* #undef HAVE_SYS_SYSTEMINFO_H */

/* Define to 1 if you have readdir_r */
/* #undef HAVE_READDIR_R */

/* Define if your system has a prototype for feenableexcept in fenv.h */
/* #undef HAVE_PROTOTYPE_FEENABLEEXCEPT */

/* Define if your system has a prototype for finite in math.h */
/* #undef HAVE_PROTOTYPE_FINITE */

/* Define to 1 if your has a prototype for `TryAcquireSRWLockShared' in windows.h (Win32 only). */
#define HAVE_PROTOTYPE_TRYACQUIRESRWLOCKSHARED 1

/* Define if your system has a prototype for flock in sys/file.h */
/* #undef HAVE_PROTOTYPE_FLOCK */

/* Define if your system has a prototype for gethostbyname_r in libc.h unistd.h
   stdlib.h netdb.h */
/* #undef HAVE_PROTOTYPE_GETHOSTBYNAME_R */

/* Define if your system has a prototype for gethostbyaddr_r in libc.h unistd.h
   stdlib.h netdb.h */
/* #undef HAVE_PROTOTYPE_GETHOSTBYADDR_R */

/* Define if your system has a prototype for gethostid in libc.h unistd.h
   stdlib.h netdb.h */
/* #undef HAVE_PROTOTYPE_GETHOSTID */

/* Define if your system has a prototype for gettimeofday in sys/time.h
   unistd.h */
/* #undef HAVE_PROTOTYPE_GETTIMEOFDAY */

/* Define if your system has a prototype for std::isinf in cmath */
#define HAVE_PROTOTYPE_STD__ISINF 1

/* Define if your system has a prototype for std::isnan in cmath */
#define HAVE_PROTOTYPE_STD__ISNAN 1

/* Define if your system has a prototype for fpclassf in math.h */
#define HAVE_PROTOTYPE__FPCLASSF 1

/* Define if your system has a prototype for mkstemp in libc.h unistd.h
   stdlib.h */
/* #undef HAVE_PROTOTYPE_MKSTEMP */

/* Define if your system has a prototype for mktemp in libc.h unistd.h
   stdlib.h */
#define HAVE_PROTOTYPE_MKTEMP 1

/* Define if your system has a prototype for std::vfprintf in stdarg.h */
#define HAVE_PROTOTYPE_STD__VFPRINTF 1

/* Define if your system has a prototype for std::vsnprintf in stdio.h */
#define HAVE_PROTOTYPE_STD__VSNPRINTF 1

/* Define if your system has a prototype for strcasecmp in string.h */
/* #undef HAVE_PROTOTYPE_STRCASECMP */

/* Define if your system has a prototype for strcasestr in string.h
   that is visible in C */
/* #undef HAVE_PROTOTYPE_STRCASESTR */

/* Define if your system has a prototype for strncasecmp in string.h */
/* #undef HAVE_PROTOTYPE_STRNCASECMP */

/* Define if your system has a prototype for strerror_r in string.h */
/* #undef HAVE_PROTOTYPE_STRERROR_R */

/* Define if your system has a prototype for gettid */
/* #undef HAVE_SYS_GETTID */

/* Define if your system has a prototype for usleep in libc.h unistd.h
   stdlib.h */
/* #undef HAVE_PROTOTYPE_USLEEP */

/* Define if your system has a prototype for waitpid in sys/wait.h sys/time.h
   sys/resource.h */
/* #undef HAVE_PROTOTYPE_WAITPID */

/* Define if your system has a prototype for _stricmp in string.h */
#define HAVE_PROTOTYPE__STRICMP 1

/* Define if your system has a prototype for nanosleep in time.h */
/* #undef HAVE_PROTOTYPE_NANOSLEEP */

/* Define to 1 if you have the <pthread.h> header file. */
/* #undef HAVE_PTHREAD_H */

/* Define if your system supports POSIX read/write locks */
/* #undef HAVE_PTHREAD_RWLOCK */

/* Define to 1 if you have the <pwd.h> header file. */
/* #undef HAVE_PWD_H */

/* Define to 1 if you have the `rindex' function. */
/* #undef HAVE_RINDEX */

/* Define to 1 if you have the <semaphore.h> header file. */
/* #undef HAVE_SEMAPHORE_H */

/* Define to 1 if you have the `setuid' function. */
/* #undef HAVE_SETUID */

/* Define to 1 if you have the `sleep' function. */
/* #undef HAVE_SLEEP */

/* Define to 1 if you have the `stat' function. */
#define HAVE_STAT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the C++11 <system_error> header file. */
#define HAVE_SYSTEM_ERROR 1

/* Define to 1 if you have the <cstdint> header file. */
#define HAVE_CSTDINT 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if `strerror_r' returns a char*. */
#define HAVE_CHARP_STRERROR_R 1

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the `strlcat' function. */
/* #undef HAVE_STRLCAT */

/* Define to 1 if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */

/* Define to 1 if you have the <synch.h> header file. */
/* #undef HAVE_SYNCH_H */

/* Define if __sync_add_and_fetch is available */
/* #undef HAVE_SYNC_ADD_AND_FETCH */

/* Define if __sync_sub_and_fetch is available */
/* #undef HAVE_SYNC_SUB_AND_FETCH */

/* Define if InterlockedIncrement is available */
#define HAVE_INTERLOCKED_INCREMENT 1

/* Define if InterlockedDecrement is available */
#define HAVE_INTERLOCKED_DECREMENT 1

/* Define if passwd::pw_gecos is available */
/* #undef HAVE_PASSWD_GECOS */

/* Define to 1 if you have the `sysinfo' function. */
/* #undef HAVE_SYSINFO */

/* Define to 1 if you have the <syslog.h> header file. */
/* #undef HAVE_SYSLOG_H */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.*/
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/errno.h> header file. */
/* #undef HAVE_SYS_ERRNO_H */

/* Define to 1 if you have the <sys/file.h> header file. */
/* #undef HAVE_SYS_FILE_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
/* #undef HAVE_SYS_MMAN_H */

/* Define to 1 if you have the <sys/msg.h> header file. */
/* #undef HAVE_SYS_MSG_H */

/* Define to 1 if you have the <sys/param.h> header file. */
/* #undef HAVE_SYS_PARAM_H */

/* Define to 1 if you have the <sys/queue.h> header file. */
/* #undef HAVE_SYS_QUEUE_H */

/* Define to 1 if you have the <sys/resource.h> header file. */
/* #undef HAVE_SYS_RESOURCE_H */

/* Define to 1 if you have the <sys/select.h> header file. */
/* #undef HAVE_SYS_SELECT_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
/* #undef HAVE_SYS_SOCKET_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/syscall.h> header file. */
/* #undef HAVE_SYS_SYSCALL_H */

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/un.h> header file. */
/* #undef HAVE_SYS_UN_H */

/* Define to 1 if you have the <sys/utime.h> header file. */
#define HAVE_SYS_UTIME_H 1

/* Define to 1 if you have the <sys/utsname.h> header file. */
/* #undef HAVE_SYS_UTSNAME_H */

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
/* #undef HAVE_SYS_WAIT_H */

/* Define to 1 if you have the <thread.h> header file. */
/* #undef HAVE_THREAD_H */

/* Define to 1 if you have the C++11 <tuple> header file. */
#define HAVE_TUPLE 1

/* Define to 1 if you have the C++11 <type_traits> header file. */
#define HAVE_TYPE_TRAITS 1

/* Define to 1 if you have the `uname' function. */
/* #undef HAVE_UNAME */

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define to 1 if you have the <unix.h> header file. */
/* #undef HAVE_UNIX_H */

/* Define to 1 if you have the `usleep' function. */
/* #undef HAVE_USLEEP */

/* Define to 1 if you have the <utime.h> header file. */
/* #undef HAVE_UTIME_H */

/* Define to 1 if you have the `_vsnprintf_s' function. */
#define HAVE__VSNPRINTF_S 1

/* Define to 1 if you have the `vfprintf_s' function. */
#define HAVE_VFPRINTF_S 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `vsprintf_s' function. */
#define HAVE_VSPRINTF_S 1

/* Define to 1 if you have the `waitpid' function. */
/* #undef HAVE_WAITPID */

/* Define to 1 if you have the `_findfirst' function. */
#define HAVE__FINDFIRST 1

/* Define to 1 if you have the `_set_output_format' function. */
/* #undef HAVE__SET_OUTPUT_FORMAT */

/* Define to 1 if the compiler supports __FUNCTION__. */
#define HAVE___FUNCTION___MACRO 1

/* Define to 1 if the compiler supports __PRETTY_FUNCTION__. */
/* #undef HAVE___PRETTY_FUNCTION___MACRO */

/* Define to 1 if the compiler supports __func__. */
#define HAVE___func___MACRO 1

/* Define to 1 if you have the `nanosleep' function. */
/* #undef HAVE_NANOSLEEP */

/* Define to 1 if you have variable length arrays. */
/* #undef HAVE_VLA */

/* Define to the address where bug reports for this package should be sent. */
/* #undef PACKAGE_BUGREPORT */

/* Define to the full name of this package. */
#define PACKAGE_NAME "dcmtk"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the default configuration directory (used by some applications) */
#define DEFAULT_CONFIGURATION_DIR ""

/* Define to the default support data directory (used by some applications) */
#define DEFAULT_SUPPORT_DATA_DIR ""

/* Define to the one symbol short name of this package. */
/* #undef PACKAGE_TARNAME */

/* Define to the date of this package. */
#define PACKAGE_DATE "DEV"

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.6.8"

/* Define to the version suffix of this package. */
#define PACKAGE_VERSION_SUFFIX "+"

/* Define to the version number of this package. */
#define PACKAGE_VERSION_NUMBER 368

/* Define path separator */
#define PATH_SEPARATOR '/'

/* LFS mode constants. */
#define DCMTK_LFS 1
#define DCMTK_LFS64 2

/* Select LFS mode (defined above) that shall be used or don't define it */
#define DCMTK_ENABLE_LFS DCMTK_LFS

/* The size of a `double', as computed by sizeof. */
#define SIZEOF_DOUBLE 8

/* The size of a `float', as computed by sizeof. */
#define SIZEOF_FLOAT 4

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of a `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of a `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* The size of a `fpos_t', as computed by sizeof. */
#define SIZEOF_FPOS_T 8

/* The size of a `off_t', as computed by sizeof. */
/* #undef SIZEOF_OFF_T */

/* Define if we are compiling with libiconv support. */
#define WITH_LIBICONV

/* Define if the C standard library has iconv builtin. */
/* #undef WITH_STDLIBC_ICONV */

/* character set conversion constants. */
#define DCMTK_CHARSET_CONVERSION_ICONV 2
#define DCMTK_CHARSET_CONVERSION_STDLIBC_ICONV 3
#define DCMTK_CHARSET_CONVERSION_OFICONV 4

/* Define to select character set conversion implementation. */
#define DCMTK_ENABLE_CHARSET_CONVERSION DCMTK_CHARSET_CONVERSION_OFICONV

/* Define if the second argument to iconv() is const */
#define LIBICONV_SECOND_ARGUMENT_CONST 1

/* Try to define the iconv behavior as conversion flags */
/* #undef DCMTK_FIXED_ICONV_CONVERSION_FLAGS */

/* Define if iconv_open() accepts "" as an argument */
/* #undef DCMTK_STDLIBC_ICONV_HAS_DEFAULT_ENCODING */

/* Define if we are compiling with libpng support */
#define WITH_LIBPNG 1

/* Define if we are compiling with libtiff support */
#define WITH_LIBTIFF 1

/* Define if we are compiling with libxml support */
#define WITH_LIBXML 1

/* Define if we are compiling with OpenJPEG support */
#define WITH_OPENJPEG 1

/* Define if we are compiling with OpenSSL support */
/* #undef WITH_OPENSSL */

/* Define if we are compiling for built-in private tag dictionary */
/* #undef ENABLE_PRIVATE_TAGS */

/* Define if we are compiling with sndfile support. */
/* #undef WITH_SNDFILE */

/* Define if we are compiling with libwrap (TCP wrapper) support */
/* #undef WITH_TCPWRAPPER */

/* Define if we are compiling with zlib support */
#define WITH_ZLIB 1

/* Define if we are compiling with any type of Multi-thread support */
#define WITH_THREADS

/* Define if pthread_t is a pointer type on your system */
//#define HAVE_POINTER_TYPE_PTHREAD_T 1

/* Define to 1 if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message. */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define `pid_t' to `int' if <sys/types.h> does not define. */
#define HAVE_NO_TYPEDEF_PID_T
#ifdef HAVE_NO_TYPEDEF_PID_T
#define pid_t int
#endif

/* Define `ssize_t' to `long' if <sys/types.h> does not define. */
#define HAVE_NO_TYPEDEF_SSIZE_T
#ifdef HAVE_NO_TYPEDEF_SSIZE_T
#if defined(_WIN64)
 typedef __int64 ssize_t;
#else
 typedef long ssize_t;
#endif
#endif

/* Set typedefs as needed for JasPer library */
/* #undef HAVE_UCHAR_TYPEDEF */
#ifndef HAVE_UCHAR_TYPEDEF
typedef unsigned char uchar;
#endif

/* #undef HAVE_USHORT_TYPEDEF */
#ifndef HAVE_USHORT_TYPEDEF
typedef unsigned short ushort;
#endif

/* #undef HAVE_UINT_TYPEDEF */
#ifndef HAVE_UINT_TYPEDEF
typedef unsigned int uint;
#endif

/* #undef HAVE_ULONG_TYPEDEF */
#ifndef HAVE_ULONG_TYPEDEF
typedef unsigned long ulong;
#endif

#define HAVE_LONG_LONG
#define HAVE_UNSIGNED_LONG_LONG

#define HAVE_INT64_T 1
#define HAVE_UINT64_T 1

 /* Additional settings for Borland C++ Builder */
#ifdef __BORLANDC__
#define _stricmp stricmp    /* _stricmp in MSVC is stricmp in Borland C++ */
#define _strnicmp strnicmp  /* _strnicmp in MSVC is strnicmp in Borland C++ */
    #pragma warn -8027      /* disable Warning W8027 "functions containing while are not expanded inline" */
    #pragma warn -8004      /* disable Warning W8004 "variable is assigned a value that is never used" */
    #pragma warn -8012      /* disable Warning W8012 "comparing signed and unsigned values" */
#ifdef WITH_THREADS
#define __MT__              /* required for _beginthreadex() API in <process.h> */
#define _MT                 /* required for _errno on BCB6 */
#endif
#define HAVE_PROTOTYPE_MKTEMP 1
#undef HAVE_SYS_UTIME_H
#define _MSC_VER 1200       /* Treat Borland C++ 5.5 as MSVC6. */
#endif /* __BORLANDC__ */

/* Platform specific settings for Visual C++
 * By default, enable ANSI standard C++ includes on Visual C++ 6 and newer
 *   _MSC_VER == 1100 on Microsoft Visual C++ 5.0
 *   _MSC_VER == 1200 on Microsoft Visual C++ 6.0
 *   _MSC_VER == 1300 on Microsoft Visual C++ 7.0
 *   _MSC_VER == 1310 on Microsoft Visual C++ 7.1
 *   _MSC_VER == 1400 on Microsoft Visual C++ 8.0
 */

#ifdef _MSC_VER
#if _MSC_VER <= 1200 /* Additional settings for VC6 and older */
/* disable warning that return type for 'identifier::operator ->' is not a UDT or reference to a UDT */
    #pragma warning( disable : 4284 )
#define HAVE_OLD_INTERLOCKEDCOMPAREEXCHANGE 1
#else
#define HAVE_VSNPRINTF 1
#endif /* _MSC_VER <= 1200 */

    #pragma warning( disable : 4251 )  /* disable warnings about needed dll-interface */
                                       /* http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html */
    #pragma warning( disable : 4099 )  /* disable warning about mismatched class and struct keywords */
                                       /* http://alfps.wordpress.com/2010/06/22/cppx-is-c4099-really-a-sillywarning-disabling-msvc-sillywarnings */
    #pragma warning( disable : 4521 )  /* disable warnings about multiple copy constructors and assignment operators,*/
    #pragma warning( disable : 4522 )  /* since these are sometimes necessary for correct overload resolution*/

#if _MSC_VER >= 1400                   /* Additional settings for Visual Studio 2005 and newer */
    #pragma warning( disable : 4996 )  /* disable warnings about "deprecated" C runtime functions */
#if _MSC_VER < 1900                    /* Warning only available before Visual Studio 2015 */
    #pragma warning( disable : 4351 )  /* disable warnings about "new behavior" when initializing the elements of an array */
#endif /* _MSC_VER < 1900 */
#endif /* _MSC_VER >= 1400 */
#endif /* _MSC_VER */

/* Define if ANSI standard C++ includes use std namespace */
#define HAVE_STD_NAMESPACE 1

/* Define if the compiler supports std::nothrow */
#define HAVE_STD__NOTHROW 1

/* Define if the compiler supports operator delete (std::nothrow) */
#define HAVE_NOTHROW_DELETE 1

/* Define if the compiler supports static_assert */
#define HAVE_STATIC_ASSERT 1

/* Define if the compiler supports [[deprecated]] */
#define HAVE_CXX14_DEPRECATED_ATTRIBUTE 1

/* Define if the compiler supports [[deprecated("message")]] */
#define HAVE_CXX14_DEPRECATED_ATTRIBUTE_MSG 1

/* Define if the compiler supports __attribute__((deprecated)) */
/* #undef HAVE_ATTRIBUTE_DEPRECATED */

/* Define if the compiler supports __attribute__((deprecated("message"))) */
/* #undef HAVE_ATTRIBUTE_DEPRECATED_MSG */

/* Define if the compiler supports __declspec(deprecated) */
#define HAVE_DECLSPEC_DEPRECATED 1

/* Define if the compiler supports __declspec(deprecated("message")) */
#define HAVE_DECLSPEC_DEPRECATED_MSG 1

/* Define if your system has a prototype for std::vfprintf in stdarg.h */
#define HAVE_PROTOTYPE_STD__VFPRINTF 1

/* Define if your system has off64_t */
/* #undef HAVE_OFF64_T */

/* Define if your system has fpos64_t */
/* #undef HAVE_FPOS64_T */

/* Define if your system uses _popen instead of popen */
/* #undef HAVE_POPEN */

/* Define if your system uses _pclose instead of pclose */
/* #undef HAVE_PCLOSE */

/* Define if your system provides sigjmp_buf as conditional jmp_buf alternative */
/* #undef HAVE_SIGJMP_BUF */

/* Define if we can use C++11 */
#define HAVE_CXX11 1

#if defined(HAVE_CXX11) && defined(__cplusplus) && __cplusplus < 201103L
#error \
DCMTK was configured to use C++11 features, but your compiler does not or was not configured to provide them.
#endif

/* Define if we can use C++14 */
/* #undef HAVE_CXX14 */

#if defined(HAVE_CXX14) && defined(__cplusplus) && __cplusplus < 201402L
#error \
DCMTK was configured to use C++14 features, but your compiler does not or was not configured to provide them.
#endif

/* Define if we can use C++17 */
/* #undef HAVE_CXX17 */

#if defined(HAVE_CXX17) && defined(__cplusplus) && __cplusplus < 201703L
#error \
DCMTK was configured to use C++17 features, but your compiler does not or was not configured to provide them.
#endif

/* Define if we can use C++20 */
#define HAVE_CXX20 1

#if defined(HAVE_CXX20) && defined(__cplusplus) && __cplusplus < 202002L
#error \
DCMTK was configured to use C++20 features, but your compiler does not or was not configured to provide them.
#endif

/* Define if the compiler supports __alignof__ */
/* #undef HAVE_GNU_ALIGNOF */

/* Define if the compiler supports __alignof */
#define HAVE_MS_ALIGNOF 1

/* Define if the compiler supports __attribute__((aligned)) */
/* #undef HAVE_ATTRIBUTE_ALIGNED */

/* Define if __attribute__((aligned)) supports templates */
/* #undef ATTRIBUTE_ALIGNED_SUPPORTS_TEMPLATES */

/* Define if the compiler supports __declspec(align) */
#define HAVE_DECLSPEC_ALIGN 1

/* Define if the compiler supports default constructor detection via SFINAE */
/* #undef HAVE_DEFAULT_CONSTRUCTOR_DETECTION_VIA_SFINAE */

/* Define if we are cross compiling */
/* #undef DCMTK_CROSS_COMPILING */

/* The path on the Android device that should be used for temporary files */
/* #undef ANDROID_TEMPORARY_FILES_LOCATION */

/* Define if we are supposed to use STL's vector */
#define HAVE_STL_VECTOR 1

/* Define if we are supposed to use STL's algorithms */
#define HAVE_STL_ALGORITHM 1

/* Define if we are supposed to use STL's list */
#define HAVE_STL_LIST 1

/* Define if we are supposed to use STL's list */
#define HAVE_STL_MAP 1

/* Define if we are supposed to use STL's memory */
#define HAVE_STL_MEMORY 1

/* Define if we are supposed to use STL's stack */
#define HAVE_STL_STACK 1

/* Define if we are supposed to use STL's string */
#define HAVE_STL_STRING 1

/* Define if we are supposed to use STL's type_traits */
#define HAVE_STL_TYPE_TRAITS 1

/* Define if we are supposed to use STL's tuple */
#define HAVE_STL_TUPLE 1

/* Define if we are supposed to use STL's system_error */
#define HAVE_STL_SYSTEM_ERROR 1

/* Define if we are supposed to use STL's atomic */
#define HAVE_STL_ATOMIC 1

/* Define if the input iterator category is supported */
/* #undef HAVE_CONTIGUOUS_ITERATOR_CATEGORY */

/* Feature Tests for the OpenSSL Library */

/* Define if your OpenSSL library provides the RAND_egd function */
/* #undef HAVE_OPENSSL_PROTOTYPE_RAND_EGD */

/* Define if we have OpenSSL with the HAVE_OPENSSL_PROTOTYPE_NID_DSA_WITH_SHA512 macro */
/* #undef HAVE_OPENSSL_PROTOTYPE_NID_DSA_WITH_SHA512 */

/* Define if we have OpenSSL with the HAVE_OPENSSL_PROTOTYPE_NID_ECDSA_WITH_SHA3_256 macro */
/* #undef HAVE_OPENSSL_PROTOTYPE_NID_ECDSA_WITH_SHA3_256 */

/* Define if we have OpenSSL with the HAVE_OPENSSL_PROTOTYPE_NID_SHA512_256WITHRSAENCRYPTION macro */
/* #undef HAVE_OPENSSL_PROTOTYPE_NID_SHA512_256WITHRSAENCRYPTION */

/* Define if we have OpenSSL with the SSL_CTX_set0_tmp_dh_pkey() function */
/* #undef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET0_TMP_DH_PKEY */

/* Define if we have OpenSSL with the SSL_CTX_set1_sigalgs() function */
/* #undef HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_SIGALGS */

/* Define if we have OpenSSL with the TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8 macro */
/* #undef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8 */

/* Define if we have OpenSSL with the TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 macro */
/* #undef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 */

/* Define if we have OpenSSL with the TS_VERIFY_CTX_set0_store() function */
/* #undef HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET0_STORE */

/* Define if we have OpenSSL with the new typedef of EVP_MD_CTX as struct evp_md_ctx_st */
/* #undef HAVE_OPENSSL_DECLARATION_NEW_EVP_MD_CTX */

/* Define if we have OpenSSL with the EVP_PKEY_get_group_name() function */
/* #undef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET_GROUP_NAME */

/* Define if we have OpenSSL with the OSSL_PROVIDER_load() function */
/* #undef HAVE_OPENSSL_PROTOTYPE_OSSL_PROVIDER_LOAD */

/* Define if we have the <openssl/provider.h> header file*/
/* #undef HAVE_OPENSSL_PROVIDER_H */


/* Historical C/C++ language features and APIs that are guaranteed by C++98,
 * C89, or POSIX.1-2001 for Posix platforms, and thus not tested anymore.
 * These macros are deprecated and will be removed from a future version.
 */

#if defined(POISON_DEPRECATED_FEATURE_MACROS) && defined(__GNUC__)
#pragma GCC poison HAVE_ASSERT_H
#pragma GCC poison HAVE_BIDIRECTIONAL_ITERATOR_CATEGORY
#pragma GCC poison HAVE_CLASS_TEMPLATE
#pragma GCC poison HAVE_CLIMITS
#pragma GCC poison HAVE_CMATH
#pragma GCC poison HAVE_CONST_CAST
#pragma GCC poison HAVE_CSTDARG
#pragma GCC poison HAVE_CSTDDEF
#pragma GCC poison HAVE_CSTDIO
#pragma GCC poison HAVE_CTYPE_H
#pragma GCC poison HAVE_CXX_VOLATILE
#pragma GCC poison HAVE_C_CONST
#pragma GCC poison HAVE_DECLARATION_STRUCT_UTIMBUF
#pragma GCC poison HAVE_DYNAMIC_CAST
#pragma GCC poison HAVE_ERRNO_H
#pragma GCC poison HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
#pragma GCC poison HAVE_FENV_H
#pragma GCC poison HAVE_FLOAT_H
#pragma GCC poison HAVE_FORWARD_ITERATOR_CATEGORY
#pragma GCC poison HAVE_FSTREAM
#pragma GCC poison HAVE_FUNCTION_TEMPLATE
#pragma GCC poison HAVE_GETENV
#pragma GCC poison HAVE_INPUT_ITERATOR_CATEGORY
#pragma GCC poison HAVE_INTTYPES_H
#pragma GCC poison HAVE_IOMANIP
#pragma GCC poison HAVE_IOSTREAM
#pragma GCC poison HAVE_ISINF
#pragma GCC poison HAVE_ISNAN
#pragma GCC poison HAVE_ISO646_H
#pragma GCC poison HAVE_ITERATOR_HEADER
#pragma GCC poison HAVE_LIMITS_H
#pragma GCC poison HAVE_LOCALE_H
#pragma GCC poison HAVE_LONG_FILE_NAMES
#pragma GCC poison HAVE_MATH_H
#pragma GCC poison HAVE_MBSTOWCS
#pragma GCC poison HAVE_MEMCMP
#pragma GCC poison HAVE_MEMCPY
#pragma GCC poison HAVE_MEMMOVE
#pragma GCC poison HAVE_MEMSET
#pragma GCC poison HAVE_OUTPUT_ITERATOR_CATEGORY
#pragma GCC poison HAVE_PROTOTYPE_ISINF
#pragma GCC poison HAVE_PROTOTYPE_ISNAN
#pragma GCC poison HAVE_RANDOM_ACCESS_ITERATOR_CATEGORY
#pragma GCC poison HAVE_REINTERPRET_CAST
#pragma GCC poison HAVE_SETJMP_H
#pragma GCC poison HAVE_SIGNAL_H
#pragma GCC poison HAVE_SSTREAM
#pragma GCC poison HAVE_STATIC_CAST
#pragma GCC poison HAVE_STATIC_TEMPLATE_METHOD
#pragma GCC poison HAVE_STDARG_H
#pragma GCC poison HAVE_STDBOOL_H
#pragma GCC poison HAVE_STDDEF_H
#pragma GCC poison HAVE_STDIO_H
#pragma GCC poison HAVE_STDLIB_H
#pragma GCC poison HAVE_STRCHR
#pragma GCC poison HAVE_STRERROR
#pragma GCC poison HAVE_STRING_H
#pragma GCC poison HAVE_STRSTR
#pragma GCC poison HAVE_STRSTREAM
#pragma GCC poison HAVE_STRTOUL
#pragma GCC poison HAVE_TEMPNAM
#pragma GCC poison HAVE_TIME_H
#pragma GCC poison HAVE_TMPNAM
#pragma GCC poison HAVE_TYPENAME
#pragma GCC poison HAVE_VPRINTF
#pragma GCC poison HAVE_WCHAR_H
#pragma GCC poison HAVE_WCSTOMBS
#pragma GCC poison HAVE_WCTYPE_H
#pragma GCC poison SIZEOF_CHAR
#pragma GCC poison STDC_HEADERS
#endif

#if !defined(POISON_DEPRECATED_FEATURE_MACROS) || !defined(__GNUC__)
#define HAVE_ASSERT_H 1
#define HAVE_BIDIRECTIONAL_ITERATOR_CATEGORY 1
#define HAVE_CLASS_TEMPLATE 1
#define HAVE_CLIMITS 1
#define HAVE_CMATH 1
#define HAVE_CONST_CAST 1
#define HAVE_CSTDARG 1
#define HAVE_CSTDDEF 1
#define HAVE_CSTDIO 1
#define HAVE_CTYPE_H 1
#define HAVE_CXX_VOLATILE 1
#define HAVE_C_CONST 1
#define HAVE_DECLARATION_STRUCT_UTIMBUF 1
#define HAVE_DYNAMIC_CAST 1
#define HAVE_ERRNO_H 1
#define HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION 1
#define HAVE_FENV_H 1
#define HAVE_FLOAT_H 1
#define HAVE_FORWARD_ITERATOR_CATEGORY 1
#define HAVE_FSTREAM 1
#define HAVE_FUNCTION_TEMPLATE 1
#define HAVE_GETENV 1
#define HAVE_INPUT_ITERATOR_CATEGORY 1
#define HAVE_INTTYPES_H 1
#define HAVE_IOMANIP 1
#define HAVE_IOSTREAM 1
#define HAVE_ISINF 1
#define HAVE_ISNAN 1
#define HAVE_ISO646_H 1
#define HAVE_ITERATOR_HEADER 1
#define HAVE_LIMITS_H 1
#define HAVE_LOCALE_H 1
#define HAVE_LONG_FILE_NAMES 1
#define HAVE_MATH_H 1
#define HAVE_MBSTOWCS 1
#define HAVE_MEMCMP 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMSET 1
#define HAVE_OUTPUT_ITERATOR_CATEGORY 1
#define HAVE_PROTOTYPE_ISINF 1
#define HAVE_PROTOTYPE_ISNAN 1
#define HAVE_RANDOM_ACCESS_ITERATOR_CATEGORY 1
#define HAVE_REINTERPRET_CAST 1
#define HAVE_SETJMP_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_SSTREAM 1
#define HAVE_STATIC_CAST 1
#define HAVE_STATIC_TEMPLATE_METHOD 1
#define HAVE_STDARG_H 1
#define HAVE_STDBOOL_H 1
#define HAVE_STDDEF_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRCHR 1
#define HAVE_STRERROR 1
#define HAVE_STRING_H 1
#define HAVE_STRSTR 1
#define HAVE_STRSTREAM 1
#define HAVE_STRTOUL 1
#define HAVE_TEMPNAM 1
#define HAVE_TIME_H 1
#define HAVE_TMPNAM 1
#define HAVE_TYPENAME 1
#define HAVE_VPRINTF 1
#define HAVE_WCHAR_H 1
#define HAVE_WCSTOMBS 1
#define HAVE_WCTYPE_H 1
#define SIZEOF_CHAR 1
#define STDC_HEADERS 1
#endif

/* Historical feature tests that are not used in DCMTK anymore,
 * and are thus not tested for anymore.
 */

#if defined(POISON_DEPRECATED_FEATURE_MACROS) && defined(__GNUC__)
#pragma GCC poison C_INLINE
#pragma GCC poison HAVE_BCOPY
#pragma GCC poison HAVE_DECLARATION_STD__IOS_BASE__OPENMODE
#pragma GCC poison HAVE_DOPRNT
#pragma GCC poison HAVE_EMPTY_ARGC_ARGV
#pragma GCC poison HAVE_FSTREAM_H
#pragma GCC poison HAVE_IOMANIP_H
#pragma GCC poison HAVE_IOSTREAM_H
#pragma GCC poison HAVE_IOS_NOCREATE
#pragma GCC poison HAVE_LIBIOSTREAM
#pragma GCC poison HAVE_LONGLONG
#pragma GCC poison HAVE_MEMORY_H
#pragma GCC poison HAVE_NDIR_H
#pragma GCC poison HAVE_NEW_H
#pragma GCC poison HAVE_OLD_READDIR_R
#pragma GCC poison HAVE_PROTOTYPE_STD__FINITE
#pragma GCC poison HAVE_PROTOTYPE_WAIT3
#pragma GCC poison HAVE_SSTREAM_H
#pragma GCC poison HAVE_STAT_H
#pragma GCC poison HAVE_STREAMBUF_H
#pragma GCC poison HAVE_STRSTREAM_H
#pragma GCC poison HAVE_STRSTREA_H
#pragma GCC poison HAVE_SYS_NDIR_H
#pragma GCC poison HAVE_ULONGLONG
#pragma GCC poison HAVE_WAIT3
#pragma GCC poison INCLUDE_LIBC_H_AS_CXX
#pragma GCC poison INCLUDE_MATH_H_AS_CXX
#pragma GCC poison RETSIGTYPE
#pragma GCC poison TM_IN_SYS_TIME
#endif

/* #undef C_INLINE */
/* #undef HAVE_BCOPY */
/* #undef HAVE_DECLARATION_STD__IOS_BASE__OPENMODE */
/* #undef HAVE_DOPRNT */
/* #undef HAVE_EMPTY_ARGC_ARGV */
/* #undef HAVE_FSTREAM_H */
/* #undef HAVE_IOMANIP_H */
/* #undef HAVE_IOSTREAM_H */
/* #undef HAVE_IOS_NOCREATE */
/* #undef HAVE_LIBIOSTREAM */
/* #undef HAVE_LONGLONG */
/* #undef HAVE_MEMORY_H */
/* #undef HAVE_NDIR_H */
/* #undef HAVE_NEW_H */
/* #undef HAVE_OLD_READDIR_R */
/* #undef HAVE_PROTOTYPE_STD__FINITE */
/* #undef HAVE_PROTOTYPE_WAIT3 */
/* #undef HAVE_SSTREAM_H */
/* #undef HAVE_STAT_H */
/* #undef HAVE_STREAMBUF_H */
/* #undef HAVE_STRSTREAM_H */
/* #undef HAVE_STRSTREA_H */
/* #undef HAVE_SYS_NDIR_H */
/* #undef HAVE_ULONGLONG */
/* #undef HAVE_WAIT3 */
/* #undef INCLUDE_LIBC_H_AS_CXX */
/* #undef INCLUDE_MATH_H_AS_CXX */
/* #undef RETSIGTYPE */
/* #undef TM_IN_SYS_TIME */
/* #undef __CHAR_UNSIGNED__ */


/* Historical feature tests for BSD socket functions, which exist on POSIX
 * systems and on Windows. These are not tested anymore.
 */

#if defined(POISON_DEPRECATED_FEATURE_MACROS) && defined(__GNUC__)
#pragma GCC poison HAVE_ACCEPT
#pragma GCC poison HAVE_BIND
#pragma GCC poison HAVE_CONNECT
#pragma GCC poison HAVE_DECLARATION_SOCKLEN_T
#pragma GCC poison HAVE_GETADDRINFO
#pragma GCC poison HAVE_GETHOSTBYNAME
#pragma GCC poison HAVE_GETHOSTNAME
#pragma GCC poison HAVE_GETSOCKNAME
#pragma GCC poison HAVE_GETSOCKOPT
#pragma GCC poison HAVE_PROTOTYPE_ACCEPT
#pragma GCC poison HAVE_PROTOTYPE_BIND
#pragma GCC poison HAVE_PROTOTYPE_CONNECT
#pragma GCC poison HAVE_PROTOTYPE_GETHOSTBYNAME
#pragma GCC poison HAVE_PROTOTYPE_GETHOSTNAME
#pragma GCC poison HAVE_PROTOTYPE_GETSOCKNAME
#pragma GCC poison HAVE_PROTOTYPE_GETSOCKOPT
#pragma GCC poison HAVE_PROTOTYPE_LISTEN
#pragma GCC poison HAVE_PROTOTYPE_SELECT
#pragma GCC poison HAVE_PROTOTYPE_SETSOCKOPT
#pragma GCC poison HAVE_PROTOTYPE_SOCKET
#pragma GCC poison HAVE_SELECT
#pragma GCC poison HAVE_SETSOCKOPT
#pragma GCC poison HAVE_SOCKET
#endif

#if !defined(POISON_DEPRECATED_FEATURE_MACROS) || !defined(__GNUC__)
#define HAVE_ACCEPT 1
#define HAVE_BIND 1
#define HAVE_CONNECT 1
#define HAVE_DECLARATION_SOCKLEN_T 1
#define HAVE_GETADDRINFO 1
#define HAVE_GETHOSTBYNAME 1
#define HAVE_GETHOSTNAME 1
#define HAVE_GETSOCKNAME 1
#define HAVE_GETSOCKOPT 1
#define HAVE_PROTOTYPE_ACCEPT 1
#define HAVE_PROTOTYPE_BIND 1
#define HAVE_PROTOTYPE_CONNECT 1
#define HAVE_PROTOTYPE_GETHOSTBYNAME 1
#define HAVE_PROTOTYPE_GETHOSTNAME 1
#define HAVE_PROTOTYPE_GETSOCKNAME 1
#define HAVE_PROTOTYPE_GETSOCKOPT 1
#define HAVE_PROTOTYPE_LISTEN 1
#define HAVE_PROTOTYPE_SELECT 1
#define HAVE_PROTOTYPE_SETSOCKOPT 1
#define HAVE_PROTOTYPE_SOCKET 1
#define HAVE_SELECT 1
#define HAVE_SETSOCKOPT 1
#define HAVE_SOCKET 1
#endif

/* Historical OpenSSL feature tests that are not required anymore for OpenSSL 1.1.1 and newer.
 * We do not execute these tests and do not use these macros anymore inside DCMTK anymore.
 * The hard-coded defines are valid for OpenSSL 1.1.1 and newer, and for LibreSSL 3.7.2.
 * These macros are deprecated and will be removed from a future version.
 */

#if defined(POISON_DEPRECATED_FEATURE_MACROS) && defined(__GNUC__)
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_ASN1_STRING_GET0_DATA
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_DH_BITS
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_BASE_ID
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET0_EC_KEY
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_ID
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_RSA_PSS
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_OPENSSL_INIT_CRYPTO
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET0_PARAM
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CERT_STORE
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CIPHERS
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_CURVES
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_ECDH_AUTO
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_MAX_PROTO_VERSION
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_SECURITY_LEVEL
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC_JOB
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_CLIENT_HELLO_CB
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_128_CCM_8_SHA256
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_CHACHA20_POLY1305_SHA256
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TLS_METHOD
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_FAILURE_INFO
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_STATUS
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_TEXT
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTS_SET_CERTS
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_DATA
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_FLAGS
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_STORE
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTAFTER
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTBEFORE
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_X509_GET_SIGNATURE_NID
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_X509_STORE_CTX_GET0_CERT
#pragma GCC poison HAVE_OPENSSL_PROTOTYPE_X509_STORE_GET0_PARAM
#pragma GCC poison HAVE_OPENSSL_X509_ALGOR_GET0_CONST_PARAM
#endif

#if !defined(POISON_DEPRECATED_FEATURE_MACROS) || !defined(__GNUC__)
#define HAVE_OPENSSL_PROTOTYPE_ASN1_STRING_GET0_DATA 1
#define HAVE_OPENSSL_PROTOTYPE_DH_BITS 1
#define HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_BASE_ID 1
#define HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET0_EC_KEY 1
#define HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_ID 1
#define HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_RSA_PSS 1
#define HAVE_OPENSSL_PROTOTYPE_OPENSSL_INIT_CRYPTO 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET0_PARAM 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CERT_STORE 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CIPHERS 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_CURVES 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_ECDH_AUTO 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_MAX_PROTO_VERSION 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_SECURITY_LEVEL 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC_JOB 1
#define HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_CLIENT_HELLO_CB 1
#define HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_128_CCM_8_SHA256 1
#define HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384 1
#define HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_CHACHA20_POLY1305_SHA256 1
#define HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305 1
#define HAVE_OPENSSL_PROTOTYPE_TLS_METHOD 1
#define HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_FAILURE_INFO 1
#define HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_STATUS 1
#define HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_TEXT 1
#define HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTS_SET_CERTS 1
#define HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_DATA 1
#define HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_FLAGS 1
#define HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_STORE 1
#define HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTAFTER 1
#define HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTBEFORE 1
#define HAVE_OPENSSL_PROTOTYPE_X509_GET_SIGNATURE_NID 1
#define HAVE_OPENSSL_PROTOTYPE_X509_STORE_CTX_GET0_CERT 1
#define HAVE_OPENSSL_PROTOTYPE_X509_STORE_GET0_PARAM 1
#define HAVE_OPENSSL_X509_ALGOR_GET0_CONST_PARAM 1
#endif

// zlib-ng tweaks:
#define zlibVersion()   zlibng_version()

// oflog settings:
#define oflog_EXPORTS

#endif /* !OSCONFIG_H*/
