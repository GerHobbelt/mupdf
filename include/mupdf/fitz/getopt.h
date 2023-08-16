// Copyright (C) 2004-2021 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

#ifndef MUPDF_FITZ_GETOPT_H
#define MUPDF_FITZ_GETOPT_H

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
	Simple functions/variables for use in tools.

	ostr = option string. Comprises single letter options, followed by : if there
	is an argument to the option. Also comprises long-options; '--foo' would be
	defined as '-foo'. Long options are terminated either by ':', the end of the
	options string, or ','.

	For example:

	"vhp:-foo,-bar:" would indicate support for:
		-v
		-h
		-p <arg>
		--foo
		--bar <arg>


	In the event of matching a single char option, this function returns the char.

	In the event of matching a long option, this function returns FZ_LONGOPT. The long
	option can then be tested using fz_longoptis().

	In the event of reaching the end of the arg list or '--', this function returns EOF.

	In the event of failing to match anything, an error is printed, and we return '?'.

	If an argument is expected for the option, then fz_optarg will be returned pointing
	at the start of the argument. Examples of supported argument formats: '-r500', '-r 500',
	'--resolution 500', '--resolution=500'.
*/
extern int fz_getopt(int nargc, const char * const *nargv, const char *ostr);

/**
	If fz_getopt returns FZ_LONGOPT, then this function tests for specific
	long options.
*/
extern int fz_longopt_is(const char *opt);

/**
	fz_optind is updated to point to the current index being read from the
	arguments.
*/
FZ_DATA extern int fz_optind;

/**
	fz_optarg is a pointer to the argument data for the most recently
	read option.
*/
FZ_DATA extern const char *fz_optarg;

/**
	Magic value returned to indicate that we match a long option.
*/
#define FZ_LONGOPT -2

/**
	Return the index of a (case-insensitive) option within an optlist.

	For instance for optlist = "Foo|Bar|Baz", and  opt = "bar",
	this would return 1.

	In the event of no-match found, prints an error and returns -1.
*/
int fz_opt_from_list(const char *optlist, const char *opt);

/**
    Reset the fz_getopt() internals for when you want to use it on yet another argv/argc set.
*/
extern void fz_getopt_reset(void);

#ifdef __cplusplus
}
#endif

#endif
