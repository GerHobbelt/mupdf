/*
 * curl_tool -- our incantation of the ubiquitous curl tool
 */

#include "../dll/pch.h"

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"

#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define main main_utf8
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
		return 1;
	}

	//fz_info(ctx, "usage: curl [options]");
	int rv = curl_main(argc, argv);

	fz_drop_context(ctx);

	return rv;
}

#ifdef _MSC_VER
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
