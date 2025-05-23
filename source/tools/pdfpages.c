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
 * Information tool.
 * Print information about pages of a pdf.
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/jmemcust.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if FZ_ENABLE_RENDER_CORE

static fz_context* ctx = NULL;

static int
usage(void)
{
	fz_info(ctx,
		"usage: mutool pages [options] file.pdf [pages]\n"
		"\t-o -\toutput file path. Default: page data will be written to stdout\n"
		"\t-p -\tpassword for decryption\n"
		"\tpages\tcomma separated list of page numbers and ranges\n"
	);

	return EXIT_FAILURE;
}

static int
showbox(fz_context *ctx, fz_output *out, pdf_obj *page, char *text, pdf_obj *name)
{
	fz_rect bbox;
	pdf_obj *obj;
	int failed = 0;

	fz_try(ctx)
	{
		obj = pdf_dict_get(ctx, page, name);
		if (!pdf_is_array(ctx, obj))
			break;

		bbox = pdf_to_rect(ctx, obj);

		fz_write_printf(ctx, out, "<%s l=\"%g\" b=\"%g\" r=\"%g\" t=\"%g\" />\n", text, bbox.x0, bbox.y0, bbox.x1, bbox.y1);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "box error: %s", fz_convert_error(ctx, NULL));
		failed = 1;
	}

	return failed;
}

static int
shownum(fz_context *ctx, fz_output *out, pdf_obj *page, char *text, pdf_obj *name)
{
	pdf_obj *obj;
	int failed = 0;

	fz_try(ctx)
	{
		obj = pdf_dict_get(ctx, page, name);
		if (!pdf_is_number(ctx, obj))
			break;

		fz_write_printf(ctx, out, "<%s v=\"%g\" />\n", text, pdf_to_real(ctx, obj));
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "num error: %s", fz_convert_error(ctx, NULL));
		failed = 1;
	}

	return failed;
}

static int
showpage(fz_context *ctx, pdf_document *doc, fz_output *out, int page)
{
	pdf_obj *pageref;
	int failed = 0;

	fz_write_printf(ctx, out, "<page pagenum=\"%d\">\n", page);
	fz_try(ctx)
	{
		pageref = pdf_lookup_page_obj(ctx, doc, page-1);
		if (!pageref)
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);
	}
	fz_catch(ctx)
	{
		fz_write_printf(ctx, out, "Failed to gather information for page %d\n", page);
		failed = 1;
	}

	if (!failed)
	{
		failed |= showbox(ctx, out, pageref, "MediaBox", PDF_NAME(MediaBox));
		failed |= showbox(ctx, out, pageref, "CropBox", PDF_NAME(CropBox));
		failed |= showbox(ctx, out, pageref, "ArtBox", PDF_NAME(ArtBox));
		failed |= showbox(ctx, out, pageref, "BleedBox", PDF_NAME(BleedBox));
		failed |= showbox(ctx, out, pageref, "TrimBox", PDF_NAME(TrimBox));
		failed |= shownum(ctx, out, pageref, "Rotate", PDF_NAME(Rotate));
		failed |= shownum(ctx, out, pageref, "UserUnit", PDF_NAME(UserUnit));
	}

	fz_write_printf(ctx, out, "</page>\n");

	return failed;
}

static int
showpages(fz_context *ctx, pdf_document *doc, fz_output *out, const char *pagelist)
{
	int page, spage, epage;
	int pagecount;
	int ret = 0;

	if (!doc)
	{
		usage();
		fz_throw(ctx, FZ_ERROR_GENERIC, "no pdf document specified");
	}

	pagecount = pdf_count_pages(ctx, doc);
	while ((pagelist = fz_parse_page_range(ctx, pagelist, &spage, &epage, pagecount)))
	{
		int fail;
		if (spage > epage)
			page = spage, spage = epage, epage = page;
		for (page = spage; page <= epage; page++)
		{
			fail = showpage(ctx, doc, out, page);
			/* On the first failure, check for the pagecount having changed. */
			if (fail && !ret)
			{
				pagecount = pdf_count_pages(ctx, doc);
				if (epage > pagecount)
					epage = pagecount;
			}
			ret |= fail;
		}
	}

	return ret;
}

static int
pdfpages_pages(fz_context *ctx, fz_output *out, const char *filename, const char *password, const char *argv[], int argc)
{
	enum { NO_FILE_OPENED, NO_INFO_GATHERED, INFO_SHOWN } state;
	int argidx = 0;
	pdf_document *doc = NULL;
	int ret = 0;

	state = NO_FILE_OPENED;
	while (argidx < argc)
	{
		if (state == NO_FILE_OPENED || !fz_is_page_range(ctx, argv[argidx]))
		{
			if (state == NO_INFO_GATHERED)
			{
				showpages(ctx, doc, out, "1-N");
			}

			pdf_drop_document(ctx, doc);

			filename = argv[argidx];
			fz_write_printf(ctx, out, "%s:\n", filename);
			doc = pdf_open_document(ctx, filename);
			if (pdf_needs_password(ctx, doc))
				if (!pdf_authenticate_password(ctx, doc, password))
					fz_throw(ctx, FZ_ERROR_ARGUMENT, "cannot authenticate password: %s", filename);

			state = NO_INFO_GATHERED;
		}
		else
		{
			ret |= showpages(ctx, doc, out, argv[argidx]);
			state = INFO_SHOWN;
		}

		argidx++;
	}

	if (state == NO_INFO_GATHERED)
		showpages(ctx, doc, out, "1-N");

	pdf_drop_document(ctx, doc);

	return ret;
}

int pdfpages_main(int argc, const char** argv)
{
	const char *filename = "";
	const char *password = "";
	const char* output = NULL;
	int c;
	int ret;
	fz_output *out = NULL;

	ctx = NULL;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:p:h")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;
		case 'p': password = fz_optarg; break;
		default:
			return usage();
		}
	}

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
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

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	ret = 0;
	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
			out = fz_stdout(ctx);
		else
		{
			char fbuf[PATH_MAX];
			fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
			fz_normalize_path(ctx, fbuf, sizeof fbuf, fbuf);
			fz_sanitize_path(ctx, fbuf, sizeof fbuf, fbuf);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		ret = pdfpages_pages(ctx, out, filename, password, &argv[fz_optind], argc - fz_optind);

		fz_close_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		ret = EXIT_FAILURE;
	}
	fz_drop_output(ctx, out);
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return ret;
}

#endif
