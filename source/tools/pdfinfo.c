/*
 * Information tool.
 * Print information about the input pdf.
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include "mupdf/helpers/pkcs7-openssl.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

enum
{
	DIMENSIONS = 0x01,
	FONTS = 0x02,
	IMAGES = 0x04,
	SHADINGS = 0x08,
	PATTERNS = 0x10,
	XOBJS = 0x20,
	LAYERS = 0x40,
	ANNOTATIONS = 0x80,
	LINKS = 0x100,
	ALL = DIMENSIONS | FONTS | IMAGES | SHADINGS | PATTERNS | XOBJS | LAYERS | ANNOTATIONS | LINKS
};

struct info
{
	int page;
	pdf_obj *pageref;
	union {
		struct {
			pdf_obj *obj;
		} info;
		struct {
			pdf_obj *obj;
		} crypt;
		struct {
			pdf_obj *obj;
			fz_rect *bbox;
		} dim;
		struct {
			pdf_obj *obj;
			pdf_obj *subtype;
			pdf_obj *name;
			pdf_obj *encoding;
		} font;
		struct {
			pdf_obj *obj;
			pdf_obj *width;
			pdf_obj *height;
			pdf_obj *bpc;
			pdf_obj *filter;
			pdf_obj *cs;
			pdf_obj *altcs;
		} image;
		struct {
			pdf_obj *obj;
			pdf_obj *type;
		} shading;
		struct {
			pdf_obj *obj;
			pdf_obj *type;
			pdf_obj *paint;
			pdf_obj *tiling;
			pdf_obj *shading;
		} pattern;
		struct {
			pdf_obj *obj;
			pdf_obj *groupsubtype;
			pdf_obj *reference;
		} form;
	} u;
};

typedef struct
{
	pdf_document *doc;
	fz_context *ctx;
	fz_output *out;
	int pagecount;
	struct info *dim;
	int dims;
	struct info *font;
	int fonts;
	struct info *image;
	int images;
	struct info *shading;
	int shadings;
	struct info *pattern;
	int patterns;
	struct info *form;
	int forms;
	struct info *psobj;
	int psobjs;
} globals;

static void clearinfo(fz_context *ctx, globals *glo)
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

	if (glo->shading)
	{
		fz_free(ctx, glo->shading);
		glo->shading = NULL;
		glo->shadings = 0;
	}

	if (glo->pattern)
	{
		fz_free(ctx, glo->pattern);
		glo->pattern = NULL;
		glo->patterns = 0;
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

static void closexref(fz_context *ctx, globals *glo)
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
		"usage: mutool info [options] file.pdf [pages]\n"
		"\t-o -\toutput file path. Default: info will be written to stdout\n"
		"\t-p -\tpassword for decryption\n"
		"\t-A\tlist annotations\n"
		"\t-F\tlist fonts\n"
		"\t-I\tlist images\n"
		"\t-M\tlist dimensions\n"
		"\t-P\tlist patterns\n"
		"\t-U\tlist URLs (links in pages)\n"
		"\t-S\tlist shadings\n"
		"\t-X\tlist form and postscript xobjects\n"
		"\tpages\tcomma separated list of page numbers and ranges\n"
	);
	
	return EXIT_FAILURE;
}

static int is_xml_metadata(fz_context* ctx, pdf_obj* obj)
{
	if (pdf_name_eq(ctx, pdf_dict_get(ctx, obj, PDF_NAME(Type)), PDF_NAME(Metadata)))
		if (pdf_name_eq(ctx, pdf_dict_get(ctx, obj, PDF_NAME(Subtype)), PDF_NAME(XML)))
			return 1;
	return 0;
}

static const char* bool2str(int state)
{
	return state ? "YES" : "NO";
}

static void write_item(fz_context* ctx, fz_output* out, const char* label, const char* value)
{
	char buf[1024];
	size_t len = 0;
	if (!value)
		value = "<null>";
	pdf_obj* obj = pdf_new_string(ctx, value, strlen(value));

	const char* v = pdf_sprint_obj(ctx, buf, sizeof(buf), &len, obj, 0 /* tight */, 1 /* ascii */);
	fz_write_printf(ctx, out, "+ %s:\n"
		"%s\n",
		label,
		v);
	fz_free(ctx, obj);
}

