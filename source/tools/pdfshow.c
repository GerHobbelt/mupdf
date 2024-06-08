// Copyright (C) 2004-2022 Artifex Software, Inc.
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
 * pdfshow -- the ultimate pdf debugging tool
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include "mupdf/helpers/jmemcust.h"
#include "mupdf/helpers/dir.h"
#include "utf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if FZ_ENABLE_PDF

static pdf_document* doc = NULL;
static fz_context* ctx = NULL;
static fz_output* out = NULL;
static int showbinary = 0;
static int showdecode = 1;
static int tight = 0;
static int showcolumn = 0;
static int resolve = 0;

static int usage(void)
{
	fz_info(ctx,
		"usage: mutool show [options] file.pdf ( trailer | xref | pages | grep | outline | js | form | <path> | streams ) *\n"
		"\t-p -\tpassword\n"
		"\t-o -\toutput file\n"
		"\t-e\tleave stream contents in their original form\n"
		"\t-b\tprint only stream contents, as raw binary data\n"
		"\t-g\tprint only object, one line per object, suitable for grep\n"
		"\t-r\tresolve all objects\n"
		"\tpath: path to an object, starting with either an object number,\n"
		"\t\t'pages', 'trailer', or a property in the trailer;\n"
		"\t\tpath elements separated by '.' or '/'. Path elements must be\n"
		"\t\tarray index numbers, dictionary property names, or '*'.\n"
	);

	return EXIT_FAILURE;
}

static void showtrailer(void)
{
	if (tight)
		fz_write_printf(ctx, out, "trailer ");
	else
		fz_write_printf(ctx, out, "trailer\n");
	pdf_print_obj(ctx, out, pdf_trailer(ctx, doc), tight, resolve | 1);
	fz_write_printf(ctx, out, "\n");
}

static void showxref(void)
{
	int i;
	int xref_len = pdf_xref_len(ctx, doc);
	fz_write_printf(ctx, out, "xref\n0 %d\n", xref_len);
	for (i = 0; i < xref_len; i++)
	{
		pdf_xref_entry *entry = pdf_get_xref_entry_no_null(ctx, doc, i);
		fz_write_printf(ctx, out, "%05d: %010d %05d %c \n",
			i,
			(int)entry->ofs,
			entry->gen,
			entry->type ? entry->type : '-');
	}
}

static void showpages(void)
{
	pdf_obj* ref;
	int i, n = pdf_count_pages(ctx, doc);
	for (i = 0; i < n; ++i)
	{
		ref = pdf_lookup_page_obj(ctx, doc, i);
		fz_write_printf(ctx, out, "page %d = %d 0 R\n", i + 1, pdf_to_num(ctx, ref));
	}
}

static void showsafe(unsigned char* buf, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++) {
		if (buf[i] == '\r' || buf[i] == '\n') {
			putchar('\n');
			showcolumn = 0;
		}
		else if (buf[i] < 32 || buf[i] > 126) {
			putchar('.');
			showcolumn++;
		}
		else {
			putchar(buf[i]);
			showcolumn++;
		}
		if (showcolumn == 79) {
			putchar('\n');
			showcolumn = 0;
		}
	}
}

static void showstream(int num)
{
	fz_stream* stm;
	unsigned char buf[2048];
	size_t n;

	showcolumn = 0;

	if (showdecode)
		stm = pdf_open_stream_number(ctx, doc, num);
	else
		stm = pdf_open_raw_stream_number(ctx, doc, num);

	while (1)
	{
		n = fz_read(ctx, stm, buf, sizeof buf);
		if (n == 0)
			break;
		if (showbinary)
			fz_write_data(ctx, out, buf, n);
		else
			showsafe(buf, n);
	}

	fz_drop_stream(ctx, stm);
}

