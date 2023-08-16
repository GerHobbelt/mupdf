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
#include "mupdf/fitz/string-util.h"

#include "mupdf/fitz/getopt.h"

#include <stdio.h>
#include <string.h>

#include "mupdf/fitz/getopt.h"
#include "mupdf/fitz/string-util.h"

const char *fz_optarg = NULL;    /* Global argument pointer. */
int fz_optind = 0;               /* Global argv index. */
static int fz_optlen = 0;        /* Option length. */

static const char *
find_short_option(const char *str, char c)
{
	char d;

	while ((d = *str++) != 0)
	{
		if (d == c)
		{
			fz_optlen = 1;
			return str;
		}
		/* Are we on a long option? */
		if (d == '-')
		{
			/* Skip to the next one. */
			while ((d = *str++) != 0 && d != ',');
		}
	}

	return NULL;
}

static const char *
find_long_option(const char **strp, const char *opt)
{
	char d;
	const char *str = *strp;

	while ((d = *str++) != 0)
	{
		/* Nothing more to do for a short option. For a long option, try to match. */
		if (d == '-')
		{
			/* Long option */
			const char *o = opt;
			const char *s = str;
			while (*o && *s == *o && *s != ',' && *s != ':')
				s++, o++;
			if ((*o == 0 || *o == '=') && (*s == 0 || *s == ',' || *s == ':'))
			{
				/* Matched */
				fz_optlen = o - opt;
				*strp = str;
				return s;
			}
			/* Skip to the end of the long option that we failed to match */
			while (*s != 0) {
				d = *s++;
				if (d == ',' || d == ':')
					break;
			}
			str = s;
		}
	}

	*strp = NULL;
	return NULL;
}

static const char *scan = NULL; /* Private scan pointer. */
static const char *longopt = NULL;

int
fz_getopt(int argc, const char * const *argv, const char *optstring)
{
	int c;
	const char *place;
	int long_option = 0;

	fz_optarg = NULL;

	if (!scan || *scan == '\0') {
		if (fz_optind == 0)
			fz_optind++;

		if (fz_optind >= argc || argv[fz_optind][0] != '-' || argv[fz_optind][1] == '\0')
			return EOF;
		if (argv[fz_optind][1] == '-' && argv[fz_optind][2] == '\0') {
			fz_optind++;
			return EOF;
		}
		if (argv[fz_optind][1] == '-')
			long_option = 1;

		scan = argv[fz_optind]+1+long_option;
		fz_optind++;
	}

	if (long_option) {
		c = FZ_LONGOPT;
		longopt = optstring;
		place = find_long_option(&longopt, scan);
		scan += fz_optlen;
		if (*scan == '=')
			scan++;
	} else {
		c = *scan++;
		place = find_short_option(optstring, c);
	}

	if (!place || c == ':') {
		fz_error(NULL, "%s: unknown option -%c", fz_basename(argv[0]), c);
		return '?';
	}

	if (*place == ':') {
		if (*scan != '\0') {
			fz_optarg = scan;
			scan = NULL;
		} else if( fz_optind < argc ) {
			fz_optarg = argv[fz_optind];
			fz_optind++;
		} else {
			fz_error(NULL, "%s: option requires argument -%c", fz_basename(argv[0]), c);
			return ':';
		}
	}

	return c;
}

void fz_getopt_reset(void)
{
	fz_optind = 0;
	fz_optarg = NULL;
	fz_optlen = 0;

	scan = NULL;
}

int fz_longopt_is(const char *opt)
{
	if (longopt && !strncmp(opt, longopt, fz_optlen))
		return 1;
	return 0;
}

int
fz_opt_from_list(const char *opt, const char *optlist)
{
	int n = 0;

	while (*optlist)
	{
		const char *optend = optlist;

		while (*optend != 0 && *optend != '|')
			optend++;

		if (!fz_strncasecmp(optlist, opt, optend-optlist) && (opt[optend-optlist] == 0 || opt[optend-optlist] == ','))
			return n;

		n++;
		if (*optend == '|')
			optend++;
		optlist = optend;
	}

	fz_error(NULL, "Unrecognised option: %s", opt);
	return -1;
}

