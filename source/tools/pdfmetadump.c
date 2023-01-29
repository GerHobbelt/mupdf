/*
 * Multi-purpose tool, geared towards use with Qiqqa.
 *
 * - Shows information about pdf in JSON format.
 *
 * Derived from the pdfinfo tool.
 */


#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include "mupdf/helpers/pkcs7-openssl.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/jmemcust.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if FZ_ENABLE_PDF

struct info
{
	int page;
	pdf_obj* pageref;
	union {
		struct {
			pdf_obj* obj;
		} info;
		struct {
			pdf_obj* obj;
		} crypt;
		struct {
			pdf_obj* obj;
			fz_rect* bbox;
		} dim;
		struct {
			pdf_obj* obj;
			pdf_obj* subtype;
			pdf_obj* name;
			pdf_obj* encoding;
		} font;
		struct {
			pdf_obj* obj;
			pdf_obj* width;
			pdf_obj* height;
			pdf_obj* bpc;
			pdf_obj* filter;
			pdf_obj* cs;
			pdf_obj* altcs;
		} image;
		struct {
			pdf_obj* obj;
			pdf_obj* groupsubtype;
			pdf_obj* reference;
		} form;
	} u;
};

typedef struct
{
	pdf_document* doc;
	fz_context* ctx;
	fz_output* out;
	int pagecount;
	struct info* dim;
	int dims;
	struct info* font;
	int fonts;
	struct info* image;
	int images;
	struct info* form;
	int forms;
	struct info* psobj;
	int psobjs;
} globals;

static int PRINT_OBJ_TO_JSON_FLAGS = (PDF_PRINT_RESOLVE_ALL_INDIRECT | /* PDF_PRINT_JSON_ILLEGAL_UNICODE_AS_HEX | */ PDF_PRINT_JSON_BINARY_DATA_AS_HEX_PLUS_RAW | 1 /* ASCII flag */);
static int ignore_password_troubles = 0;
static int has_password_troubles = 0;

static int skip_fonts_dump = 0;
static int skip_images_dump = 0;
static int skip_forms_dump = 0;
static int skip_psobjects_dump = 0;
static int skip_annotations_dump = 0;
static int skip_uri_links_dump = 0;
static int skip_separations_dump = 0;

static void dump_observed_errors(fz_context* ctx, fz_output* out);
static int write_level_start(fz_context* ctx, fz_output* out, const char bracket_open);

static void clearinfo(fz_context* ctx, globals* glo)
{
	int i;

	if (glo->dim)
	{
		for (i = 0; i < glo->dims; i++)
			fz_free(ctx, glo->dim[i].u.dim.bbox);
		fz_free(ctx, glo->dim);
		glo->dim = NULL;
		glo->dims = 0;
	}

	if (glo->font)
	{
		fz_free(ctx, glo->font);
		glo->font = NULL;
		glo->fonts = 0;
	}

	if (glo->image)
	{
		fz_free(ctx, glo->image);
		glo->image = NULL;
		glo->images = 0;
	}

	if (glo->form)
	{
		fz_free(ctx, glo->form);
		glo->form = NULL;
		glo->forms = 0;
	}

	if (glo->psobj)
	{
		fz_free(ctx, glo->psobj);
		glo->psobj = NULL;
		glo->psobjs = 0;
	}
}

static void closexref(fz_context* ctx, globals* glo)
{
	if (glo->doc)
	{
		pdf_drop_document(ctx, glo->doc);
		glo->doc = NULL;
	}

	clearinfo(ctx, glo);
}

static fz_context* ctx = NULL;

static int
usage(void)
{
	fz_info(ctx,
		"usage: mutool info [options] file.pdf\n"
		"\t-o -\toutput file path. Default: info will be written to stdout\n"
		"\t-p -\tpassword for decryption\n"
		"\t-i -\tignore:\n"
		"\t  p \t- password troubles (delivers starkly reduced info)\n"
		"\t-s -\tskip / do not dump:\n"
		"\t  f \t- fonts\n"
		"\t  i \t- images\n"
		"\t  q \t- forms ('questionaires')\n"
		"\t  x \t- PostScript XObjects\n"
		"\t  a \t- annotations\n"
		"\t  u \t- (uri) links\n"
		"\t  s \t- separations\n"
		"\t  i \t- images\n"
		"\t-m {0,1,2}\tmode for printing bad string content:\n"
		"\t          \t- 0: massaged to hex,\n"
		"\t          \t- 1: hex dumped with legible characters interleaved,\n"
		"\t          \t- 2: dump as JSON {HEX:'...', MASSAGED:'...'} struct.\n"
	);

	return EXIT_FAILURE;
}

static int json_sep_state = 0;

static void write_sep(fz_context* ctx, fz_output* out)
{
	switch (json_sep_state)
	{
	case 0:
		// start of output:
		write_level_start(ctx, out, '{');
		break;

	case 1:
		// first element in object:
		fz_write_printf(ctx, out, "  ");
		json_sep_state = 2;
		break;

	case 2:
		// next element in object:
		fz_write_printf(ctx, out, ",\n  ");
		break;
	}
}

static void write_string(fz_context* ctx, fz_output* out, const char* str)
{
	if (!str) {
		fz_write_printf(ctx, out, "null");
		return;
	}

	fz_write_printf(ctx, out, "%jq", str);
}

static void write_item(fz_context* ctx, fz_output* out, const char* label, const char* value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: ", label);
	write_string(ctx, out, value);
}

static void write_item_bool(fz_context* ctx, fz_output* out, const char* label, int value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: %s",
		label,
		value ? "true" : "false");
}

static void write_item_int(fz_context* ctx, fz_output* out, const char* label, int value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: %d",
		label,
		value);
}

static void write_item_bits(fz_context* ctx, fz_output* out, const char* label, unsigned int value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: %jB",
		label,
		value);
}

static void write_item_float(fz_context* ctx, fz_output* out, const char* label, float value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: %g",
		label,
		value);
}

static void write_item_bbox(fz_context* ctx, fz_output* out, const char* label, const fz_rect* bbox)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: [ %,R ]",
		label,
		bbox);
}

static void write_item_coord(fz_context* ctx, fz_output* out, const char* label, float x, float y)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: { %q: %g, %q: %g }", label, "X", x, "Y", y);
}

static void write_item_date(fz_context* ctx, fz_output* out, const char* label, int64_t value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: %jT",
		label,
		value);
}

static void write_item_printf(fz_context* ctx, fz_output* out, const char* label, const char* value_fmt, ...)
{
	va_list ap;

	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: ",
		label);
	va_start(ap, value_fmt);
	fz_write_vprintf(ctx, out, value_fmt,
		ap);
	va_end(ap);
}

// we consider any JSON with more than 1024 levels of object/array "removed from sanity" ;-)
static char json_stack[1024] = "";

static int write_level_get_level(fz_context* ctx)
{
	size_t stack_offset = strlen(json_stack);
	return (int)stack_offset;
}

static int write_level_start(fz_context* ctx, fz_output* out, const char bracket_open)
{
	fz_write_printf(ctx, out, "%c\n",
		bracket_open);

	size_t stack_offset = strlen(json_stack);
	if (stack_offset > sizeof(json_stack) - 2)
		fz_throw(ctx, FZ_ERROR_GENERIC, "JSON tracking stack overflow");
	json_stack[stack_offset++] = (bracket_open == '{' ? '}' : ']');
	json_stack[stack_offset] = 0;

	// Also (re)set the JSON formatting output state as this call (re)starts another JSON object/array:
	json_sep_state = 1;

	return (int)stack_offset - 1;
}

static int write_level_end(fz_context* ctx, fz_output* out, const char bracket_close)
{
	size_t stack_offset = strlen(json_stack);
	if (stack_offset == 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "JSON tracking stack underflow");
	if (json_stack[stack_offset - 1] != bracket_close)
		fz_throw(ctx, FZ_ERROR_GENERIC, "JSON tracking stack: mismatching closing bracket");
	json_stack[stack_offset - 1] = 0;

	fz_write_printf(ctx, out, "\n%c", bracket_close);

	// Also set the JSON formatting output state as this closes an object, so the next sibling should be preceded by a comma at least:
	json_sep_state = 2;

	return (int)stack_offset;
}

static size_t write_level_guarantee_level(fz_context* ctx, fz_output* out, int target_stack_level)
{
	int stack_offset = (int)strlen(json_stack);

	assert(target_stack_level >= 0);

	// see if we need to pop off elements off the stack:
	if (target_stack_level < stack_offset) {
		fz_error(ctx, "JSON stack tracking: recovering from unclosed elements. Target level %d < Current Depth %d\n", target_stack_level, stack_offset);

		while (target_stack_level < stack_offset) {
			fz_write_printf(ctx, out, "\n%c\n", json_stack[--stack_offset]);
		}
		//ASSERT(stack_offset == target_stack_level);
		json_stack[stack_offset] = 0;

		// Also set the JSON formatting output state as this closes an object, so the next sibling should be preceded by a comma at least:
		json_sep_state = 2;
	}
	if (target_stack_level != stack_offset) {
		fz_throw(ctx, FZ_ERROR_GENERIC, "JSON tracking stack: internal error, corrupted concept of stack position");
	}
	return stack_offset;
}

static int write_level_end_guaranteed(fz_context* ctx, fz_output* out, const char bracket_close, int target_stack_level)
{
	size_t stack_offset = strlen(json_stack);
	if (stack_offset == 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "JSON tracking stack underflow");

	// see if we need to pop off elements off the stack:
	stack_offset = write_level_guarantee_level(ctx, out, target_stack_level + 1);

	if (json_stack[stack_offset - 1] != bracket_close)
		fz_throw(ctx, FZ_ERROR_GENERIC, "JSON tracking stack: mismatching closing bracket");
	json_stack[stack_offset - 1] = 0;

	fz_write_printf(ctx, out, "\n%c", bracket_close);

	// Also set the JSON formatting output state as this closes an object, so the next sibling should be preceded by a comma at least:
	json_sep_state = 2;

	return (int)stack_offset - 1;
}

static int write_item_starter_block(fz_context* ctx, fz_output* out, const char* label, const char bracket_open)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: ",
		label);

	return write_level_start(ctx, out, bracket_open);
}

static void write_item_starter(fz_context* ctx, fz_output* out, const char* label)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "%jq: ",
		label);
}

