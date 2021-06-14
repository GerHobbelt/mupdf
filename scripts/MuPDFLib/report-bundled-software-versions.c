// define stubs for the possibly missing APIs:

#include "mupdf/fitz.h"

#include <string.h>
#include <stdio.h>

#include "versions-api.h"


#ifdef __cplusplus
extern "C" {
#endif


const char *
muq_report_version(enum muq_version_mode mode)
{
	const char* bn = FZ_VERSION_BUILD;

	switch (mode)
	{
	default:
	case MUQ_VERSION_FULL_PACKAGE:
		return "Qiqqa-PDF-tooling v" FZ_VERSION;

	case MUQ_VERSION_FULL:
		return FZ_VERSION;

	case MUQ_VERSION_MAJOR:
		return FZ_VERSION_ELEMENT_STR(FZ_VERSION_MAJOR);

	case MUQ_VERSION_MINOR:
		return FZ_VERSION_ELEMENT_STR(FZ_VERSION_MINOR);

	case MUQ_VERSION_PATCH:
		return FZ_VERSION_ELEMENT_STR(FZ_VERSION_PATCH);

	case MUQ_VERSION_BUILD:
		return bn + 3; // skip 'GHO' prefix in build sequence ID string
	}
}


#include "../../scripts/libglob/glob/version.h"
#include "../../thirdparty/owemdjee/cryptopp/config_ver.h"

static const char* clipp_version = "clipp v1.2.3 (https://github.com/muellan/clipp)";
static const char* libglob_version = "libglob v" GLOB_VERSION " (https://github.com/p-ranav/glob)";
static const char* glog_version = "google-glog v0.5.0 (https://github.com/google/glog)";
static const char* gtest_version = "googletest v1.11.0 (https://github.com/google/googletest)";
static const char* libzstd_version = "zstd v1.5.0 (https://github.com/facebook/zstd)";
static const char* lib_nas_filelock_version = "lib_nas_filelock v0.1 (https://github.com/GerHobbelt/lib_nas_lockfile)";
static const char* libcryptopp_version = "Crypto++ v" FZ_VERSION_ELEMENT_STR(CRYPTOPP_MAJOR) "." FZ_VERSION_ELEMENT_STR(CRYPTOPP_MINOR) "." FZ_VERSION_ELEMENT_STR(CRYPTOPP_REVISION) " (https://github.com/weidai11/cryptopp)";

const char** muq_report_bundled_software(void)
{
	const char* bundles_array[] =
	{
		clipp_version,
		libglob_version,
		glog_version,
		gtest_version,
		libzstd_version,
		lib_nas_filelock_version,
		libcryptopp_version,
		NULL
	};

	return bundles_array;
}


#ifdef __cplusplus
}
#endif

