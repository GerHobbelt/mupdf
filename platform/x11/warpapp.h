#ifndef WARPAPP_H
#define WARPAPP_H

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <time.h>

/*
 * Utility object for handling a pdf application / view
 * Takes care of PDF loading and displaying and navigation,
 * uses a number of callbacks to the GUI app.
 */

/* 25% .. 1600% */
#define MINRES 18
#define MAXRES 1152
#define MAX_WIN_DIM 800

typedef struct warpapp_s warpapp_t;

enum { ARROW, HAND, WAIT, CARET };

enum { DISCARD, SAVE, CANCEL };

enum { QUERY_NO, QUERY_YES };

extern void winwarn(warpapp_t*, char *s);
extern void winerror(warpapp_t*, char *s);
extern void wintitle(warpapp_t*, char *title);
extern void winresize(warpapp_t*, int w, int h);
extern void winrepaint(warpapp_t*);
extern void winrepaintsearch(warpapp_t*);
extern char *winpassword(warpapp_t*, char *filename);
extern char *wintextinput(warpapp_t*, char *inittext, int retry);
extern int winchoiceinput(warpapp_t*, int nopts, const char *opts[], int *nvals, const char *vals[]);
extern void winopenuri(warpapp_t*, char *s);
extern void wincursor(warpapp_t*, int curs);
extern void windocopy(warpapp_t*);
extern void windrawstring(warpapp_t*, int x, int y, char *s);
extern void winclose(warpapp_t*);
extern void winhelp(warpapp_t*);
extern void winfullscreen(warpapp_t*, int state);
extern int winsavequery(warpapp_t*);
extern int winquery(warpapp_t*, const char*);
extern int wingetcertpath(warpapp_t *, char *buf, int len);
extern int wingetsavepath(warpapp_t*, char *buf, int len);
extern void winalert(warpapp_t *, pdf_alert_event *alert);
extern void winprint(warpapp_t *);
extern void winadvancetimer(warpapp_t *, float duration);
extern void winreplacefile(warpapp_t *, char *source, char *target);
extern void wincopyfile(warpapp_t *, char *source, char *target);
extern void winreloadpage(warpapp_t *);

struct warpapp_s
{
	/* current document params */
	fz_document *doc;
	char *docpath;
	char *doctitle;

	/* current view params */
	float resolution;
	fz_pixmap *image;
	int imgw, imgh;
	int grayscale;
	fz_colorspace *colorspace;

	/* current page params */
	fz_page *page;
	fz_rect page_bbox;
	fz_pixmap *full_pix;
	fz_pixmap *disp_pix;
	fz_pixmap *warped_pix;
	fz_pixmap *disp_warped_pix;

	/* window system sizes */
	int winw, winh;
	int scrw, scrh;

	/* event handling state */
	char number[256];
	int numberlen;

	/* client context storage */
	void *userdata;

	fz_point doc_corners[4];

	int dragging_corner;

	fz_context *ctx;
};

void warpapp_init(fz_context *ctx, warpapp_t *app);
void warpapp_setresolution(warpapp_t *app, int res);
void warpapp_open(warpapp_t *app, char *filename, int reload);
void warpapp_open_progressive(warpapp_t *app, char *filename, int reload, int kbps);
void warpapp_close(warpapp_t *app);
int warpapp_preclose(warpapp_t *app);
void warpapp_reloadfile(warpapp_t *app);

char *warpapp_version(warpapp_t *app);
char *warpapp_usage(warpapp_t *app);

void warpapp_onkey(warpapp_t *app, int c, int modifiers);
void warpapp_onmouse(warpapp_t *app, int x, int y, int btn, int modifiers, int state);
void warpapp_oncopy(warpapp_t *app, unsigned short *ucsbuf, int ucslen);
void warpapp_onresize(warpapp_t *app, int w, int h);
void warpapp_gotopage(warpapp_t *app, int number);
void warpapp_reloadpage(warpapp_t *app);
void warpapp_autozoom_horizontal(warpapp_t *app);
void warpapp_autozoom_vertical(warpapp_t *app);
void warpapp_autozoom(warpapp_t *app);

void warpapp_invert(warpapp_t *app, fz_rect rect);
void warpapp_inverthit(warpapp_t *app);

void warpapp_postblit(warpapp_t *app);

void warpapp_warn(warpapp_t *app, const char *fmt, ...);
void warpapp_error(warpapp_t *app, char *msg);

#endif