static void showobject(pdf_obj* ref)
{
	pdf_obj* obj = pdf_resolve_indirect(ctx, ref);
	int num = pdf_to_num(ctx, ref);
	if (pdf_is_stream(ctx, ref))
	{
		if (showbinary)
		{
			showstream(num);
		}
		else
		{
			if (tight)
			{
				fz_write_printf(ctx, out, "%d 0 obj ", num);
				pdf_print_obj(ctx, out, obj, 1, resolve | 1);
				fz_write_printf(ctx, out, " stream\n");
			}
			else
			{
				fz_write_printf(ctx, out, "%d 0 obj\n", num);
				pdf_print_obj(ctx, out, obj, 0, resolve | 1);
				fz_write_printf(ctx, out, "\nstream\n");
				showstream(num);
				fz_write_printf(ctx, out, "endstream\n");
				fz_write_printf(ctx, out, "endobj\n");
			}
		}
	}
	else
	{
		if (tight)
		{
			fz_write_printf(ctx, out, "%d 0 obj ", num);
			pdf_print_obj(ctx, out, obj, 1, resolve | 1);
			fz_write_printf(ctx, out, "\n");
		}
		else
		{
			fz_write_printf(ctx, out, "%d 0 obj\n", num);
			pdf_print_obj(ctx, out, obj, 0, resolve | 1);
			fz_write_printf(ctx, out, "\nendobj\n");
		}
	}
}

static void showgrep(void)
{
	pdf_obj* ref, * obj;
	int i, len;

	len = pdf_count_objects(ctx, doc);
	for (i = 0; i < len; i++)
	{
		pdf_xref_entry *entry = pdf_get_xref_entry_no_null(ctx, doc, i);
		if (entry->type == 'n' || entry->type == 'o')
		{
			fz_try(ctx)
			{
				ref = pdf_new_indirect(ctx, doc, i, 0);
				obj = pdf_resolve_indirect(ctx, ref);
			}
			fz_catch(ctx)
			{
				pdf_drop_obj(ctx, ref);
				fz_warn(ctx, "skipping object (%d 0 R)", i);
				continue;
			}

			pdf_sort_dict(ctx, obj);

			fz_write_printf(ctx, out, "%d 0 obj ", i);
			pdf_print_obj(ctx, out, obj, 1, resolve | 1);
			if (pdf_is_stream(ctx, ref))
				fz_write_printf(ctx, out, " stream");
			fz_write_printf(ctx, out, "\n");

			pdf_drop_obj(ctx, ref);
		}
	}

	fz_write_printf(ctx, out, "trailer ");
	pdf_print_obj(ctx, out, pdf_trailer(ctx, doc), 1, resolve | 1);
	fz_write_printf(ctx, out, "\n");
}

static void showstreams(void)
{
	int i, len;

	len = pdf_count_objects(ctx, doc);
	for (i = 0; i < len; i++)
	{
		pdf_xref_entry *entry = pdf_get_xref_entry_no_null(ctx, doc, i);
		if (entry->type == 'n' || entry->type == 'o')
		{
			if (pdf_obj_num_is_stream(ctx, doc, i))
			{
				fz_write_printf(ctx, out, "%d 0 stream ", i);
				showstream(i);
				fz_write_printf(ctx, out, "\n");
			}
		}
	}
}

static void
print_outline(fz_outline* outline, int level)
{
	int i;
	while (outline)
	{
		if (outline->down)
			fz_write_byte(ctx, out, outline->is_open ? '-' : '+');
		else
			fz_write_byte(ctx, out, '|');

		for (i = 0; i < level; i++)
			fz_write_byte(ctx, out, '\t');
		fz_write_printf(ctx, out, "%Q\t%s\n", outline->title, outline->uri);
		if (outline->down)
			print_outline(outline->down, level + 1);
		outline = outline->next;
	}
}

