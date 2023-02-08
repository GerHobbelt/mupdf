//
// Provide default memory and backing-store handlers, depending on the occasion.
//

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "jmemsys.h"


#if 01
#include "../../thirdparty/owemdjee/libjpeg-turbo/jmemnobs.c"
#endif

#if 0
#include "../../thirdparty/libjpeg/jmemansi.c"
#endif

#if 0
#include "../../thirdparty/owemdjee/CxImage/jpeg/jmemname.c"
#endif

#if 0
#if defined(USE_MAC_MEMMGR)
#include "../../thirdparty/owemdjee/CxImage/jpeg/jmemmac.c"
#endif
#endif

#if 0
#include "../../source/fitz/jmemcust.c"
#endif
