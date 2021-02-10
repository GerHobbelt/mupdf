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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
		"\tpages\tcomma separated list of page numbers and ranges\n"
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
		fz_write_printf(ctx, out, "{\n");
		json_sep_state = 1;
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
	char buf[4096];
	size_t len = 0;

	if (!str) {
		fz_write_printf(ctx, out, "undefined");
		return;
	}

	const char* v = pdf_sprint_str_to_json(ctx, buf, sizeof(buf), &len, str, strlen(str), 0);
	fz_write_string(ctx, out, v);
	if (v != buf)
		fz_free(ctx, v);
}

static void write_item(fz_context* ctx, fz_output* out, const char* label, const char* value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "\"%s\": ", label);
	write_string(ctx, out, value);
}

static void write_item_bool(fz_context* ctx, fz_output* out, const char* label, int value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "\"%s\": %s",
		label,
		value ? "true" : "false");
}

static void write_item_int(fz_context* ctx, fz_output* out, const char* label, int value)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "\"%s\": %d",
		label,
		value);
}

static void write_item_bbox(fz_context* ctx, fz_output* out, const char* label, fz_rect* bbox)
{
	write_sep(ctx, out);
	fz_write_printf(ctx, out, "\"%s\": %R",
		label,
		bbox);
}

static void
showglobalinfo(fz_context* ctx, globals* glo)
{
	pdf_obj* obj;
	fz_output* out = glo->out;
	pdf_document* doc = glo->doc;
	int version = pdf_version(ctx, doc);

	write_sep(ctx, out);
	fz_write_printf(ctx, out, "GlobalInfo: {\n");
	json_sep_state = 1;

	write_sep(ctx, out);
	fz_write_printf(ctx, out, "Version: \"PDF-%d.%d\"", version / 10, version % 10);

	obj = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Info));
	if (obj)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "Info:\n");
		pdf_print_obj_to_json(ctx, out, obj, 0);
	}

	obj = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Encrypt));
	if (obj)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "Encryption:\n");
		pdf_print_obj_to_json(ctx, out, obj, 0);
	}

	obj = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/Metadata");
	if (obj)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "Metadata:\n");
		pdf_print_obj_to_json(ctx, out, obj, 0);
	}

	write_item_int(ctx, out, "Pages", glo->pagecount);

	fz_document* pdf = (fz_document*)glo->doc;

	int chaptercount = fz_count_chapters(ctx, pdf);

	write_item_int(ctx, out, "Chapters", chaptercount);

	if (chaptercount > 1)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "ChapterPages: [\n");
		json_sep_state = 1;
	}

	int alt_page_count = 0;
	for (int i = 0; i < chaptercount; i++)
	{
		int count = fz_count_chapter_pages(ctx, pdf, i);

		if (chaptercount > 1)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "%d", count);
		}

		alt_page_count += count;
	}

	if (chaptercount > 1)
	{
		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}

	if (alt_page_count != glo->pagecount)
	{
		fz_warn(ctx, "Document page count (%d) does not match with the sum of pages (%d) in all chapters combined.\n", glo->pagecount, alt_page_count);
	}

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
				write_sep(ctx, out);
				fz_write_printf(ctx, out, "DocumentOutline: [\n");
				json_sep_state = 1;
			}

			fz_outline* outline_parents[100];
			int parents_index = 0;
			fz_outline* outline = outlines;

			while (outline)
			{
				write_sep(ctx, out);
				fz_write_printf(ctx, out, "{\n");
				json_sep_state = 1;

				if (!fz_is_external_link(ctx, outline->uri))
				{
					int target_page = outline->page + 1;

					write_item(ctx, out, "InternalLink", outline->uri);
					write_item_int(ctx, out, "TargetPageNumber", target_page);
					write_sep(ctx, out);
					fz_write_printf(ctx, out, "TargetCoordinates: { X: %f Y: %f }", outline->x, outline->y);
				}
				else
				{
					write_item(ctx, out, "ExternalLink", outline->uri);
				}

				write_item(ctx, out, "Title", outline->title);
				write_item_bool(ctx, out, "IsOpen", outline->is_open);

				fz_write_printf(ctx, out, "\n}");

				if (outline->down)
				{
					outline_parents[parents_index++] = outline->next;
					if (parents_index >= 100)
					{
						fz_throw(ctx, FZ_ERROR_GENERIC, "PDF Outline has too many levels: 100 or more!");
					}

					write_sep(ctx, out);
					fz_write_printf(ctx, out, "Children: [\n");
					json_sep_state = 1;

					outline = outline->down;
				}

				outline = outline->next;
				while (!outline && parents_index > 0)
				{
					outline = outline_parents[--parents_index];

					fz_write_printf(ctx, out, "\n]");
					json_sep_state = 2;
				}
			}
		}
		fz_always(ctx)
		{
			if (outlines)
			{
				fz_write_printf(ctx, out, "\n]");
				json_sep_state = 2;
			}

			fz_drop_outline(ctx, outlines);
		}
		fz_catch(ctx)
		{
			fz_error(ctx, "Error while processing PDF Outline: %s\n, fz_caught_message(ctx)");
		}
	}

	fz_write_printf(ctx, out, "\n}");
	json_sep_state = 2;
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
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "PDFLayerConfigs: [\n");
		json_sep_state = 1;
		for (config = 0; config < num_configs; config++)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{");
			json_sep_state = 1;
			pdf_layer_config_info(ctx, pdoc, config, &info);
			write_item(ctx, out, "Name", info.name ? info.name : "");
			write_item(ctx, out, "Creator", info.creator ? info.creator : "");
			fz_write_printf(ctx, out, "\n}");
		}
		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}

	n = pdf_count_layer_config_ui(ctx, pdoc);
	if (n > 0)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "PDFUILayerConfigs: [\n");
		json_sep_state = 1;

		for (j = 0; j < n; j++)
		{
			pdf_layer_config_ui ui;

			pdf_layer_config_ui_info(ctx, pdoc, j, &ui);
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{");
			json_sep_state = 1;
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "Depth: %d", ui.depth);

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
				write_sep(ctx, out);
				fz_write_printf(ctx, out, "Type: \"UNKNOWN UI.TYPE:%d\"", (int)ui.type);
				break;
			}

			if (ui.type != PDF_LAYER_UI_LABEL)
			{
				write_item_bool(ctx, out, "Locked", ui.locked);
			}
			write_item(ctx, out, "Text", ui.text ? ui.text : "");
			fz_write_printf(ctx, out, "}");
		}
		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}
}

