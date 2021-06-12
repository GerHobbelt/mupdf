/*
 * mudraw -- command line tool for drawing and converting documents
 */

#include "timeval.h"

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"

#if FZ_ENABLE_PDF
#include "mupdf/pdf.h" /* for pdf output */
#endif

#ifndef DISABLE_MUTHREADS
#include "mupdf/helpers/mu-threads.h"
#include "mupdf/helpers/cpu.h"
#endif

#include "../fitz/tessocr.h"

#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <direct.h> /* for getcwd */
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif
#else
#include <unistd.h> /* for getcwd */
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Allow for windows stdout being made binary */
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#if !defined(HAVE_LEPTONICA) || !defined(HAVE_TESSERACT)
#ifndef OCR_DISABLED
#define OCR_DISABLED
#endif
#endif

/* Enable for helpful threading debug */
#if 01
#define DEBUG_THREADS(code) do { code; } while (0)
#else
#define DEBUG_THREADS(code) do { } while (0)
#endif

enum {
	OUT_NONE,
	OUT_BBOX,
	OUT_HTML,
	OUT_OCR_HTML,
	OUT_OCR_PDF,
	OUT_OCR_STEXT_JSON,
	OUT_OCR_STEXT_XML,
	OUT_OCR_TEXT,
	OUT_OCR_TRACE,
	OUT_OCR_XHTML,
	OUT_OCR_XMLTEXT,
	OUT_PAM,
	OUT_PBM,
	OUT_PCL,
	OUT_PCLM,
	OUT_PGM,
	OUT_PKM,
	OUT_PNG,
	OUT_PNM,
	OUT_PPM,
	OUT_PS,
	OUT_PSD,
	OUT_PWG,
	OUT_STEXT_JSON,
	OUT_STEXT_XML,
	OUT_SVG,
	OUT_TEXT,
	OUT_TRACE,
	OUT_XHTML,
	OUT_XMLTEXT,
	OUT_PDF,
};

enum { CS_INVALID, CS_UNSET, CS_MONO, CS_GRAY, CS_GRAY_ALPHA, CS_RGB, CS_RGB_ALPHA, CS_CMYK, CS_CMYK_ALPHA, CS_ICC };

enum { SPOTS_NONE, SPOTS_OVERPRINT_SIM, SPOTS_FULL };

typedef struct
{
	const char *suffix;
	int format;
	int spots;
} suffix_t;

static const suffix_t suffix_table[] =
{
	/* All the 'triple extension' ones must go first. */
	{ ".ocr.stext.json", OUT_OCR_STEXT_JSON, 0 },

	/* All the 'double extension' ones must go next. */
	{ ".ocr.txt", OUT_OCR_TEXT, 0 },
	{ ".ocr.text", OUT_OCR_TEXT, 0 },
	{ ".ocr.html", OUT_OCR_HTML, 0 },
	{ ".ocr.xhtml", OUT_OCR_XHTML, 0 },
	{ ".ocr.stext", OUT_OCR_STEXT_XML, 0 },
	{ ".ocr.xmltext", OUT_OCR_XMLTEXT, 0 },
	{ ".ocr.xml", OUT_OCR_XMLTEXT, 0 },
	{ ".ocr.pdf", OUT_OCR_PDF, 0 },
	{ ".ocr.trace", OUT_OCR_TRACE, 0 },
	{ ".stext.json", OUT_STEXT_JSON, 0 },

	/* And the 'single extension' ones go last. */
	{ ".png", OUT_PNG, 0 },
	{ ".pgm", OUT_PGM, 0 },
	{ ".ppm", OUT_PPM, 0 },
	{ ".pnm", OUT_PNM, 0 },
	{ ".pam", OUT_PAM, 0 },
	{ ".pbm", OUT_PBM, 0 },
	{ ".pkm", OUT_PKM, 0 },
	{ ".svg", OUT_SVG, 0 },
	{ ".pwg", OUT_PWG, 0 },
	{ ".pclm", OUT_PCLM, 0 },
	{ ".pcl", OUT_PCL, 0 },
	{ ".pdf", OUT_PDF, 0 },
	{ ".psd", OUT_PSD, 1 },
	{ ".ps", OUT_PS, 0 },

	{ ".txt", OUT_TEXT, 0 },
	{ ".text", OUT_TEXT, 0 },
	{ ".html", OUT_HTML, 0 },
	{ ".xhtml", OUT_XHTML, 0 },
	{ ".stext", OUT_STEXT_XML, 0 },

	{ ".trace", OUT_TRACE, 0 },
	{ ".xmltext", OUT_XMLTEXT, 0 },
	{ ".xml", OUT_XMLTEXT, 0 },
	{ ".bbox", OUT_BBOX, 0 },
};

typedef struct
{
	const char *name;
	int colorspace;
} cs_name_t;

static const cs_name_t cs_name_table[] =
{
	{ "m", CS_MONO },
	{ "mono", CS_MONO },
	{ "g", CS_GRAY },
	{ "gray", CS_GRAY },
	{ "grey", CS_GRAY },
	{ "ga", CS_GRAY_ALPHA },
	{ "grayalpha", CS_GRAY_ALPHA },
	{ "greyalpha", CS_GRAY_ALPHA },
	{ "rgb", CS_RGB },
	{ "rgba", CS_RGB_ALPHA },
	{ "rgbalpha", CS_RGB_ALPHA },
	{ "cmyk", CS_CMYK },
	{ "cmyka", CS_CMYK_ALPHA },
	{ "cmykalpha", CS_CMYK_ALPHA },
};

typedef struct
{
	int format;
	int default_cs;
	int permitted_cs[7];
} format_cs_table_t;

static const format_cs_table_t format_cs_table[] =
{
	{ OUT_PNG, CS_RGB, { CS_GRAY, CS_GRAY_ALPHA, CS_RGB, CS_RGB_ALPHA, CS_ICC } },
	{ OUT_PPM, CS_RGB, { CS_GRAY, CS_RGB } },
	{ OUT_PNM, CS_GRAY, { CS_GRAY, CS_RGB } },
	{ OUT_PAM, CS_RGB_ALPHA, { CS_GRAY, CS_GRAY_ALPHA, CS_RGB, CS_RGB_ALPHA, CS_CMYK, CS_CMYK_ALPHA } },
	{ OUT_PGM, CS_GRAY, { CS_GRAY, CS_RGB } },
	{ OUT_PBM, CS_MONO, { CS_MONO } },
	{ OUT_PKM, CS_CMYK, { CS_CMYK } },
	{ OUT_PWG, CS_RGB, { CS_MONO, CS_GRAY, CS_RGB, CS_CMYK } },
	{ OUT_PCL, CS_MONO, { CS_MONO, CS_RGB } },
	{ OUT_PCLM, CS_RGB, { CS_RGB, CS_GRAY } },
	{ OUT_PS, CS_RGB, { CS_GRAY, CS_RGB, CS_CMYK } },
	{ OUT_PSD, CS_CMYK, { CS_GRAY, CS_GRAY_ALPHA, CS_RGB, CS_RGB_ALPHA, CS_CMYK, CS_CMYK_ALPHA, CS_ICC } },

	{ OUT_TRACE, CS_RGB, { CS_RGB } },
	{ OUT_XMLTEXT, CS_RGB, { CS_RGB } },
	{ OUT_BBOX, CS_RGB, { CS_RGB } },
	{ OUT_SVG, CS_RGB, { CS_RGB } },
	{ OUT_OCR_PDF, CS_RGB, { CS_RGB, CS_GRAY } },
	{ OUT_PDF, CS_RGB, { CS_RGB } },

	{ OUT_TEXT, CS_RGB, { CS_RGB } },
	{ OUT_HTML, CS_RGB, { CS_RGB } },
	{ OUT_XHTML, CS_RGB, { CS_RGB } },
	{ OUT_STEXT_XML, CS_RGB, { CS_RGB } },
	{ OUT_STEXT_JSON, CS_RGB, { CS_RGB } },
	{ OUT_OCR_TEXT, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_HTML, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_XHTML, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_STEXT_XML, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_XMLTEXT, CS_RGB, { CS_RGB } },
	{ OUT_OCR_STEXT_JSON, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_TRACE, CS_GRAY, { CS_GRAY } },
};

static fz_stext_options stext_options = { 0 };

static fz_cookie master_cookie = { 0 };

static time_t
stat_mtime(const char *path)
{
	struct_stat info;

	if (stat(path, &info) < 0)
		return 0;

	return info.st_mtime;
}

/*
	In the presence of pthreads or Windows threads, we can offer
	a multi-threaded option. In the absence of such we degrade
	nicely.
*/
#ifndef DISABLE_MUTHREADS

static mu_mutex mutexes[FZ_LOCK_MAX];

static void mudraw_lock(void *user, int lock)
{
	mu_lock_mutex(&mutexes[lock]);
}

static void mudraw_unlock(void *user, int lock)
{
	mu_unlock_mutex(&mutexes[lock]);
}

static fz_locks_context mudraw_locks =
{
	NULL, mudraw_lock, mudraw_unlock
};

static void fin_mudraw_locks(void)
{
	int i;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		mu_destroy_mutex(&mutexes[i]);
}

static fz_locks_context *init_mudraw_locks(void)
{
	int i;
	int failed = 0;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		failed |= mu_create_mutex(&mutexes[i]);

	if (failed)
	{
		fin_mudraw_locks();
		return NULL;
	}

	return &mudraw_locks;
}

#endif

typedef struct worker_t {
	fz_context *ctx;
	int num;
	int band; /* -1 to shutdown, or band to render */
	int error;
	int running; /* set to 1 by main thread when it thinks the worker is running, 0 when it thinks it is not running */
	fz_display_list *list;
	fz_matrix ctm;
	fz_rect tbounds;
	fz_pixmap *pix;
	fz_bitmap *bit;
	fz_cookie cookie;
#ifndef DISABLE_MUTHREADS
	mu_semaphore start;
	mu_semaphore stop;
	mu_thread thread;
#endif
} worker_t;

static int mudraw_is_toplevel_ctx = 0;

static fz_context *ctx = NULL;
static const char *output = NULL;
static fz_output *out = NULL;
static int output_pagenum = 0;
static int output_file_per_page = 0;

static const char *format = NULL;
static int output_format = OUT_NONE;

static int rotation = -1;
static float resolution = 72;
static int res_specified = 0;
static int width = 0;
static int height = 0;
static int fit = 0;

static float layout_w = FZ_DEFAULT_LAYOUT_W;
static float layout_h = FZ_DEFAULT_LAYOUT_H;
static float layout_em = FZ_DEFAULT_LAYOUT_EM;
static const char *layout_css = NULL;
static int layout_use_doc_css = 1;
static float min_line_width = 0.0f;

static int showfeatures = 0;
static int showtime = 0;
static int showmemory = 0;
static int showmd5 = 0;

#if FZ_ENABLE_PDF
static pdf_document *pdfout = NULL;
static const char *pdfoutpath = NULL;
#endif

static int no_icc = 0;
static int ignore_errors = 0;
static int uselist = 1;
static int alphabits_text = 8;
static int alphabits_graphics = 8;

static int out_cs = CS_UNSET;
static const char *proof_filename = NULL;
fz_colorspace *proof_cs = NULL;
static const char *icc_filename = NULL;
static float gamma_value = 1;
static int invert = 0;
static int band_height = 0;
static int lowmemory = 0;

static int quiet = 0;
static int errored = 0;
static fz_colorspace *colorspace = NULL;
static fz_colorspace *oi = NULL;
#if FZ_ENABLE_SPOT_RENDERING
static int spots = SPOTS_OVERPRINT_SIM;
#else
static int spots = SPOTS_NONE;
#endif
static int alpha;
static int useaccel = 1;
static const char *filename;
static int files = 0;
static int num_workers = 0;
static worker_t *workers = NULL;
static fz_band_writer *bander = NULL;

static const char *layer_config = NULL;

static const char ocr_language_default[] = "eng";
static const char *ocr_language = ocr_language_default;

