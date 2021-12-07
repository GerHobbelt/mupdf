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

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static pdf_document *doc = NULL;
static fz_context *ctx = NULL;

static int usage(void)
{
	fprintf(stderr, "usage: mutool tagged [options] file.pdf\n");
	fprintf(stderr, "\t-p\tpassword\n");
	return 1;
}

static void
pdf_tagged_text(fz_context *ctx, const char *text)
{
	int c;
	while ((c = *text++))
	{
		if (c == '<') { putchar('&'); putchar('l'); putchar('t'); putchar(';'); }
		else if (c == '>') { putchar('&'); putchar('g'); putchar('t'); putchar(';'); }
		else if (c == '&') { putchar('&'); putchar('a'); putchar('m'); putchar('p'); putchar(';'); }
		else putchar(c);
	}
}

static void
pdf_tagged_mcid(fz_context *ctx, pdf_mcid_table *table, int mcid)
{
	int i;
	for (i = 0; i < table->len; ++i)
		if (table->entry[i].mcid == mcid)
			pdf_tagged_text(ctx, fz_string_from_buffer(ctx, table->entry[i].text));
}

static const char *map_tag(const char *t)
{
	if (!strcmp(t, "Span")) return "span";
	if (!strcmp(t, "P")) return "div";
	if (!strcmp(t, "Document")) return "body";
	if (!strcmp(t, "Part")) return "div";
	if (!strcmp(t, "Art")) return "article";
	if (!strcmp(t, "Sect")) return "section";

	if (!strcmp(t, "L")) return "div";
	if (!strcmp(t, "LI")) return "div";
	if (!strcmp(t, "Lbl")) return "span";
	if (!strcmp(t, "LBody")) return "span";

	return t;
}

static const char *map_att_key(const char *k)
{
	if (!strcmp(k, "TextAlign")) return "text-align";
	if (!strcmp(k, "TextIndent")) return "text-indent";
	if (!strcmp(k, "StartIndent")) return "margin-left";
	if (!strcmp(k, "EndIndent")) return "margin-right";
	if (!strcmp(k, "LineHeight")) return "font-size";
	return k;
}

static const char *map_att_val(const char *v)
{
	return v;
}

static void
pdf_tagged_att(fz_context *ctx, pdf_obj *atts)
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
					printf("%s:%s;", kk, map_att_val(pdf_to_name(ctx, v)));
				else if (pdf_is_number(ctx, v))
					printf("%s:%gpt;", kk, pdf_to_real(ctx, v));
			}
		}
	}
}

static void
pdf_tagged_st(fz_context *ctx, pdf_document *doc, pdf_mcid_table *ptable, pdf_obj *role_map, pdf_obj *class_map, pdf_obj *page, pdf_obj *node)
{
	if (pdf_is_array(ctx, node))
	{
		int i, n = pdf_array_len(ctx, node);
		for (i = 0; i < n; ++i)
			pdf_tagged_st(ctx, doc, ptable, role_map, class_map, page, pdf_array_get(ctx, node, i));
	}
	else if (pdf_is_number(ctx, node))
	{
		int page_n = pdf_lookup_page_number(ctx, doc, page);
		int mcid = pdf_to_int(ctx, node);
		if (page_n >= 0)
			pdf_tagged_mcid(ctx, &ptable[page_n], mcid);
	}
	else if (pdf_is_dict(ctx, node))
	{
		pdf_obj *type = pdf_dict_get(ctx, node, PDF_NAME(Type));
		if (1) // type == NULL || type == PDF_NAME(StructElem))
		{
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
			{
				printf("<%s", map_tag(pdf_to_name(ctx, s)));
				if (a || c)
				{
					printf(" style=\"");
					pdf_tagged_att(ctx, c);
					pdf_tagged_att(ctx, a);
					printf("\"");
				}
				printf(">");
			}

			if (pdf_is_string(ctx, at))
				pdf_tagged_text(ctx, pdf_to_text_string(ctx, at));

			if (k)
				pdf_tagged_st(ctx, doc, ptable, role_map, class_map, page, k);

			if (s)
			{
				printf("</%s>", map_tag(pdf_to_name(ctx, s)));
			}
		}
	}
}

void pdf_tagged_pdf(fz_context *ctx, pdf_document *doc, pdf_mcid_table *ptable)
{
	pdf_obj *trailer = pdf_trailer(ctx, doc);
	pdf_obj *root = pdf_dict_get(ctx, trailer, PDF_NAME(Root));
	pdf_obj *stroot = pdf_dict_get(ctx, root, PDF_NAME(StructTreeRoot));
	pdf_obj *role_map = pdf_dict_get(ctx, stroot, PDF_NAME(RoleMap));
	pdf_obj *class_map = pdf_dict_get(ctx, stroot, PDF_NAME(ClassMap));
	pdf_obj *k = pdf_dict_get(ctx, stroot, PDF_NAME(K));
	printf("<!DOCTYPE html><html>");
	pdf_tagged_st(ctx, doc, ptable, role_map, class_map, NULL, k);
	printf("</html>\n");
}

int pdftagged_main(int argc, char **argv)
{
	char *infile;
	char *password = "";
	pdf_mcid_table *mcid;
	int c, i, k, n;

	while ((c = fz_getopt(argc, argv, "p:")) != -1)
	{
		switch (c)
		{
		case 'p': password = fz_optarg; break;
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
		exit(1);
	}

	doc = pdf_open_document(ctx, infile);
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

	pdf_tagged_pdf(ctx, doc, mcid);

	pdf_drop_document(ctx, doc);
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return 0;
}
