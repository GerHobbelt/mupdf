#include "warpapp.h"
#include "curl_stream.h"
#include "mupdf/helpers/pkcs7-openssl.h"
#include "../../source/fitz/pixmap-imp.h"

#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#define stat _stat
#endif

#ifdef _WIN32
#include <windows.h>
#include <direct.h> /* for getcwd */
#else
#include <unistd.h> /* for getcwd */
#endif

#define BEYOND_THRESHHOLD 40

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

time_t
stat_mtime(const char *path)
{
	struct stat info;

	if (stat(path, &info) < 0)
		return 0;

	return info.st_mtime;
}

static int convert_to_accel_path(fz_context *ctx, char outname[], char *absname, size_t len)
{
	char *tmpdir;
	char *s;

	tmpdir = getenv("TEMP");
	if (!tmpdir)
		tmpdir = getenv("TMP");
	if (!tmpdir)
		tmpdir = "/var/tmp";
	if (!fz_is_directory(ctx, tmpdir))
		tmpdir = "/tmp";

	if (absname[0] == '/' || absname[0] == '\\')
		++absname;

	s = absname;
	while (*s) {
		if (*s == '/' || *s == '\\' || *s == ':')
			*s = '%';
		++s;
	}

	if (fz_snprintf(outname, len, "%s/%s.accel", tmpdir, absname) >= len)
		return 0;
	return 1;
}

static int get_accelerator_filename(fz_context *ctx, char outname[], size_t len, const char *filename)
{
	char absname[PATH_MAX];
	if (!fz_realpath(filename, absname))
		return 0;
	if (!convert_to_accel_path(ctx, outname, absname, len))
		return 0;
	return 1;
}

static void save_accelerator(fz_context *ctx, fz_document *doc, const char *filename)
{
	char absname[PATH_MAX];

	if (!doc)
		return;
	if (!fz_document_supports_accelerator(ctx, doc))
		return;
	if (!get_accelerator_filename(ctx, absname, sizeof(absname), filename))
		return;

	fz_save_accelerator(ctx, doc, absname);
}

static void warpapp_showpage(warpapp_t *app, int loadpage, int drawpage, int repaint);

static const int zoomlist[] = {
	18, 24, 36, 54, 72, 96, 120, 144, 180,
	216, 288, 360, 432, 504, 576, 648, 720,
	792, 864, 936, 1008, 1080, 1152
};

static int zoom_in(int oldres)
{
	int i;
	for (i = 0; i < (int)nelem(zoomlist) - 1; ++i)
		if (zoomlist[i] <= oldres && zoomlist[i+1] > oldres)
			return zoomlist[i+1];
	return zoomlist[i];
}

static int zoom_out(int oldres)
{
	int i;
	for (i = 0; i < (int)nelem(zoomlist) - 1; ++i)
		if (zoomlist[i] < oldres && zoomlist[i+1] >= oldres)
			return zoomlist[i];
	return zoomlist[0];
}

void warpapp_warn(warpapp_t *app, const char *fmt, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	fz_vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	buf[sizeof(buf)-1] = 0;
	winwarn(app, buf);
}

void warpapp_error(warpapp_t *app, char *msg)
{
	winerror(app, msg);
}

char *warpapp_version(warpapp_t *app)
{
	return
		"MuPDF " FZ_VERSION "\n"
		"Copyright 2006-2020 Artifex Software, Inc.\n";
}

char *warpapp_usage(warpapp_t *app)
{
	return
		"q\t\t-- quit\n"
	;
}

void warpapp_init(fz_context *ctx, warpapp_t *app)
{
	memset(app, 0, sizeof(warpapp_t));
	app->scrw = MAX_WIN_DIM;
	app->scrh = MAX_WIN_DIM;
	app->resolution = 72;
	app->ctx = ctx;
	app->dragging_corner = -1;

#ifdef _WIN32
	app->colorspace = fz_device_bgr(ctx);
#else
	app->colorspace = fz_device_rgb(ctx);
#endif
}

