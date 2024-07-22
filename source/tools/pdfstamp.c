// Copyright (C) 2004-2024 Artifex Software, Inc.
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

/* PDF stamp tool. */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	fz_buffer *stamp;
	pdf_obj *stampfont;
	pdf_page *page;
	pdf_document *doc;
} stamp_info;

static void
after_page(fz_context *ctx, fz_buffer *buffer, pdf_obj *res, void *opaque)
{
	stamp_info *si = opaque;
	fz_rect rect = { 0 };
	fz_rect filled;
	fz_story *story;
	pdf_page *page = si->page;
	fz_buffer *buf = NULL;
	fz_device *dev;
	fz_matrix ctm;

	fz_append_string(ctx, buffer, " ");

	pdf_page_transform(ctx, page, &rect, &ctm);

	story = fz_new_story(ctx, si->stamp, NULL, 10, NULL);
	if (fz_place_story(ctx, story, rect, &filled))
	{
		fz_warn(ctx, "Failed to place story!");
		fz_drop_story(ctx, story);
		return;
	}
	/* Now offset down to the bottom of the page */
	ctm.f += rect.y0 - rect.y1 + filled.y1;

	fz_try(ctx)
	{
		buf = fz_new_buffer(ctx, 1024);
		dev = pdf_new_pdf_device(ctx, si->doc, ctm, res, buf);
		fz_draw_story(ctx, story, dev, fz_identity);
		fz_close_device(ctx, dev);
		fz_append_buffer(ctx, buffer, buf);
	}
	fz_always(ctx)
	{
		fz_drop_story(ctx, story);
		fz_drop_buffer(ctx, buf);
		fz_drop_device(ctx, dev);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);

}

static void
rewrite_page_streams(fz_context *ctx, pdf_document *doc, int page_num, fz_buffer *stamp, pdf_obj *stampfont)
{
	pdf_page *page = pdf_load_page(ctx, doc, page_num);
	pdf_filter_options options = { 0 };
	pdf_filter_factory list[2] = { 0 };
	pdf_sanitize_filter_options copts = { 0 };
	stamp_info si = { stamp, stampfont, page, doc };

	options.opaque = &si;
	options.filters = list;
	options.complete = after_page;
	list[0].filter = pdf_new_sanitize_filter;
	list[0].options = &copts;

	fz_try(ctx)
		pdf_filter_page_contents(ctx, doc, page, &options);
	fz_always(ctx)
		fz_drop_page(ctx, &page->super);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static int
usage(void)
{
	fprintf(stderr, "usage: mutool stamp [options] <input filename>\n");
	fprintf(stderr, "\t-s -\tMessage to stamp\n");
	fprintf(stderr, "\t-o -\tOutput file\n");
	return 1;
}

int pdfstamp_main(int argc, const char **argv)
{
	fz_context *ctx = NULL;
	pdf_document *pdf = NULL;
	fz_document *doc = NULL;
	pdf_write_options opts = pdf_default_write_options;
	int n, i, c;
	const char *infile = NULL;
	const char *outputfile = NULL;
	int code = EXIT_SUCCESS;
	const char *stampstr = NULL;
	pdf_obj *stampfont = NULL;
	fz_font *font = NULL;
	fz_buffer *stampbuf = NULL;

	while ((c = fz_getopt(argc, argv, "s:o:")) != -1)
	{
		switch (c)
		{
		default: return usage();

		// color convert
		case 's': stampstr = fz_optarg; break;
		case 'o': outputfile = fz_optarg; break;
		}
	}

	if (fz_optind == argc || !outputfile)
		return usage();

	infile = argv[fz_optind];

	if (stampstr == NULL)
	{
		fprintf(stderr, "Unknown colorspace\n");
		return usage();
	}

	/* Set up the options for the file saving. */
#if 1
	opts.do_compress = 1;
	opts.do_compress_images = 1;
	opts.do_compress_fonts = 1;
	opts.do_garbage = 3;
	opts.do_use_objstms = 1;
#else
	opts.do_pretty = 1;
	opts.do_compress = 0;
	opts.do_compress_images = 1;
	opts.do_compress_fonts = 0;
	opts.do_garbage = 0;
	opts.do_clean = 1;
#endif

	/* Create a MuPDF library context. */
	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fprintf(stderr, "Could not create global context.\n");
		return EXIT_FAILURE;
	}

	/* Register the document handlers (only really need PDF, but this is
	 * the simplest way. */
	fz_register_document_handlers(ctx);

	fz_try(ctx)
	{
		/* Load the input document. */
		doc = fz_open_document(ctx, infile);

		/* Get a PDF specific pointer, and count the pages. */
		pdf = pdf_document_from_fz_document(ctx, doc);
		n = fz_count_pages(ctx, doc);

		stampbuf = fz_new_buffer_from_copied_data(ctx, (const unsigned char *)stampstr, strlen(stampstr));
		font = fz_new_base14_font(ctx, "Helvetica");
		stampfont = pdf_add_simple_font(ctx, pdf, font, PDF_SIMPLE_ENCODING_LATIN);

		for (i = 0; i < n; i++)
			rewrite_page_streams(ctx, pdf, i, stampbuf, stampfont);

		pdf_save_document(ctx, pdf, outputfile, &opts);
	}
	fz_always(ctx)
	{
		fz_drop_document(ctx, doc);
		fz_drop_buffer(ctx, stampbuf);
		fz_drop_font(ctx, font);
		pdf_drop_obj(ctx, stampfont);
	}
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		code = EXIT_FAILURE;
	}
	fz_drop_context(ctx);

	return code;
}