static void
gatherresourceinfo(fz_context *ctx, globals *glo, int page, pdf_obj *rsrc)
{
	pdf_obj *pageref;
	pdf_obj *font;
	pdf_obj *xobj;
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
		if (font)
		{
			int n;

			gatherfonts(ctx, glo, page, pageref, font);
			n = pdf_dict_len(ctx, font);
			for (i = 0; i < n; i++)
			{
				pdf_obj *obj = pdf_dict_get_val(ctx, font, i);

				subrsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));
				if (subrsrc && pdf_objcmp(ctx, rsrc, subrsrc))
					gatherresourceinfo(ctx, glo, page, subrsrc);
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
				subrsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));
				if (subrsrc && pdf_objcmp(ctx, rsrc, subrsrc))
					gatherresourceinfo(ctx, glo, page, subrsrc);
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
gatherpageinfo(fz_context *ctx, globals *glo, int page)
{
	pdf_obj *pageref;
	pdf_obj *rsrc;

	pageref = pdf_lookup_page_obj(ctx, glo->doc, page-1);

	if (!pageref)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);

	gatherdimensions(ctx, glo, page, pageref);

	rsrc = pdf_dict_get(ctx, pageref, PDF_NAME(Resources));
	gatherresourceinfo(ctx, glo, page, rsrc);
}

static void
printinfo(fz_context* ctx, globals* glo, int page)
{
	int i;
	int j;
	fz_output* out = glo->out;

	if (glo->dims > 0)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "Mediaboxes: [\n");
		json_sep_state = 1;

		for (i = 0; i < glo->dims; i++)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{\n");
			json_sep_state = 1;
			write_item_int(ctx, out, "Page", glo->dim[i].page);
#if 0
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "PageRef:\n");
			pdf_print_obj_to_json(ctx, out, glo->dim[i].pageref, 0);
#endif
			write_item_bbox(ctx, out, "Bounds", glo->dim[i].u.dim.bbox);
			fz_write_printf(ctx, out, "\n}");
		}

		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}

	if (glo->fonts > 0)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "Fonts: [\n");
		json_sep_state = 1;

		for (i = 0; i < glo->fonts; i++)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{\n");
			json_sep_state = 1;
			write_item_int(ctx, out, "Page", glo->font[i].page);
#if 0
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "PageRef:\n");
			pdf_print_obj_to_json(ctx, out, glo->font[i].pageref, 0);
