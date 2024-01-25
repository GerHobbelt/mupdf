
#pragma once

#define HAVE_WARNING_CPP_DIRECTIVE  

#define HAVE_DIRENT_H  1
#define HAVE_FCNTL_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1

#undef HAVE_UNISTD_H

#undef HAVE_SYS_STATVFS_H
#undef HAVE_SYS_VFS_H
#undef HAVE_SYS_STAT_H
#undef HAVE_SYS_PARAM_H
#undef HAVE_SYS_MOUNT_H

#define HAVE_GETOPT_LONG 1
#define HAVE_GETOPT 1

// AC_CHECK_FUNCS([link mkstemp mkostemp _mktemp_s mkdtemp getopt getopt_long getprogname getexecname rand random lrand48 random_r rand_r readlink fstatvfs fstatfs lstat strerror strerror_r])

#undef HAVE_POSIX_FADVISE

#define HAVE_RAND   1

//AC_DEFINE_UNQUOTED(USE_ICONV,$use_iconv,[Use iconv.])

//AC_SUBST(PREFERRED_SUB_PIXEL_RENDERING)
//AC_SUBST(PREFERRED_HINTING)


//AC_SUBST(FC_DEFAULT_FONTS)
//AC_DEFINE_UNQUOTED(FC_ADD_FONTS,"$add_fonts",[Additional font directories])
//AC_SUBST(FC_FONTPATH)
//FC_CACHEDIR=${fc_cachedir}

//	AC_DEFINE(HAVE_STDATOMIC_PRIMITIVES, 1, [Have C99 stdatomic atomic primitives])
//	AC_DEFINE(HAVE_INTEL_ATOMIC_PRIMITIVES, 1, [Have Intel __sync_* atomic primitives])
//	AC_DEFINE(HAVE_SOLARIS_ATOMIC_OPS, 1, [Have Solaris __machine_*_barrier and atomic_* operations])
//	AC_DEFINE(HAVE_PTHREAD, 1, [Have POSIX threads])
//	AC_CHECK_PROG(HASDOCBOOK, docbook2html, yes, no)


#define FLEXIBLE_ARRAY_MEMBER  		256

#define SIZEOF_VOID_P        	8
#define ALIGNOF_DOUBLE			8


#define FC_DEFAULT_FONTS ""
#define FC_CACHEDIR		""
#define CONFIGDIR 		""
#define FC_TEMPLATEDIR 	""


#define FC_GPERF_SIZE_T  size_t




