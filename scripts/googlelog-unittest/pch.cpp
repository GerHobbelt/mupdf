//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "pch.h"


// ripped from gflags\test\gflags_unittest.cc

namespace google {

	std::ostream & gflags_stderr_ostream() {
		return std::cerr;
	}

	void gflags_stderr_printf(const char *msg, ...) {
		va_list args;
		va_start(args, msg);
		vfprintf(stderr, msg, args);
		va_end(args);
		fflush(stderr);
	}

	void gflags_std_exit(int retval) {
		::std::exit(retval);
	}

}		// namespace GFLAGS_NAMESPACE