void warpapp_setresolution(warpapp_t *app, int res)
{
	app->resolution = res;
}

void warpapp_invert(warpapp_t *app, fz_rect rect)
{
	fz_invert_pixmap_rect(app->ctx, app->image, fz_round_rect(rect));
}

void warpapp_reloadfile(warpapp_t *app)
{
	char filename[PATH_MAX];
	fz_strlcpy(filename, app->docpath, PATH_MAX);
	warpapp_close(app);
	warpapp_open(app, filename, 1);
}

static void event_cb(fz_context *ctx, pdf_document *doc, pdf_doc_event *event, void *data)
{
	warpapp_t *app = (warpapp_t *)data;

	switch (event->type)
	{
	case PDF_DOCUMENT_EVENT_ALERT:
		{
			pdf_alert_event *alert = pdf_access_alert_event(ctx, event);
			winalert(app, alert);
		}
		break;

	case PDF_DOCUMENT_EVENT_PRINT:
		winprint(app);
		break;

	case PDF_DOCUMENT_EVENT_EXEC_MENU_ITEM:
		{
			const char *item = pdf_access_exec_menu_item_event(ctx, event);

			if (!strcmp(item, "Print"))
				winprint(app);
			else
				warpapp_warn(app, "The document attempted to execute menu item: %s. (Not supported)", item);
		}
		break;

	case PDF_DOCUMENT_EVENT_LAUNCH_URL:
		{
			pdf_launch_url_event *launch_url = pdf_access_launch_url_event(ctx, event);

			warpapp_warn(app, "The document attempted to open url: %s. (Not supported by app)", launch_url->url);
		}
		break;

	case PDF_DOCUMENT_EVENT_MAIL_DOC:
		{
			pdf_mail_doc_event *mail_doc = pdf_access_mail_doc_event(ctx, event);

			warpapp_warn(app, "The document attempted to mail the document%s%s%s%s%s%s%s%s (Not supported)",
				mail_doc->to[0]?", To: ":"", mail_doc->to,
				mail_doc->cc[0]?", Cc: ":"", mail_doc->cc,
				mail_doc->bcc[0]?", Bcc: ":"", mail_doc->bcc,
				mail_doc->subject[0]?", Subject: ":"", mail_doc->subject);
		}
		break;
	}
}

#ifdef HAVE_CURL
static void
warpapp_more_data(void *app_, int complete)
{
	warpapp_t *app = (warpapp_t *)app_;
	if (complete && app->outline_deferred == PDFAPP_OUTLINE_DEFERRED)
	{
		app->outline_deferred = PDFAPP_OUTLINE_LOAD_NOW;
		winreloadpage(app);
	}
	else if (app->incomplete)
		winreloadpage(app);
}
#endif

void warpapp_open(warpapp_t *app, char *filename, int reload)
{
	fz_context *ctx = app->ctx;

	fz_try(ctx)
	{
		fz_register_document_handlers(ctx);

		app->doc = fz_open_document(ctx, filename);
	}
	fz_catch(ctx)
	{
		if (!reload)
			warpapp_error(app, "cannot open document");
	}

	fz_try(ctx)
	{
		app->docpath = fz_strdup(ctx, filename);
		app->doctitle = filename;
		if (strrchr(app->doctitle, '\\'))
			app->doctitle = strrchr(app->doctitle, '\\') + 1;
		if (strrchr(app->doctitle, '/'))
			app->doctitle = strrchr(app->doctitle, '/') + 1;
		app->doctitle = fz_strdup(ctx, app->doctitle);
	}
	fz_catch(ctx)
	{
		warpapp_error(app, "cannot open document");
	}

	if (!reload)
	{
	}

	warpapp_showpage(app, 1, 1, 1);
}