static void
showglobalinfo(fz_context* ctx, globals* glo, int show)
{
	pdf_obj* obj;
	fz_output* out = glo->out;
	pdf_document* doc = glo->doc;
	int version = pdf_version(ctx, doc);

	fz_write_printf(ctx, out, "\nPDF-%d.%d\n", version / 10, version % 10);

	obj = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Info));
	if (obj)
	{
		fz_write_printf(ctx, out, "Info object (%d 0 R):\n", pdf_to_num(ctx, obj));
		pdf_print_obj(ctx, out, pdf_resolve_indirect(ctx, obj), 0, 0);
	}

	obj = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Encrypt));
	if (obj)
	{
		fz_write_printf(ctx, out, "\nEncryption object (%d 0 R):\n", pdf_to_num(ctx, obj));
		pdf_print_obj(ctx, out, pdf_resolve_indirect(ctx, obj), 1, 1);
	}

	obj = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/Metadata");
	if (obj)
	{
		int num = pdf_to_num(ctx, obj);
		fz_write_printf(ctx, out, "\nMetadata object (%d 0 R):\n", num);
		pdf_print_obj(ctx, out, pdf_resolve_indirect(ctx, obj), 0, 0);

		if (pdf_is_stream(ctx, obj))
		{
			if (is_xml_metadata(ctx, obj))
			{
				char* data = NULL;
				fz_xml_doc* container_xml = NULL;

				fz_try(ctx)
				{
					data = pdf_new_utf8_from_pdf_stream_obj(ctx, obj);
					fz_write_printf(ctx, out, "\n%s\n", data);
				}
				fz_always(ctx)
				{
					fz_free(ctx, data);
				}
				fz_catch(ctx)
				{
					// ignore error
				}
			}
		}
	}

	fz_write_printf(ctx, out, "\nPages: %d\n\n", glo->pagecount);

	fz_document* pdf = (fz_document*)glo->doc;

	int chaptercount = fz_count_chapters(ctx, pdf);

	fz_write_printf(ctx, out, "Chapters: %d\n\n", chaptercount);

	int alt_page_count = 0;
	for (int i = 0; i < chaptercount; i++)
	{
		int count = fz_count_chapter_pages(ctx, pdf, i);

		if (chaptercount > 1)
		{
			fz_write_printf(ctx, out, "+ Chapter %d:: Pages: %d\n", i + 1, count);
		}

		alt_page_count += count;
	}

	if (chaptercount > 1)
	{
		fz_write_printf(ctx, out, "\n");
	}

	if (alt_page_count != glo->pagecount)
	{
		fz_warn(ctx, "Document page count (%d) does not match with the sum of pages (%d) in all chapters combined.\n", glo->pagecount, alt_page_count);
	}


	if (show & LINKS)
	{
		/*
		fz_outline is a tree of the outline of a document (also known
		as table of contents).

		title: Title of outline item using UTF-8 encoding. May be NULL
		if the outline item has no text string.

		uri: Destination in the document to be displayed when this
		outline item is activated. May be an internal or external
		link, or NULL if the outline item does not have a destination.

		page: The page number of an internal link, or -1 for external
		links or links with no destination.

		next: The next outline item at the same level as this outline
		item. May be NULL if no more outline items exist at this level.

		down: The outline items immediate children in the hierarchy.
		May be NULL if no children exist.
		*/
		fz_outline* outlines = NULL;

		fz_try(ctx)
		{
			outlines = pdf_load_outline(ctx, doc);

			if (outlines)
			{
				fz_write_printf(ctx, out, "\n\nDocument Outline:\n\n");
			}

			fz_outline* outline_parents[100];
			int parents_index = 0;
			fz_outline* outline = outlines;
			char indent_buf[100 * 2 + 1] = "";

			while (outline)
			{
				if (!fz_is_external_link(ctx, outline->uri))
				{
					int target_page = outline->page + 1;

					fz_write_printf(ctx, out, "%s+ Internal Level %d Link: '%s'\n", indent_buf, parents_index + 1, outline->uri);
					fz_write_printf(ctx, out, "%s  + Target Page Number: %d\n", indent_buf, target_page);
					fz_write_printf(ctx, out, "%s  + Target Coordinates: X:%f Y:%f\n", indent_buf, outline->x, outline->y);
				}
				else
				{
					fz_write_printf(ctx, out, "%s+ External Level %d Link: '%s'\n", indent_buf, parents_index + 1, outline->uri);
				}

				fz_write_printf(ctx, out, "%s  ", indent_buf);
				write_item(ctx, out, "Title", outline->title);
				fz_write_printf(ctx, out, "%s  ", indent_buf);
				write_item(ctx, out, "Is Open", bool2str(outline->is_open));

				if (outline->down)
				{
					outline_parents[parents_index++] = outline->next;
					if (parents_index >= 100)
					{
						fz_throw(ctx, FZ_ERROR_GENERIC, "PDF Outline has too many levels: 100 or more!");
					}

					fz_snprintf(indent_buf, sizeof(indent_buf), "%*s", parents_index * 2, "");

					outline = outline->down;
				}

				outline = outline->next;
				while (!outline && parents_index > 0)
				{
					outline = outline_parents[--parents_index];

					fz_snprintf(indent_buf, sizeof(indent_buf), "%*s", parents_index * 2, "");
				}
			}
		}
		fz_always(ctx)
		{
			fz_drop_outline(ctx, outlines);
		}
		fz_catch(ctx)
		{
			fz_error(ctx, "Error while processing PDF Outline.\n");
		}
	}
}


static void
gatherdimensions(fz_context *ctx, globals *glo, int page, pdf_obj *pageref)
{
	fz_rect bbox;
	pdf_obj *obj;
	int j;

	obj = pdf_dict_get(ctx, pageref, PDF_NAME(MediaBox));
	if (!pdf_is_array(ctx, obj))
		return;

	bbox = pdf_to_rect(ctx, obj);

	obj = pdf_dict_get(ctx, pageref, PDF_NAME(UserUnit));
	if (pdf_is_real(ctx, obj))
	{
		float unit = pdf_to_real(ctx, obj);
		bbox.x0 *= unit;
		bbox.y0 *= unit;
		bbox.x1 *= unit;
		bbox.y1 *= unit;
	}

	for (j = 0; j < glo->dims; j++)
		if (!memcmp(glo->dim[j].u.dim.bbox, &bbox, sizeof (fz_rect)))
			break;

	if (j < glo->dims)
		return;

	glo->dim = fz_realloc_array(ctx, glo->dim, glo->dims+1, struct info);
	glo->dims++;

	glo->dim[glo->dims - 1].page = page;
	glo->dim[glo->dims - 1].pageref = pageref;
	glo->dim[glo->dims - 1].u.dim.bbox = NULL;
	glo->dim[glo->dims - 1].u.dim.bbox = fz_malloc(ctx, sizeof(fz_rect));
	memcpy(glo->dim[glo->dims - 1].u.dim.bbox, &bbox, sizeof (fz_rect));

	return;
}

static void
gatherfonts(fz_context *ctx, globals *glo, int page, pdf_obj *pageref, pdf_obj *dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj *fontdict = NULL;
		pdf_obj *subtype = NULL;
		pdf_obj *basefont = NULL;
		pdf_obj *name = NULL;
		pdf_obj *encoding = NULL;
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

		glo->font = fz_realloc_array(ctx, glo->font, glo->fonts+1, struct info);
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
gatherimages(fz_context *ctx, globals *glo, int page, pdf_obj *pageref, pdf_obj *dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj *imagedict;
		pdf_obj *type;
		pdf_obj *width;
		pdf_obj *height;
		pdf_obj *bpc = NULL;
		pdf_obj *filter = NULL;
		pdf_obj *cs = NULL;
		pdf_obj *altcs;
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
			pdf_obj *cses = cs;

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

		glo->image = fz_realloc_array(ctx, glo->image, glo->images+1, struct info);
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
gatherforms(fz_context *ctx, globals *glo, int page, pdf_obj *pageref, pdf_obj *dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj *xobjdict;
		pdf_obj *type;
		pdf_obj *subtype;
		pdf_obj *group;
		pdf_obj *groupsubtype;
		pdf_obj *reference;
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

		glo->form = fz_realloc_array(ctx, glo->form, glo->forms+1, struct info);
		glo->forms++;

		glo->form[glo->forms - 1].page = page;
		glo->form[glo->forms - 1].pageref = pageref;
		glo->form[glo->forms - 1].u.form.obj = xobjdict;
		glo->form[glo->forms - 1].u.form.groupsubtype = groupsubtype;
		glo->form[glo->forms - 1].u.form.reference = reference;
	}
}

static void
gatherpsobjs(fz_context *ctx, globals *glo, int page, pdf_obj *pageref, pdf_obj *dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj *xobjdict;
		pdf_obj *type;
		pdf_obj *subtype;
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

		glo->psobj = fz_realloc_array(ctx, glo->psobj, glo->psobjs+1, struct info);
		glo->psobjs++;

		glo->psobj[glo->psobjs - 1].page = page;
		glo->psobj[glo->psobjs - 1].pageref = pageref;
		glo->psobj[glo->psobjs - 1].u.form.obj = xobjdict;
	}
}