static void showoutline(void)
{
	fz_outline* outline = fz_load_outline(ctx, (fz_document*)doc);
	fz_try(ctx)
		print_outline(outline, 1);
	fz_always(ctx)
		fz_drop_outline(ctx, outline);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void showtext(char* buf, size_t buflen, int indent)
{
	int bol = 1;
	int c;
	while (buflen--)
	{
		c = *buf++;
		if (c == 0)
		{
			// whoa! JS sourcefile has embedded NUL byte!
			// print so it will be noticable:
			fz_write_printf(ctx, out, "%C<NUL>%C", Runeerror, Runeerror);
			bol = 0;
			continue;
		}
		if (c == '\r')
		{
			if (*buf == '\n')
				++buf;
			c = '\n';
		}
		if (indent && bol)
			fz_write_byte(ctx, out, '\t');
		fz_write_byte(ctx, out, c);
		bol = (c == '\n');
	}
	if (!bol)
		fz_write_byte(ctx, out, '\n');
}

static void showjs(void)
{
	pdf_obj* tree;
	int i;

	tree = pdf_load_name_tree(ctx, doc, PDF_NAME(JavaScript));
	for (i = 0; i < pdf_dict_len(ctx, tree); ++i)
	{
		pdf_obj* name = pdf_dict_get_key(ctx, tree, i);
		pdf_obj* action = pdf_dict_get_val(ctx, tree, i);
		pdf_obj* js = pdf_dict_get(ctx, action, PDF_NAME(JS));
		size_t srclength = 0;
		char* src = pdf_load_stream_or_string_as_utf8(ctx, js, &srclength);
		fz_write_printf(ctx, out, "// %s\n", pdf_to_name_not_null(ctx, name));
		showtext(src, srclength, 0);
		fz_free(ctx, src);
	}
	pdf_drop_name_tree(ctx, tree);
}

static void showaction(pdf_obj* action, const char* name)
{
	if (action)
	{
		pdf_obj* js = pdf_dict_get(ctx, action, PDF_NAME(JS));
		if (js)
		{
			size_t srclength = 0;
			char* src = pdf_load_stream_or_string_as_utf8(ctx, js, &srclength);
			fz_write_printf(ctx, out, "    %s: {\n", name);
			showtext(src, srclength, 1);
			fz_write_printf(ctx, out, "    }\n", name);
			fz_free(ctx, src);
		}
		else
		{
			fz_write_printf(ctx, out, "    %s: ", name);
			if (pdf_is_indirect(ctx, action))
				action = pdf_resolve_indirect(ctx, action);
			pdf_print_obj(ctx, out, action, tight, resolve | 1);
			fz_write_printf(ctx, out, "\n");
		}
	}
}

static void showfield(pdf_obj* field)
{
	pdf_obj* kids, * ft, * parent;
	const char* tu;
	char* t;
	int ff;
	int i, n;

	t = pdf_load_field_name(ctx, field);
	tu = pdf_dict_get_text_string(ctx, field, PDF_NAME(TU));
	ft = pdf_dict_get_inheritable(ctx, field, PDF_NAME(FT));
	ff = pdf_field_flags(ctx, field);
	parent = pdf_dict_get(ctx, field, PDF_NAME(Parent));

	fz_write_printf(ctx, out, "field %d\n", pdf_to_num(ctx, field));
	fz_write_printf(ctx, out, "    Type: %s\n", pdf_to_name_not_null(ctx, ft));
	if (ff)
	{
		fz_write_printf(ctx, out, "    Flags:");
		if (ff & PDF_FIELD_IS_READ_ONLY) fz_write_string(ctx, out, " readonly");
		if (ff & PDF_FIELD_IS_REQUIRED) fz_write_string(ctx, out, " required");
		if (ff & PDF_FIELD_IS_NO_EXPORT) fz_write_string(ctx, out, " noExport");
		if (ft == PDF_NAME(Btn))
		{
			if (ff & PDF_BTN_FIELD_IS_NO_TOGGLE_TO_OFF) fz_write_string(ctx, out, " noToggleToOff");
			if (ff & PDF_BTN_FIELD_IS_RADIO) fz_write_string(ctx, out, " radio");
			if (ff & PDF_BTN_FIELD_IS_PUSHBUTTON) fz_write_string(ctx, out, " pushButton");
			if (ff & PDF_BTN_FIELD_IS_RADIOS_IN_UNISON) fz_write_string(ctx, out, " radiosInUnison");
		}
		if (ft == PDF_NAME(Tx))
		{
			if (ff & PDF_TX_FIELD_IS_MULTILINE) fz_write_string(ctx, out, " multiline");
			if (ff & PDF_TX_FIELD_IS_PASSWORD) fz_write_string(ctx, out, " password");
			if (ff & PDF_TX_FIELD_IS_FILE_SELECT) fz_write_string(ctx, out, " fileSelect");
			if (ff & PDF_TX_FIELD_IS_DO_NOT_SPELL_CHECK) fz_write_string(ctx, out, " dontSpellCheck");
			if (ff & PDF_TX_FIELD_IS_DO_NOT_SCROLL) fz_write_string(ctx, out, " dontScroll");
			if (ff & PDF_TX_FIELD_IS_COMB) fz_write_string(ctx, out, " comb");
			if (ff & PDF_TX_FIELD_IS_RICH_TEXT) fz_write_string(ctx, out, " richText");
		}
		if (ft == PDF_NAME(Ch))
		{
			if (ff & PDF_CH_FIELD_IS_COMBO) fz_write_string(ctx, out, " combo");
			if (ff & PDF_CH_FIELD_IS_EDIT) fz_write_string(ctx, out, " edit");
			if (ff & PDF_CH_FIELD_IS_SORT) fz_write_string(ctx, out, " sort");
			if (ff & PDF_CH_FIELD_IS_MULTI_SELECT) fz_write_string(ctx, out, " multiSelect");
			if (ff & PDF_CH_FIELD_IS_DO_NOT_SPELL_CHECK) fz_write_string(ctx, out, " dontSpellCheck");
			if (ff & PDF_CH_FIELD_IS_COMMIT_ON_SEL_CHANGE) fz_write_string(ctx, out, " commitOnSelChange");
		}
		fz_write_string(ctx, out, "\n");
	}
	fz_write_printf(ctx, out, "    Name: %(\n", t);
	fz_free(ctx, t);
	if (*tu)
		fz_write_printf(ctx, out, "    Label: %q\n", tu);
	if (parent)
		fz_write_printf(ctx, out, "    Parent: %d\n", pdf_to_num(ctx, parent));

	showaction(pdf_dict_getp(ctx, field, "A"), "Action");

	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/K"), "Keystroke");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/V"), "Validate");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/F"), "Format");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/C"), "Calculate");

	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/E"), "Enter");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/X"), "Exit");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/D"), "Down");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/U"), "Up");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/Fo"), "Focus");
	showaction(pdf_dict_getp_inheritable(ctx, field, "AA/Bl"), "Blur");

	fz_write_string(ctx, out, "\n");

	kids = pdf_dict_get(ctx, field, PDF_NAME(Kids));
	n = pdf_array_len(ctx, kids);
	for (i = 0; i < n; ++i)
		showfield(pdf_array_get(ctx, kids, i));
}