#endif
			write_item(ctx, out, "FontType", pdf_to_name(ctx, glo->font[i].u.font.subtype));
			write_item(ctx, out, "FontName", pdf_to_name(ctx, glo->font[i].u.font.name));
			write_item(ctx, out, "FontEncoding", glo->font[i].u.font.encoding ? pdf_to_name(ctx, glo->font[i].u.font.encoding) : "");
#if 0
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "Font:\n");
			pdf_print_obj_to_json(ctx, out, glo->font[i].u.font.obj, 0);
#endif
			fz_write_printf(ctx, out, "\n}");
		}

		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}

	if (glo->images > 0)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "Images: [\n");
		json_sep_state = 1;

		for (i = 0; i < glo->images; i++)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{\n");
			json_sep_state = 1;
			write_item_int(ctx, out, "Page", glo->image[i].page);
#if 0
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "PageRef:\n");
			pdf_print_obj_to_json(ctx, out, glo->image[i].pageref, 0);
#endif
			if (pdf_is_array(ctx, glo->image[i].u.image.filter))
			{
				write_sep(ctx, out);
				fz_write_printf(ctx, out, "ImageFilters: [\n");
				json_sep_state = 1;

				int n = pdf_array_len(ctx, glo->image[i].u.image.filter);
				for (j = 0; j < n; j++)
				{
					pdf_obj* obj = pdf_array_get(ctx, glo->image[i].u.image.filter, j);
					char* filter = fz_strdup(ctx, pdf_to_name(ctx, obj));

					if (strstr(filter, "Decode"))
						*(strstr(filter, "Decode")) = '\0';

					write_sep(ctx, out);
					fz_write_printf(ctx, out, "{\n");
					json_sep_state = 1;
					write_item(ctx, out, "Filter", filter);
					fz_free(ctx, filter);
					fz_write_printf(ctx, out, "\n}");
				}
				fz_write_printf(ctx, out, "\n]");
				json_sep_state = 2;
			}
			else if (glo->image[i].u.image.filter)
			{
				pdf_obj* obj = glo->image[i].u.image.filter;
				char* filter = fz_strdup(ctx, pdf_to_name(ctx, obj));

				if (strstr(filter, "Decode"))
					*(strstr(filter, "Decode")) = '\0';

				write_sep(ctx, out);
				fz_write_printf(ctx, out, "ImageFilter: {\n");
				json_sep_state = 1;
				write_item(ctx, out, "Filter", filter);
				fz_free(ctx, filter);
				fz_write_printf(ctx, out, "\n}");
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

			write_item_int(ctx, out, "ImageWidth", pdf_to_int(ctx, glo->image[i].u.image.width));
			write_item_int(ctx, out, "ImageHeight", pdf_to_int(ctx, glo->image[i].u.image.height));
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "ImageDimensions: %dx%d",
				pdf_to_int(ctx, glo->image[i].u.image.width),
				pdf_to_int(ctx, glo->image[i].u.image.height));
			write_item_int(ctx, out, "ImageBPC", glo->image[i].u.image.bpc ? pdf_to_int(ctx, glo->image[i].u.image.bpc) : 1);
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "ImageCS: %s%s%s",
				glo->image[i].u.image.cs ? cs : "ImageMask",
				glo->image[i].u.image.altcs ? " " : "",
				glo->image[i].u.image.altcs ? altcs : "");
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "Image:\n");
			pdf_print_obj_to_json(ctx, out, glo->image[i].u.image.obj, 0);

			fz_free(ctx, cs);
			fz_free(ctx, altcs);

			fz_write_printf(ctx, out, "\n}");
		}

		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}

	if (glo->forms > 0)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "FormXObjects: [\n");
		json_sep_state = 1;

		for (i = 0; i < glo->forms; i++)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{\n");
			json_sep_state = 1;
			write_item_int(ctx, out, "Page", glo->form[i].page);
#if 0
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "PageRef:\n");
			pdf_print_obj_to_json(ctx, out, glo->form[i].pageref, 0);