static void
gathershadings(fz_context *ctx, globals *glo, int page, pdf_obj *pageref, pdf_obj *dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj *shade;
		pdf_obj *type;
		int k;

		shade = pdf_dict_get_val(ctx, dict, i);
		if (!pdf_is_dict(ctx, shade))
		{
			fz_warn(ctx, "not a shading dict (%d 0 R)", pdf_to_num(ctx, shade));
			continue;
		}

		type = pdf_dict_get(ctx, shade, PDF_NAME(ShadingType));
		if (!pdf_is_int(ctx, type) || pdf_to_int(ctx, type) < 1 || pdf_to_int(ctx, type) > 7)
		{
			fz_warn(ctx, "not a shading type (%d 0 R)", pdf_to_num(ctx, shade));
			type = NULL;
		}

		for (k = 0; k < glo->shadings; k++)
			if (!pdf_objcmp(ctx, glo->shading[k].u.shading.obj, shade))
				break;

		if (k < glo->shadings)
			continue;

		glo->shading = fz_realloc_array(ctx, glo->shading, glo->shadings+1, struct info);
		glo->shadings++;

		glo->shading[glo->shadings - 1].page = page;
		glo->shading[glo->shadings - 1].pageref = pageref;
		glo->shading[glo->shadings - 1].u.shading.obj = shade;
		glo->shading[glo->shadings - 1].u.shading.type = type;
	}
}

static void
gatherpatterns(fz_context *ctx, globals *glo, int page, pdf_obj *pageref, pdf_obj *dict)
{
	int i, n;

	n = pdf_dict_len(ctx, dict);
	for (i = 0; i < n; i++)
	{
		pdf_obj *patterndict;
		pdf_obj *type;
		pdf_obj *paint = NULL;
		pdf_obj *tiling = NULL;
		pdf_obj *shading = NULL;
		int k;

		patterndict = pdf_dict_get_val(ctx, dict, i);
		if (!pdf_is_dict(ctx, patterndict))
		{
			fz_warn(ctx, "not a pattern dict (%d 0 R)", pdf_to_num(ctx, patterndict));
			continue;
		}

		type = pdf_dict_get(ctx, patterndict, PDF_NAME(PatternType));
		if (!pdf_is_int(ctx, type) || pdf_to_int(ctx, type) < 1 || pdf_to_int(ctx, type) > 2)
		{
			fz_warn(ctx, "not a pattern type (%d 0 R)", pdf_to_num(ctx, patterndict));
			type = NULL;
		}

		if (pdf_to_int(ctx, type) == 1)
		{
			paint = pdf_dict_get(ctx, patterndict, PDF_NAME(PaintType));
			if (!pdf_is_int(ctx, paint) || pdf_to_int(ctx, paint) < 1 || pdf_to_int(ctx, paint) > 2)
			{
				fz_warn(ctx, "not a pattern paint type (%d 0 R)", pdf_to_num(ctx, patterndict));
				paint = NULL;
			}

			tiling = pdf_dict_get(ctx, patterndict, PDF_NAME(TilingType));
			if (!pdf_is_int(ctx, tiling) || pdf_to_int(ctx, tiling) < 1 || pdf_to_int(ctx, tiling) > 3)
			{
				fz_warn(ctx, "not a pattern tiling type (%d 0 R)", pdf_to_num(ctx, patterndict));
				tiling = NULL;
			}
		}
		else
		{
			shading = pdf_dict_get(ctx, patterndict, PDF_NAME(Shading));
		}

		for (k = 0; k < glo->patterns; k++)
			if (!pdf_objcmp(ctx, glo->pattern[k].u.pattern.obj, patterndict))
				break;

		if (k < glo->patterns)
			continue;

		glo->pattern = fz_realloc_array(ctx, glo->pattern, glo->patterns+1, struct info);
		glo->patterns++;

		glo->pattern[glo->patterns - 1].page = page;
		glo->pattern[glo->patterns - 1].pageref = pageref;
		glo->pattern[glo->patterns - 1].u.pattern.obj = patterndict;
		glo->pattern[glo->patterns - 1].u.pattern.type = type;
		glo->pattern[glo->patterns - 1].u.pattern.paint = paint;
		glo->pattern[glo->patterns - 1].u.pattern.tiling = tiling;
		glo->pattern[glo->patterns - 1].u.pattern.shading = shading;
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
	char msgbuf[2048];

	if (!pdoc)
	{
		fz_warn(ctx, "Only PDF files have layers");
		return;
	}

	int num_configs = pdf_count_layer_configs(ctx, pdoc);

	if (num_configs > 0)
	{
		fz_write_printf(ctx, out, "\nPDF Layer configs (%d):\n", num_configs);
		for (config = 0; config < num_configs; config++)
		{
			fz_snprintf(msgbuf, sizeof(msgbuf), "%3d:", config);
			pdf_layer_config_info(ctx, pdoc, config, &info);
			fz_write_printf(ctx, out, "%s Name=\"%s\" Creator=\"%s\"\n", msgbuf, info.name ? info.name : "", info.creator ? info.creator : "");
		}
	}

	n = pdf_count_layer_config_ui(ctx, pdoc);
	if (n > 0)
	{
		fz_write_printf(ctx, out, "PDF UI Layer configs (%d):\n", n);
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

			fz_write_printf(ctx, out, "%s\n", msgbuf);
		}
	}
}