static void
showglobalinfo(fz_context* ctx, globals* glo)
{
	pdf_obj* obj;
	fz_output* out = glo->out;
	pdf_document* doc = glo->doc;
	int version = pdf_version(ctx, doc);

	int json_stack_level = write_item_starter_block(ctx, out, "GlobalInfo", '{');

	fz_try(ctx)
	{
		write_item_printf(ctx, out, "Version", "\"PDF-%d.%d\"", version / 10, version % 10);

		if (!has_password_troubles)
		{
			obj = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Info));
			if (obj)
			{
				write_item_starter(ctx, out, "Info");
				pdf_print_obj_to_json(ctx, out, obj, PRINT_OBJ_TO_JSON_FLAGS);
			}
		}

		obj = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Encrypt));
		if (obj)
		{
			write_item_starter(ctx, out, "Encryption");
			pdf_print_obj_to_json(ctx, out, obj, PRINT_OBJ_TO_JSON_FLAGS | PDF_PRINT_JSON_STRING_OBJECTS_AS_BLOB | PDF_PRINT_LIMITED_ARRAY_DUMP);
		}

		obj = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/Metadata");
		if (obj)
		{
			write_item_starter(ctx, out, "Metadata");
			pdf_print_obj_to_json(ctx, out, obj, PRINT_OBJ_TO_JSON_FLAGS | PDF_PRINT_LIMITED_ARRAY_DUMP | PDF_DO_NOT_THROW_ON_CONTENT_PARSE_FAULTS);
		}

		if (!has_password_troubles)
		{
			write_item_int(ctx, out, "Pages", glo->pagecount);

			int chaptercount = fz_count_chapters(ctx, fz_document_from_pdf_document(ctx, glo->doc));

			write_item_int(ctx, out, "Chapters", chaptercount);

			if (chaptercount > 1)
			{
				write_item_starter_block(ctx, out, "ChapterPages", '[');

				for (int i = 0; i < chaptercount; i++)
				{
					int count = fz_count_chapter_pages(ctx, fz_document_from_pdf_document(ctx, glo->doc), i);

					write_sep(ctx, out);
					fz_write_printf(ctx, out, "%d", count);
				}

				write_level_end(ctx, out, ']');
			}

			fz_outline* outlines = NULL;
			int json_stack_outlines_level = -1;

			fz_try(ctx)
			{
				outlines = pdf_load_outline(ctx, doc);

				if (outlines)
				{
					json_stack_outlines_level = write_item_starter_block(ctx, out, "DocumentOutlines", '[');

					fz_outline* outline_parents[500];
					int parents_index = 0;
					fz_outline* outline = outlines;

					while (outline)
					{
						write_sep(ctx, out);
						write_level_start(ctx, out, '{');

						if (!fz_is_external_link(ctx, outline->uri))
						{
							int target_chapter = outline->page.chapter + 1;
							int target_page = outline->page.page + 1;

							write_item(ctx, out, "InternalLink", outline->uri);
							write_item_int(ctx, out, "TargetChapterNumber", target_chapter);
							write_item_int(ctx, out, "TargetPageNumber", target_page);
							write_item_coord(ctx, out, "TargetCoordinates", outline->x, outline->y);
						}
						else
						{
							write_item(ctx, out, "ExternalLink", outline->uri);
						}

						write_item(ctx, out, "Title", outline->title);
						write_item_bool(ctx, out, "IsOpen", outline->is_open);

						if (outline->down)
						{
							outline_parents[parents_index++] = outline->next;
							if (parents_index >= nelem(outline_parents))
							{
								fz_throw(ctx, FZ_ERROR_GENERIC, "PDF Outline has too many levels: %d or more!", (int)nelem(outline_parents));
							}

							write_item_starter_block(ctx, out, "Children", '[');

							outline = outline->down;
						}
						else
						{
							write_level_end(ctx, out, '}');

							outline = outline->next;
							while (!outline && parents_index > 0)
							{
								outline = outline_parents[--parents_index];

								write_level_end(ctx, out, ']');
								write_level_end(ctx, out, '}');
							}
						}
					}
				}
			}
			fz_always(ctx)
			{
				if (json_stack_outlines_level >= 0)
					write_level_end_guaranteed(ctx, out, ']', json_stack_outlines_level);

				fz_drop_outline(ctx, outlines);
			}
			fz_catch(ctx)
			{
				fz_error(ctx, "Error while processing PDF Outlines: %s\n", fz_caught_message(ctx));
			}
		}

		// See if there are any embedded files:
		pdf_obj* files_obj = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/Names");
		if (files_obj)
		{
			write_item_starter(ctx, out, "AttachedFiles");
			pdf_print_obj_to_json(ctx, out, files_obj, PRINT_OBJ_TO_JSON_FLAGS | PDF_PRINT_LIMITED_ARRAY_DUMP);
		}

		// See if there's any embedded JavaScript code
		{
			pdf_obj* javascript;
			int len, i;
			
			javascript = pdf_load_name_tree(ctx, doc, PDF_NAME(JavaScript));
			len = pdf_dict_len(ctx, javascript);

			if (len > 0)
			{
				int js_outline_level = write_item_starter_block(ctx, out, "EmbeddedJavaScripts", '[');

				fz_try(ctx)
				{
					for (i = 0; i < len; i++)
					{
						pdf_obj* fragment = pdf_dict_get_val(ctx, javascript, i);
						pdf_obj* code = pdf_dict_get(ctx, fragment, PDF_NAME(JS));
						size_t codebuflength = 0;
						char* codebuf = pdf_load_stream_or_string_as_utf8(ctx, code, &codebuflength);

						char buf[100];
						if (pdf_is_indirect(ctx, code))
							fz_snprintf(buf, sizeof buf, "%d", pdf_to_num(ctx, code));
						else
							fz_snprintf(buf, sizeof buf, "Root/Names/JavaScript/Names/%d/JS", (i + 1) * 2);

						write_sep(ctx, out);
						write_level_start(ctx, out, '{');
						write_item(ctx, out, "JSFilePath", buf);
						write_item_int(ctx, out, "JSScriptLength", codebuflength);

						// basic validation check of the JS source file
						if (strlen(codebuf) != codebuflength)
						{
							write_item_bool(ctx, out, "JSHasEmbeddedNULCharacters", 1);
							fz_error(ctx, "PDF Javascript file %q has embedded NUL characters.", buf);
						}

						write_item(ctx, out, "JSScriptContent", codebuf);
						fz_free(ctx, codebuf);

						write_level_end(ctx, out, '}');
					}
				}
				fz_always(ctx)
				{
					pdf_drop_obj(ctx, javascript);

					write_level_end_guaranteed(ctx, out, ']', js_outline_level);
				}
				fz_catch(ctx)
				{
					fz_rethrow(ctx);
				}
			}
		}

		// and dump a PDF internal aspects list while we're at it: use that
		// for test bench diagnostics where you want to dig out PDFs with particular
		// 'aspects' to them for (regression) testing.
		// Also useful to help diagnose a PDF to see "what's so special about *this* one".
	}
	fz_always(ctx)
	{
		write_level_guarantee_level(ctx, out, json_stack_level + 1);
		dump_observed_errors(ctx, out);
		write_level_end_guaranteed(ctx, out, '}', json_stack_level);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}


static void
gatherdimensions(fz_context* ctx, globals* glo, int page, pdf_obj* pageref)
{
	fz_rect bbox;
	pdf_obj* obj;
	int j;

	obj = pdf_dict_get(ctx, pageref, PDF_NAME(MediaBox));
	if (!pdf_is_array(ctx, obj))
		return;

	bbox = pdf_to_rect(ctx, obj);

	obj = pdf_dict_get(ctx, pageref, PDF_NAME(UserUnit));
	if (pdf_is_number(ctx, obj))
	{
		float unit = pdf_to_real(ctx, obj);
		bbox.x0 *= unit;
		bbox.y0 *= unit;
		bbox.x1 *= unit;
		bbox.y1 *= unit;
	}

	for (j = 0; j < glo->dims; j++)
		if (!memcmp(glo->dim[j].u.dim.bbox, &bbox, sizeof(fz_rect)))
			break;

	if (j < glo->dims)
		return;

	glo->dim = fz_realloc_array(ctx, glo->dim, glo->dims + 1, struct info);
	glo->dims++;

	glo->dim[glo->dims - 1].page = page;
	glo->dim[glo->dims - 1].pageref = pageref;
	glo->dim[glo->dims - 1].u.dim.bbox = NULL;
	glo->dim[glo->dims - 1].u.dim.bbox = fz_malloc(ctx, sizeof(fz_rect));
	memcpy(glo->dim[glo->dims - 1].u.dim.bbox, &bbox, sizeof(fz_rect));

	return;
}

static void
gatherfonts(fz_context* ctx, globals* glo, int page, pdf_obj* pageref, pdf_obj* dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj* fontdict = NULL;
		pdf_obj* subtype = NULL;
		pdf_obj* basefont = NULL;
		pdf_obj* name = NULL;
		pdf_obj* encoding = NULL;
		int k;

		fontdict = pdf_dict_get_val(ctx, dict, i);
		if (!pdf_is_dict(ctx, fontdict))
		{
			fz_warn(ctx, "not a font dict (%d 0 R)", pdf_to_num(ctx, fontdict));
			continue;
		}

		subtype = pdf_dict_get(ctx, fontdict, PDF_NAME(Subtype));
		basefont = pdf_dict_get(ctx, fontdict, PDF_NAME(BaseFont));
		if (!basefont || pdf_is_null(ctx, basefont))
			name = pdf_dict_get(ctx, fontdict, PDF_NAME(Name));
		encoding = pdf_dict_get(ctx, fontdict, PDF_NAME(Encoding));
		if (pdf_is_dict(ctx, encoding))
			encoding = pdf_dict_get(ctx, encoding, PDF_NAME(BaseEncoding));

		for (k = 0; k < glo->fonts; k++)
			if (!pdf_objcmp(ctx, glo->font[k].u.font.obj, fontdict))
				break;

		if (k < glo->fonts)
			continue;

		glo->font = fz_realloc_array(ctx, glo->font, glo->fonts + 1, struct info);
		glo->fonts++;

		glo->font[glo->fonts - 1].page = page;
		glo->font[glo->fonts - 1].pageref = pageref;
		glo->font[glo->fonts - 1].u.font.obj = fontdict;
		glo->font[glo->fonts - 1].u.font.subtype = subtype;
		glo->font[glo->fonts - 1].u.font.name = basefont ? basefont : name;
		glo->font[glo->fonts - 1].u.font.encoding = encoding;
	}
}