static void showform(void)
{
	pdf_obj* fields;
	int i, n;

	fields = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/AcroForm/Fields");
	n = pdf_array_len(ctx, fields);
	for (i = 0; i < n; ++i)
		showfield(pdf_array_get(ctx, fields, i));
}

#define SEP ".[]/"

static int isnumber(const char* s)
{
	if (*s == '-')
		s++;
	while (*s)
	{
		if (*s < '0' || *s > '9')
			return 0;
		++s;
	}
	return 1;
}

#define PDF_XPATH_SIZE 1000

static void showpath(char* path, pdf_obj* obj)
{
	if (path && path[0])
	{
		char* part = fz_strsep(&path, SEP);
		if (part && part[0])
		{
			if (!strcmp(part, "*"))
			{
				int i, n;
				char buf[PDF_XPATH_SIZE];
				if (pdf_is_array(ctx, obj))
				{
					n = pdf_array_len(ctx, obj);
					for (i = 0; i < n; ++i)
					{
						if (path)
						{
							fz_strncpy_s(ctx, buf, path, sizeof buf);
							showpath(buf, pdf_array_get(ctx, obj, i));
						}
						else
							showpath(NULL, pdf_array_get(ctx, obj, i));
					}
				}
				else if (pdf_is_dict(ctx, obj))
				{
					n = pdf_dict_len(ctx, obj);
					for (i = 0; i < n; ++i)
					{
						if (path)
						{
							fz_strncpy_s(ctx, buf, path, sizeof buf);
							showpath(buf, pdf_dict_get_val(ctx, obj, i));
						}
						else
							showpath(NULL, pdf_dict_get_val(ctx, obj, i));
					}
				}
				else
				{
					fz_write_string(ctx, out, "null\n");
				}
			}
			else if (isnumber(part) && pdf_is_array(ctx, obj))
			{
				int num = atoi(part);
				num = num < 0 ? pdf_array_len(ctx, obj) + num : num - 1;
				showpath(path, pdf_array_get(ctx, obj, num));
			}
			else
				showpath(path, pdf_dict_gets(ctx, obj, part));
		}
		else
			fz_write_string(ctx, out, "null\n");
	}
	else
	{
		if (pdf_is_indirect(ctx, obj))
			showobject(obj);
		else
		{
			pdf_print_obj(ctx, out, obj, tight, resolve | 0);
			fz_write_string(ctx, out, "\n");
		}
	}
}

