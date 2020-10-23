/* config.h.in. 
 */


/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS		1

/* The number of bytes in a int.  */
#define SIZEOF_INT			4

/* The number of bytes in a long.  */
#define SIZEOF_LONG			4

/* Define if you have the getopt_long function.  */
#define HAVE_GETOPT_LONG	1

/* Define if you have the strcasecmp function.  */
#undef HAVE_STRCASECMP

/* Define if you have the <getopt.h> header file.  */
#define HAVE_GETOPT_H		1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H		1

/* Define if you have the <strings.h> header file.  */
#undef HAVE_STRINGS_H		

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you have the jpeg library (-ljpeg).  */
#define HAVE_LIBJPEG		1



/* Define if you have broken jmorecfg.h (SGI's usually have this problem) */
#undef BROKEN_METHODDEF


#if defined(_M_X64)
#define HOST_TYPE			"Win64"
#elif defined(_M_AMD64)
#define HOST_TYPE			"Win64"
#elif defined(_M_IX86)
#define HOST_TYPE			"Win32"
#else
#define HOST_TYPE			""
#endif


/* Cope with systems (such as Windows) with no S_ISDIR */
#ifndef S_ISDIR
#define S_ISDIR(mode) ((mode) & S_IFDIR)
#endif

#ifndef HAVE_STRCASECMP
#define strcasecmp		_stricmp
#define strncasecmp		_strnicmp
#endif

/* eof :-) */