static void
gatherimages(fz_context* ctx, globals* glo, int page, pdf_obj* pageref, pdf_obj* dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj* imagedict;
		pdf_obj* type;
		pdf_obj* width;
		pdf_obj* height;
		pdf_obj* bpc = NULL;
		pdf_obj* filter = NULL;
		pdf_obj* cs = NULL;
		pdf_obj* altcs;
		int k;

		imagedict = pdf_dict_get_val(ctx, dict, i);
		if (!pdf_is_dict(ctx, imagedict))
		{
			fz_warn(ctx, "not an image dict (%d 0 R)", pdf_to_num(ctx, imagedict));
			continue;
		}

		type = pdf_dict_get(ctx, imagedict, PDF_NAME(Subtype));
		if (!pdf_name_eq(ctx, type, PDF_NAME(Image)))
			continue;

		filter = pdf_dict_get(ctx, imagedict, PDF_NAME(Filter));

		altcs = NULL;
		cs = pdf_dict_get(ctx, imagedict, PDF_NAME(ColorSpace));
		if (pdf_is_array(ctx, cs))
		{
			pdf_obj* cses = cs;

			cs = pdf_array_get(ctx, cses, 0);
			if (pdf_name_eq(ctx, cs, PDF_NAME(DeviceN)) || pdf_name_eq(ctx, cs, PDF_NAME(Separation)))
			{
				altcs = pdf_array_get(ctx, cses, 2);
				if (pdf_is_array(ctx, altcs))
					altcs = pdf_array_get(ctx, altcs, 0);
			}
		}

		width = pdf_dict_get(ctx, imagedict, PDF_NAME(Width));
		height = pdf_dict_get(ctx, imagedict, PDF_NAME(Height));
		bpc = pdf_dict_get(ctx, imagedict, PDF_NAME(BitsPerComponent));

		for (k = 0; k < glo->images; k++)
			if (!pdf_objcmp(ctx, glo->image[k].u.image.obj, imagedict))
				break;

		if (k < glo->images)
			continue;

		glo->image = fz_realloc_array(ctx, glo->image, glo->images + 1, struct info);
		glo->images++;

		glo->image[glo->images - 1].page = page;
		glo->image[glo->images - 1].pageref = pageref;
		glo->image[glo->images - 1].u.image.obj = imagedict;
		glo->image[glo->images - 1].u.image.width = width;
		glo->image[glo->images - 1].u.image.height = height;
		glo->image[glo->images - 1].u.image.bpc = bpc;
		glo->image[glo->images - 1].u.image.filter = filter;
		glo->image[glo->images - 1].u.image.cs = cs;
		glo->image[glo->images - 1].u.image.altcs = altcs;
	}
}

static void
gatherforms(fz_context* ctx, globals* glo, int page, pdf_obj* pageref, pdf_obj* dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj* xobjdict;
		pdf_obj* type;
		pdf_obj* subtype;
		pdf_obj* group;
		pdf_obj* groupsubtype;
		pdf_obj* reference;
		int k;

		xobjdict = pdf_dict_get_val(ctx, dict, i);
		if (!pdf_is_dict(ctx, xobjdict))
		{
			fz_warn(ctx, "not a xobject dict (%d 0 R)", pdf_to_num(ctx, xobjdict));
			continue;
		}

		type = pdf_dict_get(ctx, xobjdict, PDF_NAME(Subtype));
		if (!pdf_name_eq(ctx, type, PDF_NAME(Form)))
			continue;

		subtype = pdf_dict_get(ctx, xobjdict, PDF_NAME(Subtype2));
		if (!pdf_name_eq(ctx, subtype, PDF_NAME(PS)))
			continue;

		group = pdf_dict_get(ctx, xobjdict, PDF_NAME(Group));
		groupsubtype = pdf_dict_get(ctx, group, PDF_NAME(S));
		reference = pdf_dict_get(ctx, xobjdict, PDF_NAME(Ref));

		for (k = 0; k < glo->forms; k++)
			if (!pdf_objcmp(ctx, glo->form[k].u.form.obj, xobjdict))
				break;

		if (k < glo->forms)
			continue;

		glo->form = fz_realloc_array(ctx, glo->form, glo->forms + 1, struct info);
		glo->forms++;

		glo->form[glo->forms - 1].page = page;
		glo->form[glo->forms - 1].pageref = pageref;
		glo->form[glo->forms - 1].u.form.obj = xobjdict;
		glo->form[glo->forms - 1].u.form.groupsubtype = groupsubtype;
		glo->form[glo->forms - 1].u.form.reference = reference;
	}
}

static void
gatherpsobjs(fz_context* ctx, globals* glo, int page, pdf_obj* pageref, pdf_obj* dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj* xobjdict;
		pdf_obj* type;
		pdf_obj* subtype;
		int k;

		xobjdict = pdf_dict_get_val(ctx, dict, i);
		if (!pdf_is_dict(ctx, xobjdict))
		{
			fz_warn(ctx, "not a xobject dict (%d 0 R)", pdf_to_num(ctx, xobjdict));
			continue;
		}

		type = pdf_dict_get(ctx, xobjdict, PDF_NAME(Subtype));
		subtype = pdf_dict_get(ctx, xobjdict, PDF_NAME(Subtype2));
		if (!pdf_name_eq(ctx, type, PDF_NAME(PS)) &&
			(!pdf_name_eq(ctx, type, PDF_NAME(Form)) || !pdf_name_eq(ctx, subtype, PDF_NAME(PS))))
			continue;

		for (k = 0; k < glo->psobjs; k++)
			if (!pdf_objcmp(ctx, glo->psobj[k].u.form.obj, xobjdict))
				break;

		if (k < glo->psobjs)
			continue;

		glo->psobj = fz_realloc_array(ctx, glo->psobj, glo->psobjs + 1, struct info);
		glo->psobjs++;

		glo->psobj[glo->psobjs - 1].page = page;
		glo->psobj[glo->psobjs - 1].pageref = pageref;
		glo->psobj[glo->psobjs - 1].u.form.obj = xobjdict;
	}
}

static void
gatherlayersinfo(fz_context* ctx, globals* glo)
{
	pdf_document* pdoc = glo->doc;
	fz_output* out = glo->out;
	int config;
	int n, j;
	pdf_layer_config info;

	if (!pdoc)
	{
		fz_warn(ctx, "Only PDF files have layers");
		return;
	}

	int num_configs = pdf_count_layer_configs(ctx, pdoc);

	if (num_configs > 0)
	{
		write_item_starter_block(ctx, out, "PDFLayerConfigs", '[');

		for (config = 0; config < num_configs; config++)
		{
			write_sep(ctx, out);
			write_level_start(ctx, out, '{');
			pdf_layer_config_info(ctx, pdoc, config, &info);
			write_item(ctx, out, "Name", info.name ? info.name : "");
			write_item(ctx, out, "Creator", info.creator ? info.creator : "");
			write_level_end(ctx, out, '}');
		}

		write_level_end(ctx, out, ']');
	}

	n = pdf_count_layer_config_ui(ctx, pdoc);
	if (n > 0)
	{
		write_item_starter_block(ctx, out, "PDFUILayerConfigs", '[');

		for (j = 0; j < n; j++)
		{
			pdf_layer_config_ui ui;

			pdf_layer_config_ui_info(ctx, pdoc, j, &ui);
			write_sep(ctx, out);
			write_level_start(ctx, out, '{');
			write_item_printf(ctx, out, "Depth", "%d", ui.depth);

			switch (ui.type)
			{
			case PDF_LAYER_UI_CHECKBOX:
				write_item(ctx, out, "Type", "CheckBox");
				write_item_bool(ctx, out, "Selected", ui.selected);
				break;

			case PDF_LAYER_UI_RADIOBOX:
				write_item(ctx, out, "Type", "RadioBox");
				write_item_bool(ctx, out, "Selected", ui.selected);
				break;

			case PDF_LAYER_UI_LABEL:
				write_item(ctx, out, "Type", "Label");
				break;

			default:
				write_item_printf(ctx, out, "Type", "\"UNKNOWN UI.TYPE:%d\"", (int)ui.type);
				break;
			}

			if (ui.type != PDF_LAYER_UI_LABEL)
			{
				write_item_bool(ctx, out, "Locked", ui.locked);
			}
			write_item(ctx, out, "Text", ui.text ? ui.text : "");

			write_level_end(ctx, out, '}');
		}
		write_level_end(ctx, out, ']');
	}
}

static void
gatherresourceinfo(fz_context *ctx, pdf_mark_list *mark_list, globals *glo, int page, pdf_obj *rsrc)
{
	pdf_obj* pageref;
	pdf_obj* font;
	pdf_obj* xobj;
	pdf_obj* subrsrc;
	int i;

	pageref = pdf_lookup_page_obj(ctx, glo->doc, page-1);
	if (!pageref)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);

	/* stop on cyclic resource dependencies */
	if (pdf_mark_list_push(ctx, mark_list, rsrc))
		return;

	font = pdf_dict_get(ctx, rsrc, PDF_NAME(Font));
	if (font)
	{
		int n;

		gatherfonts(ctx, glo, page, pageref, font);
		n = pdf_dict_len(ctx, font);
		for (i = 0; i < n; i++)
		{
			pdf_obj *obj = pdf_dict_get_val(ctx, font, i);

			/* stop on cyclic resource dependencies */
			if (pdf_mark_list_push(ctx, mark_list, obj))
				return;

			subrsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));
			if (subrsrc && pdf_objcmp(ctx, rsrc, subrsrc))
			{
				gatherresourceinfo(ctx, mark_list, glo, page, subrsrc);
			}
		}
	}

	xobj = pdf_dict_get(ctx, rsrc, PDF_NAME(XObject));
	if (xobj)
	{
		int n;

		gatherimages(ctx, glo, page, pageref, xobj);
		gatherforms(ctx, glo, page, pageref, xobj);
		gatherpsobjs(ctx, glo, page, pageref, xobj);

		n = pdf_dict_len(ctx, xobj);
		for (i = 0; i < n; i++)
		{
			pdf_obj *obj = pdf_dict_get_val(ctx, xobj, i);

			/* stop on cyclic resource dependencies */
			if (pdf_mark_list_push(ctx, mark_list, obj))
				return;

			subrsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));
			if (subrsrc && pdf_objcmp(ctx, rsrc, subrsrc))
			{
				gatherresourceinfo(ctx, mark_list, glo, page, subrsrc);
			}
		}
	}

}

static void
gatherpageinfo(fz_context* ctx, globals* glo, int page)
{
	pdf_mark_list mark_list;
	pdf_obj* pageref;
	pdf_obj* rsrc;

	pageref = pdf_lookup_page_obj(ctx, glo->doc, page - 1);

	if (!pageref)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);

	gatherdimensions(ctx, glo, page, pageref);

	pdf_mark_list_init(ctx, &mark_list);
	fz_try(ctx)
	{
		rsrc = pdf_dict_get(ctx, pageref, PDF_NAME(Resources));
		gatherresourceinfo(ctx, &mark_list, glo, page, rsrc);
	}
	fz_always(ctx)
		pdf_mark_list_free(ctx, &mark_list);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