void warpapp_close(warpapp_t *app)
{
	fz_free(app->ctx, app->docpath);
	app->docpath = NULL;

	fz_drop_pixmap(app->ctx, app->image);
	app->image = NULL;

	fz_drop_page(app->ctx, app->page);
	app->page = NULL;

	fz_drop_document(app->ctx, app->doc);
	app->doc = NULL;

#ifdef HAVE_CURL
	fz_drop_stream(app->ctx, app->stream);
#endif

	fz_flush_warnings(app->ctx);
}

static int gen_tmp_file(char *buf, int len)
{
	int i;
	char *name = strrchr(buf, '/');

	if (name == NULL)
		name = strrchr(buf, '\\');

	if (name != NULL)
		name++;
	else
		name = buf;

	for (i = 0; i < 10000; i++)
	{
		FILE *f;
		sprintf(name, "tmp%04d", i);
		f = fopen(buf, "r");
		if (f == NULL)
			return 1;
		fclose(f);
	}

	return 0;
}

static void
do_warp(warpapp_t *app)
{
	int w = app->page_bbox.x1 - app->page_bbox.x0;
	int h = app->page_bbox.y1 - app->page_bbox.y0;

	if (w > h)
	{
		app->resolution = 72.0 * MAX_WIN_DIM / w;
		h = MAX_WIN_DIM * h / w;
		w = MAX_WIN_DIM;
	}
	else
	{
		app->resolution = 72.0 * MAX_WIN_DIM / h;
		w = MAX_WIN_DIM * w / h;
		h = MAX_WIN_DIM;
	}
	winresize(app, w, h);

	app->warped_pix = fz_autowarp_pixmap(app->ctx, app->full_pix, app->doc_corners);
	app->disp_warped_pix = fz_scale_pixmap(app->ctx, app->warped_pix, 0, 0, w, h, NULL);
}

static int warpapp_save(warpapp_t *app)
{
	char buf[PATH_MAX];

	if (app->warped_pix == NULL)
		do_warp(app);

	if (wingetsavepath(app, buf, PATH_MAX))
	{
		fz_save_pixmap_as_png(app->ctx, app->warped_pix, buf);
	}

	return 0;
}

int warpapp_preclose(warpapp_t *app)
{
	pdf_document *idoc = pdf_specifics(app->ctx, app->doc);

	if (idoc && pdf_has_unsaved_changes(app->ctx, idoc))
	{
		switch (winsavequery(app))
		{
		case DISCARD:
			return 1;

		case CANCEL:
			return 0;

		case SAVE:
			return warpapp_save(app);
		}
	}

	return 1;
}

static void warpapp_viewctm(fz_matrix *mat, warpapp_t *app)
{
	*mat = fz_transform_page(app->page_bbox, app->resolution, 0);
}

static void warpapp_panview(warpapp_t *app, int newx, int newy)
{
}

static void sanitize_corners(fz_point *c, int w, int h)
{
	int i;
	float t;

	for (i = 0; i < 4; i++)
	{
		if (c[i].x < 0)
			c[i].x = 0;
		else if (c[i].x >= w)
			c[i].x = w-1;
		if (c[i].y < 0)
			c[i].y = 0;
		else if (c[i].y >= h)
			c[i].y = h-1;
	}

	for (i = 0; i < 4; i++)
	{
		t = c[0].y;
		if (t < c[1].y)
			t = c[1].y;
		if (t <= c[2].y && t <= c[3].y)
		{
			/* 0 and 1 are the top ones. */
			break;
		}
		/* Rotate everything by 1. */
		t = c[0].y;
		c[0].y = c[1].y;
		c[1].y = c[2].y;
		c[2].y = c[3].y;
		c[3].y = t;
		t = c[0].x;
		c[0].x = c[1].x;
		c[1].x = c[2].x;
		c[2].x = c[3].x;
		c[3].x = t;
	}

	if (c[0].x > c[1].x)
	{
		/* Flip */
		t = c[0].x; c[0].x = c[1].x; c[1].x = t;
		t = c[0].y; c[0].y = c[1].y; c[1].y = t;
		t = c[2].x; c[2].x = c[3].x; c[3].x = t;
		t = c[2].y; c[2].y = c[3].y; c[3].y = t;
	}
}

