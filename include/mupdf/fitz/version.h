#ifndef MUPDF_FITZ_VERSION_H
#define MUPDF_FITZ_VERSION_H
#ifndef FZ_VERSION

#define FZ_VERSION_MAJOR 1
#define FZ_VERSION_MINOR 18
#define FZ_VERSION_PATCH 0
// build number: YYYY MM DD NNN, where NN is the sequence number within the given day / date
#define FZ_VERSION_BUILD "GHO2021061401"

// construct the version number:

#define FZ_VERSION_ELEMENT_STR__(part)	#part
#define FZ_VERSION_ELEMENT_STR(part)	FZ_VERSION_ELEMENT_STR__(part)

#define FZ_VERSION										\
	FZ_VERSION_ELEMENT_STR(FZ_VERSION_MAJOR) "."		\
	FZ_VERSION_ELEMENT_STR(FZ_VERSION_MINOR) "."		\
	FZ_VERSION_ELEMENT_STR(FZ_VERSION_PATCH) "."		\
	FZ_VERSION_BUILD

#endif
#endif
