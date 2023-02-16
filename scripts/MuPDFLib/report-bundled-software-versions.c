// define stubs for the possibly missing APIs:

#include "mupdf/fitz.h"
#include "../../thirdparty/owemdjee/sqlite-amalgamation/sqlite3.h"

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

// TODO: add link to own forks

static const char* clipp_version = "clipp v1.2.3 (https://github.com/muellan/clipp)";
static const char* libglob_version = "libglob v" GLOB_VERSION " (https://github.com/p-ranav/glob)";
static const char* glog_version = "google-glog v0.5.0 (https://github.com/google/glog)";
static const char* gtest_version = "googletest v1.11.0 (https://github.com/google/googletest)";
static const char* libzstd_version = "zstd v1.5.0 (https://github.com/facebook/zstd)";
static const char* lib_nas_filelock_version = "lib_nas_filelock v0.1 (https://github.com/GerHobbelt/lib_nas_lockfile)";
static const char* libcryptopp_version = "Crypto++ v" FZ_VERSION_ELEMENT_STR(CRYPTOPP_MAJOR) "." FZ_VERSION_ELEMENT_STR(CRYPTOPP_MINOR) "." FZ_VERSION_ELEMENT_STR(CRYPTOPP_REVISION) " (https://github.com/weidai11/cryptopp)";
static const char* libfmt_version = "fmt v8.0.0 (https://github.com/fmtlib/fmt)";
static char libsqlite_version[2048];
static const char* libsvgcharter_version = "charter v2018.01.09 (https://github.com/Mandarancio/charter)";
static const char* libtinyexpr_version = "tinyexpr (https://github.com/codeplea/tinyexpr)";
static const char* libsoldout_version = "libsoldout/upskirt/sundown/scidown (http://fossil.instinctive.eu/libsoldout/)";
static const char* libxml2_version = "libxml2 v2.9.12 (https://gitlab.gnome.org/GNOME/libxml2)";
static const char* libgif_version = "libgif ... (...)";
static const char* libjpegturbo_version = "libjpeg-turbo ... (...)";
static const char* libwebp_version = "libwebp ... (...)";
static const char* pngpmttools_version = "png-pmt-tools ... (...)";
static const char* libpthreadswin32_version = "pthreads-win32 ... (...)";
static const char* libcrow_version = "crow ... (...)";
static const char* libopenssl_version = "OpenSSL ... (...)";
static const char* libcurl_version = "curl ... (...)";
static const char* jpeginfo_version = "jpeginfo ... (...)";
static const char* libtesseract_version = "tesseract ... (...)";
static const char* libextract_version = "extract ... (...)";
static const char* libfreeglut_version = "freeglut ... (...)";
static const char* libleptonica_version = "leptonica ... (...)";
static const char* libgumbo_version = "gumbo-parser ... (...)";
static const char* libharfbuzz_version = "harfbuzz ... (...)";
static const char* liblcms2_version = "lcms2 ... (...)";
static const char* libfreetype_version = "freetype ... (...)";
static const char* libjpeg_version = "libjpeg ... (...)";
static const char* libjbig2dec_version = "jbig2dec ... (...)";
static const char* libmujs_version = "mujs ... (...)";
static const char* libquickjs_version = "QuickJS ... (...)";
static const char* libopenjpeg_version = "openJPEG ... (...)";
static const char* libpng_version = "libpng ... (...)";
static const char* libtiff_version = "libtiff ... (...)";
static const char* libzlibng_version = "zlib/zlib-ng ... (...)";
static const char* libmupdf_version = "mupdf ... (...)";

size_t muq_report_bundled_software(const char** array_buffer, size_t array_element_count)
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
		libfmt_version,
		libsqlite_version,
		libsvgcharter_version,
		libtinyexpr_version,
		libsoldout_version,
		libxml2_version,
		libgif_version,
		libjpegturbo_version,
		libwebp_version,
		pngpmttools_version,
		libpthreadswin32_version,
		libcrow_version,
		libopenssl_version,
		libcurl_version,
		jpeginfo_version,
		libtesseract_version,
		libextract_version,
		libfreeglut_version,
		libleptonica_version,
		libgumbo_version,
		libharfbuzz_version,
		liblcms2_version,
		libfreetype_version,
		libjpeg_version,
		libjbig2dec_version,
		libmujs_version,
		libquickjs_version,
		libopenjpeg_version,
		libpng_version,
		libtiff_version,
		libzlibng_version,
		libmupdf_version,
		NULL
	};
	size_t count = sizeof(bundles_array) / sizeof(bundles_array[0]);

	if (array_buffer && array_element_count > 0)
	{
		{
			fz_snprintf(libsqlite_version, sizeof(libsqlite_version), "SQLite v%s [", sqlite3_libversion());
			char* dst = libsqlite_version;
			size_t n = strlen(libsqlite_version);
			size_t len = sizeof(libsqlite_version);
			dst += n;
			ASSERT0(len > n);
			len -= n;
			for (int i = 0; ; i++)
			{
				const char* opt = sqlite3_compileoption_get(i);
				if (!opt)
					break;

				fz_snprintf(dst, len, "%s,", opt);
				n = strlen(dst);
				dst += n;
				ASSERT0(len > n);
				len -= n;
			}
			dst--; // drop that last comma!
			fz_snprintf(dst, len, "] (https://sqlite.org/)");
			n = strlen(dst);
			ASSERT0(len > n);
		}

		if (array_element_count < count)
		{
			count = array_element_count;
		}
		bundles_array[count - 1] = NULL;  // make sure output has NULL sentinel at end.
		memcpy((void *)array_buffer, bundles_array, count * sizeof(bundles_array[0]));
	}
	return count;
}


#ifdef __cplusplus
}
#endif

