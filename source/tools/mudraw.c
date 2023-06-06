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

/*
 * mudraw -- command line tool for drawing and converting documents
 */

#include "mupdf/fitz.h"
#include "mupdf/mutool.h"
#include "mupdf/assertions.h"
#include "mupdf/helpers/jmemcust.h"

#if FZ_ENABLE_RENDER_CORE 

#include "mupdf/helpers/system-header-files.h"

#include "timeval.h"

#if FZ_ENABLE_PDF
#include "mupdf/pdf.h" /* for pdf output */
#endif

#include "mupdf/helpers/mu-threads.h"
#include "mupdf/helpers/cpu.h"

#include "mupdf/helpers/dir.h"

#include "../fitz/tessocr.h"

#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <direct.h> /* for getcwd */
#else
#include <sys/stat.h> /* for mkdir */
#include <unistd.h> /* for getcwd */
#endif

/* Allow for windows stdout being made binary */
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

/* Enable for helpful threading debug */
#if 01
#define DEBUG_THREADS(code) do { if (verbosity >= 2) { code; } } while (0)
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
	OUT_EMPTY_BOX,
	OUT_PNG,
	OUT_TIFF,
	OUT_WEBP,
	OUT_PAM,
	OUT_PBM,
	OUT_PCL,
	OUT_PCLM,
	OUT_PGM,
	OUT_MURAW,
	OUT_PNM,
	OUT_PPM,
	OUT_PKM,
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
	{ ".emptybox", OUT_EMPTY_BOX, 0 },

	/* And the 'single extension' ones go last. */
	{ ".png", OUT_PNG, 0 },
	{ ".tiff", OUT_TIFF, 0 },
	{ ".muraw", OUT_MURAW, 0 },
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
	{ ".webp", OUT_WEBP, 0 },
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
	int permitted_cs[8];
} format_cs_table_t;

static const format_cs_table_t format_cs_table[] =
{
	{ OUT_PNG, CS_RGB, { CS_GRAY, CS_GRAY_ALPHA, CS_RGB, CS_RGB_ALPHA, CS_ICC } },
	{ OUT_MURAW, CS_RGB_ALPHA, { CS_MONO, CS_GRAY, CS_GRAY_ALPHA, CS_RGB, CS_RGB_ALPHA, CS_CMYK, CS_CMYK_ALPHA, CS_ICC } },
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
	{ OUT_WEBP, CS_RGB, { CS_GRAY, CS_GRAY_ALPHA, CS_RGB, CS_RGB_ALPHA, CS_ICC } },

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
	{ OUT_EMPTY_BOX, CS_RGB, { CS_RGB } },
	{ OUT_OCR_TEXT, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_HTML, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_XHTML, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_STEXT_XML, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_XMLTEXT, CS_RGB, { CS_RGB } },
	{ OUT_OCR_STEXT_JSON, CS_GRAY, { CS_GRAY } },
	{ OUT_OCR_TRACE, CS_GRAY, { CS_GRAY } },
};

static fz_stext_options stext_options;

static fz_cookie master_cookie = { 0 };

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
static const suffix_t *output_format = NULL;

static int rotation = -1;			// actual ~ calculated values
static float resolution = -1;

static int user_specified_rotation = -1;	// user-specified values
static float user_specified_resolution = 72;
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
static int subpix_preset = 0;

static int out_cs = CS_UNSET;
static const char *proof_filename = NULL;
fz_colorspace *proof_cs = NULL;
static const char *icc_filename = NULL;
static int use_gamma = 0;
static float gamma_value = 1;
static int invert = 0;
static int kill = 0;
static int band_height = 0;
static int lowmemory = 0;

static int verbosity = 1;
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
static int layer_list = 0;
static int layer_on[1000];
static int layer_off[1000];
static int layer_on_len;
static int layer_off_len;

static const char ocr_language_default[] = "eng";
static const char *ocr_language = ocr_language_default;
static const char *ocr_datadir = NULL;

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
	fz_cookie cookie;
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
#if FZ_ENABLE_PDF
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
	assert(dst > annot_type_list);
	dst[-1] = 0;   // nuke that last ','
	assert(strlen(annot_type_list) <= sizeof(annot_type_list));