static void warpapp_loadpage(warpapp_t *app)
{
	int w, h;

	fz_drop_pixmap(app->ctx, app->full_pix);
	app->full_pix = NULL;
	fz_drop_pixmap(app->ctx, app->disp_pix);
	app->disp_pix = NULL;
	fz_drop_page(app->ctx, app->page);

	app->page = NULL;
	app->page_bbox.x0 = 0;
	app->page_bbox.y0 = 0;
	app->page_bbox.x1 = 100;
	app->page_bbox.y1 = 100;

	fz_try(app->ctx)
	{
		app->page = fz_load_page(app->ctx, app->doc, 0);
		app->page_bbox = fz_bound_page(app->ctx, app->page);
	}
	fz_catch(app->ctx)
	{
		warpapp_warn(app, "Failed to load page.");
		return;
	}

	w = app->page_bbox.x1 - app->page_bbox.x0;
	h = app->page_bbox.y1 - app->page_bbox.y0;

	if (w > h)
	{
		app->resolution = 72.0 * MAX_WIN_DIM / w;
		h = MAX_WIN_DIM * h / w;
		w = MAX_WIN_DIM;
	}
	else
	{
		app->resolution = 72.0 * MAX_WIN_DIM / h;
		w = MAX_WIN_DIM * w / h;
		h = MAX_WIN_DIM;
	}
	winresize(app, w, h);

	fz_enable_icc(app->ctx);

	fz_try(app->ctx)
	{
		fz_matrix mat = { 96.0/72, 0, 0, 96.0/72, 0, 0 };
		app->full_pix = fz_new_pixmap_from_page(app->ctx, app->page, mat, fz_device_rgb(app->ctx), 0);
		app->disp_pix = fz_scale_pixmap(app->ctx, app->full_pix, 0, 0, w, h, NULL);
	}
	fz_always(app->ctx)
	{
	}
	fz_catch(app->ctx)
	{
		warpapp_warn(app, "Failed to load page.");
	}

	/* Do the document detection */
	if (fz_detect_document(app->ctx, app->doc_corners, app->full_pix))
	{
		sanitize_corners(app->doc_corners, app->full_pix->w, app->full_pix->h);
	}
	else
	{
		app->doc_corners[0].x = 0;
		app->doc_corners[0].y = 0;
		app->doc_corners[1].x = app->full_pix->w-1;
		app->doc_corners[1].y = 0;
		app->doc_corners[2].x = app->full_pix->w-1;
		app->doc_corners[2].y = app->full_pix->h-1;
		app->doc_corners[3].x = 0;
		app->doc_corners[3].y = app->full_pix->h-1;
	}
}

static void warpapp_runpage(warpapp_t *app, fz_device *dev, const fz_matrix ctm, fz_rect scissor, fz_cookie *cookie)
{
	fz_point points[4];
	int i;
	fz_path *path;
	fz_stroke_state *stroke;
	float blue[3] = { 0, 0, 1 };
	fz_color_params params = { 0 };

	for (i = 0; i < 4; i++)
	{
		points[i].x = app->doc_corners[i].x * app->disp_pix->w / app->full_pix->w;
		points[i].y = app->doc_corners[i].y * app->disp_pix->h / app->full_pix->h;
	}
	path = fz_new_path(app->ctx);
	fz_moveto(app->ctx, path, points[0].x, points[0].y);
	for (i = 1; i < 4; i++)
		fz_lineto(app->ctx, path, points[i].x, points[i].y);
	fz_closepath(app->ctx, path);
	stroke = fz_new_stroke_state(app->ctx);
	stroke->linewidth = 2;
	fz_stroke_path(app->ctx, dev, path, stroke, fz_identity, fz_device_rgb(app->ctx), blue, 1, params);
	fz_drop_stroke_state(app->ctx, stroke);
	fz_drop_path(app->ctx, path);
}