static struct {
	int active;
	int started;
	fz_context *ctx;
#ifndef DISABLE_MUTHREADS
	mu_thread thread;
	mu_semaphore start;
	mu_semaphore stop;
#endif
	int pagenum;
	int error;
	const char *filename;
	fz_display_list *list;
	fz_page *page;
	int interptime;
	fz_separations *seps;
} bgprint = { 0 };

static struct {
	int count, total;
	int min, max;
	int mininterp, maxinterp;
	int minpage, maxpage;
	const char *minfilename;
	const char *maxfilename;
	int layout;
	int minlayout, maxlayout;
	const char *minlayoutfilename;
	const char *maxlayoutfilename;
} timing = { 0 };

static int usage(void)
{
	// create a nice list of the annotation types for use in the usage help message:
	char annot_type_list[500] = "";
	size_t buflen = sizeof(annot_type_list);
	char* dst = annot_type_list;
	size_t width = 0;

	for (int i = PDF_ANNOT_TEXT; ; i++)
	{
		const char *name = pdf_string_from_annot_type(ctx, i);
		if (!strcmp(name, "UNKNOWN"))
			break;
		if (width + strlen(name) > 70)
		{
			size_t extra_len = fz_snprintf(dst, buflen, "\n        ");
			width = 0;
			dst += extra_len;
			buflen -= extra_len;
		}
		size_t nlen = fz_snprintf(dst, buflen, "%s,", name);
		width += nlen;
		dst += nlen;
		buflen -= nlen;
	}
	dst[-1] = 0;   // nuke that last ','

	fz_info(ctx,
		"mudraw version " FZ_VERSION "\n"
		"Usage: mudraw [options] file [pages]\n"
		"  -p -  password\n"
		"\n"
		"  -o -  output file name (%%d or ### for page number, '-' for stdout)\n"
		"  -F -  output format (default inferred from output file name)\n"
		"    raster: png, pnm, pam, pbm, pkm, pwg, pcl, ps\n"
		"    vector: svg, pdf, trace, ocr.trace\n"
		"    text: txt, html, xhtml, stext, stext.json\n"
#ifndef OCR_DISABLED
		"    ocr'd text: ocr.txt, ocr.html, ocr.xhtml, ocr.stext, ocr.stext.json\n"
#else
		"    ocr'd text: ocr.txt, ocr.html, ocr.xhtml, ocr.stext, ocr.stext.json (disabled)\n"
#endif
		"    bitmap-wrapped-as-pdf: pclm, ocr.pdf\n"
		"\n"
		"  -q    be quiet (don't print progress messages)\n"
		"  -s -  show extra information:\n"
		"    m - show memory use\n"
		"    t - show timings\n"
		"    f - show page features\n"
		"    5 - show md5 checksum of rendered image\n"
		"\n"
		"  -R {auto,0,90,180,270}\n"
		"        rotate clockwise (default: auto)\n"
		"  -r -  resolution in dpi (default: 72)\n"
		"  -w -  page width (in pixels) (maximum width if -r is specified)\n"
		"  -h -  page height (in pixels) (maximum height if -r is specified)\n"
		"  -f    fit file to page if too large\n"
		"  -B -  maximum band height (pXm, pcl, pclm, ocr.pdf, ps, psd and png output\n"
		"        only)\n"
#ifndef DISABLE_MUTHREADS
		"  -T -  number of threads to use for rendering (banded mode only)\n"
#else
		"  -T -  number of threads to use for rendering (disabled in this non-threading\n"
		"        build)\n"
#endif
		"\n"
		"  -W -  page width for EPUB layout\n"
		"  -H -  page height for EPUB layout\n"
		"  -S -  font size for EPUB layout\n"
		"  -U -  file name of user stylesheet for EPUB layout\n"
		"  -X    disable document styles for EPUB layout\n"
		"  -a    disable usage of accelerator file\n"
		"  -x -  text extract options:\n"
		"     preserve-ligatures,preserve-whitespace,inhibit-spaces,dehyphenate\n"
		"\n"
		"  -c -  colorspace (mono, gray, grayalpha, rgb, rgba, cmyk, cmykalpha,\n"
		"        filename of ICC profile)\n"
		"  -e -  proof icc profile (filename of ICC profile)\n"
		"  -G -  apply gamma correction\n"
		"  -I    invert colors\n"
		"\n"
		"  -A -  number of bits of anti-aliasing (0 to 8)\n"
		"  -A -/-  number of bits of anti-aliasing (0 to 8) (graphics, text)\n"
		"  -l -  minimum stroked line width (in pixels)\n"
		"  -D    disable use of display list\n"
		"  -j -  render only selected types of content. Use a comma-separated list\n"
		"        to combine types (everything,content,annotations,widgets,Unknown,\n"
		"        %s)\n"
		"  -J -  set PNG output compression level: 0 (none), 1 (fast)..9 (best)\n"
		"  -i    ignore errors\n"
		"  -L    low memory mode (avoid caching, clear objects after each page)\n"
#ifndef DISABLE_MUTHREADS
		"  -P    parallel interpretation/rendering\n"
#else
		"  -P    parallel interpretation/rendering (disabled in this non-threading build)\n"
#endif
		"  -N    disable ICC workflow (\"N\"o color management)\n"
		"  -O -  Control spot/overprint rendering\n"
#if FZ_ENABLE_SPOT_RENDERING
		"     0 = No spot rendering\n"
		"     1 = Overprint simulation (default)\n"
		"     2 = Full spot rendering\n"
#else
		"     0 = No spot rendering (default)\n"
		"     1 = Overprint simulation (Disabled in this build)\n"
		"     2 = Full spot rendering (Disabled in this build)\n"
#endif
#ifndef OCR_DISABLED
		"  -t -  Specify language/script for OCR (default: eng)\n"
#else
		"  -t -  Specify language/script for OCR (default: eng) (disabled)\n"
#endif
		"\n"
		"  -y l  List the layer configs to stderr\n"
		"  -y -  Select layer config (by number)\n"
		"  -y -{,-}*  Select layer config (by number), and toggle the listed entries\n"
		"\n"
		"  -m -  specify custom memory limits:\n"
		"    sNNN   set memory limit to NNN bytes: the application will not be allowed\n"
		"           to consume more that NNN bytes heap memory at any time\n"
		"    aNNN   set alloc limit to NNN: the application will not execute more than\n"
		"           NNN heap allocation requests.\n"
		"    NNN    set memory limit to NNN bytes (same as 'sNNN' above)\n"
		"\n"
		"  -v    display the version of this application and terminate\n"
		"\n"
		"  pages  comma separated list of page numbers and ranges\n",
		annot_type_list
	);
	
	return EXIT_FAILURE;
}

static int gettime_once = 1;

static int gettime(void)
{
	static struct curltime first;
	struct curltime now;
	if (gettime_once)
	{
		first = Curl_now();
		gettime_once = 0;
	}
	now = Curl_now();
	return Curl_timediff(now, first);
}

static int page_counter = 0;

static int inc_page_counter(void) {
	return page_counter++;
}
static void reset_page_counter(void) {
	page_counter = 0;
}

/* Output file level (as opposed to page level) headers */
static void
file_level_headers(fz_context *ctx)
{
	if (output_format == OUT_STEXT_XML || output_format == OUT_OCR_STEXT_XML
		|| output_format == OUT_TRACE || output_format == OUT_OCR_TRACE || output_format == OUT_BBOX
		|| output_format == OUT_XMLTEXT || output_format == OUT_OCR_XMLTEXT)
	{
		fz_write_printf(ctx, out, "<?xml version=\"1.0\"?>\n");
	}

	if (output_format == OUT_HTML || output_format == OUT_OCR_HTML)
		fz_print_stext_header_as_html(ctx, out);
	if (output_format == OUT_XHTML || output_format == OUT_OCR_XHTML)
		fz_print_stext_header_as_xhtml(ctx, out);

	if (output_format == OUT_STEXT_XML || output_format == OUT_OCR_STEXT_XML
		|| output_format == OUT_TRACE || output_format == OUT_OCR_TRACE || output_format == OUT_BBOX)
	{
		fz_write_printf(ctx, out, "<document name=\"%s\">\n", filename);
	}
	if (output_format == OUT_STEXT_JSON || output_format == OUT_OCR_STEXT_JSON)
		fz_write_printf(ctx, out, "{%q:%q,%q:[", "file", filename, "pages");

	if (output_format == OUT_PS)
		fz_write_ps_file_header(ctx, out);

	if (output_format == OUT_PWG)
		fz_write_pwg_file_header(ctx, out);

	if (output_format == OUT_PCLM)
	{
		fz_pclm_options opts = { 0 };
		fz_parse_pclm_options(ctx, &opts, "compression=flate");
		bander = fz_new_pclm_band_writer(ctx, out, &opts);
	}

	if (output_format == OUT_OCR_PDF)
	{
		char options[300];
		fz_pdfocr_options opts = { 0 };
		fz_snprintf(options, sizeof(options), "compression=flate,ocr-language=%s", ocr_language);
		fz_parse_pdfocr_options(ctx, &opts, options);
		bander = fz_new_pdfocr_band_writer(ctx, out, &opts);
	}
}

static void
file_level_trailers(fz_context *ctx)
{
	if (output_format == OUT_STEXT_XML || output_format == OUT_OCR_STEXT_XML
		|| output_format == OUT_TRACE || output_format == OUT_OCR_TRACE || output_format == OUT_BBOX)
	{
		fz_write_printf(ctx, out, "</document>\n");
	}
	if (output_format == OUT_STEXT_JSON || output_format == OUT_OCR_STEXT_JSON)
		fz_write_printf(ctx, out, "]}");

	if (output_format == OUT_HTML || output_format == OUT_OCR_HTML)
		fz_print_stext_trailer_as_html(ctx, out);
	if (output_format == OUT_XHTML || output_format == OUT_OCR_XHTML)
		fz_print_stext_trailer_as_xhtml(ctx, out);

	if (output_format == OUT_PS)
		fz_write_ps_file_trailer(ctx, out, output_pagenum);

	if (output_format == OUT_PCLM || output_format == OUT_OCR_PDF)
	{
		fz_drop_band_writer(ctx, bander);
	}
}

static void drawband(fz_context *ctx, fz_page *page, fz_display_list *list, fz_matrix ctm, fz_rect tbounds, fz_cookie *cookie, int band_start, fz_pixmap *pix, fz_bitmap **bit)
{
	fz_device *dev = NULL;

	fz_var(dev);

	*bit = NULL;

	fz_try(ctx)
	{
		if (pix->alpha)
			fz_clear_pixmap(ctx, pix);
		else
			fz_clear_pixmap_with_value(ctx, pix, 255);

		dev = fz_new_draw_device_with_proof(ctx, fz_identity, pix, proof_cs);
		if (lowmemory)
			fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
		if (alphabits_graphics == 0)
			fz_enable_device_hints(ctx, dev, FZ_DONT_INTERPOLATE_IMAGES);
		if (list)
			fz_run_display_list(ctx, list, dev, ctm, tbounds, cookie);
		else
			fz_run_page(ctx, page, dev, ctm, cookie);
		fz_close_device(ctx, dev);
		fz_drop_device(ctx, dev);
		dev = NULL;

		if (invert)
			fz_invert_pixmap(ctx, pix);
		if (gamma_value != 1)
			fz_gamma_pixmap(ctx, pix, gamma_value);

		if (((output_format == OUT_PCL || output_format == OUT_PWG) && out_cs == CS_MONO) || (output_format == OUT_PBM) || (output_format == OUT_PKM))
			*bit = fz_new_bitmap_from_pixmap_band(ctx, pix, NULL, band_start);
	}
	fz_catch(ctx)
	{
		fz_drop_device(ctx, dev);
		fz_rethrow(ctx);
	}
}

