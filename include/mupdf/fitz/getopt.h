#ifndef MUPDF_FITZ_GETOPT_H
#define MUPDF_FITZ_GETOPT_H

/**
	Simple functions/variables for use in tools.
*/
extern int fz_getopt(int nargc, const char * const *nargv, const char *ostr);
extern int fz_optind;
extern const char *fz_optarg;

/**
	Windows unicode versions.
*/
#if defined(_WIN32) || defined(_WIN64)
extern int fz_getoptw(int nargc, const wchar_t * const *nargv, const wchar_t *ostr);
extern int fz_optindw;
extern const wchar_t *fz_optargw;
#endif

/**
    Reset the fz_getopt() internals for when you want to use it on yet another argv/argc set.
*/
extern void fz_getopt_reset(void);

#endif