printinfo(fz_context* ctx, globals* glo)
{
	int i;
	int j;
	fz_output* out = glo->out;

	if (glo->dims > 0)
	{
		write_item_starter_block(ctx, out, "Mediaboxes", '[');

		for (i = 0; i < glo->dims; i++)
		{
			write_sep(ctx, out);
			write_level_start(ctx, out, '{');
			write_item_int(ctx, out, "Page", glo->dim[i].page);
#if 0
			write_item_starter(ctx, out, "PageRef");
			pdf_print_obj_to_json(ctx, out, glo->dim[i].pageref, PRINT_OBJ_TO_JSON_FLAGS);
#endif
			write_item_bbox(ctx, out, "Bounds", glo->dim[i].u.dim.bbox);
			write_level_end(ctx, out, '}');
		}

		write_level_end(ctx, out, ']');
	}

	if (glo->fonts > 0 && !skip_fonts_dump)
	{
		write_item_int(ctx, out, "FontsCount", glo->fonts);
		write_item_starter_block(ctx, out, "Fonts", '[');

		for (i = 0; i < glo->fonts; i++)
		{
			write_sep(ctx, out);
			write_level_start(ctx, out, '{');
			write_item_int(ctx, out, "Page", glo->font[i].page);
#if 0
			write_item_starter(ctx, out, "PageRef");
			pdf_print_obj_to_json(ctx, out, glo->font[i].pageref, PRINT_OBJ_TO_JSON_FLAGS);
#endif
			write_item(ctx, out, "FontType", pdf_to_name(ctx, glo->font[i].u.font.subtype));
			write_item(ctx, out, "FontName", pdf_to_name(ctx, glo->font[i].u.font.name));
			write_item(ctx, out, "FontEncoding", glo->font[i].u.font.encoding ? pdf_to_name(ctx, glo->font[i].u.font.encoding) : "");
#if 0
			write_item_starter(ctx, out, "Font");
			pdf_print_obj_to_json(ctx, out, glo->font[i].u.font.obj, PRINT_OBJ_TO_JSON_FLAGS);
#endif
			write_level_end(ctx, out, '}');
		}

		write_level_end(ctx, out, ']');
	}

	if (glo->images > 0 && !skip_images_dump)
	{
		int n = glo->images;
		write_item_int(ctx, out, "ImagesCount", n);
		if (n > 42)
		{
			// cut back on large numbers of images: there'll be a lot of nonsense then anyway.
			n = 20;
			write_item_int(ctx, out, "__MuPDFRestrictedImagesCount__", n);
		}
		write_item_starter_block(ctx, out, "Images", '[');

		for (i = 0; i < n; i++)
		{
			write_sep(ctx, out);
			write_level_start(ctx, out, '{');
			write_item_int(ctx, out, "Page", glo->image[i].page);
#if 0
			write_item_starter(ctx, out, "PageRef");
			pdf_print_obj_to_json(ctx, out, glo->image[i].pageref, PRINT_OBJ_TO_JSON_FLAGS);
#endif
			if (pdf_is_array(ctx, glo->image[i].u.image.filter))
			{
				int n = pdf_array_len(ctx, glo->image[i].u.image.filter);

				write_item_int(ctx, out, "ImageFiltersCount", n);
				write_item_starter_block(ctx, out, "ImageFilters", '[');

				for (j = 0; j < n; j++)
				{
					pdf_obj* obj = pdf_array_get(ctx, glo->image[i].u.image.filter, j);
					char* filter = fz_strdup(ctx, pdf_to_name(ctx, obj));

					if (strstr(filter, "Decode"))
						*(strstr(filter, "Decode")) = '\0';

					write_sep(ctx, out);
					write_string(ctx, out, filter);
					fz_free(ctx, filter);
				}
				write_level_end(ctx, out, ']');
			}
			else if (glo->image[i].u.image.filter)
			{
				pdf_obj* obj = glo->image[i].u.image.filter;
				char* filter = fz_strdup(ctx, pdf_to_name(ctx, obj));

				if (strstr(filter, "Decode"))
					*(strstr(filter, "Decode")) = '\0';

				write_item(ctx, out, "ImageFilter", filter);
				fz_free(ctx, filter);
			}
			else
			{
				write_item(ctx, out, "Imagefilter", "Raw");
			}

			char* cs = NULL;
			char* altcs = NULL;

			if (glo->image[i].u.image.cs)
			{
				cs = fz_strdup(ctx, pdf_to_name(ctx, glo->image[i].u.image.cs));

				if (!strncmp(cs, "Device", 6))
				{
					size_t len = strlen(cs + 6);
					memmove(cs + 3, cs + 6, len + 1);
					cs[3 + len + 1] = '\0';
				}
				if (strstr(cs, "ICC"))
					fz_strncpy_s(ctx, cs, "ICC", 4);
				if (strstr(cs, "Indexed"))
					fz_strncpy_s(ctx, cs, "Idx", 4);
				if (strstr(cs, "Pattern"))
					fz_strncpy_s(ctx, cs, "Pat", 4);
				if (strstr(cs, "Separation"))
					fz_strncpy_s(ctx, cs, "Sep", 4);
			}
			if (glo->image[i].u.image.altcs)
			{
				altcs = fz_strdup(ctx, pdf_to_name(ctx, glo->image[i].u.image.altcs));

				if (!strncmp(altcs, "Device", 6))
				{
					size_t len = strlen(altcs + 6);
					memmove(altcs + 3, altcs + 6, len + 1);
					altcs[3 + len + 1] = '\0';
				}
				if (strstr(altcs, "ICC"))
					fz_strncpy_s(ctx, altcs, "ICC", 4);
				if (strstr(altcs, "Indexed"))
					fz_strncpy_s(ctx, altcs, "Idx", 4);
				if (strstr(altcs, "Pattern"))
					fz_strncpy_s(ctx, altcs, "Pat", 4);
				if (strstr(altcs, "Separation"))
					fz_strncpy_s(ctx, altcs, "Sep", 4);
			}

			write_item_int(ctx, out, "ImageWidth", pdf_to_int(ctx, glo->image[i].u.image.width));
			write_item_int(ctx, out, "ImageHeight", pdf_to_int(ctx, glo->image[i].u.image.height));
			write_item_printf(ctx, out, "ImageDimensions", "{ %q: %g, %q: %g }",
				"W", pdf_to_real(ctx, glo->image[i].u.image.width),
				"H", pdf_to_real(ctx, glo->image[i].u.image.height));
			write_item_int(ctx, out, "ImageBPC", glo->image[i].u.image.bpc ? pdf_to_int(ctx, glo->image[i].u.image.bpc) : 1);
			write_item_printf(ctx, out, "ImageCS", "\"%s%s%s\"",
				glo->image[i].u.image.cs ? cs : "ImageMask",
				glo->image[i].u.image.altcs ? " " : "",
				glo->image[i].u.image.altcs ? altcs : "");
			{
				pdf_obj* img_obj = glo->image[i].u.image.obj;
				int img_of_known_type = 0;

				if (pdf_is_stream(ctx, img_obj))
				{
					img_obj = pdf_resolve_indirect_chain(ctx, img_obj);
					pdf_obj* colorspace = pdf_resolve_indirect(ctx, pdf_dict_get(ctx, img_obj, PDF_NAME(ColorSpace)));
					const char* img_type = NULL;
					const char* dev_type = NULL;

					if (pdf_is_array(ctx, colorspace))
					{
						img_type = pdf_to_name(ctx, pdf_array_get(ctx, colorspace, 0));
						dev_type = pdf_to_name(ctx, pdf_array_get(ctx, colorspace, 1));

						img_of_known_type = (img_type && dev_type);
					}
					else if (pdf_is_name(ctx, colorspace))
					{
						img_type = pdf_to_name(ctx, colorspace);

						img_of_known_type = (!!img_type);
					}

					if (img_of_known_type)
					{
						write_item(ctx, out, "ImageColorSpace", img_type);
						if (dev_type)
							write_item(ctx, out, "ImageColorMode", dev_type);
					}
				}
				if (!img_of_known_type)
				{
					write_item_starter(ctx, out, "Image");
					pdf_print_obj_to_json(ctx, out, glo->image[i].u.image.obj, PRINT_OBJ_TO_JSON_FLAGS | PDF_PRINT_LIMITED_ARRAY_DUMP);
				}
			}

			fz_free(ctx, cs);
			fz_free(ctx, altcs);

			write_level_end(ctx, out, '}');
		}

		write_level_end(ctx, out, ']');
	}

	if (glo->forms > 0 && !skip_forms_dump)
	{
		write_item_int(ctx, out, "FormXObjectsCount", glo->forms);
		write_item_starter_block(ctx, out, "FormXObjects", '[');

		for (i = 0; i < glo->forms; i++)
		{
			write_sep(ctx, out);
			write_level_start(ctx, out, '{');
			write_item_int(ctx, out, "Page", glo->form[i].page);
#if 0
			write_item_starter(ctx, out, "PageRef");
			pdf_print_obj_to_json(ctx, out, glo->form[i].pageref, PRINT_OBJ_TO_JSON_FLAGS);
#endif
			write_item_printf(ctx, out, "FormType", "\"Form%s%s%s%s\"",
				glo->form[i].u.form.groupsubtype ? " " : "",
				glo->form[i].u.form.groupsubtype ? pdf_to_name(ctx, glo->form[i].u.form.groupsubtype) : "",
				glo->form[i].u.form.groupsubtype ? " Group" : "",
				glo->form[i].u.form.reference ? " Reference" : "");
			write_item_starter(ctx, out, "Form");
			pdf_print_obj_to_json(ctx, out, glo->form[i].u.form.obj, PRINT_OBJ_TO_JSON_FLAGS);
			write_level_end(ctx, out, '}');
		}

		write_level_end(ctx, out, ']');
	}

	if (glo->psobjs > 0 && !skip_psobjects_dump)
	{
		write_item_int(ctx, out, "PostscriptXObjectsCount", glo->psobjs);
		write_item_starter_block(ctx, out, "PostscriptXObjects", '[');

		for (i = 0; i < glo->psobjs; i++)
		{
			write_sep(ctx, out);
			write_level_start(ctx, out, '{');
			write_item_int(ctx, out, "Page", glo->psobj[i].page);
#if 0
			write_item_starter(ctx, out, "PageRef");
			pdf_print_obj_to_json(ctx, out, glo->psobj[i].pageref, PRINT_OBJ_TO_JSON_FLAGS);
#endif
			write_item_starter(ctx, out, "Form");
			pdf_print_obj_to_json(ctx, out, glo->psobj[i].u.form.obj, PRINT_OBJ_TO_JSON_FLAGS);
			write_level_end(ctx, out, '}');
		}

		write_level_end(ctx, out, ']');
	}
}