#endif
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "FormType: \"Form%s%s%s%s\"",
				glo->form[i].u.form.groupsubtype ? " " : "",
				glo->form[i].u.form.groupsubtype ? pdf_to_name(ctx, glo->form[i].u.form.groupsubtype) : "",
				glo->form[i].u.form.groupsubtype ? " Group" : "",
				glo->form[i].u.form.reference ? " Reference" : "");
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "Form:\n");
			pdf_print_obj_to_json(ctx, out, glo->form[i].u.form.obj, 0);
			fz_write_printf(ctx, out, "\n}");
		}

		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}

	if (glo->psobjs > 0)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "PostscriptXObjects: [\n");
		json_sep_state = 1;

		for (i = 0; i < glo->psobjs; i++)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{\n");
			json_sep_state = 1;
			write_item_int(ctx, out, "Page", glo->psobj[i].page);
#if 0
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "PageRef:\n");
			pdf_print_obj_to_json(ctx, out, glo->psobj[i].pageref, 0);
#endif
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "Form:\n");
			pdf_print_obj_to_json(ctx, out, glo->psobj[i].u.form.obj, 0);
			fz_write_printf(ctx, out, "\n}");
		}

		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}
}

static void
printadvancedinfo(fz_context* ctx, globals* glo, int page)
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

		write_item_bbox(ctx, out, "PageBounds", &mediabox);

		{
			pdf_annot* annot;

			int n = 0;
			for (annot = pdf_first_annot(ctx, page_obj); annot; annot = pdf_next_annot(ctx, annot))
				++n;

			if (n > 0)
			{
				write_sep(ctx, out);
				fz_write_printf(ctx, out, "Annotations: [\n");
				json_sep_state = 1;
			}

			int idx;
			for (idx = 0, annot = pdf_first_annot(ctx, page_obj); annot; ++idx, annot = pdf_next_annot(ctx, annot))
			{
				char buf[1024];

				write_sep(ctx, out);
				fz_write_printf(ctx, out, "{\n");
				json_sep_state = 1;

				int num = pdf_to_num(ctx, pdf_annot_obj(ctx, annot));
				enum pdf_annot_type subtype = pdf_annot_type(ctx, annot);
				write_item_int(ctx, out, "AnnotNumber", num);
				write_item(ctx, out, "AnnotType", pdf_string_from_annot_type(ctx, subtype));

				fz_rect bounds_in_doc = pdf_annot_rect(ctx, annot);

				write_item_bbox(ctx, out, "BoundsInDocument", &bounds_in_doc);

				fz_rect bounds = pdf_bound_annot(ctx, annot);
				bounds = fz_transform_rect(bounds, ctm);
				//fz_irect area = fz_irect_from_rect(bounds);

				write_item_bbox(ctx, out, "Bounds", &bounds);

				write_item_bool(ctx, out, "NeedsNewAP", pdf_annot_needs_new_ap(ctx, annot));

				write_item(ctx, out, "Author", pdf_annot_author(ctx, annot));

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
							write_item(ctx, out, "CreationDate", buf);
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
							write_item(ctx, out, "ModificationDate", buf);
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

				write_item(ctx, out, "Icon", pdf_annot_icon_name(ctx, annot));

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
						const char* filename = pdf_embedded_file_name(ctx, fs);
						write_item(ctx, out, "EmbeddedFileName", filename);
						const char* filetype = pdf_embedded_file_type(ctx, fs);
						write_item(ctx, out, "EmbeddedFileType", filetype);
					}
				}

				{
					pdf_obj* obj = pdf_dict_get(ctx, pdf_annot_obj(ctx, annot), PDF_NAME(Popup));
					if (obj)
					{
						write_sep(ctx, out);
						fz_write_printf(ctx, out, "Popup:\n");
						pdf_print_obj_to_json(ctx, out, obj, 0);
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

				fz_write_printf(ctx, out, "\n}");
			}

			if (n > 0)
			{
				fz_write_printf(ctx, out, "\n]");
				json_sep_state = 2;
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
		{
			//pdf_page* page_obj = NULL;
			fz_link* links = NULL;

			fz_try(ctx)
			{
				//page_obj = pdf_load_page(ctx, doc, page);
				links = fz_load_links(ctx, page_ref);

				if (links)
				{
					write_sep(ctx, out);
					fz_write_printf(ctx, out, "LinksInPage: [\n");
					json_sep_state = 1;
				}

				fz_rect bounds;
				float link_x = 0.0f, link_y = 0.0f;
				fz_link* link = links;

				while (link)
				{
					write_sep(ctx, out);
					fz_write_printf(ctx, out, "{\n");
					json_sep_state = 1;

					bounds = link->rect;
					bounds = fz_transform_rect(bounds, ctm);
					//area = fz_irect_from_rect(bounds);

					if (!fz_is_external_link(ctx, link->uri))
					{
						fz_location loc = fz_resolve_link(ctx, pdf, link->uri, &link_x, &link_y);
						int target_page = fz_page_number_from_location(ctx, pdf, loc) + 1;

						write_item(ctx, out, "LinkType", "Internal");
						write_item(ctx, out, "Url:", link->uri);
						if (loc.chapter >= 0 && loc.page >= 0)
						{
							write_item_int(ctx, out, "TargetPageNumber", target_page);
							write_item_int(ctx, out, "TargetChapter", loc.chapter + 1);
							write_item_int(ctx, out, "TargetChapterPage", loc.page + 1);
						}
						else
						{
							write_sep(ctx, out);
							fz_write_printf(ctx, out, "TargetError: \"(Chapter: %d, Chapter Page: %d)\"", loc.chapter, loc.page);
						}
						write_sep(ctx, out);
						fz_write_printf(ctx, out, "TargetCoordinates: { X: %f Y: %f }", link_x, link_y);
						write_item_bbox(ctx, out, "LinkBounds", &bounds);
					}
					else
					{
						write_item(ctx, out, "LinkType", "External");
						write_item(ctx, out, "Url:", link->uri);
						write_item_bbox(ctx, out, "LinkBounds", &bounds);
					}
					fz_write_printf(ctx, out, "\n}");

					link = link->next;
				}

				if (links)
				{
					fz_write_printf(ctx, out, "\n]");
					json_sep_state = 2;
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

	write_sep(ctx, out);
	fz_write_printf(ctx, out, "DocumentGeneralInfo: {\n");
	json_sep_state = 1;

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
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "MetaInfoDictionary:\n");
			pdf_print_obj_to_json(ctx, out, pdf_resolve_indirect_chain(ctx, info), 0);
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

	write_sep(ctx, out);
	fz_write_printf(ctx, out, "Status: \"PDF %sdocument with %d update%s.\"",
		pdf_doc_was_linearized(ctx, doc) ? "linearized " : "",
		updates, updates > 1 ? "s" : "");
	write_item_bool(ctx, out, "DocWasLinearized", pdf_doc_was_linearized(ctx, doc));
	write_item_int(ctx, out, "DocumentUpdateCount", updates);

	if (updates > 0)
	{
		int n = pdf_validate_change_history(ctx, doc);

		char buf[256];

		if (n == 0)
			fz_strlcpy(buf, "Change history seems valid.", sizeof(buf));
		else if (n == 1)
			fz_strlcpy(buf, "Invalid changes made to the document in the last update.", sizeof(buf));
		else if (n == 2)
			fz_strlcpy(buf, "Invalid changes made to the document in the penultimate update.", sizeof(buf));
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
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "FormFieldSignatures: [\n");
			json_sep_state = 1;

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

						write_sep(ctx, out);
						fz_write_printf(ctx, out, "{\n");
						json_sep_state = 1;

						write_item(ctx, out, "Type", "Signed");
						write_item(ctx, out, "CERT", short_signature_error_desc(sig_cert_error));
						write_sep(ctx, out);
						fz_write_printf(ctx, out, "DIGEST: \"%s%s\"",
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
					write_item(ctx, out, "Type", "Error");
					write_sep(ctx, out);
					fz_write_printf(ctx, out, "Error: \"%s\"", fz_caught_message(ctx));
				}
				fz_write_printf(ctx, out, "\n}");
			}

			fz_write_printf(ctx, out, "\n]");
			json_sep_state = 2;

			fz_free(ctx, list.sig);
		}
	}

	{
		char buf[256];

		if (updates == 0)
		{
			fz_strlcpy(buf, "No updates since document creation", sizeof(buf));
		}
		else
		{
			int n = pdf_validate_change_history(ctx, doc);
			if (n == 0)
				fz_strlcpy(buf, "Document changes conform to permissions", sizeof(buf));
			else
				fz_snprintf(buf, sizeof(buf), "Document permissions violated %d updates ago", n);
		}
		write_item(ctx, out, "UpdatesStatus", buf);
	}

	write_item_bool(ctx, out, "WasRepaired", pdf_was_repaired(ctx, glo->doc));
	write_item_bool(ctx, out, "NeedsPassword", pdf_needs_password(ctx, glo->doc));

	fz_write_printf(ctx, out, "\n}");
	json_sep_state = 2;
}

