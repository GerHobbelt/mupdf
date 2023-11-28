// Copyright (C) 2004-2023 Artifex Software, Inc.
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
 * muconvert -- command line tool for converting documents
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/jmemcust.h"

#include "mupdf/assertions.h"
#include <stdlib.h>
#include <stdio.h>

#if FZ_ENABLE_RENDER_CORE

/* input options */
static const char *password = "";
static int alphabits = 8;
static float layout_w = FZ_DEFAULT_LAYOUT_W;
static float layout_h = FZ_DEFAULT_LAYOUT_H;
static float layout_em = FZ_DEFAULT_LAYOUT_EM;
static const char *layout_css = NULL;
static int layout_use_doc_css = 1;

/* output options */
static const char *output = NULL;
static const char *format = NULL;
static const char *options = "";

static fz_context *ctx;
static fz_document *doc;
static fz_document_writer *out;
static fz_box_type page_box = FZ_CROP_BOX;
static int count;

static int usage(void)
{
	fz_info(ctx,
		"Usage: mutool convert [options] file [pages]\n"
		"  -p -   password\n"
		"\n"
		"  -b -    use named page box (MediaBox, CropBox, BleedBox, TrimBox, or ArtBox)\n"
		"  -A -    number of bits of antialiasing (0 to 8)\n"
		"  -W -    page width for EPUB layout\n"
		"  -H -    page height for EPUB layout\n"
		"  -S -    font size for EPUB layout\n"
		"  -U -    file name of user stylesheet for EPUB layout\n"
		"  -X      disable document styles for EPUB layout\n"
		"\n"
		"  -o -    output file name (%%d for page number)\n"
		"  -F -    output format (default inferred from output file name)\n"
		"          raster: cbz, png, pnm, pgm, ppm, pam, pbm, pkm.\n"
		"          print-raster: pcl, pclm, ps, pwg.\n"
		"          vector: pdf, svg.\n"
		"          text: html, xhtml, text, stext.\n"
		"  -O -    comma separated list of options for output format\n"
		"\n"
		"  pages   comma separated list of page ranges (N=last page)\n"
		"\n"
	);
	fz_info(ctx, "%s", fz_draw_options_usage);
	fz_info(ctx, "%s", fz_pcl_write_options_usage);
	fz_info(ctx, "%s", fz_pclm_write_options_usage);
	fz_info(ctx, "%s", fz_pwg_write_options_usage);
	fz_info(ctx, "%s", fz_stext_options_usage);
#if FZ_ENABLE_PDF
	fz_info(ctx, "%s", fz_pdf_write_options_usage);
#endif
	fz_info(ctx, "%s", fz_svg_write_options_usage);

	return EXIT_FAILURE;
}

