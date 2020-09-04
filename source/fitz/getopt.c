/*
 * This is a version of the public domain getopt implementation by
 * Henry Spencer originally posted to net.sources.
 *
 * This file is in the public domain.
 */

#include "mupdf/fitz/version.h"
#include "mupdf/fitz/config.h"
#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"

#include "mupdf/fitz/getopt.h"

#include <stdio.h>
#include <string.h>

#define getopt fz_getopt
#define optarg fz_optarg
#define optind fz_optind

const char *optarg; /* Global argument pointer. */
int optind = 0; /* Global argv index. */

static const char *scan = NULL; /* Private scan pointer. */

int
getopt(int argc, const char * const *argv, const char *optstring)
{
	char c;
	char *place;

	optarg = NULL;

	if (!scan || *scan == '\0') {
		if (optind == 0)
			optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
			return EOF;
		if (argv[optind][1] == '-' && argv[optind][2] == '\0') {
			optind++;
			return EOF;
		}

		scan = argv[optind]+1;
		optind++;
	}

	c = *scan++;
	place = strchr(optstring, c);

	if (!place || c == ':') {
		fz_error(NULL, "%s: unknown option -%c", argv[0], c);
		return '?';
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			optarg = scan;
			scan = NULL;
		} else if( optind < argc ) {
			optarg = argv[optind];
			optind++;
		} else {
			fz_error(NULL, "%s: option requires argument -%c", argv[0], c);
			return ':';
		}
	}

	return c;
}

void fz_getopt_reset(void)
{
	optind = 0;
	optarg = NULL;

	scan = NULL;
}