static void
show_annot_info(fz_context* ctx, fz_output* out, fz_matrix ctm, pdf_annot* annot)
{
	fz_rect bounds_in_doc = pdf_annot_rect(ctx, annot);

	write_item_bbox(ctx, out, "BoundsInDocument", &bounds_in_doc);

	fz_rect bounds = pdf_bound_annot(ctx, annot);
	bounds = fz_transform_rect(bounds, ctm);
	//fz_irect area = fz_irect_from_rect(bounds);

	write_item_bbox(ctx, out, "Bounds", &bounds);

	write_item_bool(ctx, out, "NeedsNewAP", pdf_annot_needs_new_ap(ctx, annot));

	const char* author = NULL;
	fz_try(ctx)
	{
		author = pdf_annot_author(ctx, annot);
	}
	fz_catch(ctx)
	{
		author = NULL;
	}
	if (author)
	{
		write_item(ctx, out, "Author", author);
	}

	{
		// check if the Date nodes exist: only if they do, do we print their contents.
		pdf_obj* cdo = pdf_dict_get(ctx, pdf_annot_obj(ctx, annot), PDF_NAME(CreationDate));
		pdf_obj* mdo = pdf_dict_get(ctx, pdf_annot_obj(ctx, annot), PDF_NAME(M));
		if (cdo || mdo)
		{
			write_item_date(ctx, out, "CreationDate", pdf_annot_creation_date(ctx, annot));
			write_item_date(ctx, out, "ModificationDate", pdf_annot_modification_date(ctx, annot));
		}
	}

	int flags = pdf_annot_flags(ctx, annot);

	char buf[1024];

	buf[0] = 0;

	if (flags & PDF_ANNOT_IS_INVISIBLE)
		fz_strlcat(buf, "invisible | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_HIDDEN)
		fz_strlcat(buf, "hidden | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_PRINT)
		fz_strlcat(buf, "print | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_NO_ZOOM)
		fz_strlcat(buf, "no_zoom | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_NO_ROTATE)
		fz_strlcat(buf, "no_rotate | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_NO_VIEW)
		fz_strlcat(buf, "no_view | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_READ_ONLY)
		fz_strlcat(buf, "read_only | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_LOCKED)
		fz_strlcat(buf, "locked | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_TOGGLE_NO_VIEW)
		fz_strlcat(buf, "toggle_no_view | ", sizeof(buf));
	if (flags & PDF_ANNOT_IS_LOCKED_CONTENTS)
		fz_strlcat(buf, "locked_contents | ", sizeof(buf));
	if (*buf) {
		char* end = buf + strlen(buf);
		end[-3] = 0;
	}
	else
	{
		fz_strlcat(buf, "<none>", sizeof(buf));
	}
	write_item(ctx, out, "Flags", buf);

	{
		fz_rect popup_bounds = pdf_annot_popup(ctx, annot);
		write_item_bbox(ctx, out, "PopupBounds", &popup_bounds);
	}

	write_item_bool(ctx, out, "HasInkList", pdf_annot_has_ink_list(ctx, annot));
	write_item_bool(ctx, out, "HasQuadPoints", pdf_annot_has_quad_points(ctx, annot));
	write_item_bool(ctx, out, "HasVertexData", pdf_annot_has_vertices(ctx, annot));
	write_item_bool(ctx, out, "HasLineData", pdf_annot_has_line(ctx, annot));
	write_item_bool(ctx, out, "HasInteriorColor", pdf_annot_has_interior_color(ctx, annot));
	write_item_bool(ctx, out, "HasLineEndingStyles", pdf_annot_has_line_ending_styles(ctx, annot));
	write_item_bool(ctx, out, "HasIconName", pdf_annot_has_icon_name(ctx, annot));
	write_item_bool(ctx, out, "HasOpenAction", pdf_annot_has_open(ctx, annot));
	write_item_bool(ctx, out, "HasAuthorData", pdf_annot_has_author(ctx, annot));
	write_item_bool(ctx, out, "IsActive", pdf_annot_active(ctx, annot));
	write_item_bool(ctx, out, "IsHot", pdf_annot_hot(ctx, annot));

	/*
		Retrieve the annotations text language (either from the
		annotation, or from the document).
	*/
	{
		fz_text_language lang = pdf_annot_language(ctx, annot);
		write_item(ctx, out, "Language", fz_string_from_text_language(buf, lang));
	}

	if (pdf_annot_has_icon_name(ctx, annot))
	{
		write_item(ctx, out, "Icon", pdf_annot_icon_name(ctx, annot));
	}

	{
		int field_flags = pdf_annot_field_flags(ctx, annot);
		const char* field_value = pdf_annot_field_value(ctx, annot);
		const char* field_key = pdf_annot_field_label(ctx, annot);

		if (field_flags || field_value || field_key)
		{
			buf[0] = 0;

			/* All fields */
			if (field_flags & PDF_FIELD_IS_READ_ONLY)
				fz_strlcat(buf, "read_only | ", sizeof(buf));
			if (field_flags & PDF_FIELD_IS_REQUIRED)
				fz_strlcat(buf, "required | ", sizeof(buf));
			if (field_flags & PDF_FIELD_IS_NO_EXPORT)
				fz_strlcat(buf, "no_export | ", sizeof(buf));

			/* Text fields */
			if (field_flags & PDF_TX_FIELD_IS_MULTILINE)
				fz_strlcat(buf, "multiline | ", sizeof(buf));
			if (field_flags & PDF_TX_FIELD_IS_PASSWORD)
				fz_strlcat(buf, "password | ", sizeof(buf));
			if (field_flags & PDF_TX_FIELD_IS_FILE_SELECT)
				fz_strlcat(buf, "file_select | ", sizeof(buf));
			if (field_flags & PDF_TX_FIELD_IS_DO_NOT_SPELL_CHECK)
				fz_strlcat(buf, "do_not_spell_check | ", sizeof(buf));
			if (field_flags & PDF_TX_FIELD_IS_DO_NOT_SCROLL)
				fz_strlcat(buf, "do_not_scroll | ", sizeof(buf));
			if (field_flags & PDF_TX_FIELD_IS_COMB)
				fz_strlcat(buf, "comb | ", sizeof(buf));
			if (field_flags & PDF_TX_FIELD_IS_RICH_TEXT)
				fz_strlcat(buf, "rich_text | ", sizeof(buf));

			/* Button fields */
			if (field_flags & PDF_BTN_FIELD_IS_NO_TOGGLE_TO_OFF)
				fz_strlcat(buf, "no_toggle_to_off | ", sizeof(buf));
			if (field_flags & PDF_BTN_FIELD_IS_RADIO)
				fz_strlcat(buf, "radio | ", sizeof(buf));
			if (field_flags & PDF_BTN_FIELD_IS_PUSHBUTTON)
				fz_strlcat(buf, "pushbutton | ", sizeof(buf));
			if (field_flags & PDF_BTN_FIELD_IS_RADIOS_IN_UNISON)
				fz_strlcat(buf, "radios_in_unison | ", sizeof(buf));

			/* Choice fields */
			if (field_flags & PDF_CH_FIELD_IS_COMBO)
				fz_strlcat(buf, "combo | ", sizeof(buf));
			if (field_flags & PDF_CH_FIELD_IS_EDIT)
				fz_strlcat(buf, "edit | ", sizeof(buf));
			if (field_flags & PDF_CH_FIELD_IS_SORT)
				fz_strlcat(buf, "sort | ", sizeof(buf));
			if (field_flags & PDF_CH_FIELD_IS_MULTI_SELECT)
				fz_strlcat(buf, "multi_select | ", sizeof(buf));
			if (field_flags & PDF_CH_FIELD_IS_DO_NOT_SPELL_CHECK)
				fz_strlcat(buf, "do_not_spell_check | ", sizeof(buf));
			if (field_flags & PDF_CH_FIELD_IS_COMMIT_ON_SEL_CHANGE)
				fz_strlcat(buf, "commit_on_sel_change | ", sizeof(buf));

			if (*buf)
			{
				char* end = buf + strlen(buf);
				end[-3] = 0;
			}
			else
			{
				fz_strlcat(buf, "<none>", sizeof(buf));
			}
			write_item(ctx, out, "FieldFlags", buf);
			write_item(ctx, out, "FieldKey", field_key);
			write_item(ctx, out, "FieldValue", field_value);
		}
	}

	//if (pdf_annot_type(ctx, annot) == PDF_ANNOT_FILE_ATTACHMENT)
	{
		pdf_obj* fs = pdf_dict_get(ctx, pdf_annot_obj(ctx, annot), PDF_NAME(FS));
		if (fs)
		{
			write_item_bool(ctx, out, "IsEmbeddedFile", pdf_is_embedded_file(ctx, fs));

			pdf_embedded_file_params fs_params = { NULL };
			pdf_get_embedded_file_params(ctx, fs, &fs_params);
			write_item(ctx, out, "EmbeddedFileName", fs_params.filename);
			write_item(ctx, out, "EmbeddedFileType", fs_params.mimetype);
			write_item_int(ctx, out, "EmbeddedFileSize", fs_params.size);
			write_item_date(ctx, out, "EmbeddedFileCreationDate", fs_params.created);
			write_item_date(ctx, out, "EmbeddedFileModificationDate", fs_params.modified);
		}
	}

	{
		pdf_obj* obj = pdf_dict_get(ctx, pdf_annot_obj(ctx, annot), PDF_NAME(Popup));
		if (obj)
		{
			write_item_starter(ctx, out, "Popup");
			pdf_print_obj_to_json(ctx, out, obj, PRINT_OBJ_TO_JSON_FLAGS);
		}
	}

	{
		const char* contents = pdf_annot_contents(ctx, annot);

		write_item(ctx, out, "Contents", contents);
	}

	if (pdf_annot_type(ctx, annot) == PDF_ANNOT_WIDGET)
	{
		enum pdf_widget_type subtype = pdf_widget_type(ctx, annot);
		write_item(ctx, out, "WidgetType", pdf_string_from_widget_type(ctx, subtype));

		if (subtype == PDF_WIDGET_TYPE_TEXT)
		{
			write_item_int(ctx, out, "WidgetTextMaximumLength", pdf_text_widget_max_len(ctx, annot));
			enum pdf_widget_tx_format txf = pdf_text_widget_format(ctx, annot);
			write_item(ctx, out, "WidgetTextFormat", pdf_string_from_widget_tx_format(ctx, txf));
		}
	}
}

static void
printadvancedinfo(fz_context* ctx, globals* glo, int page, fz_gathered_statistics* stats)
{
	fz_output* out = glo->out;
	pdf_page* page_obj = NULL;
	fz_device* stats_dev = NULL;
	int json_stack_level = write_level_get_level(ctx);

	fz_try(ctx)
	{
		page_obj = pdf_load_page(ctx, glo->doc, page - 1);

		fz_rect mediabox = fz_bound_page(ctx, fz_page_from_pdf_page(ctx, page_obj));

		fz_matrix ctm = fz_pre_scale(fz_rotate(0), 1.0, 1.0);

		write_item_bbox(ctx, out, "PageBounds", &mediabox);

		stats_dev = fz_new_stats_device(ctx, stats, NULL);
		fz_run_page(ctx, fz_page_from_pdf_page(ctx, page_obj), stats_dev, fz_identity);
		// and pick up the updated statistics...
		fz_extract_device_statistics(ctx, stats_dev, stats);

		if (!skip_annotations_dump)
		{
			pdf_annot* annot;

			int an = 0;
			int wn = 0;
			for (annot = pdf_first_annot(ctx, page_obj); annot; annot = pdf_next_annot(ctx, annot))
				++an;
			for (annot = pdf_first_widget(ctx, page_obj); annot; annot = pdf_next_widget(ctx, annot))
				++wn;

			write_item_int(ctx, out, "AnnotationCount", an);
			write_item_int(ctx, out, "WidgetCount", wn);

			if (an > 0)
			{
				write_item_starter_block(ctx, out, "Annotations", '[');

				int idx;
				for (idx = 0, annot = pdf_first_annot(ctx, page_obj); annot; ++idx, annot = pdf_next_annot(ctx, annot))
				{
					write_sep(ctx, out);
					write_level_start(ctx, out, '{');

					int num = pdf_to_num(ctx, pdf_annot_obj(ctx, annot));
					enum pdf_annot_type subtype = pdf_annot_type(ctx, annot);
					write_item_int(ctx, out, "AnnotNumber", num);
					write_item(ctx, out, "AnnotType", pdf_string_from_annot_type(ctx, subtype));

					show_annot_info(ctx, out, ctm, annot);

					write_level_end(ctx, out, '}');
				}

				write_level_end(ctx, out, ']');
			}

			if (wn > 0)
			{
				write_item_starter_block(ctx, out, "Widgets", '[');

				int idx;
				for (idx = 0, annot = pdf_first_widget(ctx, page_obj); annot; ++idx, annot = pdf_next_widget(ctx, annot))
				{
					write_sep(ctx, out);
					write_level_start(ctx, out, '{');

					int num = pdf_to_num(ctx, pdf_annot_obj(ctx, annot));
					enum pdf_annot_type subtype = pdf_annot_type(ctx, annot);
					write_item_int(ctx, out, "AnnotNumber", num);
					write_item(ctx, out, "AnnotType", pdf_string_from_annot_type(ctx, subtype));

					show_annot_info(ctx, out, ctm, annot);

					write_level_end(ctx, out, '}');
				}

				write_level_end(ctx, out, ']');
			}
		}


		/*
		fz_link is a list of interactive links on a page.

		There is no relation between the order of the links in the
		list and the order they appear on the page. The list of links
		for a given page can be obtained from fz_load_links.

		A link is reference counted. Dropping a reference to a link is
		done by calling fz_drop_link.

		rect: The hot zone. The area that can be clicked in
		untransformed coordinates.

		uri: Link destinations come in two forms: internal and external.
		Internal links refer to other pages in the same document.
		External links are URLs to other documents.

		next: A pointer to the next link on the same page.
		*/
		if (!skip_uri_links_dump)
		{
			fz_link* links = NULL;
			int json_stack_outlines_level = -1;
			int links_count = 0;

			fz_try(ctx)
			{
				links = fz_load_links(ctx, fz_page_from_pdf_page(ctx, page_obj));

				if (links)
				{
					json_stack_outlines_level = write_item_starter_block(ctx, out, "LinksInPage", '[');
				}

				fz_rect bounds;
				float link_x = 0.0f, link_y = 0.0f;
				fz_link* link = links;

				while (link)
				{
					write_sep(ctx, out);
					write_level_start(ctx, out, '{');

					bounds = link->rect;
					bounds = fz_transform_rect(bounds, ctm);
					//area = fz_irect_from_rect(bounds);

					if (!fz_is_external_link(ctx, link->uri))
					{
						fz_location loc = fz_resolve_link(ctx, fz_document_from_pdf_document(ctx, glo->doc), link->uri, &link_x, &link_y);
						int target_page = fz_page_number_from_location(ctx, fz_document_from_pdf_document(ctx, glo->doc), loc) + 1;

						links_count++;
						write_item(ctx, out, "LinkType", "Internal");
						write_item(ctx, out, "Url", link->uri);
						if (loc.chapter >= 0 && loc.page >= 0)
						{
							write_item_int(ctx, out, "TargetPageNumber", target_page);
							write_item_int(ctx, out, "TargetChapter", loc.chapter + 1);
							write_item_int(ctx, out, "TargetChapterPage", loc.page + 1);
						}
						else
						{
							write_item_int(ctx, out, "TargetErrorChapter", loc.chapter);
							write_item_int(ctx, out, "TargetErrorChapterPage", loc.page);
						}
						write_item_coord(ctx, out, "TargetCoordinates", link_x, link_y);
						write_item_bbox(ctx, out, "LinkBounds", &bounds);
					}
					else
					{
						write_item(ctx, out, "LinkType", "External");
						write_item(ctx, out, "Url", link->uri);
						write_item_bbox(ctx, out, "LinkBounds", &bounds);
					}
					write_level_end(ctx, out, '}');

					link = link->next;
				}
			}
			fz_always(ctx)
			{
				if (links)
				{
					write_level_end_guaranteed(ctx, out, ']', json_stack_outlines_level);
				}
				if (links_count > 0)
				{
					write_item_int(ctx, out, "LinksInPageCount", links_count);
				}

				fz_drop_link(ctx, links);
				//fz_drop_page(ctx, page_obj);
			}
			fz_catch(ctx)
			{
				fz_error(ctx, "Error while processing links in page %d\n", page);
			}
		}

		if (!skip_separations_dump)
		{
			fz_separations* seps = pdf_page_separations(ctx, page_obj);
			int count = fz_count_separations(ctx, seps);
			write_item_int(ctx, out, "PageSeparationsCount", count);

			if (count)
			{
				write_item_int(ctx, out, "PageActiveSeparationsCount", fz_count_active_separations(ctx, seps));

				write_item_starter_block(ctx, out, "PageSeparations", '[');

				int i;
				for (i = 0; i < count; i++)
				{
					write_sep(ctx, out);
					write_level_start(ctx, out, '{');

					write_item(ctx, out, "PageSeparationName", fz_separation_name(ctx, seps, i));

					fz_separation_behavior behaviour = fz_separation_current_behavior(ctx, seps, i);
					char buf[100];
					const char* beh_descr = buf;

					switch (behaviour)
					{
					case FZ_SEPARATION_COMPOSITE:
						beh_descr = "Composite";
						break;
					case FZ_SEPARATION_SPOT:
						beh_descr = "Spot";
						break;
					case FZ_SEPARATION_DISABLED:
						beh_descr = "Disabled";
						break;
					default:
						fz_snprintf(buf, sizeof(buf), "UNKOWN-%d", behaviour);
						break;
					}
					write_item(ctx, out, "PageSeparationBehaviour", beh_descr);

					write_level_end(ctx, out, '}');
				}

				write_level_end(ctx, out, ']');
			}

			fz_drop_separations(ctx, seps);
		}
	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, fz_page_from_pdf_page(ctx, page_obj));
		fz_drop_device(ctx, stats_dev);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "Error while loading/processing page %d: %s\n", page, fz_caught_message(ctx));
	}

	write_level_guarantee_level(ctx, out, json_stack_level);
}


typedef struct
{
	int max;
	int len;
	pdf_obj** sig;
} sigs_list;

static void
process_sigs(fz_context* ctx_, pdf_obj* field, void* arg, pdf_obj** ft)
{
	sigs_list* sigs = (sigs_list*)arg;

	if (!pdf_name_eq(ctx, pdf_dict_get(ctx, field, PDF_NAME(Type)), PDF_NAME(Annot)) ||
		!pdf_name_eq(ctx, pdf_dict_get(ctx, field, PDF_NAME(Subtype)), PDF_NAME(Widget)) ||
		!pdf_name_eq(ctx, pdf_dict_get(ctx, field, ft[0]), PDF_NAME(Sig)))
		return;

	if (sigs->len == sigs->max)
	{
		int newsize = sigs->max * 2;
		if (newsize == 0)
			newsize = 4;
		sigs->sig = fz_realloc_array(ctx, sigs->sig, newsize, pdf_obj*);
		sigs->max = newsize;
	}

	sigs->sig[sigs->len++] = field;
}

static char* short_signature_error_desc(pdf_signature_error err)
{
	switch (err)
	{
	case PDF_SIGNATURE_ERROR_OKAY:
		return "OK";
	case PDF_SIGNATURE_ERROR_NO_SIGNATURES:
		return "No signatures";
	case PDF_SIGNATURE_ERROR_NO_CERTIFICATE:
		return "No certificate";
	case PDF_SIGNATURE_ERROR_DIGEST_FAILURE:
		return "Invalid";
	case PDF_SIGNATURE_ERROR_SELF_SIGNED:
		return "Self-signed";
	case PDF_SIGNATURE_ERROR_SELF_SIGNED_IN_CHAIN:
		return "Self-signed in chain";
	case PDF_SIGNATURE_ERROR_NOT_TRUSTED:
		return "Untrusted";
	default:
	case PDF_SIGNATURE_ERROR_UNKNOWN:
		return "Unknown error";
	}
}

static void
printtail(fz_context* ctx, globals* glo, const fz_gathered_statistics* stats)
{
	fz_output* out = glo->out;
	char buf[1024];
	pdf_document* doc = glo->doc;
	fz_document* pdf = (fz_document*)doc;

	int general_info_stack_level = write_item_starter_block(ctx, out, "DocumentGeneralInfo", '{');

	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_TITLE, buf, sizeof buf) > 0)
		write_item(ctx, out, "Title", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_AUTHOR, buf, sizeof buf) > 0)
		write_item(ctx, out, "Author", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_FORMAT, buf, sizeof buf) > 0)
		write_item(ctx, out, "Format", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_ENCRYPTION, buf, sizeof buf) > 0)
		write_item(ctx, out, "Encryption", buf);

	int updates = pdf_count_versions(ctx, doc);

	if (!has_password_troubles)
	{
		if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_CREATOR, buf, sizeof buf) > 0)
			write_item(ctx, out, "PDF_Creator", buf);
		if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_PRODUCER, buf, sizeof buf) > 0)
			write_item(ctx, out, "PDF_Producer", buf);
		if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_SUBJECT, buf, sizeof buf) > 0)
			write_item(ctx, out, "Subject", buf);
		if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_KEYWORDS, buf, sizeof buf) > 0)
			write_item(ctx, out, "Keywords", buf);
		if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_CREATIONDATE, buf, sizeof buf) > 0)
			write_item(ctx, out, "Creation_Date", buf);
		if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_MODIFICATIONDATE, buf, sizeof buf) > 0)
			write_item(ctx, out, "Modification_Date", buf);

		{
			pdf_obj* info = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Info));

			if (info)
			{
				write_item_starter(ctx, out, "MetaInfoDictionary");
				pdf_print_obj_to_json(ctx, out, pdf_resolve_indirect_chain(ctx, info), PRINT_OBJ_TO_JSON_FLAGS);
			}
		}
	}

	buf[0] = 0;
	if (fz_has_permission(ctx, pdf, FZ_PERMISSION_PRINT))
		fz_strlcat(buf, "print, ", sizeof buf);
	if (fz_has_permission(ctx, pdf, FZ_PERMISSION_COPY))
		fz_strlcat(buf, "copy, ", sizeof buf);
	if (fz_has_permission(ctx, pdf, FZ_PERMISSION_EDIT))
		fz_strlcat(buf, "edit, ", sizeof buf);
	if (fz_has_permission(ctx, pdf, FZ_PERMISSION_ANNOTATE))
		fz_strlcat(buf, "annotate, ", sizeof buf);
	if (strlen(buf) > 2)
		buf[strlen(buf) - 2] = 0;
	else
		fz_strlcat(buf, "none", sizeof buf);
	write_item(ctx, out, "Permissions", buf);

	write_item_printf(ctx, out, "Status", "\"PDF %sdocument with %d update%s.\"",
		pdf_doc_was_linearized(ctx, doc) ? "linearized " : "",
		updates, updates > 1 ? "s" : "");
	write_item_bool(ctx, out, "DocWasLinearized", pdf_doc_was_linearized(ctx, doc));
	write_item_int(ctx, out, "DocumentUpdateCount", updates);

	if (updates > 0)
	{
		int n = pdf_validate_change_history(ctx, doc);

		char buf[256];

		if (n == 0)
			fz_strncpy_s(ctx, buf, "Change history seems valid.", sizeof(buf));
		else if (n == 1)
			fz_strncpy_s(ctx, buf, "Invalid changes made to the document in the last update.", sizeof(buf));
		else if (n == 2)
			fz_strncpy_s(ctx, buf, "Invalid changes made to the document in the penultimate update.", sizeof(buf));
		else
			fz_snprintf(buf, sizeof(buf), "Invalid changes made to the document %d updates ago.", n);
		write_item(ctx, out, "ChangeHistoryValidation", buf);
	}

	{
		sigs_list list = { 0, 0, NULL };
		static pdf_obj* ft_list[2] = { PDF_NAME(FT), NULL };
		pdf_obj* ft = NULL;
		pdf_obj* form_fields = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/AcroForm/Fields");
		pdf_walk_tree(ctx, form_fields, PDF_NAME(Kids), process_sigs, NULL, &list, &ft_list[0], &ft);

		if (list.len)
		{
			write_item_starter_block(ctx, out, "FormFieldSignatures", '[');

			int i;
			for (i = 0; i < list.len; i++)
			{
				pdf_obj* field = list.sig[i];
				int json_stack_sig_level = -1;

				fz_try(ctx)
				{
					write_sep(ctx, out);
					json_stack_sig_level = write_level_start(ctx, out, '{');

					if (pdf_signature_is_signed(ctx, doc, field))
					{
						pdf_pkcs7_verifier* verifier = pkcs7_openssl_new_verifier(ctx);
						pdf_signature_error sig_cert_error = pdf_check_certificate(ctx, verifier, doc, field);
						pdf_signature_error sig_digest_error = pdf_check_digest(ctx, verifier, doc, field);

						write_item(ctx, out, "Type", "Signed");
						write_item(ctx, out, "CERT", short_signature_error_desc(sig_cert_error));
						write_item_printf(ctx, out, "DIGEST", "\"%s%s\"",
							short_signature_error_desc(sig_digest_error),
							pdf_signature_incremental_change_since_signing(ctx, doc, field) ? ", Changed since" : "");

						pdf_drop_verifier(ctx, verifier);
					}
					else
					{
						write_item(ctx, out, "Type", "Unsigned");
					}
				}
				fz_catch(ctx)
				{
					write_level_guarantee_level(ctx, out, json_stack_sig_level + 1);
					write_item(ctx, out, "Type", "Error");
					write_item(ctx, out, "SignatureError", fz_caught_message(ctx));
				}
				write_level_end_guaranteed(ctx, out, '}', json_stack_sig_level);
			}

			write_level_end(ctx, out, ']');

			fz_free(ctx, list.sig);
		}
	}

	{
		char buf[256];

		if (updates == 0)
		{
			fz_strncpy_s(ctx, buf, "No updates since document creation", sizeof(buf));
		}
		else
		{
			int n = pdf_validate_change_history(ctx, doc);
			if (n == 0)
				fz_strncpy_s(ctx, buf, "Document changes conform to permissions", sizeof(buf));
			else
				fz_snprintf(buf, sizeof(buf), "Document permissions violated %d updates ago", n);
		}
		write_item(ctx, out, "UpdatesStatus", buf);
	}

	// This is one of the last things to write/report, as several activitiees of our COULD have triggered
	// SOME repair behaviour. Hence we only are assured we'll get a *final* value returned here if we
	// call this API *after* we've done all our other PDF metadata scan&report work.
	write_item_bool(ctx, out, "WasRepaired", pdf_was_repaired(ctx, glo->doc));

	write_item_bool(ctx, out, "NeedsPassword", pdf_needs_password(ctx, glo->doc));
	write_item_bool(ctx, out, "WasCryptedWithEmptyPassword", glo->doc->crypt && !pdf_needs_password(ctx, glo->doc));

	write_level_end_guaranteed(ctx, out, '}', general_info_stack_level);

	{
		int stats_stack_level = write_item_starter_block(ctx, out, "DocumentPrimitivesStatistics", '{');

		write_item_int(ctx, out, "RegularAlphaCount", stats->opaque_alpha);
		write_item_int(ctx, out, "TransparentAlphaCount", stats->transparent_alpha);
		write_item_int(ctx, out, "OutOfBoundsAlphaCount", stats->outofbounds_alpha);

		write_item_int(ctx, out, "NonOpaqueAlphaCount", stats->non_opaque_alpha);
		if (stats->non_opaque_alpha)
		{
			write_item_float(ctx, out, "NonOpaqueAlphaMax", stats->non_opaque_alpha_largest);
			write_item_float(ctx, out, "NonOpaqueAlphaMin", stats->non_opaque_alpha_smallest);
		}

		write_item_int(ctx, out, "LuminosityCount", stats->luminosity);
		if (stats->luminosity)
		{
			write_item_float(ctx, out, "LuminosityMax", stats->luminosity_largest);
			write_item_float(ctx, out, "LuminosityMin", stats->luminosity_smallest);
		}

		write_item_bits(ctx, out, "RenderingIntent", stats->color_params_aggregate.ri);
		write_item_bits(ctx, out, "BlackPointCompensation", stats->color_params_aggregate.bp);
		write_item_bits(ctx, out, "Overprinting", stats->color_params_aggregate.op);
		write_item_bits(ctx, out, "OverprintingModes", stats->color_params_aggregate.opm);

		write_item_bits(ctx, out, "BlendModes", stats->blendmodes_as_bits);

		if (fz_is_empty_rect(stats->bounds_smallest))
			write_item(ctx, out, "ContentBoundsSmallest", "(empty)");
		else
			write_item_bbox(ctx, out, "ContentBoundsSmallest", &stats->bounds_smallest);
		if (fz_is_infinite_rect(stats->bounds_largest))
			write_item(ctx, out, "ContentBoundsLargest", "(infinite)");
		else
			write_item_bbox(ctx, out, "ContentBoundsLargest", &stats->bounds_largest);

		int name_count = 0;
		const int max_names = sizeof(stats->layer_names) / sizeof(stats->layer_names[0]);
		for (int i = 0; i < max_names; i++)
		{
			const char* nm = stats->layer_names[i];
			if (!nm)
				break;
			name_count++;
		}
		write_item_int(ctx, out, "LayerNamesCount", name_count);
		if (name_count)
		{
			write_item_starter_block(ctx, out, "LayerNames", '[');
			for (int i = 0; i < max_names; i++)
			{
				const char* nm = stats->layer_names[i];
				if (!nm)
					break;

				write_sep(ctx, out);
				fz_write_printf(ctx, out, "%q", stats->layer_names[i]);
			}
			write_level_end(ctx, out, ']');
		}

		write_item_int(ctx, out, "FillPathCount", stats->fill_path);
		write_item_int(ctx, out, "StrokePathCount", stats->stroke_path);
		write_item_int(ctx, out, "ClipPathCount", stats->clip_path);
		write_item_int(ctx, out, "ClipStrokePathCount", stats->clip_stroke_path);
		write_item_int(ctx, out, "FillTextCount", stats->fill_text);
		write_item_int(ctx, out, "StrokeTextCount", stats->stroke_text);
		write_item_int(ctx, out, "ClipTextCount", stats->clip_text);
		write_item_int(ctx, out, "ClipStrokeTextCount", stats->clip_stroke_text);
		write_item_int(ctx, out, "IgnoreTextCount", stats->ignore_text);
		write_item_int(ctx, out, "FillShadeCount", stats->fill_shade);
		write_item_int(ctx, out, "FillImageCount", stats->fill_image);
		write_item_int(ctx, out, "FillImageMaskCount", stats->fill_image_mask);
		write_item_int(ctx, out, "ClipImageMaskCount", stats->clip_image_mask);

		write_item_int(ctx, out, "ClipCount", stats->clip_count);
		write_item_int(ctx, out, "ClipDepth", stats->clip_depth);
		write_item_int(ctx, out, "MaskCount", stats->mask_count);
		write_item_int(ctx, out, "MaskDepth", stats->mask_depth);
		write_item_int(ctx, out, "GroupCount", stats->group_count);
		write_item_int(ctx, out, "GroupDepth", stats->group_depth);
		write_item_int(ctx, out, "GroupKnockoutCount", stats->group_knockout);
		write_item_int(ctx, out, "GroupIsolatedCount", stats->group_isolated);

		write_item_int(ctx, out, "TileCount", stats->tile_count);
		write_item_int(ctx, out, "TileDepth", stats->tile_depth);

		write_item_int(ctx, out, "DefaultColorspacesCount", stats->default_colorspaces);

		write_item_int(ctx, out, "LayerCount", stats->layer_count);
		write_item_int(ctx, out, "LayerDepth", stats->layer_depth);

		write_item_int(ctx, out, "StackDepth", stats->stack_depth);

		write_item_int(ctx, out, "GreyColorspaceCount", stats->colorspaces.grey);
		write_item_int(ctx, out, "RgbColorspaceCount", stats->colorspaces.rgb);
		write_item_int(ctx, out, "CmykColorspaceCount", stats->colorspaces.cmyk);
		write_item_int(ctx, out, "IndexedColorspaceCount", stats->colorspaces.indexed);
		write_item_int(ctx, out, "IndexedColorspaceSmallestColorCount", stats->colorspaces.indexed_smallest);
		write_item_int(ctx, out, "IndexedColorspaceLargestColorCount", stats->colorspaces.indexed_largest);

		write_level_end_guaranteed(ctx, out, '}', stats_stack_level);
	}
}