#define MAX_TITLE 256

void warpapp_reloadpage(warpapp_t *app)
{
	warpapp_showpage(app, 1, 1, 1);
}

static void warpapp_showpage(warpapp_t *app, int loadpage, int drawpage, int repaint)
{
	char buf[MAX_TITLE];
	fz_device *idev = NULL;
	fz_colorspace *colorspace;
	fz_matrix ctm;
	fz_rect bounds;
	fz_irect ibounds;
	fz_cookie cookie = { 0 };

	//if (!app->nowaitcursor)
	//	wincursor(app, WAIT);

	if (loadpage)
	{
		warpapp_loadpage(app);
	}

	if (drawpage)
	{
		char buf2[64];
		size_t len;

		sprintf(buf2, " - (%g dpi)",
				app->resolution);
		len = MAX_TITLE-strlen(buf2);
		if (strlen(app->doctitle) > len)
		{
			fz_strlcpy(buf, app->doctitle, len-3);
			fz_strlcat(buf, "...", MAX_TITLE);
			fz_strlcat(buf, buf2, MAX_TITLE);
		}
		else
			sprintf(buf, "%s%s", app->doctitle, buf2);
		wintitle(app, buf);

		warpapp_viewctm(&ctm, app);
		bounds = fz_transform_rect(app->page_bbox, ctm);
		ibounds = fz_round_rect(bounds);
		bounds = fz_rect_from_irect(ibounds);

		/* Draw */
		fz_drop_pixmap(app->ctx, app->image);
		if (app->grayscale)
			colorspace = fz_device_gray(app->ctx);
		else
			colorspace = app->colorspace;

		app->image = NULL;
		app->imgw = 0;
		app->imgh = 0;

		fz_var(app->image);
		fz_var(idev);

		fz_try(app->ctx)
		{
			fz_irect rect = { 0, 0, app->disp_pix->w, app->disp_pix->h };
			fz_pixmap *display = app->disp_warped_pix ? app->disp_warped_pix : app->disp_pix;

			app->image = fz_new_pixmap_with_bbox(app->ctx, colorspace, ibounds, NULL, 1);
			app->imgw = fz_pixmap_width(app->ctx, app->image);
			app->imgh = fz_pixmap_height(app->ctx, app->image);

			fz_copy_pixmap_rect(app->ctx, app->image, display, rect, NULL);

			if (app->disp_warped_pix == NULL)
			{
				idev = fz_new_draw_device(app->ctx, fz_identity, app->image);
				warpapp_runpage(app, idev, ctm, bounds, &cookie);
				fz_close_device(app->ctx, idev);
			}
		}
		fz_always(app->ctx)
			fz_drop_device(app->ctx, idev);
		fz_catch(app->ctx)
			cookie.d.errors++;
	}

	if (repaint)
	{
		winrepaint(app);

		wincursor(app, ARROW);
	}

	fz_flush_warnings(app->ctx);
}

static void warpapp_gotouri(warpapp_t *app, char *uri)
{
	char buf[PATH_MAX];

	/* Relative file:// URI, make it absolute! */
	if (!strncmp(uri, "file://", 7) && uri[7] != '/')
	{
		char buf_base[PATH_MAX];
		char buf_cwd[PATH_MAX];
		fz_dirname(buf_base, app->docpath, sizeof buf_base);
		getcwd(buf_cwd, sizeof buf_cwd);
		fz_snprintf(buf, sizeof buf, "file://%s/%s/%s", buf_cwd, buf_base, uri+7);
		fz_cleanname(buf+7);
		uri = buf;
	}

	winopenuri(app, uri);
}

void warpapp_gotopage(warpapp_t *app, int number)
{
	warpapp_showpage(app, 1, 1, 1);
}

void warpapp_inverthit(warpapp_t *app)
{
}

