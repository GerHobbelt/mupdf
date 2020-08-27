/*
 * mutool -- swiss army knife of pdf manipulation tools
 */

//#include "mupdf/fitz.h"
#include "../dll/pch.h"

#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define main main_utf8
#endif

int main(int argc, const char **argv)
{
	char *start, *end;
	char buf[32];
	int i;

	if (argc == 0)
	{
		fprintf(stderr, "No command name found!\n");
		return 1;
	}

	/* Check argv[0] */

	/* Print usage */

	fprintf(stderr, "usage: mutool <command> [options]\n");

#if 0   // test to see the reverse ordered number gen in this call work as expected.
	char p[256];
	fz_format_output_path(NULL, p, sizeof(p), "out-%04d.png", 42);
#endif

	return 1;
}

#ifdef _MSC_VER
int wmain(int argc, const wchar_t *wargv[])
{
	const char **argv = fz_argv_from_wargv(argc, wargv);
	if (!argv)
		return EXIT_FAILURE;
	int ret = main(argc, argv);
	fz_free_argv(argc, argv);
	return ret;
}
#endif