static void showpathpage(char* path)
{
	if (path)
	{
		char* part = fz_strsep(&path, SEP);
		if (part && part[0])
		{
			if (!strcmp(part, "*"))
			{
				int i, n;
				char buf[PDF_XPATH_SIZE];
				n = pdf_count_pages(ctx, doc);
				for (i = 0; i < n; ++i)
				{
					if (path)
					{
						fz_strncpy_s(ctx, buf, path, sizeof buf);
						showpath(buf, pdf_lookup_page_obj(ctx, doc, i));
					}
					else
						showpath(NULL, pdf_lookup_page_obj(ctx, doc, i));
				}
			}
			else if (isnumber(part))
			{
				int num = atoi(part);
				num = num < 0 ? pdf_count_pages(ctx, doc) + num : num - 1;
				showpath(path, pdf_lookup_page_obj(ctx, doc, num));
			}
			else
				fz_write_string(ctx, out, "null\n");
		}
		else
			fz_write_string(ctx, out, "null\n");
	}
	else
	{
		showpages();
	}
}

static void showpathroot(const char* path)
{
	char buf[PDF_XPATH_SIZE];
	char* list = buf;
	char* part;
	fz_strncpy_s(ctx, buf, path, sizeof buf);
	part = fz_strsep(&list, SEP);
	if (part && part[0])
	{
		if (!strcmp(part, "trailer") || !strcmp(part, "*"))
			showpath(list, pdf_trailer(ctx, doc));
		else if (!strcmp(part, "pages"))
			showpathpage(list);
		else if (isnumber(part))
		{
			pdf_obj *obj;
			int num = atoi(part);
			num = num < 0 ? pdf_xref_len(ctx, doc) + num : num;
			obj = pdf_new_indirect(ctx, doc, num, 0);
			fz_try(ctx)
				showpath(list, obj);
			fz_always(ctx)
				pdf_drop_obj(ctx, obj);
			fz_catch(ctx)
				;
		}
		else
		{
			pdf_obj* obj = pdf_dict_gets(ctx, pdf_trailer(ctx, doc), part);

			showpath(list, obj);
		}
	}
	else
		fz_write_string(ctx, out, "null\n");
}