static void
showinfo(fz_context *ctx, globals *glo, const char *pagelist)
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

	write_item(ctx, out, "InfoMode", allpages ? "AllPages" : "PageRange");

	pagecount = pdf_count_pages(ctx, glo->doc);

	if (!allpages)
	{
		write_sep(ctx, out);
		fz_write_printf(ctx, out, "PageSeries: [\n");
		json_sep_state = 1;
	}

	while ((pagelist = fz_parse_page_range(ctx, pagelist, &spage, &epage, pagecount)))
	{
		fz_info(ctx, "Retrieving info from pages %d-%d...\n", spage, epage);

		if (!allpages)
		{
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "{\n");
			json_sep_state = 1;
			write_item_int(ctx, out, "FirstPage", spage);
			write_item_int(ctx, out, "LastPage", epage);
			write_sep(ctx, out);
			fz_write_printf(ctx, out, "Info: [\n");
			json_sep_state = 1;
		}

		for (page = spage; page <= epage; page++)
		{
			gatherpageinfo(ctx, glo, page);

			if (!allpages)
			{
				write_sep(ctx, out);
				fz_write_printf(ctx, out, "{\n");
				json_sep_state = 1;

				write_item_int(ctx, out, "PageNumber", page);

				printinfo(ctx, glo, page);
				fz_write_printf(ctx, out, "\n");
				clearinfo(ctx, glo);

				fz_write_printf(ctx, out, "\n}");
			}
		}

		if (!allpages)
		{
			fz_write_printf(ctx, out, "\n]");
			fz_write_printf(ctx, out, "\n}");
			json_sep_state = 2;
		}
	}

	if (!allpages)
	{
		fz_write_printf(ctx, out, "\n]");
		json_sep_state = 2;
	}


	if (allpages)
	{
		printinfo(ctx, glo, -1);
	}

	printtail(ctx, glo);
}