static void warpapp_search_in_direction(warpapp_t *app, enum panning *panto, int dir)
{
}

void warpapp_onresize(warpapp_t *app, int w, int h)
{
	if (app->winw != w || app->winh != h)
	{
		app->winw = w;
		app->winh = h;
		winrepaint(app);
	}
}

void warpapp_onkey(warpapp_t *app, int c, int modifiers)
{
	int loadpage = 1;

	switch (c)
	{
	case 'q':
		winclose(app);
		break;

	case 27:
		if (app->disp_warped_pix)
		{
			fz_drop_pixmap(app->ctx, app->disp_warped_pix);
			app->disp_warped_pix = NULL;
			fz_drop_pixmap(app->ctx, app->warped_pix);
			app->warped_pix = NULL;
			warpapp_showpage(app, 0, 1, 1);
		}
		break;

	case 10:
	case 13:

		if (app->disp_warped_pix == NULL)
		{
			do_warp(app);
			warpapp_showpage(app, 0, 1, 1);
		}
		else
			warpapp_save(app);
		break;

	}
}

static void handlescroll(warpapp_t *app, int modifiers, int dir)
{
}

void warpapp_onmouse(warpapp_t *app, int x, int y, int btn, int modifiers, int state)
{
	fz_context *ctx = app->ctx;
	fz_irect irect;
	fz_matrix ctm;
	fz_point p;
	int i, closest;
	float closest_x, closest_y;
	int repaint = 0;

	irect = fz_pixmap_bbox(app->ctx, app->image);
	p.x = x + irect.x0;
	p.y = y + irect.y0;

	warpapp_viewctm(&ctm, app);
	ctm = fz_invert_matrix(ctm);

	p = fz_transform_point(p, ctm);

	closest = app->dragging_corner;
	if (closest == -1)
	{
		closest_x = 99999999.f;
		closest_y = 99999999.f;
		for (i = 0; i < 4; i++)
		{
			float x = app->doc_corners[i].x - p.x;
			float y = app->doc_corners[i].y - p.y;

			x = fz_abs(x);
			y = fz_abs(y);
			if (x < 30 && y < 30)
			{
				if (x <= closest_x && y <= closest_y)
					closest = i, closest_x = x, closest_y = y;
			}
		}
	}

	if (closest >= 0)
	{
		wincursor(app, HAND);
		switch (state)
		{
		case 1:
			/* Mouse initial down */
			app->dragging_corner = closest;
			repaint |= app->doc_corners[closest].x != p.x;
			repaint |= app->doc_corners[closest].y != p.y;
			app->doc_corners[closest].x = p.x;
			app->doc_corners[closest].y = p.y;
			if (repaint)
				warpapp_showpage(app, 0, 1, 1);
			return;
		case -1:
			app->dragging_corner = -1;
			break;
		case 0:
			if (app->dragging_corner >= 0)
			{
				repaint |= app->doc_corners[closest].x != p.x;
				repaint |= app->doc_corners[closest].y != p.y;
				app->doc_corners[closest].x = p.x;
				app->doc_corners[closest].y = p.y;
				if (repaint)
					warpapp_showpage(app, 0, 1, 1);
			}
			break;
		}
	}
	else
	{
		wincursor(app, ARROW);
	}

	if (state == 1)
	{
		if (btn == 4 || btn == 5) /* scroll wheel */
		{
			handlescroll(app, modifiers, btn == 4 ? 1 : -1);
		}
		if (btn == 6 || btn == 7) /* scroll wheel (horizontal) */
		{
			/* scroll left/right or up/down if shift is pressed */
			handlescroll(app, modifiers ^ (1<<0), btn == 6 ? 1 : -1);
		}
	}

	else if (state == -1)
	{
	}
}

void warpapp_oncopy(warpapp_t *app, unsigned short *ucsbuf, int ucslen)
{
}

void warpapp_postblit(warpapp_t *app)
{
}
