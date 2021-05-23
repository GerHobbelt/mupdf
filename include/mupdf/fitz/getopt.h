#ifndef MUPDF_FITZ_GETOPT_H
#define MUPDF_FITZ_GETOPT_H

/**
	Simple functions/variables for use in tools.
*/
extern int fz_getopt(int nargc, const char * const *nargv, const char *ostr);
FZ_DATA extern int fz_optind;
FZ_DATA extern const char *fz_optarg;

/**
    Reset the fz_getopt() internals for when you want to use it on yet another argv/argc set.
*/
extern void fz_getopt_reset(void);

#endif