static void
pdfinfo_info(fz_context* ctx, fz_output* out, const char* password, const char* argv[], int argc)
{
	enum { NO_FILE_OPENED, NO_INFO_GATHERED, INFO_SHOWN } state;
	int argidx = 0;
	globals glo = { 0 };

	glo.out = out;
	glo.ctx = ctx;

	fz_write_printf(ctx, out, "[\n");
	json_sep_state = 1;

	state = NO_FILE_OPENED;

	fz_try(ctx)
	{
		while (argidx < argc)
		{
			if (state == NO_FILE_OPENED || !fz_is_page_range(ctx, argv[argidx]))
			{
				if (state == NO_INFO_GATHERED)
				{
					showinfo(ctx, &glo, "1-N");
				}

				closexref(ctx, &glo);

				if (state != NO_FILE_OPENED)
				{
					fz_write_printf(ctx, out, "\n}");
				}

				const char* filename = argv[argidx];

				glo.doc = pdf_open_document(glo.ctx, filename);
				if (pdf_needs_password(ctx, glo.doc))
				{
					if (!pdf_authenticate_password(ctx, glo.doc, password))
					{
						fz_throw(glo.ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
					}
				}
				glo.pagecount = pdf_count_pages(ctx, glo.doc);

				write_sep(ctx, out);
				fz_write_printf(ctx, out, "{\n");
				json_sep_state = 1;

				write_item(ctx, out, "DocumentFilePath", filename);

				showglobalinfo(ctx, &glo);

				state = NO_INFO_GATHERED;
			}
			else
			{
				write_sep(ctx, out);
				fz_write_printf(ctx, out, "{\n");
				json_sep_state = 1;

				showinfo(ctx, &glo, argv[argidx]);
				state = INFO_SHOWN;

				fz_write_printf(ctx, out, "\n}");
			}

			argidx++;
		}

		if (state == NO_INFO_GATHERED)
		{
			showinfo(ctx, &glo, "1-N");
		}
	}
	fz_always(ctx)
		closexref(ctx, &glo);
	fz_catch(ctx)
		fz_rethrow(ctx);

	if (state != NO_FILE_OPENED)
	{
		fz_write_printf(ctx, out, "\n}");
	}

	fz_write_printf(ctx, out, "\n]\n");
	// Done!
}

int pdfmultipurp_main(int argc, const char **argv)
{
	const char *password = "";
	const char* output = NULL;
	int c;
	int ret;
	fz_output* out = NULL;

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
			fz_normalize_path(ctx, fbuf, sizeof fbuf, fbuf);
			fz_sanitize_path(ctx, fbuf, sizeof fbuf, fbuf);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		pdfinfo_info(ctx, out, password, &argv[fz_optind], argc - fz_optind);
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
