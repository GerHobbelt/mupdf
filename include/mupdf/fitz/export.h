#ifndef MUPDF_FITZ_EXPORT_H
#define MUPDF_FITZ_EXPORT_H

#if defined(WIN32)
	#if defined(FZ_DLL)
		/* Building DLL. */
		#define FZ_FN __declspec(dllexport)
		#define FZ_DATA __declspec(dllexport)
	#elif defined(FZ_DLL_CLIENT)
		/* Building DLL client code. */
		#define FZ_FN __declspec(dllexport)
		#define FZ_DATA __declspec(dllimport)
	#else
		#define FZ_FN
		#define FZ_DATA
	#endif
#else
	#define FZ_DATA
#endif

#endif
