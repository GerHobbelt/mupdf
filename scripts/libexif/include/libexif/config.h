
#define PACKAGE_BUGREPORT  "libexif-devel@lists.sourceforge.net"

#define VERSION   "0.6.25.1"

#define PACKAGE  "exif + libexif"



#undef HAVE_SIGACTION

//<langinfo.h>
#undef HAVE_LANGINFO_H
//<libintl.h>
#undef HAVE_LIBINTL_H

#undef HAVE_UNISTD_H

#define HAVE_IO_H   1




// bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR)
#define GETTEXT_PACKAGE    "exif"
#define LOCALEDIR          ""

#define CODESET_UTF_8     1

#define nl_langinfo(codeset)         codeset

#include <stdint.h>

typedef int64_t ssize_t;
