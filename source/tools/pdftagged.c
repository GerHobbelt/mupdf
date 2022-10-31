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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

/*
 * pdftagged -- extract Tagged PDF content as HTML
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if FZ_ENABLE_RENDER_CORE 

static pdf_document *doc = NULL;
static fz_context *ctx = NULL;

static int usage(void)
{
	fprintf(stderr, "usage: mutool tagged [options] file.pdf\n");
	fprintf(stderr, "\t-p\tpassword\n");
	return 1;
}

static void
pdf_tagged_text(fz_context *ctx, fz_output *out, const char *text)
{
	int c;
	while ((c = *text++))
	{
		if (c == '<') { fz_write_string(ctx, out, "&lt;"); }
		else if (c == '>') { fz_write_string(ctx, out, "&gt;"); }
		else if (c == '&') { fz_write_string(ctx, out, "&amp;"); }
		else fz_write_byte(ctx, out, c);
	}
}

static void
pdf_tagged_mcid(fz_context *ctx, fz_output *out, pdf_mcid_table *table, int mcid)
{
	int i;
	for (i = 0; i < table->len; ++i)
		if (table->entry[i].mcid == mcid)
			pdf_tagged_text(ctx, out, fz_string_from_buffer(ctx, table->entry[i].text));
}

static const char *map_tag(const char *t)
{
	if (!strcmp(t, "Span")) return "span";
	if (!strcmp(t, "P")) return "p";
	if (!strcmp(t, "Document")) return "body";
	if (!strcmp(t, "Part")) return "div";
	if (!strcmp(t, "Art")) return "article";
	if (!strcmp(t, "Sect")) return "section";
	if (!strcmp(t, "Link")) return "a";

	if (!strcmp(t, "L")) return "dl";
	if (!strcmp(t, "LI")) return "div";
	if (!strcmp(t, "Lbl")) return "dt";
	if (!strcmp(t, "LBody")) return "dd";

	return t;
}

static const char *map_att_key(const char *k)
{
	if (!strcmp(k, "TextAlign")) return "text-align";
	if (!strcmp(k, "TextIndent")) return "text-indent";
	if (!strcmp(k, "StartIndent")) return "margin-left";
	if (!strcmp(k, "EndIndent")) return "margin-right";
	if (!strcmp(k, "LineHeight")) return "font-size";
	if (!strcmp(k, "SpaceBefore")) return "margin-top";
	if (!strcmp(k, "SpaceAfter")) return "margin-bottom";
	return k;
}

static const char *map_att_val(const char *v)
{
	return v;
}

static void
pdf_tagged_att(fz_context *ctx, fz_output *out, pdf_obj *atts)
{
	if (pdf_dict_get(ctx, atts, PDF_NAME(O)) == PDF_NAME(Layout))
	{
		int i, n = pdf_dict_len(ctx, atts);
		for (i = 0; i < n; ++i)
		{
			pdf_obj *k = pdf_dict_get_key(ctx, atts, i);
			pdf_obj *v = pdf_dict_get_val(ctx, atts, i);
			if (k != PDF_NAME(O))
			{
				const char *kk = map_att_key(pdf_to_name(ctx, k));
				if (pdf_is_name(ctx, v))
					fz_write_printf(ctx, out, "%s:%s;", kk, map_att_val(pdf_to_name(ctx, v)));
				else if (pdf_is_number(ctx, v))
					fz_write_printf(ctx, out, "%s:%gpt;", kk, pdf_to_real(ctx, v));
			}
		}
	}
}

static void
pdf_tagged_st(fz_context *ctx, fz_output *out, pdf_document *doc, pdf_mcid_table *ptable, pdf_obj *role_map, pdf_obj *class_map, pdf_obj *page, pdf_obj *node, int use_style)
{
	if (pdf_is_array(ctx, node))
	{
		int i, n = pdf_array_len(ctx, node);
		for (i = 0; i < n; ++i)
			pdf_tagged_st(ctx, out, doc, ptable, role_map, class_map, page, pdf_array_get(ctx, node, i), use_style);
	}
	else if (pdf_is_number(ctx, node))
	{
		int page_n = pdf_lookup_page_number(ctx, doc, page);
		int mcid = pdf_to_int(ctx, node);
		if (page_n >= 0)
			pdf_tagged_mcid(ctx, out, &ptable[page_n], mcid);
	}
	else if (pdf_is_dict(ctx, node))
	{
		pdf_obj *type = pdf_dict_get(ctx, node, PDF_NAME(Type));
		if (1) // type == NULL || type == PDF_NAME(StructElem))
		{
			const char *tag = NULL;
			pdf_obj *s = pdf_dict_get(ctx, node, PDF_NAME(S));
			pdf_obj *k = pdf_dict_get(ctx, node, PDF_NAME(K));
			pdf_obj *at = pdf_dict_get(ctx, node, PDF_NAME(ActualText));
			pdf_obj *a = pdf_dict_get(ctx, node, PDF_NAME(A));
			pdf_obj *c = pdf_dict_get(ctx, node, PDF_NAME(C));
			page = pdf_dict_get(ctx, node, PDF_NAME(Pg));

			if (role_map)
			{
				pdf_obj *ms = pdf_dict_get(ctx, role_map, s);
				if (ms)
					s = ms;
			}

			if (class_map)
			{
				pdf_obj *mc = pdf_dict_get(ctx, class_map, c);
				if (mc)
					c = mc;
			}

			if (s)
				tag = map_tag(pdf_to_name(ctx, s));

			if (tag && (strcmp(tag, "span") || (use_style && (a || c))))
			{
				fz_write_printf(ctx, out, "<%s", tag);
				if (use_style && (a || c))
				{
					fz_write_printf(ctx, out, " style=\"");
					pdf_tagged_att(ctx, out, c);
					pdf_tagged_att(ctx, out, a);
					fz_write_printf(ctx, out, "\"");
				}
				fz_write_printf(ctx, out, ">");
			}

			if (pdf_is_string(ctx, at))
				pdf_tagged_text(ctx, out, pdf_to_text_string(ctx, at, NULL));

			if (k)
				pdf_tagged_st(ctx, out, doc, ptable, role_map, class_map, page, k, use_style);

			if (tag && (strcmp(tag, "span") || (use_style && (a || c))))
			{
				fz_write_printf(ctx, out, "</%s>", tag);
			}
		}
	}
}

void pdf_tagged_pdf(fz_context *ctx, fz_output *out, pdf_document *doc, pdf_mcid_table *ptable, int use_style)
{
	pdf_obj *trailer = pdf_trailer(ctx, doc);
	pdf_obj *root = pdf_dict_get(ctx, trailer, PDF_NAME(Root));
	pdf_obj *stroot = pdf_dict_get(ctx, root, PDF_NAME(StructTreeRoot));
	pdf_obj *role_map = pdf_dict_get(ctx, stroot, PDF_NAME(RoleMap));
	pdf_obj *class_map = pdf_dict_get(ctx, stroot, PDF_NAME(ClassMap));
	pdf_obj *k = pdf_dict_get(ctx, stroot, PDF_NAME(K));
	fz_write_string(ctx, out, "<!DOCTYPE html><html>");
	fz_write_string(ctx, out, "<head>");
	fz_write_string(ctx, out, "<style>");
	if (use_style)
		fz_write_string(ctx, out, "p{margin:0}");
	fz_write_string(ctx, out, "table{border-collapse:collapse}");
	fz_write_string(ctx, out, "th,td{border:1px solid black;padding:3px}");
	fz_write_string(ctx, out, "</style>");
	fz_write_string(ctx, out, "<meta charset=\"UTF-8\">");
	fz_write_string(ctx, out, "</head>");
	pdf_tagged_st(ctx, out, doc, ptable, role_map, class_map, NULL, k, use_style);
	fz_write_string(ctx, out, "</html>\n");
}

int pdftagged_main(int argc, const char **argv)
{
	pdf_document *doc = NULL;
	fz_context *ctx = NULL;

	const char *infile;
	const char *password = "";
	pdf_mcid_table *mcid;
	int rv = 1;
	int c, i, n;
	int use_style = 1;

	while ((c = fz_getopt(argc, argv, "Xp:")) != -1)
	{
		switch (c)
		{
		case 'p': password = fz_optarg; break;
		case 'X': use_style = 0; break;
		default: return usage();
		}
	}

	if (fz_optind == argc)
		return usage();

	infile = argv[fz_optind++];

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fprintf(stderr, "cannot initialise context\n");
		return 1;
	}

	doc = pdf_open_document(ctx, infile);
	fz_try(ctx)
	{
		if (pdf_needs_password(ctx, doc))
			if (!pdf_authenticate_password(ctx, doc, password))
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", infile);

		n = pdf_count_pages(ctx, doc);

		/* Extract tagged content from all pages. */
		mcid = fz_malloc_array(ctx, n, pdf_mcid_table);
		memset(mcid, 0, n * sizeof *mcid);
		for (i = 0; i < n; ++i)
		{
			// printf("EXTRACTING PAGE %d\n", i+1);
			pdf_page *page = pdf_load_page(ctx, doc, i);
			pdf_obj *resources = pdf_page_resources(ctx, page);
			pdf_obj *contents = pdf_page_contents(ctx, page);
			pdf_processor *proc = pdf_new_mcid_processor(ctx, &mcid[i]);
			pdf_process_contents(ctx, proc, page->doc, resources, contents, NULL);
			pdf_close_processor(ctx, proc);
			pdf_drop_processor(ctx, proc);
			fz_drop_page(ctx, (fz_page*)page);
			// for (k = 0; k < mcid[i].len; ++k) printf("MCID %d: %s\n", mcid[i].entry[k].mcid, fz_string_from_buffer(ctx, mcid[i].entry[k].text));
		}

		pdf_tagged_pdf(ctx, fz_stdout(ctx), doc, mcid, use_style);
		
		rv = 0;
	}
	fz_always(ctx)
	{
		pdf_drop_document(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "error: %s", fz_caught_message(ctx));
	}

	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	
	return rv;
}

#endif