static void show(const char* sel)
{
	if (!strcmp(sel, "trailer"))
		showtrailer();
	else if (!strcmp(sel, "xref"))
		showxref();
	else if (!strcmp(sel, "pages"))
		showpages();
	else if (!strcmp(sel, "grep"))
		showgrep();
	else if (!strcmp(sel, "streams"))
		showstreams();
	else if (!strcmp(sel, "outline"))
		showoutline();
	else if (!strcmp(sel, "js"))
		showjs();
	else if (!strcmp(sel, "form"))
		showform();
	else
		showpathroot(sel);
}

int pdfshow_main(int argc, const char** argv)
{
	const char* password = NULL; /* don't throw errors if encrypted */
	const char* filename = NULL;
	const char* output = NULL;
	int c;
	int errored = 0;

	doc = NULL;
	ctx = NULL;
	out = NULL;
	showbinary = 0;
	showdecode = 1;
	tight = 0;
	showcolumn = 0;
	resolve = 0;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "p:o:rbegh")) != -1)
	{
		switch (c)
		{
		case 'p': password = fz_optarg; break;
		case 'o': output = fz_optarg; break;
		case 'b': showbinary = 1; break;
		case 'e': showdecode = 0; break;
		case 'g': tight = 1; break;
		case 'r': resolve = PDF_PRINT_RESOLVE_ALL_INDIRECT; break;
		default: return usage();
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

	filename = argv[fz_optind++];

	if (!output || *output == 0 || !strcmp(output, "-"))
	{
		out = fz_stdout(ctx);
		output = NULL;
	}
	else
	{
		char fbuf[PATH_MAX];
		fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
		fz_normalize_path(ctx, fbuf, sizeof fbuf, fbuf);
		fz_sanitize_path(ctx, fbuf, sizeof fbuf, fbuf);
		out = fz_new_output_with_path(ctx, fbuf, 0);
	}

	fz_var(doc);
	fz_try(ctx)
	{
		doc = pdf_open_document(ctx, filename);
		if (pdf_needs_password(ctx, doc))
			if (!pdf_authenticate_password(ctx, doc, password))
				fz_warn(ctx, "cannot authenticate password: %s", filename);

		if (fz_optind == argc)
			showtrailer();

		// when we are requested to show multiple sections, we include headers/separators:
		int print_header = (fz_optind + 1 < argc);

		while (fz_optind < argc)
		{
			const char* name = argv[fz_optind++];

			// run every 'show' command independently, i.e. a fault in one does not mean we won't execute the others:
			fz_try(ctx)
			{
				if (print_header)
					fz_write_printf(ctx, out, "=== %s =======================================================================\n\n", name);

				show(name);
			}
			fz_catch(ctx)
			{
				// only log any type of exception which hasn't logged itself yet: prevent duplicate error log entries
				int code = fz_caught(ctx);
				if (code == FZ_ERROR_ABORT || code == FZ_ERROR_TRYLATER)
				{
					fz_report_error(ctx);
				}
				fz_write_printf(ctx, out, "\n\n**ERROR**: %s: %s\n", name, fz_convert_error(ctx, NULL));
				errored = EXIT_FAILURE;
			}
			if (print_header)
				fz_write_printf(ctx, out, "\n\n");
		}
		if (print_header)
			fz_write_printf(ctx, out, "===============================================================================\n");
	}
	fz_always(ctx)
	{
		fz_close_output(ctx, out);
		fz_drop_output(ctx, out);
		pdf_drop_document(ctx, doc);
	}
	fz_catch(ctx)
	{
		// only log any type of exception which hasn't logged itself yet: prevent duplicate error log entries
		int code = fz_caught(ctx);
		if (code == FZ_ERROR_ABORT || code == FZ_ERROR_TRYLATER)
		{
			fz_report_error(ctx);
		}
		errored = EXIT_FAILURE;
	}

	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return errored;
}

#endif // FZ_ENABLE_PDF
