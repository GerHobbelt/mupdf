/*
 * misc_tests_tool
 */

#include "../dll/pch.h"

#include "mupdf/fitz.h"

#include <string.h>
#include <stdio.h>

static int setup_ctx(void)
{
	if (!fz_has_global_context())
	{
		fz_context* ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}
	return 0;
}

#ifdef _MSC_VER
#define main main_utf8
#endif

int main(int argc, const char **argv)
{
	int rv = setup_ctx();
	if (rv) return rv;

	fz_context* ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
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

	/* Check argv[0] */

	/* Print usage */

	fz_info(ctx, "usage: mutool <command> [options]");

#if 0   // test to see the reverse ordered number gen in this call work as expected.
	char p[256];
	fz_format_output_path(NULL, p, sizeof(p), "out-%04d.png", 42);
#endif

	fz_drop_context(ctx);

	return EXIT_FAILURE;
}

#ifdef _MSC_VER
int wmain(int argc, const wchar_t **wargv)
{
	int rv = setup_ctx();
	if (rv) return rv;

	fz_context* ctx = fz_get_global_context();
	const char **argv = fz_argv_from_wargv(ctx, argc, wargv);
	if (!argv)
		return EXIT_FAILURE;
	int ret = main(argc, argv);
	fz_free_argv(ctx, argc, argv);
	return ret;
}
#endif