static void
gatherresourceinfo(fz_context *ctx, globals *glo, int page, pdf_obj *rsrc, int show)
{
	pdf_obj *pageref;
	pdf_obj *font;
	pdf_obj *xobj;
	pdf_obj *shade;
	pdf_obj *pattern;
	pdf_obj *subrsrc;
	int i;

	pageref = pdf_lookup_page_obj(ctx, glo->doc, page-1);
	if (!pageref)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);

	/* stop on cyclic resource dependencies */
	if (pdf_mark_obj(ctx, rsrc))
		return;

	fz_try(ctx)
	{
		font = pdf_dict_get(ctx, rsrc, PDF_NAME(Font));
		if (show & FONTS && font)
		{
			int n;

			gatherfonts(ctx, glo, page, pageref, font);
			n = pdf_dict_len(ctx, font);
			for (i = 0; i < n; i++)
			{
				pdf_obj *obj = pdf_dict_get_val(ctx, font, i);

				subrsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));
				if (subrsrc && pdf_objcmp(ctx, rsrc, subrsrc))
					gatherresourceinfo(ctx, glo, page, subrsrc, show);
			}
		}

		xobj = pdf_dict_get(ctx, rsrc, PDF_NAME(XObject));
		if (show & (IMAGES|XOBJS) && xobj)
		{
			int n;

			if (show & IMAGES)
				gatherimages(ctx, glo, page, pageref, xobj);
			if (show & XOBJS)
			{
				gatherforms(ctx, glo, page, pageref, xobj);
				gatherpsobjs(ctx, glo, page, pageref, xobj);
			}
			n = pdf_dict_len(ctx, xobj);
			for (i = 0; i < n; i++)
			{
				pdf_obj *obj = pdf_dict_get_val(ctx, xobj, i);
				subrsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));
				if (subrsrc && pdf_objcmp(ctx, rsrc, subrsrc))
					gatherresourceinfo(ctx, glo, page, subrsrc, show);
			}
		}

		shade = pdf_dict_get(ctx, rsrc, PDF_NAME(Shading));
		if (show & SHADINGS && shade)
			gathershadings(ctx, glo, page, pageref, shade);

		pattern = pdf_dict_get(ctx, rsrc, PDF_NAME(Pattern));
		if (show & PATTERNS && pattern)
		{
			int n;
			gatherpatterns(ctx, glo, page, pageref, pattern);
			n = pdf_dict_len(ctx, pattern);
			for (i = 0; i < n; i++)
			{
				pdf_obj *obj = pdf_dict_get_val(ctx, pattern, i);
				subrsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));
				if (subrsrc && pdf_objcmp(ctx, rsrc, subrsrc))
					gatherresourceinfo(ctx, glo, page, subrsrc, show);
			}
		}

		gatherlayersinfo(ctx, glo);
	}
	fz_always(ctx)
		pdf_unmark_obj(ctx, rsrc);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
gatherpageinfo(fz_context *ctx, globals *glo, int page, int show)
{
	pdf_obj *pageref;
	pdf_obj *rsrc;

	pageref = pdf_lookup_page_obj(ctx, glo->doc, page-1);

	if (!pageref)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);

	gatherdimensions(ctx, glo, page, pageref);

	rsrc = pdf_dict_get(ctx, pageref, PDF_NAME(Resources));
	gatherresourceinfo(ctx, glo, page, rsrc, show);
}

