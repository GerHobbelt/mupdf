
#include "system_override_internal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cassert>


extern void bz_internal_error (int errcode);

#if 0
#include "bzlib_private.h"
#endif

// void BZ2_bz__AssertH__fail (int errcode)
void bz_internal_error (int errcode)
{
	fprintf(stderr,
		 "\n\nbzip2/libbzip2: internal error number %d.\n"
		 "This is a bug in our compiled-in bzip2/libbzip2.\n"
		 "Please make an effort to report this bug;\n"
		 "timely and accurate bug reports eventually lead to higher\n"
		 "quality software.  Thanks.\n\n",
		 errcode
	);

	if (errcode == 1007) {
		fprintf(stderr,
			 "\n*** A special note about internal error number 1007 ***\n"
			 "\n"
			 "Experience suggests that a common cause of i.e. 1007\n"
			 "is unreliable memory or other hardware.  The 1007 assertion\n"
			 "just happens to cross-check the results of huge numbers of\n"
			 "memory reads/writes, and so acts (unintendedly) as a stress\n"
			 "test of your memory system.\n"
			 "\n"
			 "I suggest the following: try compressing the file again,\n"
			 "possibly monitoring progress in detail with the -vv flag.\n"
			 "\n"
			 "* If the error cannot be reproduced, and/or happens at different\n"
			 "  points in compression, you may have a flaky memory system.\n"
			 "  Try a memory-test program.  I have used Memtest86\n"
			 "  (www.memtest86.com).  At the time of writing it is free (GPLd).\n"
			 "  Memtest86 tests memory much more thoroughly than your BIOSs\n"
			 "  power-on test, and may find failures that the BIOS doesn't.\n"
			 "\n"
			 "* If the error can be repeatably reproduced, this is a bug in\n"
			 "  bzip2, and I would very much like to hear about it.  Please\n"
			 "  let me know, and, ideally, save a copy of the file causing the\n"
			 "  problem -- without which I will be unable to investigate it.\n"
			 "\n"
		);
	}

	SystemOverride_KickInTheDoor();
	exit(666);
}