static void calc_page_render_details(fz_context* ctx, fz_page* page, fz_rect mediabox)
{
	/* MuPDF measures in points (72ths of an inch). */

	// has rotation been configured for auto-detect?
	// if so, quickest approach is to pick maximum resolution for rotation=0 or rotation=90
	float best_res = resolution;
	float best_res_override = resolution;
	int best_rot = rotation;
	int rotations[2] = { rotation, 90 };
	int n = 1;
	if (rotation == -1)
	{
		n = 2;
		rotations[0] = 0;
		best_rot = 0;
	}

	// when dpi has been set to zero, recalculate it below.

	if (resolution <= 0)
	{
		best_res = 0;
		best_res_override = 0;

		if (width > 0 || height > 0)
		{
			// when the page is width/height constrained, calculate the maximum potential resolution for the given constraints:
			float pb = fz_min(mediabox.x1 - mediabox.x0, mediabox.y1 - mediabox.y0);
			float uq = fz_max(width, height);
			resolution = 2.0 * 72 * uq / pb;  // overestimate dpi by a factor of 2
		}
		else
		{
			// no constraints known, no resolution specified, hence assume default
			resolution = 72;
		}
	}

	if (width > 0 || height > 0)
	{
		// width and/or height have been specified: REDUCE resolution to fit?
		best_res = 0;
		best_res_override = 0;
	}
	else
	{
		// no width/height constraints have been specified.
		//
		// hence only tweak the rotation, when that one was set to 'auto'
		if (n == 2)
			best_res = 0;
	}

	for (int i = 0; i < n; i++)
	{
		float zoom = resolution / 72;
		fz_matrix ctm = fz_pre_scale(fz_rotate(rotations[i]), zoom, zoom);

		fz_rect tbounds = fz_transform_rect(mediabox, ctm);
		fz_irect ibounds = fz_round_rect(tbounds);

		/* Make local copies of our width/height */
		int w = (width > 0 ? width : ibounds.x1 - ibounds.x0);
		int h = (height > 0 ? height : ibounds.y1 - ibounds.y0);

		float scalex = w / (float)(ibounds.x1 - ibounds.x0);
		float scaley = h / (float)(ibounds.y1 - ibounds.y0);
		fz_matrix scale_mat;

		// get the largest scaling factor which still allows page to fit within the w/h constraints
		if (scalex > scaley)
			scalex = scaley;

		// might NOT want to scale UP to fit snuggly within the constraints
		if (!fit)
			scalex = fz_min(1.0f, scalex);
		scaley = scalex;

		scale_mat = fz_scale(scalex, scaley);
		ctm = fz_concat(ctm, scale_mat);
		tbounds = fz_transform_rect(mediabox, ctm);
		ibounds = fz_round_rect(tbounds);

		// reconstruct dpi from these numbers:
		fz_matrix ctm2 = fz_pre_scale(fz_rotate(rotations[i]), 1.0f, 1.0f);

		fz_rect tbounds2 = fz_transform_rect(mediabox, ctm2);
		fz_irect ibounds2 = fz_round_rect(tbounds2);

		float res = max(72 * (ibounds.x1 - ibounds.x0) / (float)(ibounds2.x1 - ibounds2.x0), 72 * (ibounds.y1 - ibounds.y0) / (float)(ibounds2.y1 - ibounds2.y0));
		// round resolution down to nearest int:
		res = (int)res;
		if (best_res < res)
		{
			best_res = res;
			best_rot = rotations[i];
		}
	}

	rotation = best_rot;
	if (best_res_override > best_res)
		best_res = best_res_override;
	resolution = best_res;
}

