/*
 * tesseract CLI
 */

#include "../dll/pch.h"

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/jmemcust.h"

#include "../../source/fitz/tessocr.h"

#include <string.h>
#include <stdio.h>

#if defined(BUILD_MONOLITHIC)
#define main tesseract_tool_main
#else
#ifdef _MSC_VER
#define main main_utf8
#endif
#endif

int main(int argc, const char** argv)
{
	fz_context* ctx = NULL;

	if (!fz_has_global_context())
	{
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	if (argc == 0)
	{
		fz_error(ctx, "No command name found!");
		return EXIT_FAILURE;
	}

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	fz_set_leptonica_mem(ctx);

	//fz_info(ctx, "usage: tesseract <command> [options]");
	int rv = tesseract_main(argc, argv);

	fz_clear_leptonica_mem(ctx);

	fz_drop_context(ctx);

	return rv;
}


#if !defined(BUILD_MONOLITHIC) && defined(_MSC_VER)

int wmain(int argc, const wchar_t *wargv[])
{
	fz_context* ctx = fz_get_global_context();
	const char **argv = fz_argv_from_wargv(ctx, argc, wargv);
	if (!argv)
		return EXIT_FAILURE;
	int ret = main(argc, argv);
	fz_free_argv(ctx, argc, argv);
	return ret;
}

#endif
