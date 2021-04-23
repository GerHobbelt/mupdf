#ifndef MUPDF_FITZ_EXPORT_H
#define MUPDF_FITZ_EXPORT_H

/*
 * Support for building/using MuPDF DLL on Windows.
 *
 * When compiling code that uses MuPDF DLL, FZ_DLL_CLIENT should be defined.
 *
 * When compiling MuPDF DLL itself, FZ_DLL should be defined.
 */

#if defined(WIN32) || defined(WIN64)
	#if defined(FZ_DLL)
		/* Building DLL. */
		#pragma message("FZ_DLL defined")
		#define FZ_FUNCTION __declspec(dllexport)
		#define FZ_DATA __declspec(dllexport)
	#elif defined(FZ_DLL_CLIENT)
		/* Building DLL client code. */
		#pragma message("FZ_DLL_CLIENT defined")
		#define FZ_FUNCTION __declspec(dllexport)
		#define FZ_DATA __declspec(dllimport)
	#else
		#pragma message("FZ_DLL* not defined")
		#define FZ_FUNCTION
		#define FZ_DATA
	#endif
#else
	#pragma message("Not WIN32 or WIN64")
	#define FZ_FUNCTION
	#define FZ_DATA
#endif

#endif