static void
showinfo(fz_context* ctx, globals* glo, int spage, int epage, fz_gathered_statistics* stats, const char** first_important_error)
{
	int page;
	fz_output* out = glo->out;

	write_item_int(ctx, out, "FirstPage", spage);
	write_item_int(ctx, out, "LastPage", epage);
	int page_info_array_stack_level = write_item_starter_block(ctx, out, "PageInfo", '[');

	for (page = spage; page <= epage; page++)
	{
		int json_stack_level = -1;

		fz_try(ctx)
		{
			write_sep(ctx, out);
			json_stack_level = write_level_start(ctx, out, '{');

			write_item_int(ctx, out, "PageNumber", page);

			gatherpageinfo(ctx, glo, page);

			printinfo(ctx, glo);
			printadvancedinfo(ctx, glo, page, stats);
		}
		fz_always(ctx)
		{
			clearinfo(ctx, glo);
		}
		fz_catch(ctx)
		{
			write_level_guarantee_level(ctx, out, json_stack_level + 1);

			write_item(ctx, out, "PageError", fz_caught_message(ctx));
			if (*first_important_error == NULL) {
				*first_important_error = fz_strdup(ctx, fz_caught_message(ctx));
			}
		}

		write_level_guarantee_level(ctx, out, json_stack_level + 1);
		dump_observed_errors(ctx, out);
		write_level_end_guaranteed(ctx, out, '}', json_stack_level);
	}

	write_level_end_guaranteed(ctx, out, ']', page_info_array_stack_level);
}