static void
printinfo(fz_context* ctx, globals* glo, int show, int page)
{
	int i;
	int j;
	fz_output* out = glo->out;

#define PAGE_FMT_zu "\t%d\t(%d 0 R):\t"

	if (show & DIMENSIONS && glo->dims > 0)
	{
		fz_write_printf(ctx, out, "Mediaboxes (%d):\n", glo->dims);
		for (i = 0; i < glo->dims; i++)
		{
			fz_write_printf(ctx, out, PAGE_FMT_zu "[ %g %g %g %g ]\n",
				glo->dim[i].page,
				pdf_to_num(ctx, glo->dim[i].pageref),
				glo->dim[i].u.dim.bbox->x0,
				glo->dim[i].u.dim.bbox->y0,
				glo->dim[i].u.dim.bbox->x1,
				glo->dim[i].u.dim.bbox->y1);
		}
		fz_write_printf(ctx, out, "\n");
	}

	if (show & FONTS && glo->fonts > 0)
	{
		fz_write_printf(ctx, out, "Fonts (%d):\n", glo->fonts);
		for (i = 0; i < glo->fonts; i++)
		{
			fz_write_printf(ctx, out, PAGE_FMT_zu "%s '%s' %s%s(%d 0 R)\n",
				glo->font[i].page,
				pdf_to_num(ctx, glo->font[i].pageref),
				pdf_to_name(ctx, glo->font[i].u.font.subtype),
				pdf_to_name(ctx, glo->font[i].u.font.name),
				glo->font[i].u.font.encoding ? pdf_to_name(ctx, glo->font[i].u.font.encoding) : "",
				glo->font[i].u.font.encoding ? " " : "",
				pdf_to_num(ctx, glo->font[i].u.font.obj));
		}
		fz_write_printf(ctx, out, "\n");
	}

	if (show & IMAGES && glo->images > 0)
	{
		fz_write_printf(ctx, out, "Images (%d):\n", glo->images);
		for (i = 0; i < glo->images; i++)
		{
			char* cs = NULL;
			char* altcs = NULL;

			fz_write_printf(ctx, out, PAGE_FMT_zu "[ ",
				glo->image[i].page,
				pdf_to_num(ctx, glo->image[i].pageref));

			if (pdf_is_array(ctx, glo->image[i].u.image.filter))
			{
				int n = pdf_array_len(ctx, glo->image[i].u.image.filter);
				for (j = 0; j < n; j++)
				{
					pdf_obj* obj = pdf_array_get(ctx, glo->image[i].u.image.filter, j);
					char* filter = fz_strdup(ctx, pdf_to_name(ctx, obj));

					if (strstr(filter, "Decode"))
						*(strstr(filter, "Decode")) = '\0';

					fz_write_printf(ctx, out, "%s%s",
						filter,
						j == pdf_array_len(ctx, glo->image[i].u.image.filter) - 1 ? "" : " ");
					fz_free(ctx, filter);
				}
			}
			else if (glo->image[i].u.image.filter)
			{
				pdf_obj* obj = glo->image[i].u.image.filter;
				char* filter = fz_strdup(ctx, pdf_to_name(ctx, obj));

				if (strstr(filter, "Decode"))
					*(strstr(filter, "Decode")) = '\0';

				fz_write_printf(ctx, out, "%s", filter);
				fz_free(ctx, filter);
			}
			else
				fz_write_printf(ctx, out, "Raw");

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
					fz_strlcpy(cs, "ICC", 4);
				if (strstr(cs, "Indexed"))
					fz_strlcpy(cs, "Idx", 4);
				if (strstr(cs, "Pattern"))
					fz_strlcpy(cs, "Pat", 4);
				if (strstr(cs, "Separation"))
					fz_strlcpy(cs, "Sep", 4);
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
					fz_strlcpy(altcs, "ICC", 4);
				if (strstr(altcs, "Indexed"))
					fz_strlcpy(altcs, "Idx", 4);
				if (strstr(altcs, "Pattern"))
					fz_strlcpy(altcs, "Pat", 4);
				if (strstr(altcs, "Separation"))
					fz_strlcpy(altcs, "Sep", 4);
			}

			fz_write_printf(ctx, out, " ] %dx%d %dbpc %s%s%s (%d 0 R)\n",
				pdf_to_int(ctx, glo->image[i].u.image.width),
				pdf_to_int(ctx, glo->image[i].u.image.height),
				glo->image[i].u.image.bpc ? pdf_to_int(ctx, glo->image[i].u.image.bpc) : 1,
				glo->image[i].u.image.cs ? cs : "ImageMask",
				glo->image[i].u.image.altcs ? " " : "",
				glo->image[i].u.image.altcs ? altcs : "",
				pdf_to_num(ctx, glo->image[i].u.image.obj));

			fz_free(ctx, cs);
			fz_free(ctx, altcs);
		}
		fz_write_printf(ctx, out, "\n");
	}

	if (show & SHADINGS && glo->shadings > 0)
	{
		fz_write_printf(ctx, out, "Shading patterns (%d):\n", glo->shadings);
		for (i = 0; i < glo->shadings; i++)
		{
			char* shadingtype[] =
			{
				"",
				"Function",
				"Axial",
				"Radial",
				"Triangle mesh",
				"Lattice",
				"Coons patch",
				"Tensor patch",
			};

			fz_write_printf(ctx, out, PAGE_FMT_zu "%s (%d 0 R)\n",
				glo->shading[i].page,
				pdf_to_num(ctx, glo->shading[i].pageref),
				shadingtype[pdf_to_int(ctx, glo->shading[i].u.shading.type)],
				pdf_to_num(ctx, glo->shading[i].u.shading.obj));
		}
		fz_write_printf(ctx, out, "\n");
	}

	if (show & PATTERNS && glo->patterns > 0)
	{
		fz_write_printf(ctx, out, "Patterns (%d):\n", glo->patterns);
		for (i = 0; i < glo->patterns; i++)
		{
			if (pdf_to_int(ctx, glo->pattern[i].u.pattern.type) == 1)
			{
				char* painttype[] =
				{
					"",
					"Colored",
					"Uncolored",
				};
				char* tilingtype[] =
				{
					"",
					"Constant",
					"No distortion",
					"Constant/fast tiling",
				};

				fz_write_printf(ctx, out, PAGE_FMT_zu "Tiling %s %s (%d 0 R)\n",
					glo->pattern[i].page,
					pdf_to_num(ctx, glo->pattern[i].pageref),
					painttype[pdf_to_int(ctx, glo->pattern[i].u.pattern.paint)],
					tilingtype[pdf_to_int(ctx, glo->pattern[i].u.pattern.tiling)],
					pdf_to_num(ctx, glo->pattern[i].u.pattern.obj));
			}
			else
			{
				fz_write_printf(ctx, out, PAGE_FMT_zu "Shading %d 0 R (%d 0 R)\n",
					glo->pattern[i].page,
					pdf_to_num(ctx, glo->pattern[i].pageref),
					pdf_to_num(ctx, glo->pattern[i].u.pattern.shading),
					pdf_to_num(ctx, glo->pattern[i].u.pattern.obj));
			}
		}
		fz_write_printf(ctx, out, "\n");
	}

	if (show & XOBJS && glo->forms > 0)
	{
		fz_write_printf(ctx, out, "Form xobjects (%d):\n", glo->forms);
		for (i = 0; i < glo->forms; i++)
		{
			fz_write_printf(ctx, out, PAGE_FMT_zu "Form%s%s%s%s (%d 0 R)\n",
				glo->form[i].page,
				pdf_to_num(ctx, glo->form[i].pageref),
				glo->form[i].u.form.groupsubtype ? " " : "",
				glo->form[i].u.form.groupsubtype ? pdf_to_name(ctx, glo->form[i].u.form.groupsubtype) : "",
				glo->form[i].u.form.groupsubtype ? " Group" : "",
				glo->form[i].u.form.reference ? " Reference" : "",
				pdf_to_num(ctx, glo->form[i].u.form.obj));
		}
		fz_write_printf(ctx, out, "\n");
	}

	if (show & XOBJS && glo->psobjs > 0)
	{
		fz_write_printf(ctx, out, "Postscript xobjects (%d):\n", glo->psobjs);
		for (i = 0; i < glo->psobjs; i++)
		{
			fz_write_printf(ctx, out, PAGE_FMT_zu "(%d 0 R)\n",
				glo->psobj[i].page,
				pdf_to_num(ctx, glo->psobj[i].pageref),
				pdf_to_num(ctx, glo->psobj[i].u.form.obj));
		}
		fz_write_printf(ctx, out, "\n");
	}
}