static void runpage(int number)
{
	fz_rect box;
	fz_page *page;
	fz_device *dev = NULL;
	fz_matrix ctm;

	fz_info(ctx, "processing page %d\n", number);

	page = fz_load_page(ctx, doc, number - 1);

	fz_var(dev);

	fz_try(ctx)
	{
		box = fz_bound_page_box(ctx, page, page_box);

		// Realign page box on 0,0
		ctm = fz_translate(-box.x0, -box.y0);
		box = fz_transform_rect(box, ctm);

		dev = fz_begin_page(ctx, out, box);
		fz_run_page(ctx, page, dev, ctm);
		fz_end_page(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, page);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void runrange(const char *range)
{
	int start, end, i;

	while ((range = fz_parse_page_range(ctx, range, &start, &end, count)))
	{
		if (start < end)
			for (i = start; i <= end; ++i)
				runpage(i);
		else
			for (i = start; i >= end; --i)
				runpage(i);
	}
}

int muconvert_main(int argc, const char** argv)
{
	int i, c;
	int retval = EXIT_SUCCESS;

	/* input options */
	password = "";
	alphabits = 8;
	layout_w = FZ_DEFAULT_LAYOUT_W;
	layout_h = FZ_DEFAULT_LAYOUT_H;
	layout_em = FZ_DEFAULT_LAYOUT_EM;
	layout_css = NULL;
	layout_use_doc_css = 1;

	/* output options */
	output = NULL;
	format = NULL;
	options = "";

	ctx = NULL;
	doc = NULL;
	out = NULL;
	page_box = FZ_MEDIA_BOX;
	count = 0;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "p:A:W:H:S:U:Xo:F:O:b:")) != -1)
	{
		switch (c)
		{
		default: return usage();

		case 'p': password = fz_optarg; break;
		case 'A': alphabits = atoi(fz_optarg); break;
		case 'W': layout_w = fz_atof(fz_optarg); break;
		case 'H': layout_h = fz_atof(fz_optarg); break;
		case 'S': layout_em = fz_atof(fz_optarg); break;
		case 'U': layout_css = fz_optarg; break;
		case 'X': layout_use_doc_css = 0; break;

		case 'o': output = fz_optarg; break;
		case 'F': format = fz_optarg; break;
		case 'O': options = fz_optarg; break;

		case 'b':
			page_box = fz_box_type_from_string(fz_optarg);
			if (page_box == FZ_UNKNOWN_BOX)
			{
				fprintf(stderr, "Invalid box type: %s\n", fz_optarg);
				return 1;
			}
			break;
		}
	}

	if (fz_optind == argc || (!format && !output))
	{
		return usage();
	}

	if (!fz_has_global_context())
	{
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}

	/* Create a context to hold the exception stack and various caches. */
	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	/* Register the default file types to handle. */
	fz_try(ctx)
		fz_register_document_handlers(ctx);
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fz_log_error(ctx, "cannot register document handlers.");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	fz_set_aa_level(ctx, alphabits);

	if (layout_css)
	{
		fz_buffer *buf = fz_read_file(ctx, layout_css);
		fz_set_user_css(ctx, fz_string_from_buffer(ctx, buf));
		fz_drop_buffer(ctx, buf);
	}

	fz_set_use_document_css(ctx, layout_use_doc_css);

	/* Open the output document. */
	fz_try(ctx)
    {
		if (output[0] == '-' && output[1] == 0)
		{
			out = fz_new_document_writer_with_output(ctx, fz_stdout(ctx), format, options);
		}
		else
		{
	        char file_path[PATH_MAX];
	        fz_format_output_path(ctx, file_path, sizeof file_path, output, 0);
	        fz_normalize_path(ctx, file_path, sizeof file_path, file_path);
	        fz_sanitize_path(ctx, file_path, sizeof file_path, file_path);
	        out = fz_new_document_writer(ctx, file_path, format, options);
		}
    }
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		fz_log_error_printf(ctx, "cannot create document: %s", output);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	fz_var(doc);

	assert(retval == EXIT_SUCCESS);
	{
		const char *infname = NULL;

		fz_try(ctx)
		{
			for (i = fz_optind; i < argc; ++i)
			{
				infname = argv[i];
				doc = fz_open_document(ctx, infname);
				if (fz_needs_password(ctx, doc))
					if (!fz_authenticate_password(ctx, doc, password))
						fz_throw(ctx, FZ_ERROR_ARGUMENT, "cannot authenticate password: %s", infname);
				fz_layout_document(ctx, doc, layout_w, layout_h, layout_em);
				count = fz_count_pages(ctx, doc);

				if (i + 1 < argc && fz_is_page_range(ctx, argv[i + 1]))
					runrange(argv[++i]);
				else
					runrange("1-N");

				fz_drop_document(ctx, doc);
				doc = NULL;
	    	}
	        fz_close_document_writer(ctx, out);
		}
		fz_always(ctx)
		{
			fz_drop_document(ctx, doc);
			fz_drop_document_writer(ctx, out);
		}
		fz_catch(ctx)
		{
			fz_report_error(ctx);
			fz_log_error_printf(ctx, "cannot load document: %s", infname);
			retval = EXIT_FAILURE;
		}
	}

	fz_drop_context(ctx);

	// and delete incomplete/damaged output file:
	if (retval != EXIT_SUCCESS && output != NULL)
	{
		unlink(output);
	}

	return retval;
}

#endif
