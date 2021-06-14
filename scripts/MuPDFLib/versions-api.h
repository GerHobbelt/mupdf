#pragma once


enum muq_version_mode
{
	MUQ_VERSION_FULL,
	MUQ_VERSION_MAJOR, // = 1
	MUQ_VERSION_MINOR, // = 2
	MUQ_VERSION_PATCH, // = 3
	MUQ_VERSION_BUILD, // = 4
	MUQ_VERSION_FULL_PACKAGE,  // full version number with package name/bundle name as prefix
};



#ifdef __cplusplus
extern "C" {
#endif

const char* muq_report_version(enum muq_version_mode mode);

// Report each bundled library / tool, one per entry.
// The returned array is terminated by a NULL entry.
const char** muq_report_bundled_software(void);

#ifdef __cplusplus
}
#endif