static void
printadvancedinfo(fz_context* ctx, globals* glo, int show, int page)
{
	fz_output* out = glo->out;
	pdf_document* doc = glo->doc;
	fz_document* pdf = (fz_document*)glo->doc;
	pdf_page* page_obj = NULL;
	fz_page* page_ref = (fz_page*)page_obj;

	fz_try(ctx)
	{
		page_obj = pdf_load_page(ctx, doc, page - 1);

		fz_rect mediabox = fz_bound_page(ctx, page_ref);

		fz_matrix ctm = fz_pre_scale(fz_rotate(0), 1.0, 1.0);

		if (show & ANNOTATIONS)
		{
			pdf_annot* annot;

			int n = 0;
			for (annot = pdf_first_annot(ctx, page_obj); annot; annot = pdf_next_annot(ctx, annot))
				++n;

			if (n)
			{
				fz_write_printf(ctx, out, "\n\nAnnotations in Page %d:\n\n", page);
				fz_write_printf(ctx, out, "+ Page Bounds: %R\n", mediabox);
				fz_write_printf(ctx, out, "+ Number of Annotations: %d\n", n);
			}

			int idx;
			for (idx = 0, annot = pdf_first_annot(ctx, page_obj); annot; ++idx, annot = pdf_next_annot(ctx, annot))
			{
				char buf[256];
				int num = pdf_to_num(ctx, pdf_annot_obj(ctx, annot));
				enum pdf_annot_type subtype = pdf_annot_type(ctx, annot);
				fz_write_printf(ctx, out, "Annot %d: %s", num, pdf_string_from_annot_type(ctx, subtype));

				fz_rect bounds_in_doc = pdf_annot_rect(ctx, annot);

				fz_write_printf(ctx, out, "+ Bounds In Document: %R\n", bounds_in_doc);

				fz_rect bounds = pdf_bound_annot(ctx, annot);
				bounds = fz_transform_rect(bounds, ctm);
				//fz_irect area = fz_irect_from_rect(bounds);

				fz_write_printf(ctx, out, "+ Bounds: %R\n", bounds);

				fz_write_printf(ctx, out, "+ Needs new AP: %s\n", bool2str(pdf_annot_needs_new_ap(ctx, annot)));

				if (pdf_annot_has_author(ctx, annot))
				{
					const char* author = pdf_annot_author(ctx, annot);
					if (strlen(author) > 0)
					{
						write_item(ctx, out, "Author", author);
					}
					else
					{
						fz_write_printf(ctx, out, "+ Author: <empty>\n");
					}
				}
				else
				{
					fz_write_printf(ctx, out, "+ Author: <none specified>\n");
				}

				{
					time_t secs = pdf_annot_creation_date(ctx, annot);
					if (secs >= 0)
					{
#ifdef _POSIX_SOURCE
						struct tm tmbuf, * tm = gmtime_r(&secs, &tmbuf);
#else
						struct tm* tm = gmtime(&secs);
#endif
						if (tm)
						{
							strftime(buf, sizeof buf, "%Y-%m-%d %H:%M UTC", tm);
							write_item(ctx, out, "Creation Date", buf);
						}
					}
				}

				{
					time_t secs = pdf_annot_modification_date(ctx, annot);
					if (secs >= 0)
					{
#ifdef _POSIX_SOURCE
						struct tm tmbuf, * tm = gmtime_r(&secs, &tmbuf);
#else
						struct tm* tm = gmtime(&secs);
#endif
						if (tm)
						{
							strftime(buf, sizeof buf, "%Y-%m-%d %H:%M UTC", tm);
							write_item(ctx, out, "Modification Date", buf);
						}
					}
				}

				int flags = pdf_annot_flags(ctx, annot);

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
					fz_write_printf(ctx, out, "+ Popup Bounds: %R\n", popup_bounds);
				}

				write_item(ctx, out, "Has Ink List", bool2str(pdf_annot_has_ink_list(ctx, annot)));
				write_item(ctx, out, "Has Quad Points", bool2str(pdf_annot_has_quad_points(ctx, annot)));
				write_item(ctx, out, "Has Vertex Data", bool2str(pdf_annot_has_vertices(ctx, annot)));
				write_item(ctx, out, "Has Line Data", bool2str(pdf_annot_has_line(ctx, annot)));
				write_item(ctx, out, "Has Interior Color", bool2str(pdf_annot_has_interior_color(ctx, annot)));
				write_item(ctx, out, "Has Line Ending Styles", bool2str(pdf_annot_has_line_ending_styles(ctx, annot)));
				write_item(ctx, out, "Has Icon Name", bool2str(pdf_annot_has_icon_name(ctx, annot)));
				write_item(ctx, out, "Has Open Action", bool2str(pdf_annot_has_open(ctx, annot)));
				write_item(ctx, out, "Has Author Data", bool2str(pdf_annot_has_author(ctx, annot)));
				write_item(ctx, out, "Is Active", bool2str(pdf_annot_active(ctx, annot)));
				write_item(ctx, out, "Is Hot", bool2str(pdf_annot_hot(ctx, annot)));

				/*
					Retrieve the annotations text language (either from the
					annotation, or from the document).
				*/
				{
					fz_text_language lang = pdf_annot_language(ctx, annot);
					const char* lang_str = fz_string_from_text_language(buf, lang);
					write_item(ctx, out, "Language", lang_str ? lang_str : "<not set>");
				}

				{
					const char* icon_name = pdf_annot_icon_name(ctx, annot);
					write_item(ctx, out, "Icon", icon_name ? icon_name : "<not set>");
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
						write_item(ctx, out, "Field Flags", buf);
						write_item(ctx, out, "Field Key", field_key);
						write_item(ctx, out, "Field Value", field_value);
					}
				}

				//if (pdf_annot_type(ctx, annot) == PDF_ANNOT_FILE_ATTACHMENT)
				{
					pdf_obj* fs = pdf_dict_get(ctx, pdf_annot_obj(ctx, annot), PDF_NAME(FS));
					if (fs)
					{
						write_item(ctx, out, "Is Embedded File", bool2str(pdf_is_embedded_file(ctx, fs)));
						const char* filename = pdf_embedded_file_name(ctx, fs);
						write_item(ctx, out, "Embedded File Name", filename ? filename : "<none>");
						const char* filetype = pdf_embedded_file_type(ctx, fs);
						write_item(ctx, out, "Embedded File Type", filetype ? filetype : "<none>");
					}
				}

				{
					pdf_obj* obj = pdf_dict_get(ctx, pdf_annot_obj(ctx, annot), PDF_NAME(Popup));
					if (obj)
					{
						fz_write_printf(ctx, out, "+ Popup: %d 0 R", pdf_to_num(ctx, obj));
					}
				}

				{
					const char* contents = pdf_annot_contents(ctx, annot);

					write_item(ctx, out, "Contents", contents);
				}

				switch (subtype)
				{
				case PDF_ANNOT_TEXT:
				case PDF_ANNOT_LINK:
				case PDF_ANNOT_FREE_TEXT:
				case PDF_ANNOT_LINE:
				case PDF_ANNOT_SQUARE:
				case PDF_ANNOT_CIRCLE:
				case PDF_ANNOT_POLYGON:
				case PDF_ANNOT_POLY_LINE:
				case PDF_ANNOT_HIGHLIGHT:
				case PDF_ANNOT_UNDERLINE:
				case PDF_ANNOT_SQUIGGLY:
				case PDF_ANNOT_STRIKE_OUT:
				case PDF_ANNOT_REDACT:
				case PDF_ANNOT_STAMP:
				case PDF_ANNOT_CARET:
				case PDF_ANNOT_INK:
				case PDF_ANNOT_POPUP:
				case PDF_ANNOT_FILE_ATTACHMENT:
				case PDF_ANNOT_SOUND:
				case PDF_ANNOT_MOVIE:
				case PDF_ANNOT_RICH_MEDIA:
				case PDF_ANNOT_WIDGET:
				case PDF_ANNOT_SCREEN:
				case PDF_ANNOT_PRINTER_MARK:
				case PDF_ANNOT_TRAP_NET:
				case PDF_ANNOT_WATERMARK:
				case PDF_ANNOT_3D:
				case PDF_ANNOT_PROJECTION:
				case PDF_ANNOT_UNKNOWN:
				default:
					break;
				}
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
		if (show & LINKS)
		{
			//pdf_page* page_obj = NULL;
			fz_link* links = NULL;

			fz_try(ctx)
			{
				//page_obj = pdf_load_page(ctx, doc, page);
				links = fz_load_links(ctx, page_ref);

				if (links)
				{
					fz_write_printf(ctx, out, "\n\nInternal and External Links (URLs) in Page %d:\n\n", page);
				}

				fz_rect bounds;
				float link_x = 0.0f, link_y = 0.0f;
				fz_link* link = links;

				while (link)
				{
					bounds = link->rect;
					bounds = fz_transform_rect(bounds, ctm);
					//area = fz_irect_from_rect(bounds);

					if (!fz_is_external_link(ctx, link->uri))
					{
						fz_location loc = fz_resolve_link(ctx, pdf, link->uri, &link_x, &link_y);
						int target_page = fz_page_number_from_location(ctx, pdf, loc) + 1;

						fz_write_printf(ctx, out, "+ Internal Link: '%s'\n", link->uri);
						if (loc.chapter >= 0 && loc.page >= 0)
						{
							fz_write_printf(ctx, out, "  + Target Page Number: %d (Chapter: %d, Chapter Page: %d)\n", target_page, loc.chapter + 1, loc.page + 1);
						}
						else
						{
							fz_write_printf(ctx, out, "  + Target Error: (Chapter: %d, Chapter Page: %d)\n", loc.chapter, loc.page);
						}
						fz_write_printf(ctx, out, "  + Target Coordinates: X:%f Y:%f\n", link_x, link_y);
						fz_write_printf(ctx, out, "  + Link Bounds: %R\n", bounds);
					}
					else
					{
						fz_write_printf(ctx, out, "+ External Link: '%s'\n", link->uri);
						fz_write_printf(ctx, out, "  + Link Bounds: %R\n", bounds);
					}

					link = link->next;
				}
			}
			fz_always(ctx)
			{
				fz_drop_link(ctx, links);
				//fz_drop_page(ctx, page_obj);
			}
			fz_catch(ctx)
			{
				fz_error(ctx, "Error while processing links in page %d\n", page);
			}
		}
	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, page_ref);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "Error while loading/processing page %d\n", page);
	}
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
printtail(fz_context* ctx, globals* glo)
{
	fz_output* out = glo->out;
	char buf[1024];
	pdf_document* doc = glo->doc;
	fz_document* pdf = (fz_document*)doc;

	fz_write_printf(ctx, out, "\n\nDocument General Status / Info:\n\n");

	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_TITLE, buf, sizeof buf) > 0)
		write_item(ctx, out, "Title", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_AUTHOR, buf, sizeof buf) > 0)
		write_item(ctx, out, "Author", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_FORMAT, buf, sizeof buf) > 0)
		write_item(ctx, out, "Format", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_ENCRYPTION, buf, sizeof buf) > 0)
		write_item(ctx, out, "Encryption", buf);

	int updates = pdf_count_versions(ctx, doc);

	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_CREATOR, buf, sizeof buf) > 0)
		write_item(ctx, out, "PDF Creator", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_PRODUCER, buf, sizeof buf) > 0)
		write_item(ctx, out, "PDF Producer", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_SUBJECT, buf, sizeof buf) > 0)
		write_item(ctx, out, "Subject", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_KEYWORDS, buf, sizeof buf) > 0)
		write_item(ctx, out, "Keywords", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_CREATION_DATE, buf, sizeof buf) > 0)
		write_item(ctx, out, "Creation Date", buf);
	if (fz_lookup_metadata(ctx, pdf, FZ_META_INFO_MODIFICATION_DATE, buf, sizeof buf) > 0)
		write_item(ctx, out, "Modification Date", buf);

	{
		pdf_obj* info = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Info));

		if (info)
		{
			fz_write_printf(ctx, out, "+ Meta Info Dictionary\n");
			pdf_print_obj(ctx, out, pdf_resolve_indirect_chain(ctx, info), 0 /* tight */, 1 /* ascii */);
			fz_write_printf(ctx, out, "\n");
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

	fz_write_printf(ctx, out, "+ PDF %sdocument with %d update%s.\n",
		pdf_doc_was_linearized(ctx, doc) ? "linearized " : "",
		updates, updates > 1 ? "s" : "");

	if (updates > 0)
	{
		int n = pdf_validate_change_history(ctx, doc);
		if (n == 0)
			fz_write_printf(ctx, out, "+ Change history seems valid.\n");
		else if (n == 1)
			fz_write_printf(ctx, out, "+ Invalid changes made to the document in the last update.\n");
		else if (n == 2)
			fz_write_printf(ctx, out, "+ Invalid changes made to the document in the penultimate update.\n");
		else
			fz_write_printf(ctx, out, "+ Invalid changes made to the document %d updates ago.\n", n);
	}

	{
		sigs_list list = { 0, 0, NULL };
		static pdf_obj* ft_list[2] = { PDF_NAME(FT), NULL };
		pdf_obj* ft = NULL;
		pdf_obj* form_fields = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/AcroForm/Fields");
		pdf_walk_tree(ctx, form_fields, PDF_NAME(Kids), process_sigs, NULL, &list, &ft_list[0], &ft);

		if (list.len)
		{
			int i;
			for (i = 0; i < list.len; i++)
			{
				pdf_obj* field = list.sig[i];
				fz_try(ctx)
				{
					if (pdf_signature_is_signed(ctx, doc, field))
					{
						pdf_pkcs7_verifier* verifier = pkcs7_openssl_new_verifier(ctx);
						pdf_signature_error sig_cert_error = pdf_check_certificate(ctx, verifier, doc, field);
						pdf_signature_error sig_digest_error = pdf_check_digest(ctx, verifier, doc, field);
						fz_write_printf(ctx, out, "+ Signature %d:\n"
							"  + CERT: %s,\n"
							"  + DIGEST: %s%s\n",
							i + 1,
							short_signature_error_desc(sig_cert_error),
							short_signature_error_desc(sig_digest_error),
							pdf_signature_incremental_change_since_signing(ctx, doc, field) ? ", Changed since" : "");

						pdf_drop_verifier(ctx, verifier);
					}
					else
					{
						fz_write_printf(ctx, out, "+ Signature %d:\n"
							"Unsigned\n",
							i + 1);
					}
				}
				fz_catch(ctx)
				{
					fz_write_printf(ctx, out, "+ Signature %d:\n"
						"  + Error\n",
						i + 1);
				}
			}
			fz_free(ctx, list.sig);
		}
	}

	if (updates == 0)
	{
		fz_write_printf(ctx, out, "+ No updates since document creation\n");
	}
	else
	{
		int n = pdf_validate_change_history(ctx, doc);
		if (n == 0)
			fz_write_printf(ctx, out, "+ Document changes conform to permissions\n");
		else
			fz_write_printf(ctx, out, "+ Document permissions violated %d updates ago\n", n);
	}

	fz_write_printf(ctx, out,
		"  + Repaired: %s\n"
		"  + Crypted:  %s\n",
		bool2str(pdf_was_repaired(ctx, glo->doc)),
		bool2str(pdf_needs_password(ctx, glo->doc))
	);
}