static void dodrawpage(fz_context *ctx, fz_page *page, fz_display_list *list, int pagenum, fz_cookie *cookie, int start, int interptime, const char *fname, int bg, fz_separations *seps)
{
	fz_rect mediabox;
	fz_device *dev = NULL;

	fz_var(dev);

	if (output_file_per_page)
		file_level_headers(ctx);

	if (list)
		mediabox = fz_bound_display_list(ctx, list);
	else
		mediabox = fz_bound_page(ctx, page);

	/* Calculate Page resolution & rotation */
	calc_page_render_details(ctx, page, mediabox);

	if (output_format == OUT_TRACE || output_format == OUT_OCR_TRACE)
	{
		float zoom;
		fz_matrix ctm;
		fz_device *pre_ocr_dev = NULL;

		zoom = resolution / 72;
		ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

		fz_var(pre_ocr_dev);

		fz_try(ctx)
		{
			fz_rect mb = fz_transform_rect(mediabox, ctm);

			fz_write_printf(ctx, out, "<page mediabox=\"%R\">\n", &mb);
			dev = fz_new_trace_device(ctx, out);
			if (output_format == OUT_OCR_TRACE)
			{
				pre_ocr_dev = dev;
				dev = NULL;
				dev = fz_new_ocr_device(ctx, pre_ocr_dev, ctm, mediabox, 1, ocr_language, NULL, NULL);
			}
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, fz_infinite_rect, cookie);
			else
				fz_run_page(ctx, page, dev, ctm, cookie);
			fz_close_device(ctx, dev);
			fz_drop_device(ctx, dev);
			dev = NULL;
			fz_close_device(ctx, pre_ocr_dev);
			fz_drop_device(ctx, pre_ocr_dev);
			pre_ocr_dev = NULL;
			fz_write_printf(ctx, out, "</page>\n");
		}
		fz_always(ctx)
		{
			fz_drop_device(ctx, pre_ocr_dev);
			fz_drop_device(ctx, dev);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}

	if (output_format == OUT_XMLTEXT || output_format == OUT_OCR_XMLTEXT)
	{
		fz_device* pre_ocr_dev = NULL;

		fz_try(ctx)
		{
			float zoom;
			fz_matrix ctm;

			zoom = resolution / 72;
			ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

			fz_rect mb = fz_transform_rect(mediabox, ctm);

			fz_write_printf(ctx, out, "<page mediabox=\"%R\">\n", &mb);
			dev = fz_new_xmltext_device(ctx, out);
			if (output_format == OUT_OCR_XMLTEXT)
			{
				pre_ocr_dev = dev;
				dev = NULL;
				dev = fz_new_ocr_device(ctx, pre_ocr_dev, ctm, mediabox, 1, ocr_language, NULL, NULL);
			}
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, fz_infinite_rect, cookie);
			else
				fz_run_page(ctx, page, dev, ctm, cookie);
			fz_write_printf(ctx, out, "</page>\n");
			fz_close_device(ctx, dev);
			fz_drop_device(ctx, dev);
			dev = NULL;
			fz_close_device(ctx, pre_ocr_dev);
			fz_drop_device(ctx, pre_ocr_dev);
			pre_ocr_dev = NULL;
		}
		fz_always(ctx)
		{
			fz_drop_device(ctx, dev);
			fz_drop_device(ctx, pre_ocr_dev);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}

	else if (output_format == OUT_BBOX)
	{
		fz_try(ctx)
		{
			fz_rect bbox = fz_empty_rect;
			dev = fz_new_bbox_device(ctx, &bbox);
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (list)
				fz_run_display_list(ctx, list, dev, fz_identity, fz_infinite_rect, cookie);
			else
				fz_run_page(ctx, page, dev, fz_identity, cookie);
			fz_close_device(ctx, dev);
			fz_write_printf(ctx, out, "<page bbox=\"%R\" mediabox=\"%R\" />\n", &bbox, &mediabox);
		}
		fz_always(ctx)
		{
			fz_drop_device(ctx, dev);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}

	else if (output_format == OUT_TEXT || output_format == OUT_HTML || output_format == OUT_XHTML || output_format == OUT_STEXT_XML || output_format == OUT_STEXT_JSON ||
		output_format == OUT_OCR_TEXT || output_format == OUT_OCR_HTML || output_format == OUT_OCR_XHTML || output_format == OUT_OCR_STEXT_XML || output_format == OUT_OCR_STEXT_JSON)
	{
		fz_stext_page *text = NULL;
		float zoom;
		fz_matrix ctm;
		fz_device *pre_ocr_dev = NULL;

		zoom = resolution / 72;
		ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

		fz_var(text);
		fz_var(pre_ocr_dev);

		fz_try(ctx)
		{
			// Even when OCR fails, we want to see what we got thus far as text,
			// hence we should *postpone* bubbling up the OCR error, if one occurs.
			fz_stext_options page_stext_options = stext_options;

			// override the preserve_images flag:
			if (output_format == OUT_HTML ||
				output_format == OUT_XHTML ||
				output_format == OUT_OCR_HTML ||
				output_format == OUT_OCR_XHTML
				)
				page_stext_options.flags |= FZ_STEXT_PRESERVE_IMAGES;
			else
				page_stext_options.flags &= ~FZ_STEXT_PRESERVE_IMAGES;
			page_stext_options.flags |= FZ_STEXT_MEDIABOX_CLIP;
			if (output_format == OUT_STEXT_JSON || output_format == OUT_OCR_STEXT_JSON)
				page_stext_options.flags |= FZ_STEXT_PRESERVE_SPANS;
			text = fz_new_stext_page(ctx, mediabox);
			dev = fz_new_stext_device(ctx, text, &page_stext_options);
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (output_format == OUT_OCR_TEXT ||
				output_format == OUT_OCR_STEXT_JSON ||
				output_format == OUT_OCR_STEXT_XML ||
				output_format == OUT_OCR_XMLTEXT ||
				output_format == OUT_OCR_HTML ||
				output_format == OUT_OCR_XHTML)
			{
				pre_ocr_dev = dev;
				dev = NULL;
				dev = fz_new_ocr_device(ctx, pre_ocr_dev, ctm, mediabox, 1, ocr_language, NULL, NULL);
			}
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, fz_infinite_rect, cookie);
			else
				fz_run_page(ctx, page, dev, ctm, cookie);
			fz_close_device(ctx, dev);
			fz_drop_device(ctx, dev);
			dev = NULL;
			fz_close_device(ctx, pre_ocr_dev);
			fz_drop_device(ctx, pre_ocr_dev);
			pre_ocr_dev = NULL;
			if (output_format == OUT_STEXT_XML || output_format == OUT_OCR_STEXT_XML)
			{
				fz_print_stext_page_as_xml(ctx, out, text, pagenum, ctm);
			}
			else if (output_format == OUT_STEXT_JSON || output_format == OUT_OCR_STEXT_JSON)
			{
				int first = (inc_page_counter() == 0);
				if (!first)
					fz_write_string(ctx, out, ",");
				fz_print_stext_page_as_json(ctx, out, text, pagenum, ctm);
			}
			else if (output_format == OUT_HTML || output_format == OUT_OCR_HTML)
			{
				fz_print_stext_page_as_html(ctx, out, text, pagenum, ctm, &page_stext_options);
			}
			else if (output_format == OUT_XHTML || output_format == OUT_OCR_XHTML)
			{
				fz_print_stext_page_as_xhtml(ctx, out, text, pagenum, ctm, &page_stext_options);
			}
			else if (output_format == OUT_TEXT || output_format == OUT_OCR_TEXT)
			{
				fz_print_stext_page_as_text(ctx, out, text);
				fz_write_printf(ctx, out, "\f\n");
			}
		}
		fz_always(ctx)
		{
			fz_drop_device(ctx, pre_ocr_dev);
			fz_drop_device(ctx, dev);
			fz_drop_stext_page(ctx, text);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}

	else if (output_format == OUT_PDF)
	{
#if FZ_ENABLE_PDF
		fz_buffer *contents = NULL;
		pdf_obj *resources = NULL;

		fz_var(contents);
		fz_var(resources);

		fz_try(ctx)
		{
			pdf_obj *page_obj;

			dev = pdf_page_write(ctx, pdfout, mediabox, &resources, &contents);
			if (list)
				fz_run_display_list(ctx, list, dev, fz_identity, fz_infinite_rect, cookie);
			else
				fz_run_page(ctx, page, dev, fz_identity, cookie);
			fz_close_device(ctx, dev);
			fz_drop_device(ctx, dev);
			dev = NULL;

			page_obj = pdf_add_page(ctx, pdfout, mediabox, rotation, resources, contents);
			pdf_insert_page(ctx, pdfout, -1, page_obj);
			pdf_drop_obj(ctx, page_obj);
		}
		fz_always(ctx)
		{
			pdf_drop_obj(ctx, resources);
			fz_drop_buffer(ctx, contents);
			fz_drop_device(ctx, dev);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
#else
		fz_throw(ctx, FZ_ERROR_GENERIC, "PDF output is not supported by this mupdf build.");
#endif
	}

	else if (output_format == OUT_SVG)
	{
		float zoom;
		fz_matrix ctm;
		fz_rect tbounds;

		zoom = resolution / 72;
		ctm = fz_pre_rotate(fz_scale(zoom, zoom), rotation);
		tbounds = fz_transform_rect(mediabox, ctm);

		fz_try(ctx)
		{
			int text_format = (stext_options.flags & FZ_STEXT_NO_TEXT_AS_PATH ? FZ_SVG_TEXT_AS_TEXT : FZ_SVG_TEXT_AS_PATH);
			int reuse_images = !(stext_options.flags & FZ_STEXT_NO_REUSE_IMAGES);
			dev = fz_new_svg_device(ctx, out, tbounds.x1-tbounds.x0, tbounds.y1-tbounds.y0, text_format, reuse_images);
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, tbounds, cookie);
			else
				fz_run_page(ctx, page, dev, ctm, cookie);
			fz_close_device(ctx, dev);
		}
		fz_always(ctx)
		{
			fz_drop_device(ctx, dev);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}
	else
	{
		float zoom;
		fz_matrix ctm;
		fz_rect tbounds;
		fz_irect ibounds;
		fz_pixmap *pix = NULL;
		fz_bitmap *bit = NULL;

		fz_var(pix);
		fz_var(bander);
		fz_var(bit);

		zoom = resolution / 72;
		ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

		tbounds = fz_transform_rect(mediabox, ctm);
		ibounds = fz_round_rect(tbounds);

		fz_try(ctx)
		{
			fz_irect band_ibounds = ibounds;
			int band, bands = 1;
			int totalheight = ibounds.y1 - ibounds.y0;
			int drawheight = totalheight;

			if (band_height != 0)
			{
				/* Banded rendering; we'll only render to a
				 * given height at a time. */
				drawheight = band_height;
				if (totalheight > band_height)
					band_ibounds.y1 = band_ibounds.y0 + band_height;
				bands = (totalheight + band_height - 1) / band_height;
				tbounds.y1 = tbounds.y0 + band_height + 2;
				DEBUG_THREADS(fz_info(ctx, "Using %d Bands\n", bands));
			}

			if (num_workers > 0)
			{
				for (band = 0; band < fz_mini(num_workers, bands); band++)
				{
					workers[band].band = band;
					workers[band].error = 0;
					workers[band].ctm = ctm;
					workers[band].tbounds = tbounds;
					workers[band].cookie = master_cookie;
					workers[band].list = list;
					workers[band].pix = fz_new_pixmap_with_bbox(ctx, colorspace, band_ibounds, seps, alpha);
					fz_set_pixmap_resolution(ctx, workers[band].pix, resolution, resolution);
					workers[band].running = 1;
#ifndef DISABLE_MUTHREADS
					DEBUG_THREADS(fz_info(ctx, "Worker %d, Pre-triggering band %d\n", band, band));
					mu_trigger_semaphore(&workers[band].start);
#endif
					ctm.f -= drawheight;
				}
				pix = workers[0].pix;
			}
			else
			{
				pix = fz_new_pixmap_with_bbox(ctx, colorspace, band_ibounds, seps, alpha);
				fz_set_pixmap_resolution(ctx, pix, resolution, resolution);
			}

			/* Output any page level headers (for banded formats) */
			if (output_format == OUT_PGM || output_format == OUT_PPM || output_format == OUT_PNM)
				bander = fz_new_pnm_band_writer(ctx, out);
			else if (output_format == OUT_PAM)
				bander = fz_new_pam_band_writer(ctx, out);
			else if (output_format == OUT_PNG)
				bander = fz_new_png_band_writer(ctx, out);
			else if (output_format == OUT_PBM)
				bander = fz_new_pbm_band_writer(ctx, out);
			else if (output_format == OUT_PKM)
				bander = fz_new_pkm_band_writer(ctx, out);
			else if (output_format == OUT_PS)
				bander = fz_new_ps_band_writer(ctx, out);
			else if (output_format == OUT_PSD)
				bander = fz_new_psd_band_writer(ctx, out);
			else if (output_format == OUT_PWG)
			{
				if (out_cs == CS_MONO)
					bander = fz_new_mono_pwg_band_writer(ctx, out, NULL);
				else
					bander = fz_new_pwg_band_writer(ctx, out, NULL);
			}
			else if (output_format == OUT_PCL)
			{
				if (out_cs == CS_MONO)
					bander = fz_new_mono_pcl_band_writer(ctx, out, NULL);
				else
					bander = fz_new_color_pcl_band_writer(ctx, out, NULL);
			}
			if (bander)
			{
				fz_write_header(ctx, bander, pix->w, totalheight, pix->n, pix->alpha, pix->xres, pix->yres, output_pagenum++, pix->colorspace, pix->seps);
			}

			for (band = 0; band < bands; band++)
			{
				if (num_workers > 0)
				{
					worker_t *w = &workers[band % num_workers];
#ifndef DISABLE_MUTHREADS
					DEBUG_THREADS(fz_info(ctx, "Waiting for worker %d to complete band %d\n", w->num, band));
					mu_wait_semaphore(&w->stop);
#endif
					w->running = 0;
					cookie->errors += w->cookie.errors;
					pix = w->pix;
					bit = w->bit;
					w->bit = NULL;

					if (w->error)
						fz_throw(ctx, FZ_ERROR_GENERIC, "worker %d failed to render band %d", w->num, band);
				}
				else
					drawband(ctx, page, list, ctm, tbounds, cookie, band * band_height, pix, &bit);

				if (bander && (pix || bit))
					fz_write_band(ctx, bander, bit ? bit->stride : pix->stride, drawheight, bit ? bit->samples : pix->samples);
				fz_drop_bitmap(ctx, bit);
				bit = NULL;

				if (num_workers > 0 && band + num_workers < bands)
				{
					worker_t *w = &workers[band % num_workers];
					w->band = band + num_workers;
					w->ctm = ctm;
					w->tbounds = tbounds;
					w->cookie = master_cookie;
					w->running = 1;
#ifndef DISABLE_MUTHREADS
					DEBUG_THREADS(fz_info(ctx, "Triggering worker %d for band %d\n", w->num, w->band));
					mu_trigger_semaphore(&w->start);
#endif
				}
				ctm.f -= drawheight;
			}

			/* FIXME */
			if (showmd5 && pix)
			{
				unsigned char digest[16];
				int i;
				char buf[34];

				fz_md5_pixmap(ctx, pix, digest);
				strcpy(buf, " ");
				for (i = 0; i < 16; i++)
				{
					fz_snprintf(buf + 1 + i * 2, sizeof buf - 1 - i * 2, "%02x", digest[i]);
				}
				fz_info(ctx, " MD5:%s", buf);
			}
		}
		fz_always(ctx)
		{
			if (output_format != OUT_PCLM && output_format != OUT_OCR_PDF)
			{
				fz_drop_band_writer(ctx, bander);
				/* bander must be set to NULL to avoid use-after-frees. A use-after-free
				 * would occur when a valid page was followed by a page with invalid
				 * pixmap dimensions, causing bander -- a static -- to point to previously
				 * freed memory instead of a new band_writer. */
				bander = NULL;
			}
			fz_drop_bitmap(ctx, bit);
			bit = NULL;
			if (num_workers > 0)
			{
				int i;
				DEBUG_THREADS(fz_info(ctx, "Stopping workers and removing their pixmaps\n"));
				for (i = 0; i < num_workers; i++)
				{
					if (workers[i].running)
					{
#ifndef DISABLE_MUTHREADS
						DEBUG_THREADS(fz_info(ctx, "Waiting on worker %d to finish processing\n", i));
						mu_wait_semaphore(&workers[i].stop);
#endif
						workers[i].running = 0;
					}
					else
					{
						DEBUG_THREADS(fz_info(ctx, "Worker %d not processing anything\n", i));
					}
					fz_drop_pixmap(ctx, workers[i].pix);
					workers[i].pix = NULL;
				}
			}
			else
				fz_drop_pixmap(ctx, pix);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}

	if (output_file_per_page)
	{
		file_level_trailers(ctx);

#if FZ_ENABLE_PDF
		if (output_format == OUT_PDF)
		{
			// save previous page?
			pdf_save_document(ctx, pdfout, pdfoutpath, NULL);
			pdf_drop_document(ctx, pdfout);
			pdfout = NULL;
			fz_free(ctx, pdfoutpath);
			pdfoutpath = NULL;
		}
#endif
	}

	if (showtime)
	{
		int end = gettime();
		int diff = end - start;

		if (bg)
		{
			if (diff + interptime < timing.min)
			{
				timing.min = diff + interptime;
				timing.mininterp = interptime;
				timing.minpage = pagenum;
				timing.minfilename = fname;
			}
			if (diff + interptime > timing.max)
			{
				timing.max = diff + interptime;
				timing.maxinterp = interptime;
				timing.maxpage = pagenum;
				timing.maxfilename = fname;
			}
			timing.total += diff + interptime;
			timing.count ++;

			fz_info(ctx, " %dms (interpretation) %dms (rendering) %dms (total)", interptime, diff, diff + interptime);
		}
		else
		{
			if (diff < timing.min)
			{
				timing.min = diff;
				timing.minpage = pagenum;
				timing.minfilename = fname;
			}
			if (diff > timing.max)
			{
				timing.max = diff;
				timing.maxpage = pagenum;
				timing.maxfilename = fname;
			}
			timing.total += diff;
			timing.count ++;

			fz_info(ctx, " %dms (total)", diff);
		}
	}

	if (lowmemory)
		fz_empty_store(ctx);

	if (showmemory)
	{
		fz_dump_glyph_cache_stats(ctx);
	}

	fz_flush_warnings(ctx);

	if (cookie->errors)
		errored = 1;
}

static void bgprint_flush(void)
{
	if (!bgprint.active || !bgprint.started)
		return;

#ifndef DISABLE_MUTHREADS
	mu_wait_semaphore(&bgprint.stop);
#endif
	bgprint.started = 0;
}

static void drawpage(fz_context *ctx, fz_document *doc, int pagenum)
{
	fz_page *page;
	fz_display_list *list = NULL;
	fz_device *dev = NULL;
	int start;
	fz_cookie cookie = master_cookie;
	fz_separations *seps = NULL;
	const char *features = "";

	fz_var(list);
	fz_var(dev);
	fz_var(seps);

	start = (showtime ? gettime() : 0);

	page = fz_load_page(ctx, doc, pagenum - 1);

	if (spots != SPOTS_NONE)
	{
		fz_try(ctx)
		{
			seps = fz_page_separations(ctx, page);
			if (seps)
			{
				int i, n = fz_count_separations(ctx, seps);
				if (spots == SPOTS_FULL)
					for (i = 0; i < n; i++)
						fz_set_separation_behavior(ctx, seps, i, FZ_SEPARATION_SPOT);
				else
					for (i = 0; i < n; i++)
						fz_set_separation_behavior(ctx, seps, i, FZ_SEPARATION_COMPOSITE);
			}
			else if (fz_page_uses_overprint(ctx, page))
			{
				/* This page uses overprint, so we need an empty
				 * sep object to force the overprint simulation on. */
				seps = fz_new_separations(ctx, 0);
			}
			else if (oi && fz_colorspace_n(ctx, oi) != fz_colorspace_n(ctx, colorspace))
			{
				/* We have an output intent, and it's incompatible
				 * with the colorspace our device needs. Force the
				 * overprint simulation on, because this ensures that
				 * we 'simulate' the output intent too. */
				seps = fz_new_separations(ctx, 0);
			}
		}
		fz_catch(ctx)
		{
			fz_drop_page(ctx, page);
			fz_rethrow(ctx);
		}
	}

	if (uselist)
	{
		fz_try(ctx)
		{
			list = fz_new_display_list(ctx, fz_bound_page(ctx, page));
			dev = fz_new_list_device(ctx, list);
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			fz_run_page(ctx, page, dev, fz_identity, &cookie);
			fz_close_device(ctx, dev);
		}
		fz_always(ctx)
		{
			fz_drop_device(ctx, dev);
			dev = NULL;
		}
		fz_catch(ctx)
		{
			fz_drop_display_list(ctx, list);
			fz_drop_separations(ctx, seps);
			fz_drop_page(ctx, page);
			fz_rethrow(ctx);
		}

		if (bgprint.active && showtime)
		{
			int end = gettime();
			start = end - start;
		}
	}

	if (showfeatures)
	{
		int iscolor;
		dev = fz_new_test_device(ctx, &iscolor, 0.02f, 0, NULL);
		if (lowmemory)
			fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
		fz_try(ctx)
		{
			if (list)
				fz_run_display_list(ctx, list, dev, fz_identity, fz_infinite_rect, &cookie);
			else
				fz_run_page(ctx, page, dev, fz_identity, &cookie);
			fz_close_device(ctx, dev);
		}
		fz_always(ctx)
		{
			fz_drop_device(ctx, dev);
			dev = NULL;
		}
		fz_catch(ctx)
		{
			fz_drop_display_list(ctx, list);
			fz_drop_separations(ctx, seps);
			fz_drop_page(ctx, page);
			fz_rethrow(ctx);
		}
		features = iscolor ? " color" : " grayscale";
	}

	if (output_file_per_page)
	{
		char text_buffer[PATH_MAX];

		bgprint_flush();
		if (out)
		{
			fz_close_output(ctx, out);
			fz_drop_output(ctx, out);
			out = NULL;
		}
		fz_format_output_path(ctx, text_buffer, sizeof text_buffer, output, pagenum);

		if (output_format == OUT_PDF)
		{
#if FZ_ENABLE_PDF
			pdfout = pdf_create_document(ctx);
			pdfoutpath = fz_strdup(ctx, text_buffer);
#else
			fz_throw(ctx, FZ_ERROR_GENERIC, "PDF output is not supported by this mupdf build.");
#endif
		}
		else
		{
			out = fz_new_output_with_path(ctx, text_buffer, 0);
		}
	}

	if (bgprint.active)
	{
		bgprint_flush();
		if (bgprint.error)
		{
			fz_drop_display_list(ctx, list);
			fz_drop_separations(ctx, seps);
			fz_drop_page(ctx, page);

			/* it failed, do not continue trying */
			bgprint.active = 0;
		}
		else if (bgprint.active)
		{
			if (!quiet || showfeatures || showtime || showmd5)
				fz_info(ctx, "page %s %d%s", filename, pagenum, features);

			bgprint.started = 1;
			bgprint.page = page;
			bgprint.list = list;
			bgprint.seps = seps;
			bgprint.filename = filename;
			bgprint.pagenum = pagenum;
			bgprint.interptime = start;
			bgprint.error = 0;
#ifndef DISABLE_MUTHREADS
			mu_trigger_semaphore(&bgprint.start);
#else
			fz_drop_display_list(ctx, list);
			fz_drop_separations(ctx, seps);
			fz_drop_page(ctx, page);
#endif
		}
	}
	else
	{
		if (!quiet || showfeatures || showtime || showmd5)
			fz_info(ctx, "page %s %d%s", filename, pagenum, features);
		fz_try(ctx)
			dodrawpage(ctx, page, list, pagenum, &cookie, start, 0, filename, 0, seps);
		fz_always(ctx)
		{
			fz_drop_display_list(ctx, list);
			fz_drop_separations(ctx, seps);
			fz_drop_page(ctx, page);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}
}

static void drawrange(fz_context *ctx, fz_document *doc, const char *range)
{
	int page, spage, epage, pagecount;

	pagecount = fz_count_pages(ctx, doc);

	while ((range = fz_parse_page_range(ctx, range, &spage, &epage, pagecount)))
	{
		if (spage < epage)
			for (page = spage; page <= epage; page++)
			{
				fz_try(ctx)
					drawpage(ctx, doc, page);
				fz_catch(ctx)
				{
					if (ignore_errors)
						fz_warn(ctx, "ignoring error on page %d in '%s'", page, filename);
					else
						fz_rethrow(ctx);
				}
			}
		else
			for (page = spage; page >= epage; page--)
			{
				fz_try(ctx)
					drawpage(ctx, doc, page);
				fz_catch(ctx)
				{
					if (ignore_errors)
						fz_warn(ctx, "ignoring error on page %d in '%s'", page, filename);
					else
						fz_rethrow(ctx);
				}
			}
	}
}

static int
parse_colorspace(const char *name)
{
	int i;

	for (i = 0; i < (int)nelem(cs_name_table); i++)
	{
		if (!strcmp(name, cs_name_table[i].name))
			return cs_name_table[i].colorspace;
	}

	/* Assume ICC. We will error out later if not the case. */
	icc_filename = name;
	return CS_ICC;
}

typedef struct
{
	size_t size;
	size_t seqnum;
#if defined(_M_IA64) || defined(_M_AMD64) || defined(_WIN64)
	size_t magic;
	size_t align128;
#else
	size_t magic;
#endif
} trace_header;

struct trace_info
{
	int sequence_number;
	size_t current;
	size_t peak;
	size_t total;
	size_t allocs;
	size_t mem_limit;
	size_t alloc_limit;
};

static struct trace_info trace_info = { 1, 0, 0, 0, 0, 0, 0 };

static void *hit_limit(void *val)
{
	return val;
}

static void *hit_memory_limit(struct trace_info *info, int is_malloc, size_t oldsize, size_t size)
{
	if (is_malloc)
		fz_error(ctx, "Memory limit (%zu) hit upon malloc(%zu) when %zu already allocated.", info->mem_limit, size, info->current);
	else
		fz_error(ctx, "Memory limit (%zu) hit upon realloc(%zu) from %zu bytes when %zu already allocated.", info->mem_limit, size, oldsize, info->current);
	return hit_limit(NULL);
}


static void *hit_alloc_limit(struct trace_info *info, int is_malloc, size_t oldsize, size_t size)
{
	if (is_malloc)
		fz_error(ctx, "Allocation limit (%zu) hit upon malloc(%zu) when %zu already allocated.", info->alloc_limit, size, info->current);
	else
		fz_error(ctx, "Allocation limit (%zu) hit upon realloc(%zu) from %zu bytes when %zu already allocated.", info->alloc_limit, size, oldsize, info->current);
	return hit_limit(NULL);
}

static void *
trace_malloc(void *arg, size_t size)
{
	struct trace_info *info = (struct trace_info *) arg;
	trace_header *p;
	if (size == 0)
		return NULL;
	if (size > SIZE_MAX - sizeof(trace_header))
		return NULL;
	if (info->mem_limit > 0 && size > info->mem_limit - info->current)
		return hit_memory_limit(info, 1, 0, size);
	if (info->alloc_limit > 0 && info->allocs > info->alloc_limit)
		return hit_alloc_limit(info, 1, 0, size);
	p = malloc(size + sizeof(trace_header));
	if (p == NULL)
		return NULL;
	p[0].size = size;
	p[0].magic = 0xEAD;
	p[0].seqnum = info->sequence_number++;
	info->current += size;
	info->total += size;
	if (info->current > info->peak)
		info->peak = info->current;
	info->allocs++;
	return (void *)&p[1];
}

static void
trace_free(void *arg, void *p_)
{
	struct trace_info *info = (struct trace_info *) arg;
	trace_header *p = (trace_header *)p_;

	if (p_ == NULL)
		return;

	size_t size = p[-1].size;
	int rotten = 0;
	info->current -= size;
	if (p[-1].magic != 0xEAD)
	{
		fz_error(ctx, "*!* double free! %d", (int)(p[-1].magic - 0xEAD));
		p[-1].magic++;
		rotten = 1;
	}
	if (rotten)
	{
		fz_error(ctx, "*!* corrupted heap record! %p", &p[-1]);
	}
	else
	{
		free(&p[-1]);
	}
}

static void *
trace_realloc(void *arg, void *p_, size_t size)
{
	struct trace_info *info = (struct trace_info *) arg;
	trace_header *p = (trace_header *)p_;
	size_t oldsize;

	if (size == 0)
	{
		trace_free(arg, p_);
		return NULL;
	}

	if (p_ == NULL)
		return trace_malloc(arg, size);
	if (size > SIZE_MAX - sizeof(trace_header))
		return NULL;
	oldsize = p[-1].size;
	if (info->mem_limit > 0 && size > info->mem_limit - info->current + oldsize)
		return hit_memory_limit(info, 0, oldsize, size);
	if (info->alloc_limit > 0 && info->allocs > info->alloc_limit)
		return hit_alloc_limit(info, 0, oldsize, size);

	int rotten = 0;
	oldsize = p[-1].size;
	if (p[-1].magic != 0xEAD)
	{
		fz_error(ctx, "*!* double free! %d", (int)(p[-1].magic - 0xEAD));
		p[-1].magic++;
		rotten = 1;
	}
	if (rotten)
	{
		fz_error(ctx, "*!* corrupted heap record! %p", &p[-1]);
		return NULL;
	}
	else
	{
		p = realloc(&p[-1], size + sizeof(trace_header));
		if (p == NULL)
			return NULL;
		info->current += size - oldsize;
		if (size > oldsize)
			info->total += size - oldsize;
		if (info->current > info->peak)
			info->peak = info->current;
		p[0].size = size;
		info->allocs++;
		return &p[1];
	}
}

#ifndef DISABLE_MUTHREADS
static void worker_thread(void *arg)
{
	worker_t *me = (worker_t *)arg;
	int band;

	do
	{
		DEBUG_THREADS(fz_info(ctx, "Worker %d waiting\n", me->num));
		mu_wait_semaphore(&me->start);
		band = me->band;
		DEBUG_THREADS(fz_info(ctx, "Worker %d woken for band %d\n", me->num, band));
		if (band >= 0)
		{
			fz_try(me->ctx)
			{
				drawband(me->ctx, NULL, me->list, me->ctm, me->tbounds, &me->cookie, band * band_height, me->pix, &me->bit);
				DEBUG_THREADS(fz_info(ctx, "Worker %d completed band %d\n", me->num, band));
			}
			fz_catch(me->ctx)
			{
				DEBUG_THREADS(fz_info(ctx, "Worker %d failed on band %d\n", me->num, band));
				me->error = 1;
			}
		}
		mu_trigger_semaphore(&me->stop);
	}
	while (band >= 0);
	DEBUG_THREADS(fz_info(ctx, "Worker %d shutting down\n", me->num));
}

static void bgprint_worker(void *arg)
{
	fz_cookie cookie = master_cookie;
	int pagenum;

	(void)arg;

	do
	{
		DEBUG_THREADS(fz_info(ctx, "BGPrint waiting\n"));
		mu_wait_semaphore(&bgprint.start);
		pagenum = bgprint.pagenum;
		DEBUG_THREADS(fz_info(ctx, "BGPrint woken for pagenum %d\n", pagenum));
		if (pagenum >= 0)
		{
			int start = gettime();
			memset(&cookie, 0, sizeof(cookie));
			fz_try(bgprint.ctx)
			{
				dodrawpage(bgprint.ctx, bgprint.page, bgprint.list, pagenum, &cookie, start, bgprint.interptime, bgprint.filename, 1, bgprint.seps);
				DEBUG_THREADS(fz_info(ctx, "BGPrint completed page %d\n", pagenum));
			}
			fz_always(bgprint.ctx)
			{
				fz_drop_display_list(bgprint.ctx, bgprint.list);
				fz_drop_separations(bgprint.ctx, bgprint.seps);
				fz_drop_page(bgprint.ctx, bgprint.page);
			}
			fz_catch(bgprint.ctx)
			{
				DEBUG_THREADS(fz_info(ctx, "BGPrint failed on page %d\n", pagenum));
				bgprint.error = 1;
			}

		}
		mu_trigger_semaphore(&bgprint.stop);
	}
	while (pagenum >= 0);
	DEBUG_THREADS(fz_info(ctx, "BGPrint shutting down\n"));
}
#endif

static inline int iswhite(int ch)
{
	return
		ch == '\011' || ch == '\012' ||
		ch == '\014' || ch == '\015' || ch == '\040';
}

static void apply_layer_config(fz_context *ctx, fz_document *doc, const char *lc)
{
#if FZ_ENABLE_PDF
	pdf_document *pdoc = pdf_specifics(ctx, doc);
	int config;
	int n, j;
	pdf_layer_config info;
	char msgbuf[2048];

	if (!pdoc)
	{
		fz_warn(ctx, "Only PDF files have layers");
		return;
	}

	while (iswhite(*lc))
		lc++;

	if (*lc == 0 || *lc == 'l')
	{
		int num_configs = pdf_count_layer_configs(ctx, pdoc);

		if (num_configs > 0)
		{
			fz_info(ctx, "\nPDF Layer configs (%d):", num_configs);
			for (config = 0; config < num_configs; config++)
			{
				fz_snprintf(msgbuf, sizeof(msgbuf), "%3d:", config);
				pdf_layer_config_info(ctx, pdoc, config, &info);
				fz_info(ctx, "%s Name=\"%s\" Creator=\"%s\"", msgbuf, info.name ? info.name : "", info.creator ? info.creator : "");
			}
		}
		return;
	}

	/* Read the config number */
	if (*lc < '0' || *lc > '9')
	{
		fz_error(ctx, "cannot find number expected for -y");
		return;
	}
	config = fz_atoi(lc);
	pdf_select_layer_config(ctx, pdoc, config);

	while (*lc)
	{
		int item;

		/* Skip over the last number we read (in the fz_atoi) */
		while (*lc >= '0' && *lc <= '9')
			lc++;
		while (iswhite(*lc))
			lc++;
		if (*lc != ',')
			break;
		lc++;
		while (iswhite(*lc))
			lc++;
		if (*lc < '0' || *lc > '9')
		{
			fz_error(ctx, "Expected a number for UI item to toggle");
			return;
		}
		item = fz_atoi(lc);
		pdf_toggle_layer_config_ui(ctx, pdoc, item);
	}

	/* Now list the final state of the config */
	fz_info(ctx, "Layer Config %d (final):", config);
	pdf_layer_config_info(ctx, pdoc, config, &info);
	fz_info(ctx, "  Name=\"%s\" Creator=\"%s\"", info.name ? info.name : "", info.creator ? info.creator : "");

	n = pdf_count_layer_config_ui(ctx, pdoc);
	if (n > 0)
	{
		fz_info(ctx, "PDF UI Layer configs (%d):", n);
		for (j = 0; j < n; j++)
		{
			pdf_layer_config_ui ui;

			pdf_layer_config_ui_info(ctx, pdoc, j, &ui);
			fz_snprintf(msgbuf, sizeof(msgbuf), "%3d: ", j);
			while (ui.depth > 0)
			{
				ui.depth--;
				fz_strlcat(msgbuf, "  ", sizeof(msgbuf));
			}

			size_t len = strlen(msgbuf);
			switch (ui.type)
			{
			case PDF_LAYER_UI_CHECKBOX:
				fz_snprintf(msgbuf + len, sizeof(msgbuf) - len, " [%c] ", ui.selected ? 'x' : ' ');
				break;

			case PDF_LAYER_UI_RADIOBOX:
				fz_snprintf(msgbuf + len, sizeof(msgbuf) - len, " (%c) ", ui.selected ? 'x' : ' ');
				break;

			case PDF_LAYER_UI_LABEL:
				break;

			default:
				fz_snprintf(msgbuf + len, sizeof(msgbuf) - len, " {UNKNOWN UI.TYPE:%d} ", (int)ui.type);
				break;
			}

			if (ui.type != PDF_LAYER_UI_LABEL && ui.locked)
			{
				fz_strlcat(msgbuf, " <locked> ", sizeof(msgbuf));
			}
			if (ui.text)
			{
				fz_strlcat(msgbuf, ui.text, sizeof(msgbuf));
			}

			fz_info(ctx, "%s", msgbuf);
		}
	}
#endif
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

static int get_accelerator_filename(fz_context *ctx, char outname[], size_t len, const char *fname)
{
	char absname[PATH_MAX];
	if (!fz_realpath(fname, absname))
		return 0;
	if (!convert_to_accel_path(ctx, outname, absname, len))
		return 0;
	return 1;
}

static void save_accelerator(fz_context *ctx, fz_document *doc, const char *fname)
{
	char absname[PATH_MAX];

	if (!doc)
		return;
	if (!fz_document_supports_accelerator(ctx, doc))
		return;
	if (!get_accelerator_filename(ctx, absname, sizeof(absname), fname))
		return;

	fz_save_accelerator(ctx, doc, absname);
}

static int
read_rotation(const char *arg)
{
	int i;

	if (strcmp(arg, "auto"))
	{
		return -1;
	}

	i = fz_atoi(arg);

	i = i % 360;
	if (i % 90 != 0)
	{
		fz_error(ctx, "Ignoring invalid rotation");
		i = 0;
	}

	return i;
}

static int
fz_strnieq(const char* arg, size_t wlen, const char* word)
{
	if (wlen != strlen(word))
		return 0;
	return fz_strncasecmp(arg, word, wlen) == 0;
}

static void
parse_render_options(fz_cookie* cookie, const char* arg)
{
	const char* SEPS = " ,;:";

	while (*arg)
	{
		size_t wlen = strcspn(arg, SEPS);

		if (fz_strnieq(arg, wlen, "everything"))
			cookie->run_mode = FZ_RUN_EVERYTHING;
		else if (fz_strnieq(arg, wlen, "content"))
			cookie->run_mode |= FZ_RUN_CONTENT;
		else if (fz_strnieq(arg, wlen, "annotations"))
			cookie->run_mode |= FZ_RUN_ANNOTATIONS;
		else if (fz_strnieq(arg, wlen, "widgets"))
			cookie->run_mode |= FZ_RUN_WIDGETS;
		else if (fz_strnieq(arg, wlen, "unknown"))
			cookie->run_annotations_reject_mask[PDF_ANNOT_UNKNOWN + 1] = 1;
		else
		{
			// check if the item matches any of the annotation types:
			char buf[64];
			strncpy(buf, arg, wlen);
			buf[wlen] = 0;
			enum fz_annot_type type = pdf_annot_type_from_string(ctx, buf);
			if (type == PDF_ANNOT_UNKNOWN)
			{
				fz_error(ctx, "Unrecognized annotation type '%s' specified as part of the render filter. Treating it as UNKNOWN annotation type.\n", buf);
			}
			cookie->run_annotations_reject_mask[type + 1] = 1;
		}

		arg += wlen;
		arg += strspn(arg, SEPS);
	}
}

static void mu_drop_context(void)
{
	if (mudraw_is_toplevel_ctx)
	{
		if (trace_info.allocs && (trace_info.mem_limit || trace_info.alloc_limit || showmemory))
		{
			fz_info(ctx, "Memory use total=%zu peak=%zu current=%zu", trace_info.total, trace_info.peak, trace_info.current);
			fz_info(ctx, "Allocations total=%zu", trace_info.allocs);

			// reset heap tracing after reporting: this atexit handler MAY be invoked multiple times!
			memset(&trace_info, 0, sizeof(trace_info));
		}
	}

	assert(!ctx || (ctx->error.top == ctx->error.stack));

	fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	ctx = NULL;

	// nuke the locks last as they are still used by the heap free ('drop') calls in the lines just above!
	if (mudraw_is_toplevel_ctx)
	{
		// as we registered a global context, we should clean the locks on it now
		// so the atexit handler won't have to bother with it.
		assert(fz_has_global_context());
		ctx = fz_get_global_context();
		fz_drop_context_locks(ctx);
		ctx = NULL;

		fz_drop_global_context();

#ifndef DISABLE_MUTHREADS
		fin_mudraw_locks();
#endif /* DISABLE_MUTHREADS */

		mudraw_is_toplevel_ctx = 0;
	}
}

#ifdef MUDRAW_STANDALONE
int main(int argc, const char **argv)
#else
int mudraw_main(int argc, const char **argv)
#endif
{
	const char *password = "";
	const char* txtdraw_options = "";
	fz_document *doc = NULL;
	int c;
	fz_alloc_context trace_alloc_ctx = { &trace_info, trace_malloc, trace_realloc, trace_free };
	fz_alloc_context *alloc_ctx = NULL;
	size_t max_store = FZ_STORE_DEFAULT;

	fz_var(doc);

	// reset global vars: this tool MAY be re-invoked via bulktest or others and should RESET completely between runs:
	mudraw_is_toplevel_ctx = 0;
	ctx = NULL;
	output = NULL;
	out = NULL;
	output_pagenum = 0;
	output_file_per_page = 0;

	format = NULL;
	output_format = OUT_NONE;

	rotation = -1;
	resolution = 72;
	res_specified = 0;
	width = 0;
	height = 0;
	fit = 0;

	layout_w = FZ_DEFAULT_LAYOUT_W;
	layout_h = FZ_DEFAULT_LAYOUT_H;
	layout_em = FZ_DEFAULT_LAYOUT_EM;
	layout_css = NULL;
	layout_use_doc_css = 1;
	min_line_width = 0.0f;

	showfeatures = 0;
	showtime = 0;
	showmemory = 0;
	showmd5 = 0;

	memset(&trace_info, 0, sizeof trace_info);

#if FZ_ENABLE_PDF
	pdfout = NULL;
	pdfoutpath = NULL;
#endif

	no_icc = 0;
	ignore_errors = 0;
	uselist = 1;
	alphabits_text = 8;
	alphabits_graphics = 8;

	out_cs = CS_UNSET;
	proof_filename = NULL;
	proof_cs = NULL;
	icc_filename = NULL;
	gamma_value = 1;
	invert = 0;
	band_height = 0;
	lowmemory = 0;

	quiet = 0;
	errored = 0;
	colorspace = NULL;
	oi = NULL;
#if FZ_ENABLE_SPOT_RENDERING
	spots = SPOTS_OVERPRINT_SIM;
#else
	spots = SPOTS_NONE;
#endif
	alpha = 0;
	useaccel = 1;
	filename = NULL;
	files = 0;
	num_workers = 0;
	workers = NULL;
	bander = NULL;

	layer_config = NULL;

	ocr_language = ocr_language_default;

	memset(&bgprint, 0, sizeof(bgprint));
	memset(&timing, 0, sizeof(timing));

	memset(&master_cookie, 0, sizeof(master_cookie));
	master_cookie.run_mode = FZ_RUN_EVERYTHING;

	gettime_once = 1;

	// ---

	bgprint.active = 0;			/* set by -P */
	num_workers = 0;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "qp:o:F:R:r:w:h:fB:c:e:G:Is:A:DiW:H:S:T:t:U:XLvPl:y:NO:am:x:hj:J:")) != -1)
	{
		switch (c)
		{
		default: return usage();

		case 'q': quiet = 1; fz_default_error_warn_info_mode(1, 1, 1); break;

		case 'p': password = fz_optarg; break;

		case 'o': output = fz_optarg; break;
		case 'F': format = fz_optarg; break;

		case 'R': rotation = read_rotation(fz_optarg); break;
		case 'r': resolution = fz_atof(fz_optarg); res_specified = 1; break;
		case 'w': width = fz_atof(fz_optarg); break;
		case 'h': height = fz_atof(fz_optarg); break;
		case 'f': fit = 1; break;
		case 'B': band_height = atoi(fz_optarg); break;

		case 'c': out_cs = parse_colorspace(fz_optarg); break;
		case 'e': proof_filename = fz_optarg; break;
		case 'G': gamma_value = fz_atof(fz_optarg); break;
		case 'I': invert++; break;
		case 'j': parse_render_options(&master_cookie, fz_optarg); break;
		case 'J': fz_default_png_compression_level(fz_atoi(fz_optarg)); break;

		case 'W': layout_w = fz_atof(fz_optarg); break;
		case 'H': layout_h = fz_atof(fz_optarg); break;
		case 'S': layout_em = fz_atof(fz_optarg); break;
		case 'U': layout_css = fz_optarg; break;
		case 'X': layout_use_doc_css = 0; break;

		case 'O': spots = fz_atof(fz_optarg);
#ifndef FZ_ENABLE_SPOT_RENDERING
			fz_warn(ctx, "Spot rendering/Overprint/Overprint simulation not enabled in this build");
			spots = SPOTS_NONE;
#endif
			break;

		case 's':
			if (strchr(fz_optarg, 't')) ++showtime;
			if (strchr(fz_optarg, 'm')) ++showmemory;
			if (strchr(fz_optarg, 'f')) ++showfeatures;
			if (strchr(fz_optarg, '5')) ++showmd5;
			break;

		case 'A':
		{
			char *sep;
			alphabits_graphics = atoi(fz_optarg);
			sep = strchr(fz_optarg, '/');
			if (sep)
				alphabits_text = atoi(sep+1);
			else
				alphabits_text = alphabits_graphics;
			break;
		}
		case 'D': uselist = 0; break;
		case 'l': min_line_width = fz_atof(fz_optarg); break;
		case 'i': ignore_errors = 1; break;
		case 'N': no_icc = 1; break;

		case 'T':
#ifndef DISABLE_MUTHREADS
			num_workers = atoi(fz_optarg); break;
#else
			fz_warn(ctx, "Threads not enabled in this build");
			break;
#endif
		case 't':
#ifndef OCR_DISABLED
			ocr_language = fz_optarg; break;
#else
			fz_warn(ctx, "OCR functionality not enabled in this build");
			break;
#endif
		case 'm':
			if (fz_optarg[0] == 's') trace_info.mem_limit = fz_atoi64(&fz_optarg[1]);
			else if (fz_optarg[0] == 'a') trace_info.alloc_limit = fz_atoi64(&fz_optarg[1]);
			else trace_info.mem_limit = fz_atoi64(fz_optarg);
			break;
		case 'L': lowmemory = 1; break;
		case 'P':
#ifndef DISABLE_MUTHREADS
			bgprint.active = 1;
			if (!num_workers)
				num_workers = max(3, fz_get_cpu_core_count());
			break;
#else
			fz_warn(ctx, "Threads not enabled in this build");
			break;
#endif
		case 'y': layer_config = fz_optarg; break;
		case 'a': useaccel = 0; break;
		case 'x': txtdraw_options = fz_optarg; break;

		case 'v': fz_info(ctx, "mudraw version %s", FZ_VERSION); return EXIT_FAILURE;
		}
	}

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		return usage();
	}

	if (!output || *output == 0 || !strcmp(output, "-"))
	{
		// No need to set quiet mode when writing to stdout as all error/warn/info/debug info is sent via stderr!
#if 0
		quiet = 1; /* automatically be quiet if printing to stdout */
		fz_default_error_warn_info_mode(1, 1, 1);
#endif

		output = "/dev/stdout";
	}

	if (num_workers > 0)
	{
		if (uselist == 0)
		{
			fz_error(ctx, "cannot use multiple threads without using display list. Falling back to single thread processing.");
			num_workers = 0;
		}

		if (band_height == 0 && num_workers > 0)
		{
			band_height = fz_maxi(16, 11 * fz_maxi(72, resolution) / (2 * num_workers));
			fz_error(ctx, "Using multiple threads without banding is pointless. Using a band height of %d and %d workers.", band_height, num_workers);
		}
	}

	if (bgprint.active)
	{
		if (uselist == 0)
		{
			fz_error(ctx, "cannot bgprint without using display list. Ignoring bgprint setting.");
			bgprint.active = 0;
		}
	}

	if (trace_info.mem_limit || trace_info.alloc_limit || showmemory)
		alloc_ctx = &trace_alloc_ctx;

	if (lowmemory)
		max_store = 1;

	if (!fz_has_global_context())
	{
		fz_locks_context* locks = NULL;

#ifndef DISABLE_MUTHREADS
		locks = init_mudraw_locks();
		if (locks == NULL)
		{
			fz_error(ctx, "mutex initialisation failed");
			return EXIT_FAILURE;
		}
#endif

		ctx = fz_new_context(alloc_ctx, locks, max_store);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
		ctx = NULL;

		mudraw_is_toplevel_ctx = 1;
	}
	else
	{
		// caller of mudraw_main() has set global CTX.
		//
		// check if that CTX has locking, because if it has none,
		// then we CANNOT use bands or threads!
		ctx = fz_get_global_context();
		if (!fz_has_locking_support(ctx))
		{
			fz_error(ctx, "cannot use multiple threads without locking support. Falling back to single thread processing.");
			num_workers = 0;
			bgprint.active = 0;
			band_height = 0;
		}
	}
	atexit(mu_drop_context);

	ctx = fz_new_context(NULL, NULL, max_store);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_try(ctx)
	{
		if (proof_filename)
		{
			fz_buffer *proof_buffer = fz_read_file(ctx, proof_filename);
			proof_cs = fz_new_icc_colorspace(ctx, FZ_COLORSPACE_NONE, 0, NULL, proof_buffer);
			fz_drop_buffer(ctx, proof_buffer);
		}

		fz_parse_stext_options(ctx, &stext_options, txtdraw_options);

		fz_set_text_aa_level(ctx, alphabits_text);
		fz_set_graphics_aa_level(ctx, alphabits_graphics);
		fz_set_graphics_min_line_width(ctx, min_line_width);
		if (no_icc)
			fz_disable_icc(ctx);
		else
			fz_enable_icc(ctx);

#ifndef DISABLE_MUTHREADS
		// do not bgprint when there won't be any bg threads available, ever.
		if (num_workers < 1)
		{
			bgprint.active = 0;
		}

		if (bgprint.active)
		{
			int fail = 0;
			bgprint.ctx = fz_clone_context(ctx);
			fail |= mu_create_semaphore(&bgprint.start);
			fail |= mu_create_semaphore(&bgprint.stop);
			fail |= mu_create_thread(&bgprint.thread, bgprint_worker, NULL);
			if (fail)
			{
				mu_destroy_semaphore(&bgprint.start);
				mu_destroy_semaphore(&bgprint.stop);
				fz_throw(bgprint.ctx, FZ_ERROR_GENERIC, "bgprint startup failed");
			}
		}

		if (num_workers > 0)
		{
			int i;
			int fail = 0;
			workers = fz_calloc(ctx, num_workers, sizeof(*workers));
			for (i = 0; i < num_workers; i++)
			{
				workers[i].ctx = fz_clone_context(ctx);
				workers[i].num = i;
				fail |= mu_create_semaphore(&workers[i].start);
				fail |= mu_create_semaphore(&workers[i].stop);
				fail |= mu_create_thread(&workers[i].thread, worker_thread, &workers[i]);
			}
			if (fail)
			{
				for (i = 0; i < num_workers; i++)
				{
					mu_destroy_semaphore(&workers[i].start);
					mu_destroy_semaphore(&workers[i].stop);
					mu_destroy_thread(&workers[i].thread);
					fz_drop_context(workers[i].ctx);
				}
				fz_free(ctx, workers);
				fz_throw(ctx, FZ_ERROR_GENERIC, "worker startup failed");
			}
		}
#endif /* DISABLE_MUTHREADS */

		if (layout_css)
		{
			fz_buffer *buf = fz_read_file(ctx, layout_css);
			fz_set_user_css(ctx, fz_string_from_buffer(ctx, buf));
			fz_drop_buffer(ctx, buf);
		}

		fz_set_use_document_css(ctx, layout_use_doc_css);

		/* Determine output type */
		if (band_height < 0)
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "Bandheight must be > 0");
		}

		output_format = OUT_PNG;
		if (format)
		{
			int i;

			for (i = 0; i < (int)nelem(suffix_table); i++)
			{
				if (!strcmp(format, suffix_table[i].suffix+1))
				{
					output_format = suffix_table[i].format;
					if (spots == SPOTS_FULL && suffix_table[i].spots == 0)
					{
						fz_error(ctx, "Output format '%s' does not support spot rendering.\nDoing overprint simulation instead.", suffix_table[i].suffix+1);
						spots = SPOTS_OVERPRINT_SIM;
					}
					break;
				}
			}
			if (i == (int)nelem(suffix_table))
			{
				fz_throw(ctx, FZ_ERROR_GENERIC, "Unknown output format '%s'", format);
			}
		}
		else
		{
			const char *suffix = output;
			int i;

			for (i = 0; i < (int)nelem(suffix_table); i++)
			{
				char *s = strstr(suffix, suffix_table[i].suffix);

				if (s != NULL)
				{
					suffix = s+strlen(suffix_table[i].suffix);
					output_format = suffix_table[i].format;
					if (spots == SPOTS_FULL && suffix_table[i].spots == 0)
					{
						fz_error(ctx, "Output format '%s' does not support spot rendering.\nDoing overprint simulation instead.", suffix_table[i].suffix+1);
						spots = SPOTS_OVERPRINT_SIM;
					}
					// match the tail (= file extension) with the output format;
					// when there's still a tail left after this round, we need to look again:
					if (*suffix)
						i = -1;
					else
						break;
				}
			}
		}

		if (band_height)
		{
			if (output_format != OUT_PAM &&
				output_format != OUT_PGM &&
				output_format != OUT_PPM &&
				output_format != OUT_PNM &&
				output_format != OUT_PNG &&
				output_format != OUT_PBM &&
				output_format != OUT_PKM &&
				output_format != OUT_PCL &&
				output_format != OUT_PCLM &&
				output_format != OUT_PS &&
				output_format != OUT_PSD &&
				output_format != OUT_OCR_PDF)
			{
				fz_error(ctx, "Banded operation only possible with PxM, PCL, PCLM, PDFOCR, PS, PSD, and PNG outputs");
				band_height = 0;
			}
			if (showmd5 && band_height)
			{
				fz_error(ctx, "Banded operation not compatible with MD5");
				band_height = 0;
			}
		}

		{
			int i, j;

			for (i = 0; i < (int)nelem(format_cs_table); i++)
			{
				if (format_cs_table[i].format == output_format)
				{
					if (out_cs == CS_UNSET)
						out_cs = format_cs_table[i].default_cs;
					for (j = 0; j < (int)nelem(format_cs_table[i].permitted_cs); j++)
					{
						if (format_cs_table[i].permitted_cs[j] == out_cs)
							break;
					}
					if (j == (int)nelem(format_cs_table[i].permitted_cs))
					{
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unsupported colorspace (%d) for this format", output_format);
					}
				}
			}
		}

		alpha = 1;
		switch (out_cs)
		{
			case CS_MONO:
			case CS_GRAY:
			case CS_GRAY_ALPHA:
				colorspace = fz_device_gray(ctx);
				alpha = (out_cs == CS_GRAY_ALPHA);
				break;
			case CS_RGB:
			case CS_RGB_ALPHA:
				colorspace = fz_device_rgb(ctx);
				alpha = (out_cs == CS_RGB_ALPHA);
				break;
			case CS_CMYK:
			case CS_CMYK_ALPHA:
				colorspace = fz_device_cmyk(ctx);
				alpha = (out_cs == CS_CMYK_ALPHA);
				break;
			case CS_ICC:
				fz_try(ctx)
				{
					fz_buffer *icc_buffer = fz_read_file(ctx, icc_filename);
					colorspace = fz_new_icc_colorspace(ctx, FZ_COLORSPACE_NONE, 0, NULL, icc_buffer);
					fz_drop_buffer(ctx, icc_buffer);
				}
				fz_catch(ctx)
				{
					fz_throw(ctx, FZ_ERROR_GENERIC, "Invalid ICC destination color space");
				}
				if (colorspace == NULL)
				{
					fz_throw(ctx, FZ_ERROR_GENERIC, "Invalid ICC destination color space");
				}
				alpha = 0;
				break;
			default:
				fz_throw(ctx, FZ_ERROR_GENERIC, "Unknown colorspace %d!", out_cs);
		}

		if (out_cs != CS_ICC)
			colorspace = fz_keep_colorspace(ctx, colorspace);
		else
		{
			int i, j, okay;

			/* Check to make sure this icc profile is ok with the output format */
			okay = 0;
			for (i = 0; i < (int)nelem(format_cs_table); i++)
			{
				if (format_cs_table[i].format == output_format)
				{
					for (j = 0; j < (int)nelem(format_cs_table[i].permitted_cs); j++)
					{
						switch (format_cs_table[i].permitted_cs[j])
						{
							case CS_MONO:
							case CS_GRAY:
							case CS_GRAY_ALPHA:
								if (fz_colorspace_is_gray(ctx, colorspace))
									okay = 1;
								break;
							case CS_RGB:
							case CS_RGB_ALPHA:
								if (fz_colorspace_is_rgb(ctx, colorspace))
									okay = 1;
								break;
							case CS_CMYK:
							case CS_CMYK_ALPHA:
								if (fz_colorspace_is_cmyk(ctx, colorspace))
									okay = 1;
								break;
						}
					}
				}
			}

			if (!okay)
			{
				fz_throw(ctx, FZ_ERROR_GENERIC, "ICC profile uses a colorspace that cannot be used for this format");
			}
		}

		if (output_format == OUT_SVG || output_format == OUT_PNG || output_format == OUT_PSD)
		{
			/* SVG files are always opened for each page. */
			output_file_per_page = 1;
		}

		if ((fz_has_percent_d(output) || output_file_per_page) &&
			strcmp(output, "/dev/stdout") && strcmp(output, "/dev/null") && fz_strcasecmp(output, "nul:"))
		{
			output_file_per_page = 1;
		}
		else
		{
			output_file_per_page = 0;
			out = fz_new_output_with_path(ctx, output, 0);
		}

		if (output_format == OUT_PDF)
		{
#if FZ_ENABLE_PDF
			// Nuke `out`. We will be using `pdfout` instead.
			if (out)
			{
				fz_close_output(ctx, out);
				fz_drop_output(ctx, out);
				out = NULL;
			}

			if (!output_file_per_page)
			{
				pdfout = pdf_create_document(ctx);
			}
#else
			fz_throw(ctx, FZ_ERROR_GENERIC, "PDF output is not supported by this mupdf build.");
#endif
		}

		// Check if the Tesseract engine can initialize properly when one of the OCR modes is requested.
		// If it cannot init, report a warning accordingly and fall back to the non-OCR output format:
		if (output_format == OUT_OCR_TRACE ||
			output_format == OUT_OCR_TEXT ||
			output_format == OUT_OCR_STEXT_JSON ||
			output_format == OUT_OCR_STEXT_XML ||
			output_format == OUT_OCR_XMLTEXT ||
			output_format == OUT_OCR_HTML ||
			output_format == OUT_OCR_XHTML ||
			output_format == OUT_OCR_PDF)
		{
			void* tess_api = NULL;

			fz_try(ctx)
			{
				tess_api = ocr_init(ctx, ocr_language);
			}
			fz_always(ctx)
			{
				ocr_fin(ctx, tess_api);
			}
			fz_catch(ctx)
			{
				fz_error(ctx, "warning: tesseract OCR engine could not be initialized. Falling back to the non-OCR-ed output format! %s", fz_caught_message(ctx));
				switch (output_format)
				{
				case OUT_OCR_TRACE:
					output_format = OUT_TRACE; break;
				case OUT_OCR_TEXT:
					output_format = OUT_TEXT; break;
				case OUT_OCR_STEXT_JSON:
					output_format = OUT_STEXT_JSON; break;
				case OUT_OCR_STEXT_XML:
					output_format = OUT_STEXT_XML; break;
				case OUT_OCR_XMLTEXT:
					output_format = OUT_XMLTEXT; break;
				case OUT_OCR_HTML:
					output_format = OUT_HTML; break;
				case OUT_OCR_XHTML:
					output_format = OUT_XHTML; break;
				case OUT_OCR_PDF:
					output_format = OUT_PDF; break;
				}
			}
		}

		// report output format in verbode mode:
		if (!quiet)
		{
			int i;
			const char* fmtstr = ".???";

			for (i = 0; i < (int)nelem(suffix_table); i++)
			{
				if (output_format == suffix_table[i].format)
				{
					fmtstr = suffix_table[i].suffix + 1;
					break;
				}
			}
			fz_info(ctx, "Output format: %s (%s%s)", fmtstr, colorspace->name, alpha ? ", Alpha" : "");
		}

		timing.count = 0;
		timing.total = 0;
		timing.min = 1 << 30;
		timing.max = 0;
		timing.mininterp = 1 << 30;
		timing.maxinterp = 0;
		timing.minpage = 0;
		timing.maxpage = 0;
		timing.minfilename = "";
		timing.maxfilename = "";
		timing.layout = 0;
		timing.minlayout = 1 << 30;
		timing.maxlayout = 0;
		timing.minlayoutfilename = "";
		timing.maxlayoutfilename = "";
		if (showtime && bgprint.active)
			timing.total = gettime();

		fz_try(ctx)
		{
			if (!output_file_per_page)
				file_level_headers(ctx);
			fz_register_document_handlers(ctx);

			while (fz_optind < argc)
			{
				char accelpath[PATH_MAX];
				char *accel = NULL;
				time_t atime;
				time_t dtime;
				int layouttime;

				fz_try(ctx)
				{
					filename = argv[fz_optind++];
					files++;

					if (!useaccel)
						accel = NULL;
					/* If there was an accelerator to load, what would it be called? */
					else if (get_accelerator_filename(ctx, accelpath, sizeof(accelpath), filename))
					{
						/* Check whether that file exists, and isn't older than
						 * the document. */
						atime = stat_mtime(accelpath);
						dtime = stat_mtime(filename);
						if (atime == 0)
						{
							/* No accelerator */
						}
						else if (atime > dtime)
							accel = accelpath;
						else
						{
							/* Accelerator data is out of date */
							unlink(accelpath);
							accel = NULL; /* In case we have jumped up from below */
						}
					}

					doc = fz_open_accelerated_document(ctx, filename, accel);

					if (fz_needs_password(ctx, doc))
					{
						if (!fz_authenticate_password(ctx, doc, password))
							fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
					}

					/* Once document is open check for output intent colorspace */
					oi = fz_document_output_intent(ctx, doc);
					if (oi)
					{
						/* See if we had explicitly set a profile to render */
						if (out_cs != CS_ICC)
						{
							/* In this case, we want to render to the output intent
							 * color space if the number of channels is the same */
							if (fz_colorspace_n(ctx, oi) == fz_colorspace_n(ctx, colorspace))
							{
								fz_drop_colorspace(ctx, colorspace);
								colorspace = fz_keep_colorspace(ctx, oi);
							}
						}
					}

					layouttime = gettime();
					fz_layout_document(ctx, doc, layout_w, layout_h, layout_em);
					(void) fz_count_pages(ctx, doc);   // added to ensure a more proper layouttime figure
					layouttime = gettime() - layouttime;

					timing.layout += layouttime;
					if (layouttime < timing.minlayout)
					{
						timing.minlayout = layouttime;
						timing.minlayoutfilename = filename;
					}
					if (layouttime > timing.maxlayout)
					{
						timing.maxlayout = layouttime;
						timing.maxlayoutfilename = filename;
					}

					if (layer_config)
						apply_layer_config(ctx, doc, layer_config);

					// reset the page counter which is used by the JSON output to ensure we output proper format for multipage ranges.
					reset_page_counter();

					if (fz_optind == argc || !fz_is_page_range(ctx, argv[fz_optind]))
						drawrange(ctx, doc, "1-N");
					if (fz_optind < argc && fz_is_page_range(ctx, argv[fz_optind]))
						drawrange(ctx, doc, argv[fz_optind++]);

					bgprint_flush();
					if (bgprint.error)
						fz_throw(ctx, FZ_ERROR_GENERIC, "failed to parse page");

					if (useaccel)
						save_accelerator(ctx, doc, filename);
				}
				fz_always(ctx)
				{
					fz_drop_document(ctx, doc);
					doc = NULL;
				}
				fz_catch(ctx)
				{
					if (!ignore_errors)
						fz_rethrow(ctx);

					bgprint_flush();
					fz_warn(ctx, "ignoring error in '%s'", filename);
				}
			}
		}
		fz_catch(ctx)
		{
			bgprint_flush();
			fz_drop_document(ctx, doc);
			fz_error(ctx, "cannot draw '%s': %s", filename, fz_caught_message(ctx));
			errored = 1;
		}

		if (!output_file_per_page)
			file_level_trailers(ctx);

#if FZ_ENABLE_PDF
		if (pdfout)
		{
			pdf_save_document(ctx, pdfout, output, NULL);
			pdf_drop_document(ctx, pdfout);
			pdfout = NULL;
		}
#endif

		{
			fz_close_output(ctx, out);
			fz_drop_output(ctx, out);
			out = NULL;
		}
	}
	fz_always(ctx)
	{
		if (showtime)
		{
			if (timing.count > 0)
			{
				if (bgprint.active)
					timing.total = gettime() - timing.total;

				if (files == 1)
				{
					fz_info(ctx, "total %dms (%dms layout) / %d pages for an average of %dms",
						timing.total, timing.layout, timing.count, timing.total / timing.count);
					if (bgprint.active)
					{
						fz_info(ctx, "fastest page %d: %dms (interpretation) %dms (rendering) %dms(total)",
							timing.minpage, timing.mininterp, timing.min - timing.mininterp, timing.min);
						fz_info(ctx, "slowest page %d: %dms (interpretation) %dms (rendering) %dms(total)",
							timing.maxpage, timing.maxinterp, timing.max - timing.maxinterp, timing.max);
					}
					else
					{
						fz_info(ctx, "fastest page %d: %dms", timing.minpage, timing.min);
						fz_info(ctx, "slowest page %d: %dms", timing.maxpage, timing.max);
					}
				}
				else
				{
					fz_info(ctx, "total %dms (%dms layout) / %d pages for an average of %dms in %d files",
						timing.total, timing.layout, timing.count, timing.total / timing.count, files);
					fz_info(ctx, "fastest layout: %dms (%s)", timing.minlayout, timing.minlayoutfilename);
					fz_info(ctx, "slowest layout: %dms (%s)", timing.maxlayout, timing.maxlayoutfilename);
					fz_info(ctx, "fastest page %d: %dms (%s)", timing.minpage, timing.min, timing.minfilename);
					fz_info(ctx, "slowest page %d: %dms (%s)", timing.maxpage, timing.max, timing.maxfilename);
				}
			}

			fz_dump_lock_times(ctx, gettime());
		}

#ifndef DISABLE_MUTHREADS
		// bgprint also uses the workers, hence we MUST shut down bgprint BEFORE the workers themselves:
		if (bgprint.active)
		{
			bgprint.pagenum = -1;
			mu_trigger_semaphore(&bgprint.start);
			mu_wait_semaphore(&bgprint.stop);
			mu_destroy_semaphore(&bgprint.start);
			mu_destroy_semaphore(&bgprint.stop);
			mu_destroy_thread(&bgprint.thread);
			fz_drop_context(bgprint.ctx);
			bgprint.ctx = NULL;
			bgprint.active = 0;
		}

		if (num_workers > 0)
		{
			int i;
			DEBUG_THREADS(fz_info(ctx, "Asking workers to shutdown, then destroy their resources\n"));
			for (i = 0; i < num_workers; i++)
			{
				workers[i].band = -1;
				mu_trigger_semaphore(&workers[i].start);
				mu_wait_semaphore(&workers[i].stop);
				mu_destroy_semaphore(&workers[i].start);
				mu_destroy_semaphore(&workers[i].stop);
				mu_destroy_thread(&workers[i].thread);
				fz_drop_context(workers[i].ctx);
			}
			fz_free(ctx, workers);
		}
#endif /* DISABLE_MUTHREADS */

		fz_drop_colorspace(ctx, colorspace);
		fz_drop_colorspace(ctx, proof_cs);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "%s", fz_caught_message(ctx));
		if (!errored) {
			fz_error(ctx, "Rendering failed");
			errored = 1;
		}
	}

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}