static fz_error_print_callback* orig_error_print = NULL;
static fz_error_print_callback* orig_warn_print = NULL;

static struct logbuffer {
	size_t fill;		// offset where next message is to be appended to the logbuffer.
	char buf[10240];
	int overflow;
} logbuffer = { 0 };

static void do_buffer_log_output(const char* message)
{
	const char overflow_mark[] = "[...continued...]";

	size_t len = strlen(message);
	if (logbuffer.overflow == 0)
	{
		if (len + 1 + sizeof(overflow_mark) < sizeof(logbuffer.buf) - logbuffer.fill)
		{
			memcpy(logbuffer.buf + logbuffer.fill, message, len);
			logbuffer.fill += len;
			logbuffer.buf[logbuffer.fill++] = '\n';
			logbuffer.buf[logbuffer.fill] = 0;
		}
		else
		{
			logbuffer.overflow = 1;
			strcpy(logbuffer.buf + logbuffer.fill, overflow_mark);  // guaranteed safe: there always space left for this one!
		}
	}
}

static void error_print_callback(fz_context* ctx, void* user, const char* message)
{
	do_buffer_log_output(message);

	if (orig_error_print)
		orig_error_print(ctx, user, message);
}

static void warn_print_callback(fz_context* ctx, void* user, const char* message)
{
	do_buffer_log_output(message);

	if (orig_warn_print)
		orig_warn_print(ctx, user, message);
}