static void
showinfo(fz_context *ctx, globals *glo, int show, const char *pagelist)
{
	int page, spage, epage;
	int allpages;
	int pagecount;
	fz_output *out = glo->out;
	fz_document* pdf = (fz_document * )glo->doc;

	if (!glo->doc)
	{
		usage();
		fz_throw(ctx, FZ_ERROR_GENERIC, "No document specified: cannot show info without document");
	}

	allpages = !strcmp(pagelist, "1-N");

	pagecount = pdf_count_pages(ctx, glo->doc);

	while ((pagelist = fz_parse_page_range(ctx, pagelist, &spage, &epage, pagecount)))
	{
		if (allpages)
			fz_write_printf(ctx, out, "Retrieving info from pages %d-%d...\n", spage, epage);
		for (page = spage; page <= epage; page++)
		{
			gatherpageinfo(ctx, glo, page, show);
			if (!allpages)
			{
				fz_write_printf(ctx, out, "Page %d:\n", page);
				printinfo(ctx, glo, show, page);
				fz_write_printf(ctx, out, "\n");
				clearinfo(ctx, glo);
			}
		}
	}

	if (allpages)
		printinfo(ctx, glo, show, -1);

	printtail(ctx, glo);
}

static void
pdfinfo_info(fz_context *ctx, fz_output *out, const char *password, int show, const char *argv[], int argc)
{
	enum { NO_FILE_OPENED, NO_INFO_GATHERED, INFO_SHOWN } state;
	int argidx = 0;
	globals glo = { 0 };

	glo.out = out;
	glo.ctx = ctx;

	state = NO_FILE_OPENED;

	fz_try(ctx)
	{
		while (argidx < argc)
		{
			if (state == NO_FILE_OPENED || !fz_is_page_range(ctx, argv[argidx]))
			{
				if (state == NO_INFO_GATHERED)
				{
					showinfo(ctx, &glo, show, "1-N");
				}

				closexref(ctx, &glo);

				const char* filename = argv[argidx];
				fz_write_printf(ctx, out, "%s:\n", filename);
				glo.doc = pdf_open_document(glo.ctx, filename);
				if (pdf_needs_password(ctx, glo.doc))
				{
					if (!pdf_authenticate_password(ctx, glo.doc, password))
					{
						fz_throw(glo.ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
					}
				}
				glo.pagecount = pdf_count_pages(ctx, glo.doc);

				showglobalinfo(ctx, &glo, show);
				state = NO_INFO_GATHERED;
			}
			else
			{
				showinfo(ctx, &glo, show, argv[argidx]);
				state = INFO_SHOWN;
			}

			argidx++;
		}

		if (state == NO_INFO_GATHERED)
			showinfo(ctx, &glo, show, "1-N");
	}
	fz_always(ctx)
		closexref(ctx, &glo);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

int pdfinfo_main(int argc, const char **argv)
{
	const char *password = "";
	const char* output = NULL;
	int show = ALL;
	int c;
	int ret;
	fz_output* out = NULL;

	ctx = NULL;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "AFISPUXyMo:p:h")) != -1)
	{
		switch (c)
		{
		case 'A': if (show == ALL) show = ANNOTATIONS; else show |= ANNOTATIONS; break;
		case 'F': if (show == ALL) show = FONTS; else show |= FONTS; break;
		case 'I': if (show == ALL) show = IMAGES; else show |= IMAGES; break;
		case 'S': if (show == ALL) show = SHADINGS; else show |= SHADINGS; break;
		case 'P': if (show == ALL) show = PATTERNS; else show |= PATTERNS; break;
		case 'U': if (show == ALL) show = LINKS; else show |= LINKS; break;
		case 'X': if (show == ALL) show = XOBJS; else show |= XOBJS; break;
		case 'y': if (show == ALL) show = LAYERS; else show |= LAYERS; break;
		case 'M': if (show == ALL) show = DIMENSIONS; else show |= DIMENSIONS; break;
		case 'o': output = fz_optarg; break;
		case 'p': password = fz_optarg; break;
		default:
			return usage();
			break;
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

	ret = EXIT_SUCCESS;
	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
		{
			out = fz_stdout(ctx);
			output = NULL;
		}
		else
		{
			char fbuf[4096];
			fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		pdfinfo_info(ctx, out, password, show, &argv[fz_optind], argc - fz_optind);
	}
	fz_catch(ctx)
	{
		// when the data isn't dumped to stdout, echo the fatal error to the output file:
		if (output && out)
		{
			fz_write_printf(ctx, out, "\n\n\n==========================================================\n!Fatal Error: %s\n\n", fz_caught_message(ctx));
		}
		fz_error(ctx, "%s", fz_caught_message(ctx));
		ret = EXIT_FAILURE;
	}

	fz_flush_output(ctx, out);
	fz_close_output(ctx, out);
	fz_drop_output(ctx, out);
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return ret;
}