#endif

	fz_info(ctx,
		"usage: mudraw [options] file [pages]\n"
		"  -p -  password\n"
		"\n"
		"  -o -  output file name (%%d or ### for page number, '-' for stdout)\n"
		"  -F -  output format (default inferred from output file name)\n"
		"    raster: png, pgm, ppm, pnm, pam, pbm, pkm, pwg, pcl, psd, ps, muraw\n"
#if FZ_ENABLE_OCR
		"    vector: svg, pdf, trace, ocr.trace\n"
#else
		"    vector: svg, pdf, trace\n"
#endif
		"    text: txt, html, xhtml, xml, stext, stext.json, bbox\n"
#if FZ_ENABLE_OCR
		"    ocr'd text: ocr.txt, ocr.html, ocr.xhtml, ocr.xml, ocr.stext, ocr.stext.json\n"
#else
		"    (ocr'd text is disabled in this build)\n"
#endif
		"    bitmap-wrapped-as-pdf: pclm, ocr.pdf\n"
		"\n"
		"  -q    be quiet (don't print progress messages)\n"
		"  -v    verbose ~ not quiet (repeat to increase the chattiness of the application)\n"
		"  -s -  show extra information:\n"
		"     m      show memory use\n"
		"     t      show timings\n"
		"     f      show page features\n"
		"     5      show md5 checksum of rendered image\n"
		"\n"
		"  -R {auto,0,90,180,270}\n"
		"        rotate clockwise (default: auto)\n"
		"  -r -  resolution in dpi (default: 72)\n"
		"  -w -  page width (in pixels) (maximum width if -r is specified)\n"
		"  -h -  page height (in pixels) (maximum height if -r is specified)\n"
		"  -f    fit file to page if too large: fit width and/or height exactly;\n"
		"        ignore original aspect ratio\n"
		"  -B -  maximum band height (pXm, pcl, pclm, ocr.pdf, ps, psd, muraw and png output\n"
		"        only)\n"
#ifndef DISABLE_MUTHREADS
		"  -T -  number of threads to use for rendering (banded mode only), where number arg\n"
		"        is one of:\n"
		"     N      equals the number of (virtual) CPU cores on this machine\n"   
		"     *      = N\n"   
		"     4      or any other positive integer number\n"   
		"     50%    or any other percentage of N\n"
		"     -2     or any other negative integer: so many less than N. (Hence keeps\n"
		"            cores free for independent tasks.\n"
#else
		"  -T -  number of threads to use for rendering (disabled in this non-threading\n"
		"        build)\n"
#endif
#ifndef DISABLE_MUTHREADS
		"  -P    parallel interpretation/rendering\n"
#else
		"  -P    parallel interpretation/rendering (disabled in this non-threading build)\n"
#endif
		"\n"
		"  -W -  page width for EPUB layout\n"
		"  -H -  page height for EPUB layout\n"
		"  -S -  font size for EPUB layout\n"
		"  -U -  file name of user stylesheet for EPUB layout\n"
		"  -X    disable document styles for EPUB layout\n"
		"  -a    disable usage of accelerator file\n"
		"  -x -  text extract options (option=yes --> on, option=no --> off):\n"
		"     preserve-ligatures,preserve-whitespace,inhibit-spaces,dehyphenate,\n"
		"     preserve-images,preserve-spans,reference-images,reuse-images,\n"
		"     mediabox-clip,text-as-path\n"
		"  -c -  colorspace (mono, gray, grayalpha, rgb, rgba, cmyk, cmykalpha,\n"
		"        filename of ICC profile)\n"
		"  -e -  proof icc profile (filename of ICC profile)\n"
		"  -G -  apply gamma correction\n"
#if FZ_ENABLE_GAMMA
		"\t-g -\tuse gamma blending\n"
#else
		"\t-g -\tuse gamma blending (disabled in this build)\n"
#endif
		"  -I    invert colors\n"
		"\n"
		"  -A -  number of bits of anti-aliasing (0 to 8)\n"
		"  -A -/-  number of bits of anti-aliasing (0 to 8) (graphics, text)\n"
		"  -A -/-/-  number of bits of antialiasing (0 to 8) (graphics, text)\n"
		"            subpix preset: 0 = default, 1 = reduced\n"
		"  -l -  minimum stroked line width (in pixels)\n"
		"  -K    do not draw text\n"
		"  -KK   only draw text\n"
		"  -D    disable use of display list\n"
		"  -j -  render only selected types of content. Use a comma-separated list\n"
		"        to combine types (everything,content,annotations,Unknown,\n"
		"        max_nodes=NNN,max_time=MS,"
		"        %s)\n"
		"  -J -  set PNG output compression level: 0 (none), 1 (fast)..9 (best)\n"
		"  -i    ignore errors\n"
		"  -L    low memory mode (avoid caching, clear objects after each page)\n"
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
#if FZ_ENABLE_OCR
		"  -t -  Specify language/script for OCR (default: eng)\n"
		"  -d -  Specify path for OCR files (default: rely on TESSDATA_PREFIX environment variable)\n"
#else
		"  -t -  OCR language    (the OCR feature is not available in this build)\n"
		"  -d -  OCR datafiles   (the OCR feature is not available in this build)\n"
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
		"    -Y     List individual layers to stderr\n"
		"    -z -   Hide individual layer\n"
		"    -Z -   Show individual layer\n"
		"\n"
		"  pages  comma separated list of page numbers and ranges\n",
#if FZ_ENABLE_PDF
		annot_type_list
#else
		"..."
#endif
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
file_level_headers(fz_context *ctx, const char *filename)
{
	if (output_format->format == OUT_STEXT_XML || output_format->format == OUT_OCR_STEXT_XML
		|| output_format->format == OUT_TRACE || output_format->format == OUT_OCR_TRACE || output_format->format == OUT_BBOX
		|| output_format->format == OUT_XMLTEXT || output_format->format == OUT_OCR_XMLTEXT)
	{
		fz_write_printf(ctx, out, "<?xml version=\"1.0\"?>\n");
	}

	if (output_format->format == OUT_HTML || output_format->format == OUT_OCR_HTML)
		fz_print_stext_header_as_html(ctx, out);
	else if (output_format->format == OUT_XHTML || output_format->format == OUT_OCR_XHTML)
		fz_print_stext_header_as_xhtml(ctx, out);

	else if (output_format->format == OUT_STEXT_XML || output_format->format == OUT_OCR_STEXT_XML
		|| output_format->format == OUT_TRACE || output_format->format == OUT_OCR_TRACE || output_format->format == OUT_BBOX
		|| output_format->format == OUT_XMLTEXT || output_format->format == OUT_OCR_XMLTEXT)
	{
		fz_write_printf(ctx, out, "<document name=\"%s\">\n", fz_basename(filename));
	}
	else if (output_format->format == OUT_STEXT_JSON || output_format->format == OUT_OCR_STEXT_JSON)
		fz_write_printf(ctx, out, "{%q:%q,%q:[", "file", filename, "pages");

	else if (output_format->format == OUT_PS)
		fz_write_ps_file_header(ctx, out);

	else if (output_format->format == OUT_PWG)
		fz_write_pwg_file_header(ctx, out);

	else if (output_format->format == OUT_PCLM)
	{
		fz_pclm_options opts = { 0 };
		fz_parse_pclm_options(ctx, &opts, "compression=flate");
		bander = fz_new_pclm_band_writer(ctx, out, &opts);
	}
#if FZ_ENABLE_OCR_OUTPUT
	else if (output_format->format == OUT_OCR_PDF)
	{
		char options[300];
		fz_pdfocr_options opts = { 0 };
		fz_snprintf(options, sizeof(options), "compression=flate,ocr-language=%s", ocr_language);
		if (ocr_datadir)
		{
			fz_strlcat(options, ",ocr-datadir=", sizeof (options));
			fz_strlcat(options, ocr_datadir, sizeof (options));
		}
		fz_parse_pdfocr_options(ctx, &opts, options);
		bander = fz_new_pdfocr_band_writer(ctx, out, &opts);
	}
#endif
}

static void
file_level_trailers(fz_context *ctx)
{
	if (output_format->format == OUT_STEXT_XML || output_format->format == OUT_OCR_STEXT_XML
		|| output_format->format == OUT_TRACE || output_format->format == OUT_OCR_TRACE || output_format->format == OUT_BBOX
		|| output_format->format == OUT_XMLTEXT || output_format->format == OUT_OCR_XMLTEXT)
	{
		fz_write_printf(ctx, out, "</document>\n");
	}
	if (output_format->format == OUT_STEXT_JSON || output_format->format == OUT_OCR_STEXT_JSON)
		fz_write_printf(ctx, out, "]}");

	if (output_format->format == OUT_HTML || output_format->format == OUT_OCR_HTML)
		fz_print_stext_trailer_as_html(ctx, out);
	if (output_format->format == OUT_XHTML || output_format->format == OUT_OCR_XHTML)
		fz_print_stext_trailer_as_xhtml(ctx, out);

	if (output_format->format == OUT_PS)
		fz_write_ps_file_trailer(ctx, out, output_pagenum);

	if (output_format->format == OUT_PCLM || output_format->format == OUT_OCR_PDF)
	{
		fz_close_band_writer(ctx, bander);
		fz_drop_band_writer(ctx, bander);
		bander = NULL;
	}
}

static void apply_kill_switch(fz_device *dev)
{
	if (kill == 1)
	{
		/* kill all non-clipping text operators */
		dev->fill_text = NULL;
		dev->stroke_text = NULL;
		dev->ignore_text = NULL;
	}
	else if (kill == 2)
	{
		/* kill all non-clipping path, image, and shading operators */
		dev->fill_path = NULL;
		dev->stroke_path = NULL;
		dev->fill_shade = NULL;
		dev->fill_image = NULL;
		dev->fill_image_mask = NULL;
	}
}

static void drawband(fz_context *ctx, fz_page *page, fz_display_list *list, fz_matrix ctm, fz_rect tbounds, int band_start, fz_pixmap *pix, fz_bitmap **bit)
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
		apply_kill_switch(dev);
		if (lowmemory)
			fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
		if (alphabits_graphics == 0)
			fz_enable_device_hints(ctx, dev, FZ_DONT_INTERPOLATE_IMAGES);
		if (list)
			fz_run_display_list(ctx, list, dev, ctm, tbounds);
		else
			fz_run_page(ctx, page, dev, ctm);
		fz_close_device(ctx, dev);
		fz_drop_device(ctx, dev);
		dev = NULL;

		if (invert)
			fz_invert_pixmap(ctx, pix);
		if (gamma_value != 1)
			fz_gamma_pixmap(ctx, pix, gamma_value);

		if (((output_format->format == OUT_PCL || output_format->format == OUT_PWG) && out_cs == CS_MONO) || (output_format->format == OUT_PBM) || (output_format->format == OUT_PKM))
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

	resolution = user_specified_resolution;
	rotation = user_specified_rotation;

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

		float res = fz_max(72 * (ibounds.x1 - ibounds.x0) / (float)(ibounds2.x1 - ibounds2.x0), 72 * (ibounds.y1 - ibounds.y0) / (float)(ibounds2.y1 - ibounds2.y0));
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

static void dodrawpage(fz_context *ctx, fz_page *page, fz_display_list *list, int pagenum, int start, int interptime, const char *fname, int bg, fz_separations *seps)
{
	fz_rect mediabox;
	fz_device *dev = NULL;

	fz_var(dev);

	if (output_file_per_page)
		file_level_headers(ctx, fname);

	if (list)
		mediabox = fz_bound_display_list(ctx, list);
	else
		mediabox = fz_bound_page(ctx, page);

	/* Calculate Page resolution & rotation */
	calc_page_render_details(ctx, page, mediabox);

	if (verbosity >= 1)
		fz_info(ctx, "drawing page %d at resolution: %.1f, rotation: %d\n", pagenum, resolution, rotation);

	if (output_format->format == OUT_TRACE || output_format->format == OUT_OCR_TRACE)
	{
		float zoom;
		fz_matrix ctm;
		fz_device *pre_ocr_dev = NULL;
		fz_rect tmediabox;

		zoom = resolution / 72;
		ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);
		tmediabox = fz_transform_rect(mediabox, ctm);

		fz_var(pre_ocr_dev);

		fz_try(ctx)
		{
			fz_write_printf(ctx, out, "<page pagenum=\"%d\" ctm=\"%M\" bbox=\"%R\" mediabox=\"%R\">\n", pagenum, &ctm, &mediabox, &tmediabox);
			dev = fz_new_trace_device(ctx, out);
			apply_kill_switch(dev);
			if (output_format->format == OUT_OCR_TRACE)
			{
				pre_ocr_dev = dev;
				dev = NULL;
				dev = fz_new_ocr_device(ctx, pre_ocr_dev, ctm, mediabox, 1, ocr_language, ocr_datadir, NULL, NULL);
			}
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, fz_infinite_rect);
			else
				fz_run_page(ctx, page, dev, ctm);
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

	else if (output_format->format == OUT_XMLTEXT || output_format->format == OUT_OCR_XMLTEXT)
	{
		fz_device* pre_ocr_dev = NULL;

		fz_try(ctx)
		{
			float zoom;
			fz_matrix ctm;
			fz_rect tmediabox;

			zoom = resolution / 72;
			ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

			tmediabox = fz_transform_rect(mediabox, ctm);

			fz_write_printf(ctx, out, "<page pagenum=\"%d\" ctm=\"%M\" bbox=\"%R\" mediabox=\"%R\">\n", pagenum, &ctm, &mediabox, &tmediabox);
			dev = fz_new_xmltext_device(ctx, out);
			apply_kill_switch(dev);
			if (output_format->format == OUT_OCR_XMLTEXT)
			{
				pre_ocr_dev = dev;
				dev = NULL;
				dev = fz_new_ocr_device(ctx, pre_ocr_dev, ctm, mediabox, 1, ocr_language, ocr_datadir, NULL, NULL);
				apply_kill_switch(dev);
			}
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, fz_infinite_rect);
			else
				fz_run_page(ctx, page, dev, ctm);
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

	else if (output_format->format == OUT_BBOX)
	{
		fz_try(ctx)
		{
			fz_rect bbox = fz_empty_rect;
			float zoom;
			fz_matrix ctm;
			fz_rect tmediabox;

			zoom = resolution / 72;
			ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);
			tmediabox = fz_transform_rect(mediabox, ctm);

			dev = fz_new_bbox_device(ctx, &bbox);
			apply_kill_switch(dev);
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, fz_infinite_rect);
			else
				fz_run_page(ctx, page, dev, ctm);
			fz_close_device(ctx, dev);
			fz_write_printf(ctx, out, "<page pagenum=\"%d\" ctm=\"%M\" bbox=\"%R\" mediabox=\"%R\" />\n", pagenum, &ctm, &bbox, &tmediabox);
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

	else if (output_format->format == OUT_TEXT || output_format->format == OUT_HTML || output_format->format == OUT_XHTML || output_format->format == OUT_STEXT_XML || output_format->format == OUT_STEXT_JSON ||
		output_format->format == OUT_OCR_TEXT || output_format->format == OUT_OCR_HTML || output_format->format == OUT_OCR_XHTML || output_format->format == OUT_OCR_STEXT_XML || output_format->format == OUT_OCR_STEXT_JSON ||
		output_format->format == OUT_EMPTY_BOX)
	{
		fz_stext_page *text = NULL;
		float zoom;
		fz_matrix ctm;
		fz_device *pre_ocr_dev = NULL;
		fz_rect tmediabox;

		zoom = resolution / 72;
		ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

		fz_var(text);
		fz_var(pre_ocr_dev);

		fz_try(ctx)
		{
			// Even when OCR fails, we want to see what we got thus far as text,
			// hence we should *postpone* bubbling up the OCR error, if one occurs.
			fz_stext_options page_stext_options = stext_options;

			if (!(stext_options.flags_conf_mask & FZ_STEXT_PRESERVE_IMAGES))
			{
				// set the preserve_images flag when not set up explicitly by the commandline argument.
				if (output_format->format == OUT_HTML ||
					output_format->format == OUT_XHTML ||
					output_format->format == OUT_OCR_HTML ||
					output_format->format == OUT_OCR_XHTML
					)
					page_stext_options.flags |= FZ_STEXT_PRESERVE_IMAGES;
				else
					page_stext_options.flags &= ~FZ_STEXT_PRESERVE_IMAGES;
			}
			if (!(stext_options.flags_conf_mask & FZ_STEXT_MEDIABOX_CLIP))
			{
				page_stext_options.flags |= FZ_STEXT_MEDIABOX_CLIP;
			}
			if (!(stext_options.flags_conf_mask & FZ_STEXT_PRESERVE_SPANS))
			{
				if (output_format->format == OUT_STEXT_JSON || output_format->format == OUT_OCR_STEXT_JSON)
				{
					page_stext_options.flags |= FZ_STEXT_PRESERVE_SPANS;
				}
			}

			// override the default options when these have been explicitly set in the commandline:
			page_stext_options.flags &= ~stext_options.flags_conf_mask;                          // mask
			page_stext_options.flags |= (stext_options.flags & stext_options.flags_conf_mask);   // commandline overrules

			tmediabox = fz_transform_rect(mediabox, ctm);
			text = fz_new_stext_page(ctx, tmediabox);
			dev = fz_new_stext_device(ctx, text, &page_stext_options);
			apply_kill_switch(dev);
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (output_format->format == OUT_OCR_TEXT ||
				output_format->format == OUT_OCR_STEXT_JSON ||
				output_format->format == OUT_OCR_STEXT_XML ||
				output_format->format == OUT_OCR_XMLTEXT ||
				output_format->format == OUT_OCR_HTML ||
				output_format->format == OUT_OCR_XHTML)
			{
				pre_ocr_dev = dev;
				dev = NULL;
				dev = fz_new_ocr_device(ctx, pre_ocr_dev, ctm, mediabox, 1, ocr_language, ocr_datadir, NULL, NULL);
			}
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, fz_infinite_rect);
			else
				fz_run_page(ctx, page, dev, ctm);
			fz_close_device(ctx, dev);
			fz_drop_device(ctx, dev);
			dev = NULL;
			fz_close_device(ctx, pre_ocr_dev);
			fz_drop_device(ctx, pre_ocr_dev);
			pre_ocr_dev = NULL;
			if (output_format->format == OUT_STEXT_XML || output_format->format == OUT_OCR_STEXT_XML)
			{
				fz_print_stext_page_as_xml(ctx, out, text, pagenum, ctm);
			}
			else if (output_format->format == OUT_STEXT_JSON || output_format->format == OUT_OCR_STEXT_JSON)
			{
				int first = (inc_page_counter() == 0);
				if (!first)
					fz_write_string(ctx, out, ",");
				fz_print_stext_page_as_json(ctx, out, text, pagenum, ctm);
			}
			else if (output_format->format == OUT_HTML || output_format->format == OUT_OCR_HTML)
			{
				fz_print_stext_page_as_html(ctx, out, text, pagenum, ctm, &page_stext_options);
			}
			else if (output_format->format == OUT_XHTML || output_format->format == OUT_OCR_XHTML)
			{
				fz_print_stext_page_as_xhtml(ctx, out, text, pagenum, ctm, &page_stext_options);
			}
			else if (output_format->format == OUT_TEXT || output_format->format == OUT_OCR_TEXT)
			{
				fz_print_stext_page_as_text(ctx, out, text);
				fz_write_printf(ctx, out, "\f\n");
			}
			else if (output_format->format == OUT_EMPTY_BOX)
			{
				fz_print_stext_page_as_empty_box(ctx, out, text);
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

	else if (output_format->format == OUT_PDF)
	{
#if FZ_ENABLE_PDF
		fz_buffer *contents = NULL;
		pdf_obj *resources = NULL;

		fz_var(contents);
		fz_var(resources);

		fz_try(ctx)
		{
			/* We are ignoring ctm here. Understandable in a way, as resolution makes no sense
			 * when writing PDFs. Rotation is taken care of by the pdf_add_page call. */
			pdf_obj *page_obj;

			dev = pdf_page_write(ctx, pdfout, mediabox, &resources, &contents);
			apply_kill_switch(dev);
			if (list)
				fz_run_display_list(ctx, list, dev, fz_identity, fz_infinite_rect);
			else
				fz_run_page(ctx, page, dev, fz_identity);
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

	else if (output_format->format == OUT_SVG)
	{
		float zoom;
		fz_matrix ctm;
		fz_rect tbounds;

		zoom = resolution / 72;
		ctm = fz_pre_rotate(fz_scale(zoom, zoom), rotation);
		tbounds = fz_transform_rect(mediabox, ctm);

		fz_try(ctx)
		{
			int text_format = ((stext_options.flags & FZ_STEXT_NO_TEXT_AS_PATH) ? FZ_SVG_TEXT_AS_TEXT : FZ_SVG_TEXT_AS_PATH);
			int reuse_images = !(stext_options.flags & FZ_STEXT_NO_REUSE_IMAGES);
			dev = fz_new_svg_device(ctx, out, tbounds.x1-tbounds.x0, tbounds.y1-tbounds.y0, text_format, reuse_images);
			apply_kill_switch(dev);
			if (lowmemory)
				fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
			if (list)
				fz_run_display_list(ctx, list, dev, ctm, tbounds);
			else
				fz_run_page(ctx, page, dev, ctm);
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
					tbounds.y0 += band_height;
					tbounds.y1 += band_height;
					workers[band].cookie = master_cookie;
					workers[band].list = list;
					workers[band].pix = fz_new_pixmap_with_bbox(ctx, colorspace, band_ibounds, seps, alpha);
					workers[band].pix->y += band * band_height;
					fz_set_pixmap_resolution(ctx, workers[band].pix, resolution, resolution);
					workers[band].running = 1;
#ifndef DISABLE_MUTHREADS
					DEBUG_THREADS(fz_info(ctx, "Worker %d, Pre-triggering band %d\n", band, band));
					mu_trigger_semaphore(&workers[band].start);
#endif
				}
				pix = workers[0].pix;
			}
			else
			{
				pix = fz_new_pixmap_with_bbox(ctx, colorspace, band_ibounds, seps, alpha);
				fz_set_pixmap_resolution(ctx, pix, resolution, resolution);
			}

			/* Output any page level headers (for banded formats) */
			if (output_format->format == OUT_PGM || output_format->format == OUT_PPM || output_format->format == OUT_PNM)
				bander = fz_new_pnm_band_writer(ctx, out);
			else if (output_format->format == OUT_PAM)
				bander = fz_new_pam_band_writer(ctx, out);
			else if (output_format->format == OUT_PNG)
				bander = fz_new_png_band_writer(ctx, out);
			else if (output_format->format == OUT_MURAW)
				bander = fz_new_muraw_band_writer(ctx, out);
			else if (output_format->format == OUT_PBM)
				bander = fz_new_pbm_band_writer(ctx, out);
			else if (output_format->format == OUT_PKM)
				bander = fz_new_pkm_band_writer(ctx, out);
			else if (output_format->format == OUT_PS)
				bander = fz_new_ps_band_writer(ctx, out);
			else if (output_format->format == OUT_PSD)
				bander = fz_new_psd_band_writer(ctx, out);
			else if (output_format->format == OUT_WEBP)
				bander = fz_new_webp_band_writer(ctx, out);
			else if (output_format->format == OUT_PWG)
			{
				if (out_cs == CS_MONO)
					bander = fz_new_mono_pwg_band_writer(ctx, out, NULL);
				else
					bander = fz_new_pwg_band_writer(ctx, out, NULL);
			}
			else if (output_format->format == OUT_PCL)
			{
				if (out_cs == CS_MONO)
					bander = fz_new_mono_pcl_band_writer(ctx, out, NULL);
				else
					bander = fz_new_color_pcl_band_writer(ctx, out, NULL);
			}
			else if (output_format->format == OUT_PCLM || output_format->format == OUT_OCR_PDF)
			{
				assert(bander != NULL); // must've been set up already at document level!
			}
			else
			{
				fz_throw(ctx, FZ_ERROR_ABORT, "Internal error: Unhandled output format %d (%s).", output_format->format, output_format->suffix);
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
					ASSERT(ctx != w->ctx);
					ctx->cookie->d.errors += w->cookie.d.errors;
					pix = w->pix;
					bit = w->bit;
					w->bit = NULL;

					if (w->error)
						fz_throw(ctx, FZ_ERROR_GENERIC, "worker %d failed to render band %d", w->num, band);
				}
				else
				{
					drawband(ctx, page, list, ctm, tbounds, band * band_height, pix, &bit);
				}

				if (bander && (pix || bit))
				{
					ASSERT(bander->n == pix->n);
					ASSERT(bander->w == pix->w);
					ASSERT(drawheight <= pix->h);
					fz_write_band(ctx, bander, bit ? bit->stride : pix->stride, drawheight, bit ? bit->samples : pix->samples);
				}
				fz_drop_bitmap(ctx, bit);
				bit = NULL;

				if (num_workers > 0 && band + num_workers < bands)
				{
					worker_t *w = &workers[band % num_workers];
					w->band = band + num_workers;
					w->pix->y = band_ibounds.y0 + w->band * band_height;
					w->ctm = ctm;
					w->tbounds = tbounds;
					w->cookie = master_cookie;
					w->running = 1;
#ifndef DISABLE_MUTHREADS
					DEBUG_THREADS(fz_info(ctx, "Triggering worker %d for band %d\n", w->num, w->band));
					mu_trigger_semaphore(&w->start);
#endif
				}
				if (num_workers <= 0)
					pix->y += band_height;
				tbounds.y0 += band_height;
				tbounds.y1 += band_height;
			}

			if (output_format->format != OUT_PCLM && output_format->format != OUT_OCR_PDF)
			{
				fz_close_band_writer(ctx, bander);
				fz_drop_band_writer(ctx, bander);
				bander = NULL;
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
			if (output_format->format != OUT_PCLM && output_format->format != OUT_OCR_PDF)
			{
				fz_close_band_writer(ctx, bander);
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
			{
				fz_drop_pixmap(ctx, pix);
				pix = NULL;
			}
		}
		fz_catch(ctx)
		{
			if (output_format->format == OUT_PCLM || output_format->format == OUT_OCR_PDF)
			{
				fz_close_band_writer(ctx, bander);
				fz_drop_band_writer(ctx, bander);
				bander = NULL;
			}
			fz_rethrow(ctx);
		}
	}

	if (output_file_per_page)
	{
		file_level_trailers(ctx);

#if FZ_ENABLE_PDF
		if (output_format->format == OUT_PDF)
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

			fz_info(ctx, " pagenum=%d :: %dms (interpretation) %dms (rendering) %dms (total)", pagenum, interptime, diff, diff + interptime);
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

			fz_info(ctx, " pagenum=%d :: %dms (total)", pagenum, diff);
		}
	}

	if (lowmemory)
		fz_empty_store(ctx);

	if (showmemory)
	{
		fz_dump_glyph_cache_stats(ctx);
	}

	fz_flush_warnings(ctx);

	if (ctx->cookie->d.errors)
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
			fz_run_page(ctx, page, dev, fz_identity);
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
		apply_kill_switch(dev);
		if (lowmemory)
			fz_enable_device_hints(ctx, dev, FZ_NO_CACHE);
		fz_try(ctx)
		{
			if (list)
				fz_run_display_list(ctx, list, dev, fz_identity, fz_infinite_rect);
			else
				fz_run_page(ctx, page, dev, fz_identity);
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
		fz_normalize_path(ctx, text_buffer, sizeof text_buffer, text_buffer);
		fz_sanitize_path(ctx, text_buffer, sizeof text_buffer, text_buffer);

		if (output_format->format == OUT_PDF)
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
			if (verbosity >= 1 || showfeatures || showtime || showmd5)
				fz_info(ctx, "page %d file %s features: %s", pagenum, filename, features);

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
		if (verbosity >= 1 || showfeatures || showtime || showmd5)
			fz_info(ctx, "page %d file %s features %s", pagenum, filename, features);
		fz_try(ctx)
			dodrawpage(ctx, page, list, pagenum, start, 0, filename, 0, seps);
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
trace_malloc(void *arg, size_t size   FZDBG_DECL_ARGS)
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
	p = _malloc_dbg(size + sizeof(trace_header), _NORMAL_BLOCK, trace_srcfile, trace_srcline);
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
trace_realloc(void *arg, void *p_, size_t size   FZDBG_DECL_ARGS)
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
		return trace_malloc(arg, size   FZDBG_PASS);
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
		p = _realloc_dbg(&p[-1], size + sizeof(trace_header), _NORMAL_BLOCK, trace_srcfile, trace_srcline);
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
				drawband(me->ctx, NULL, me->list, me->ctm, me->tbounds, band * band_height, me->pix, &me->bit);
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
			fz_clean_cookie(ctx, &bgprint.cookie);
			fz_try(bgprint.ctx)
			{
				dodrawpage(bgprint.ctx, bgprint.page, bgprint.list, pagenum, start, bgprint.interptime, bgprint.filename, 1, bgprint.seps);
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

static void list_layers(fz_context *ctx, fz_document *doc)
{
#if FZ_ENABLE_PDF
	pdf_document *pdoc = pdf_specifics(ctx, doc);
	int k, n = pdf_count_layers(ctx, pdoc);
	for (k = 0; k < n; ++k) {
		const char *name = pdf_layer_name(ctx, pdoc, k);
		int state = pdf_layer_is_enabled(ctx, pdoc, k);
		fprintf(stderr, "layer %d (%s): %s\n", k+1, state ? "on" : "off", name);
	}
#endif
}

static void toggle_layers(fz_context *ctx, fz_document *doc)
{
#if FZ_ENABLE_PDF
	pdf_document *pdoc = pdf_specifics(ctx, doc);
	int i, k, n;

	n = pdf_count_layers(ctx, pdoc);
	for (i = 0; i < layer_off_len; ++i)
	{
		if (layer_off[i] == -1)
			for (k = 0; k < n; ++k)
				pdf_enable_layer(ctx, pdoc, k, 0);
		else if (layer_off[i] >= 1 && layer_off[i] <= n)
			pdf_enable_layer(ctx, pdoc, layer_off[i] - 1, 0);
		else
			fprintf(stderr, "invalid layer: %d\n", layer_off[i]);
	}

	for (i = 0; i < layer_on_len; ++i)
	{
		if (layer_on[i] == -1)
			for (k = 0; k < n; ++k)
				pdf_enable_layer(ctx, pdoc, k, 1);
		else if (layer_on[i] >= 1 && layer_on[i] <= n)
			pdf_enable_layer(ctx, pdoc, layer_on[i] - 1, 1);
		else
			fprintf(stderr, "invalid layer: %d\n", layer_on[i]);
	}
#endif
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

static int img_seqnum = 1;

static void write_image_to_unique_nonexisting_filepath(fz_context* ctx, fz_output* out, fz_buffer* buffer, int pagenum, const fz_stext_options* options, const char *extension)
{
	// make sure we produce a unique, non-existing image file path:
	char tplpath[PATH_MAX];
	char image_path[PATH_MAX];
	char relative_asset_path[PATH_MAX];

	fz_mk_absolute_path_using_absolute_base(ctx, tplpath, sizeof(tplpath), options->reference_image_path_template, out->filepath);
	do
	{
		fz_format_output_path_ex(ctx, image_path, sizeof(image_path), tplpath, 0, pagenum, img_seqnum, (buffer ? NULL : "ILLEGAL-ZERO-SIZED"), extension);
		fz_normalize_path(ctx, image_path, sizeof image_path, image_path);
		fz_sanitize_path(ctx, image_path, sizeof image_path, image_path);
		img_seqnum++;
	} while (fz_file_exists(ctx, image_path));

	fz_mk_relative_path(ctx, relative_asset_path, sizeof(relative_asset_path), image_path, out->filepath);

	// write the constructed image path to the target text/html/... file: HREF
	fz_write_string(ctx, out, relative_asset_path);
	// and write the actual image(data) to a separate file, indicated by the constructed image path:
	fz_save_buffer(ctx, buffer, image_path);
}

static void mudraw_process_stext_referenced_image(fz_context* ctx, fz_output* out, fz_stext_block* block, int pagenum, int object_index, fz_matrix ctm, const fz_stext_options* options)
{
	fz_image* image = block->u.i.image;

	fz_compressed_buffer* cbuf;
	fz_buffer* buf;

	cbuf = fz_compressed_image_buffer(ctx, image);

	if (cbuf && cbuf->params.type == FZ_IMAGE_JPEG)
	{
		int type = fz_colorspace_type(ctx, image->colorspace);
		if (type == FZ_COLORSPACE_GRAY || type == FZ_COLORSPACE_RGB)
		{
			write_image_to_unique_nonexisting_filepath(ctx, out, cbuf->buffer, pagenum, options, ".jpg");
			return;
		}
	}
	if (cbuf && cbuf->params.type == FZ_IMAGE_PNG)
	{
		write_image_to_unique_nonexisting_filepath(ctx, out, cbuf->buffer, pagenum, options, ".png");
		return;
	}

	buf = fz_new_buffer_from_image_as_png(ctx, image, fz_default_color_params);
	fz_try(ctx)
	{
		write_image_to_unique_nonexisting_filepath(ctx, out, buf, pagenum, options, ".png");
	}
	fz_always(ctx)
	{
		fz_drop_buffer(ctx, buf);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}


static int
fz_mkdir(char *path)
{
#ifdef _WIN32
	int ret;
	wchar_t *wpath = fz_wchar_from_utf8(path);

	if (wpath == NULL)
		return -1;

	ret = _wmkdir(wpath);

	free(wpath);

	return ret;
#else
	return mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
}

static int create_accel_path(fz_context *ctx, char outname[], size_t len, int create, const char *absname, ...)
{
	va_list args;
	char *s = outname;
	size_t z, remain = len;
	char *arg;

	va_start(args, absname);

	while ((arg = va_arg(args, char *)) != NULL)
	{
		z = fz_snprintf(s, remain, "%s", arg);
		if (z+1 > remain)
			goto fail; /* won't fit */

		if (create)
			fz_mkdir(outname);
		if (!fz_is_directory(ctx, outname))
			goto fail; /* directory creation failed, or that dir doesn't exist! */
#ifdef _WIN32
		s[z] = '\\';
#else
		s[z] = '/';
#endif
		s[z+1] = 0;
		s += z+1;
		remain -= z+1;
	}

	if (fz_snprintf(s, remain, "%s.accel", absname) >= remain)
		goto fail; /* won't fit */

	va_end(args);

	return 1;

fail:
	va_end(args);

	return 0;
}

static int convert_to_accel_path(fz_context *ctx, char outname[], char *absname, size_t len, int create)
{
	char *tmpdir;
	char *s;

	// strip off absolute path & UNC / path prefixes '//./' and '//?/':
	while (absname[0] == '/' || absname[0] == '\\' || absname[0] == '?' || absname[0] == '?')
		++absname;

	s = absname;
	while (*s) {
		if (*s == '/' || *s == '\\' || *s == ':')
			*s = '%';
		++s;
	}

#ifdef _WIN32
	tmpdir = getenv("USERPROFILE");
	if (tmpdir && create_accel_path(ctx, outname, len, create, absname, tmpdir, ".config", "mupdf", NULL))
		return 1; /* OK! */
	/* TEMP and TMP are user-specific on modern windows. */
	tmpdir = getenv("TEMP");
	if (tmpdir && create_accel_path(ctx, outname, len, create, absname, tmpdir, "mupdf", NULL))
		return 1; /* OK! */
	tmpdir = getenv("TMP");
	if (tmpdir && create_accel_path(ctx, outname, len, create, absname, tmpdir, "mupdf", NULL))
		return 1; /* OK! */
#else
	tmpdir = getenv("XDG_CACHE_HOME");
	if (tmpdir && create_accel_path(ctx, outname, len, create, absname, tmpdir, "mupdf", NULL))
		return 1; /* OK! */
	tmpdir = getenv("HOME");
	if (tmpdir && create_accel_path(ctx, outname, len, create, absname, tmpdir, ".cache", "mupdf", NULL))
		return 1; /* OK! */
#endif
	return 0; /* Fail */
}

static int get_accelerator_filename(fz_context *ctx, char outname[], size_t len, const char *filename, int create)
{
	char absname[PATH_MAX];
	if (!fz_realpath(filename, absname))
		return 0;
	if (!convert_to_accel_path(ctx, outname, absname, len, create))
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
	if (!get_accelerator_filename(ctx, absname, sizeof(absname), fname, 1))
		return;

	fz_save_accelerator(ctx, doc, absname);
}

static float humanize(size_t value, const char **unit)
{
	float result;
	if (value >= 1024 * 1024 * 1024.0f)
	{
		*unit = "G";
		result = value / (1024 * 1024 * 1024.0f);
	}
	else if (value >= 1024 * 1024.0f)
	{
		*unit = "M";
		result = value / (1024 * 1024.0f);
	}
	else if (value >= 1024.0f)
	{
		*unit = "k";
		result = value / 1024.0f;
	}
	else
	{
		*unit = "";
		result = value;
	}

	return result;
}

/* Reduced quality sub pix quantizer, intended to match pdfium. */
static void reduced_sub_pix_quantizer(float size, int *x, int *y)
{
	*x = 3;
	*y = 1;
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

static int
fz_strieq(const char* arg, const char* word)
{
	return fz_strcasecmp(arg, word) == 0;
}

static const suffix_t* get_output_format(int code)
{
	int i;

	for (i = 0; i < (int)nelem(suffix_table); i++)
	{
		if (suffix_table[i].format == code)
		{
			return &suffix_table[i];
		}
	}
	fz_throw(ctx, FZ_ERROR_GENERIC, "Unknown output format %d", code);
}

static void
parse_render_options(fz_context* ctx, fz_cookie* cookie, const char* spec)
{
	const char* SEPS = " ,;:";
	size_t len = strlen(spec);
	char* buf = fz_malloc(ctx, len + 2);

	if (!buf)
		return;

	memcpy(buf, spec, len + 1);
	assert(buf[len] == 0);
	buf[len + 1] = 0;  // second sentinel, reqd for the strcspn() loop below

	char* arg = buf;
	while (*arg)
	{
		size_t wlen = strcspn(arg, SEPS);
		arg[wlen] = 0;

		if (fz_strnieq(arg, 9, "max_nodes"))
		{
			unsigned int v = 0;
			if (1 == sscanf(arg + 9, "=%u", &v))
			{
				cookie->d.max_nodes_per_page_render = v;
			}
			else
			{
				fz_error(ctx, "Invalid 'max_nodes=NNN' limit specified as part of the render filter: '%s'. Treating it as NO LIMIT.\n", arg);
			}
		}
		else if (fz_strnieq(arg, 8, "max_time"))
		{
			unsigned int v = 0;
			if (1 == sscanf(arg + 8, "=%u", &v))
			{
				cookie->d.max_msecs_per_page_render = v;
			}
			else
			{
				fz_error(ctx, "Invalid 'max_time=NNN' millisecond time limit specified as part of the render filter: '%s'. Treating it as NO LIMIT.\n", arg);
			}
		}
		else if (fz_strieq(arg, "everything"))
			cookie->d.run_mode = FZ_RUN_EVERYTHING;
		else if (fz_strieq(arg, "content"))
			cookie->d.run_mode |= FZ_RUN_CONTENT;
		else if (fz_strieq(arg, "annotations"))
			cookie->d.run_mode |= FZ_RUN_ANNOTATIONS;
#if FZ_ENABLE_PDF
		else if (fz_strieq(arg, "unknown"))
			cookie->d.run_annotations_reject_mask[PDF_ANNOT_UNKNOWN + 1] = 1;
		else
		{
			// check if the item matches any of the annotation types:
			enum fz_annot_type type = pdf_annot_type_from_string(ctx, arg);
			if (type == PDF_ANNOT_UNKNOWN)
			{
				fz_error(ctx, "Unrecognized annotation type '%s' specified as part of the render filter. Treating it as UNKNOWN annotation type.\n", arg);
			}
			cookie->d.run_annotations_reject_mask[type + 1] = 1;
		}
#else
		else
		{
			fz_error(ctx, "Unrecognized annotation type '%s' specified as part of the render filter. Treating it as UNKNOWN annotation type.\n", arg);
		}
#endif

		arg += wlen + 1;    // <-- *this* is why we need TWO sentinels.
		arg += strspn(arg, SEPS);
	}

	fz_free(ctx, buf);
}

static void mu_drop_context(void)
{
	if (mudraw_is_toplevel_ctx)
	{
		if (trace_info.allocs && (trace_info.mem_limit || trace_info.alloc_limit || showmemory))
		{
			float total, peak, current;
			const char* total_unit;
			const char* peak_unit;
			const char* current_unit;

			ASSERT(ctx != NULL);

			total = humanize(trace_info.total, &total_unit);
			peak = humanize(trace_info.peak, &peak_unit);
			current = humanize(trace_info.current, &current_unit);

			fz_info(ctx, "Memory use total=%.2f%s peak=%.2f%s current=%.2f%s",
				total, total_unit, peak, peak_unit, current, current_unit);
			fz_info(ctx, "Allocations total=%zu", trace_info.allocs);

			// reset heap tracing after reporting: this atexit handler MAY be invoked multiple times!
			memset(&trace_info, 0, sizeof(trace_info));
		}
	}

	// WARNING: as we point `ctx` at the GLOBAL context in the app init phase, it MAY already be an INVALID
	// pointer reference by now!
	// 
	// WARNING: this assert fires when you run `mutool raster` (and probably other tools as well) and hit Ctrl+C
	// to ABORT/INTERRUPT the running application: the MSVC RTL calls this function in the atexit() handler
	// and the assert fires due to (ctx->error.top != ctx->error.stack).
	//
	// We are okay with that, as that scenario is an immediate abort anyway and the OS will be responsible
	// for cleaning up. That our fz_try/throw/catch exception stack hasn't been properly rewound at such times
	// is obvious, I suppose...
	ASSERT_AND_CONTINUE(!ctx || !fz_has_global_context() || (ctx->error.top == ctx->error.stack_base));

	if (ctx != __fz_get_RAW_global_context())
	{
		fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
		ctx = NULL;
	}

	// nuke the locks last as they are still used by the heap free ('drop') calls in the lines just above!
	if (mudraw_is_toplevel_ctx)
	{
		// as we registered a global context, we should clean the locks on it now
		// so the atexit handler won't have to bother with it.
		ASSERT_AND_CONTINUE(fz_has_global_context());
		if (fz_has_global_context())
		{
			ctx = fz_get_global_context();
			fz_drop_context_locks(ctx);
		}
		ctx = NULL;

		fz_drop_global_context();

#ifndef DISABLE_MUTHREADS
		fin_mudraw_locks();
#endif /* DISABLE_MUTHREADS */

		mudraw_is_toplevel_ctx = 0;
	}
}

#if !defined(MUDRAW_STANDALONE) && defined(BUILD_MONOLITHIC)
#define main(cnt, arr)      mudraw_main(cnt, arr)
#endif

int main(int argc, const char** argv)
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
	//mudraw_is_toplevel_ctx = 0;
	//ctx = NULL;
	output = NULL;
	out = NULL;
	output_pagenum = 0;
	output_file_per_page = 0;

	format = NULL;
	output_format = NULL;

	user_specified_rotation = -1;
	user_specified_resolution = 72;
	rotation = -1;
	resolution = -1;
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
	subpix_preset = 0;

	out_cs = CS_UNSET;
	proof_filename = NULL;
	proof_cs = NULL;
	icc_filename = NULL;
	use_gamma = 0;
	gamma_value = 1;
	invert = 0;
	band_height = 0;
	lowmemory = 0;

	kill = 0;
	verbosity = 1;
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
	ocr_datadir = NULL;

	memset(&bgprint, 0, sizeof(bgprint));
	memset(&timing, 0, sizeof(timing));

	memset(&master_cookie, 0, sizeof(master_cookie));
	master_cookie.d.run_mode = FZ_RUN_EVERYTHING;

	memset(&stext_options, 0, sizeof(stext_options));

	gettime_once = 1;

	img_seqnum = 1;

	// ---

	bgprint.active = 0;			/* set by -P */
	num_workers = 0;

	if (!fz_has_global_context())
	{
		fz_locks_context* locks = NULL;
		
		ASSERT(ctx == NULL);

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

		mudraw_is_toplevel_ctx = 1;
	}
	else if (!ctx)
	{
		// caller of mudraw_main() has set global CTX.
		//
		// check if that CTX has locking, because if it has none,
		// then we CANNOT use bands or threads!
		ctx = fz_get_global_context();
	}

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	atexit(mu_drop_context);

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "qp:o:F:R:r:w:h:fB:c:e:gG:Is:A:DiW:H:S:T:t:d:U:XLvVPl:y:Yz:Z:NO:am:x:hj:J:K")) != -1)
	{
		switch (c)
		{
		default: return usage();

		case 'q': verbosity = 0; fz_default_error_warn_info_mode(1, 1, 1); break;
		case 'v': verbosity++; fz_default_error_warn_info_mode(0, 0, 0); break;

		case 'p': password = fz_optarg; break;

		case 'o': output = fz_optarg; break;
		case 'F': format = fz_optarg; break;

		case 'R': user_specified_rotation = read_rotation(fz_optarg); break;
		case 'r': user_specified_resolution = fz_atof(fz_optarg); res_specified = 1; break;
		case 'w': width = fz_atof(fz_optarg); break;
		case 'h': height = fz_atof(fz_optarg); break;
		case 'f': fit = 1; break;
		case 'B': band_height = atoi(fz_optarg); break;

		case 'c': out_cs = parse_colorspace(fz_optarg); break;
		case 'e': proof_filename = fz_optarg; break;
		case 'G': gamma_value = fz_atof(fz_optarg); break;
		case 'g':
#if FZ_ENABLE_GAMMA
			use_gamma = 1;
#else
			fz_warn(ctx, "Gamma blending not supported in this build");
#endif
			break;
		case 'I': invert++; break;
		case 'j': parse_render_options(ctx, &master_cookie, fz_optarg); break;
		case 'J': fz_default_png_compression_level(fz_atoi(fz_optarg)); break;

		case 'W': layout_w = fz_atof(fz_optarg); break;
		case 'H': layout_h = fz_atof(fz_optarg); break;
		case 'S': layout_em = fz_atof(fz_optarg); break;
		case 'U': layout_css = fz_optarg; break;
		case 'X': layout_use_doc_css = 0; break;

		case 'K': ++kill; break;

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
			{
				alphabits_text = atoi(sep+1);
				sep = strchr(sep+1, '/');
				if (sep)
				{
					subpix_preset = atoi(sep+1);
				}
			}
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
			num_workers = fz_parse_pool_threadcount_preference(fz_optarg, 0, 0, 0);
			break;
#else
			fz_warn(ctx, "Threads not enabled in this build");
			break;
#endif
		case 'd':
#if FZ_ENABLE_OCR
			ocr_datadir = fz_optarg; break;
#else
			fz_warn(ctx, "OCR functionality not enabled in this build\n");
			break;
#endif
		case 't':
#if FZ_ENABLE_OCR
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
				num_workers = fz_maxi(2, fz_get_cpu_core_count());
			break;
#else
			fz_warn(ctx, "Threads not enabled in this build");
			break;
#endif
		case 'y': layer_config = fz_optarg; break;
		case 'Y': layer_list = 1; break;
		case 'z': layer_off[layer_off_len++] = !strcmp(fz_optarg, "all") ? -1 : fz_atoi(fz_optarg); break;
		case 'Z': layer_on[layer_on_len++] = !strcmp(fz_optarg, "all") ? -1 : fz_atoi(fz_optarg); break;
		case 'a': useaccel = 0; break;
		case 'x': txtdraw_options = fz_optarg; break;

		case 'V': fz_info(ctx, "mudraw version %s", FZ_VERSION); return EXIT_FAILURE;
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
		verbosity = 0; /* automatically be quiet if printing to stdout */
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

	ASSERT(fz_has_global_context());

	// check if global CTX has locking, because if it has none,
	// then we CANNOT use bands or threads!
	ASSERT(ctx != NULL);
	if (!fz_has_locking_support(ctx))
	{
		fz_error(ctx, "cannot use multiple threads without locking support. Falling back to single thread processing.");
		num_workers = 0;
		bgprint.active = 0;
		band_height = 0;
	}

	if (ctx != __fz_get_RAW_global_context())
	{
		fz_drop_context(ctx); // drop our previous context IFF this happens to be a re-run in monolithic mode.
		ctx = NULL;
	}

	ctx = fz_new_context(NULL, NULL, max_store);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_attach_cookie_to_context(ctx, &master_cookie);

	if (use_gamma)
		fz_set_gamma_blending(ctx, 1);

	fz_try(ctx)
	{
		if (proof_filename)
		{
			fz_buffer *proof_buffer = fz_read_file(ctx, proof_filename);
			proof_cs = fz_new_icc_colorspace(ctx, FZ_COLORSPACE_NONE, 0, NULL, proof_buffer);
			fz_drop_buffer(ctx, proof_buffer);
		}

		fz_parse_stext_options(ctx, &stext_options, txtdraw_options);
		// set up a default graphics output file path template when none has been provided by the CLI/user already:
		if (!stext_options.reference_image_path_template)
		{
			// produce a path template which has the filename extension already generalized:
			size_t l = fz_strrcspn(output, "./\\:");
			char pathbuf[PATH_MAX];
			fz_strncpy_s(ctx, pathbuf, output, fz_mini(l + 1, sizeof(pathbuf)));

			char tplpath[PATH_MAX];
			if (!fz_realpath(pathbuf, tplpath))
			{
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot process images' path template to a sane absolute path: %s", pathbuf);
			}
			fz_normalize_path(ctx, tplpath, sizeof tplpath, tplpath);
			fz_sanitize_path(ctx, tplpath, sizeof tplpath, tplpath);

			// and any `%d` regular page format specifiers removed (replaced!) as well!
			fz_sanitize_path_ex(tplpath, "f%#^$!", "_", 0, 0);
			stext_options.reference_image_path_template = fz_strdup(ctx, tplpath);
		}
		fz_set_stext_options_images_handler(ctx, &stext_options, mudraw_process_stext_referenced_image, NULL);

		fz_set_text_aa_level(ctx, alphabits_text);
		fz_set_graphics_aa_level(ctx, alphabits_graphics);
		fz_set_graphics_min_line_width(ctx, min_line_width);
		if (subpix_preset)
			fz_set_graphics_sub_pix_quantizer(ctx, reduced_sub_pix_quantizer);
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
			fz_attach_cookie_to_context(bgprint.ctx, &bgprint.cookie);
			fail |= mu_create_semaphore(&bgprint.start);
			fail |= mu_create_semaphore(&bgprint.stop);
			fail |= mu_create_thread(&bgprint.thread, bgprint_worker, NULL);
			if (fail)
			{
				fz_detach_cookie_from_context(bgprint.ctx);
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
				fz_attach_cookie_to_context(workers[i].ctx, &workers[i].cookie);
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

		/* band_height can be legally 0 when num_workers = 0 */
		if (band_height < 0)
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "Bandheight must be > 0");
		}
		assert(num_workers > 0 ? band_height != 0 : 1);

		/* Determine output type */

		// default output format: 
		output_format = get_output_format(OUT_PNG);

		if (format)
		{
			int i;

			for (i = 0; i < (int)nelem(suffix_table); i++)
			{
				if (!stricmp(format, suffix_table[i].suffix+1))
				{
					output_format = &suffix_table[i];
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
			int i;

			for (i = 0; i < (int)nelem(suffix_table); i++)
			{
				const char* suffix_ref = suffix_table[i].suffix;
				const char* s = output + fz_maxi(0, (int)strlen(output) - (int)strlen(suffix_ref));

				if (!stricmp(s, suffix_ref))
				{
					output_format = &suffix_table[i];
					if (spots == SPOTS_FULL && suffix_table[i].spots == 0)
					{
						fz_error(ctx, "Output format '%s' does not support spot rendering.\nDoing overprint simulation instead.", suffix_ref+1);
						spots = SPOTS_OVERPRINT_SIM;
					}
					break;
				}
			}
		}

		if (band_height)
		{
			if (output_format->format != OUT_PAM &&
				output_format->format != OUT_PGM &&
				output_format->format != OUT_PPM &&
				output_format->format != OUT_PNM &&
				output_format->format != OUT_PNG &&
				output_format->format != OUT_MURAW &&
				output_format->format != OUT_PBM &&
				output_format->format != OUT_PKM &&
				output_format->format != OUT_PCL &&
				output_format->format != OUT_PCLM &&
				output_format->format != OUT_PS &&
				output_format->format != OUT_PSD &&
				output_format->format != OUT_OCR_PDF)
			{
				fz_error(ctx, "Banded operation only possible with PxM, PCL, PCLM, PDFOCR, PS, PSD, MURAW and PNG outputs");
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
				if (format_cs_table[i].format == output_format->format)
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
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unsupported colorspace (%d) for this format (%d: %s)", out_cs, output_format->format, output_format->suffix);
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
				if (format_cs_table[i].format == output_format->format)
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

		if (output_format->format == OUT_SVG || output_format->format == OUT_PNG || output_format->format == OUT_MURAW || output_format->format == OUT_PSD)
		{
			/* SVG files are always opened for each page. */
			output_file_per_page = 1;
		}

		if ((fz_has_page_format_marker(output) || output_file_per_page) &&
			strcmp(output, "/dev/stdout") && strcmp(output, "/dev/null") && fz_strcasecmp(output, "nul:"))
		{
			output_file_per_page = 1;
		}
		else
		{
			output_file_per_page = 0;

			char file_path[PATH_MAX];
			fz_format_output_path(ctx, file_path, sizeof file_path, output, 0);
			fz_normalize_path(ctx, file_path, sizeof file_path, file_path);
			fz_sanitize_path(ctx, file_path, sizeof file_path, file_path);
			out = fz_new_output_with_path(ctx, file_path, 0);
		}

		if (output_format->format == OUT_PDF)
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
		if (output_format->format == OUT_OCR_TRACE ||
			output_format->format == OUT_OCR_TEXT ||
			output_format->format == OUT_OCR_STEXT_JSON ||
			output_format->format == OUT_OCR_STEXT_XML ||
			output_format->format == OUT_OCR_XMLTEXT ||
			output_format->format == OUT_OCR_HTML ||
			output_format->format == OUT_OCR_XHTML ||
			output_format->format == OUT_OCR_PDF)
		{
			void* tess_api = NULL;

			fz_try(ctx)
			{
#if FZ_ENABLE_OCR_OUTPUT && FZ_ENABLE_OCR
				tess_api = ocr_init(ctx, ocr_language, ocr_datadir);
#else
				fz_throw(ctx, FZ_ERROR_GENERIC, "OCR not supported in this build");
#endif
			}
			fz_always(ctx)
			{
#if FZ_ENABLE_OCR_OUTPUT && FZ_ENABLE_OCR
				ocr_fin(ctx, tess_api);
#endif
			}
			fz_catch(ctx)
			{
				fz_error(ctx, "warning: tesseract OCR engine could not be initialized. Falling back to the non-OCR-ed output format! %s", fz_caught_message(ctx));
				switch (output_format->format)
				{
				case OUT_OCR_TRACE:
					output_format = get_output_format(OUT_TRACE); break;
				case OUT_OCR_TEXT:
					output_format = get_output_format(OUT_TEXT); break;
				case OUT_OCR_STEXT_JSON:
					output_format = get_output_format(OUT_STEXT_JSON); break;
				case OUT_OCR_STEXT_XML:
					output_format = get_output_format(OUT_STEXT_XML); break;
				case OUT_OCR_XMLTEXT:
					output_format = get_output_format(OUT_XMLTEXT); break;
				case OUT_OCR_HTML:
					output_format = get_output_format(OUT_HTML); break;
				case OUT_OCR_XHTML:
					output_format = get_output_format(OUT_XHTML); break;
				case OUT_OCR_PDF:
					output_format = get_output_format(OUT_PDF); break;
				}
			}
		}

		// report output format in verbose mode:
		if (verbosity >= 1)
		{
			int i;
			const char* fmtstr = ".???";

			for (i = 0; i < (int)nelem(suffix_table); i++)
			{
				if (output_format->format == suffix_table[i].format)
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
				file_level_headers(ctx, fz_optind < argc ? argv[fz_optind] : "-");
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
					else if (get_accelerator_filename(ctx, accelpath, sizeof(accelpath), filename, 0))
					{
						/* Check whether that file exists, and isn't older than
						 * the document. */
						atime = fz_stat_mtime(ctx, accelpath);
						dtime = fz_stat_mtime(ctx, filename);
						if (atime == 0)
						{
							/* No accelerator */
						}
						else if (atime > dtime)
							accel = accelpath;
						else
						{
							/* Accelerator data is out of date */
							fz_remove_utf8(ctx, accelpath);
							accel = NULL; /* In case we have jumped up from below */
						}
					}

					doc = fz_open_accelerated_document(ctx, filename, accel);

#ifdef CLUSTER
					/* Load and then drop the outline if we're running under the cluster.
					 * This allows our outline handling to be tested automatically. */
					fz_try(ctx)
						fz_drop_outline(ctx, fz_load_outline(ctx, doc));
					fz_catch(ctx)
					{
						/* Drop any error */
					}
#endif

					if (fz_needs_password(ctx, doc))
					{
						if (!fz_authenticate_password(ctx, doc, password))
							fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
					}

#ifdef CLUSTER
					/* Load and then drop the outline if we're running under the cluster.
					 * This allows our outline handling to be tested automatically. */
					fz_try(ctx)
						fz_drop_outline(ctx, fz_load_outline(ctx, doc));
					fz_catch(ctx)
					{
						/* Drop any error */
					}
#endif

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
					if (layer_on_len > 0 || layer_off_len > 0)
						toggle_layers(ctx, doc);
					if (layer_list)
						list_layers(ctx, doc);

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
			fz_log_error(ctx, fz_caught_message(ctx));
			fz_log_error_printf(ctx, "cannot draw '%s'", filename);
			errored = 1;
		}

		if (!output_file_per_page)
			file_level_trailers(ctx);

#if FZ_ENABLE_PDF
		if (pdfout)
		{
			char file_path[PATH_MAX];
			fz_format_output_path(ctx, file_path, sizeof file_path, output, 0);
			fz_normalize_path(ctx, file_path, sizeof file_path, file_path);
			fz_sanitize_path(ctx, file_path, sizeof file_path, file_path);

			pdf_save_document(ctx, pdfout, file_path, NULL);
			pdf_drop_document(ctx, pdfout);
			pdfout = NULL;
		}
#endif

		{
			if (!errored)
			{
				fz_close_output(ctx, out);
			}
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
		fz_drop_stext_options(ctx, &stext_options);
	}
	fz_catch(ctx)
	{
		fz_log_error(ctx, fz_caught_message(ctx));
		if (!errored) {
			fz_error(ctx, "Rendering failed.");
			errored = 1;
		}
	}

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}

#endif