static void clear_observed_errors_logbuffers(fz_context* ctx)
{
	// make sure all warnings are flushed to output too so we clean it all...
	fz_flush_warnings(ctx);

	// reset buffer:
	logbuffer.fill = 0;
	logbuffer.overflow = 0;
}

// echo the fz_error and fz_warn output for this file/section into the output JSON for further analysis by the caller.
static void dump_observed_errors(fz_context* ctx, fz_output* out)
{
	// make sure all warnings are flushed to output before we grab the logbuffer and dump it...
	fz_flush_warnings(ctx);

	if (logbuffer.fill)
	{
		write_item_starter_block(ctx, out, "GatheredErrors", '{');
		write_item(ctx, out, "Log", logbuffer.buf);
		write_item_bool(ctx, out, "LogOverflow", logbuffer.overflow);
		write_level_end(ctx, out, '}');
	}
	clear_observed_errors_logbuffers(ctx);
}

static int
pdfinfo_info(fz_context* ctx, fz_output* out, const char* filename, const char* password)
{
	globals glo = { 0 };
	fz_gathered_statistics stats;
	const char* ex = NULL;
	int ret = EXIT_SUCCESS;

	fz_clear_statistics(ctx, &stats);

	glo.out = out;
	glo.ctx = ctx;

	int json_stack_file_level = -1;
	int json_stack_outermost_level = write_level_start(ctx, out, '[');

	fz_try(ctx)
	{
		write_sep(ctx, out);
		json_stack_file_level = write_level_start(ctx, out, '{');

		write_item(ctx, out, "DocumentFilePath", filename);

		glo.doc = pdf_open_document(glo.ctx, filename);
		if (!glo.doc)
		{
			usage();
			fz_throw(ctx, FZ_ERROR_GENERIC, "No document specified: cannot show info without document");
		}
		if (pdf_needs_password(ctx, glo.doc))
		{
			if (!pdf_authenticate_password(ctx, glo.doc, password))
			{
				if (!ignore_password_troubles)
				{
					fz_throw(glo.ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
				}
				else
				{
					has_password_troubles = 1;
				}
			}
		}

		glo.pagecount = pdf_count_pages(ctx, glo.doc);

		showglobalinfo(ctx, &glo);

		if (!has_password_troubles)
		{
			fz_info(ctx, "Retrieving info from pages %d-%d...\n", 1, glo.pagecount);

			showinfo(ctx, &glo, 1, glo.pagecount, &stats, &ex);
		}

		write_level_guarantee_level(ctx, out, json_stack_file_level + 1);  // PageSeries

		printtail(ctx, &glo, &stats);
	}
	fz_always(ctx)
	{
		closexref(ctx, &glo);

		write_level_guarantee_level(ctx, out, json_stack_file_level + 1);  // PageSeries

		dump_observed_errors(ctx, out);

		write_level_end_guaranteed(ctx, out, '}', json_stack_file_level);

		if (ex)
			ret = EXIT_FAILURE;
	}
	fz_catch(ctx)
	{
		// exceptions landing here have prevalence over page errors:
		fz_free(ctx, ex);
		ex = fz_strdup(ctx, fz_caught_message(ctx));
		ret = EXIT_FAILURE;

		write_level_guarantee_level(ctx, out, json_stack_outermost_level + 1);
	}

	// Now unwind the JSON stack, no matter how deep the errors came from:
	// when possible, dump the error infos in the last document record,
	// otherwise append an additional general error info record to the end
	// of the list.
	write_level_guarantee_level(ctx, out, json_stack_outermost_level + 1);

	// So now we must be at the outer array level: add an extra info record when there's any buffered error log pending.
	if (logbuffer.fill || ex)
	{
		write_sep(ctx, out);
		write_level_start(ctx, out, '{');
		write_item(ctx, out, "Type", "TerminalFailureInfo");
		write_item(ctx, out, "FailureMessage", ex);
		dump_observed_errors(ctx, out);
		write_level_end(ctx, out, '}');
	}

	write_level_end_guaranteed(ctx, out, ']', json_stack_outermost_level);

	// Done!
	return ret;
}

int pdfmetadump_main(int argc, const char** argv)
{
	const char* password = NULL; /* don't throw errors if encrypted */
	const char* output = NULL;
	int c;
	int ret;
	fz_output* out = NULL;

	PRINT_OBJ_TO_JSON_FLAGS = (PDF_PRINT_RESOLVE_ALL_INDIRECT | PDF_PRINT_JSON_BINARY_DATA_AS_HEX_PLUS_RAW | 1 /* ASCII flag */);
	ignore_password_troubles = 0;
	skip_fonts_dump = 0;
	skip_images_dump = 0;
	skip_forms_dump = 0;
	skip_psobjects_dump = 0;
	skip_annotations_dump = 0;
	skip_uri_links_dump = 0;
	skip_separations_dump = 0;

	ctx = NULL;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "i:o:p:m:s:h")) != -1)
	{
		switch (c)
		{
		case 'i':
			if (strchr(fz_optarg, 'p')) ++ignore_password_troubles;
			break;
		case 'o': output = fz_optarg; break;
		case 'p': password = fz_optarg; break;
		case 'm':
			switch (atoi(fz_optarg))
			{
			case 0:
				PRINT_OBJ_TO_JSON_FLAGS = (PDF_PRINT_RESOLVE_ALL_INDIRECT | 1 /* ASCII flag */);
				break;

			case 1:
				PRINT_OBJ_TO_JSON_FLAGS = (PDF_PRINT_RESOLVE_ALL_INDIRECT | PDF_PRINT_JSON_ILLEGAL_UNICODE_AS_HEX | 1 /* ASCII flag */);
				break;

			default:
				PRINT_OBJ_TO_JSON_FLAGS = (PDF_PRINT_RESOLVE_ALL_INDIRECT | PDF_PRINT_JSON_BINARY_DATA_AS_HEX_PLUS_RAW | 1 /* ASCII flag */);
				break;
			}
			break;
		case 's':
			{
			const char* spec = fz_optarg;
			while (spec && *spec)
			{
				switch (*spec)
				{
				case 'f':
					++skip_fonts_dump;
					break;

				case 'i':
					++skip_images_dump;
					break;

				case 'q':
					++skip_forms_dump;
					break;

				case 'x':
					++skip_psobjects_dump;
					break;

				case 'a':
					++skip_annotations_dump;
					break;

				case 'u':
					++skip_uri_links_dump;
					break;

				case 's':
					++skip_separations_dump;
					break;

				default:
					fz_error(ctx, "Unsupported -s argument: %c in -s %s\n\n", *spec, fz_optarg);
					return usage();
				}
			}
			}
			break;
		default:
			return usage();
			break;
		}
	}

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process.\n\n");
		return usage();
	}
	if (fz_optind + 1 < argc)
	{
		fz_error(ctx, "Too many files specified to process: you can only specify one file at a time.\n\n");
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

	// registeer a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	// keep the original error + warn callbacks and replace them with ones, which collect the log data
	// in a buffer, to be appended at the end of the metadata file.
	{
		void* userdata;
		fz_get_error_callback(ctx, &orig_error_print, &userdata);
		fz_set_error_callback(ctx, error_print_callback, userdata);

		fz_get_warning_callback(ctx, &orig_warn_print, &userdata);
		fz_set_warning_callback(ctx, warn_print_callback, userdata);
	}

	ret = EXIT_SUCCESS;
	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
		{
			out = fz_stdout(ctx);
			// help output write perf by setting stdout to block buffered mode
			setvbuf(stdout, NULL, _IOFBF, 4096);
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

		// reset the JSON object/array delimiters tracking stack:
		json_stack[0] = 0;

		ret = pdfinfo_info(ctx, out, argv[fz_optind], password);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "%s", fz_caught_message(ctx));
		ret = EXIT_FAILURE;
	}

	// restore the original error + warn callbacks.
	{
		void* userdata;
		fz_error_print_callback* dummy;
		fz_get_error_callback(ctx, &dummy, &userdata);
		fz_set_error_callback(ctx, orig_error_print, userdata);

		fz_get_warning_callback(ctx, &dummy, &userdata);
		fz_set_warning_callback(ctx, orig_warn_print, userdata);
	}

	fz_close_output(ctx, out);
	fz_drop_output(ctx, out);
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return ret;
}

#else

int pdfmetadump_main(int argc, const char** argv)
{
	fz_error(NULL, "PDFmetadump utility is not supported in this build (PDF support has been disabled).");
	return EXIT_FAILURE;
}

#endif
