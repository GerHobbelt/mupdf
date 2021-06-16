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
//
// When array_buffer arg is NULL, function returns required number of entries.
// When array_element_count is too small, the array will be truncated, ensuring there's always
// a NULL sentinel at the end (at index array_element_count-1 then).
size_t muq_report_bundled_software(const char** array_buffer, size_t array_element_count);

#ifdef __cplusplus
}
#endif
