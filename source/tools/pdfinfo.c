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
 * Print information about the input pdf.
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/jmemcust.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if FZ_ENABLE_RENDER_CORE

enum
{
	DIMENSIONS = 0x01,
	FONTS = 0x02,
	IMAGES = 0x04,
	SHADINGS = 0x08,
	PATTERNS = 0x10,
	XOBJS = 0x20,
	LAYERS = 0x40,
	ZUGFERD = 0x80,
	ALL = DIMENSIONS | FONTS | IMAGES | SHADINGS | PATTERNS | XOBJS | LAYERS | ZUGFERD
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
	pdf_drop_document(ctx, glo->doc);
	glo->doc = NULL;

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
		"\t-F\tlist fonts\n"
		"\t-I\tlist images\n"
		"\t-M\tlist dimensions\n"
		"\t-P\tlist patterns\n"
		"\t-S\tlist shadings\n"
		"\t-X\tlist form and postscript xobjects\n"
		"\t-S\tlist layers\n"
		"\t-Z\tlist ZUGFeRD info\n"
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

static void
showglobalinfo(fz_context *ctx, globals *glo)
{
	pdf_obj *obj;
	fz_output *out = glo->out;
	pdf_document *doc = glo->doc;
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
				size_t datalen = 0;

				fz_try(ctx)
				{
					data = pdf_new_utf8_from_pdf_stream_obj(ctx, obj, &datalen);
					// use %H instead of %s to cope with corrupted content containing embedded NUL bytes:
					fz_write_printf(ctx, out, "\n%.*H\n", -(PDF_PRINT_JSON_ILLEGAL_UNICODE_AS_HEX), data, datalen);
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
}

static void
gatherdimensions(fz_context *ctx, globals *glo, int page, pdf_obj *pageref)
{
	fz_rect bbox;
	pdf_obj *obj;
	float unit;
	int j;

	obj = pdf_dict_get(ctx, pageref, PDF_NAME(MediaBox));
	if (!pdf_is_array(ctx, obj))
		return;

	bbox = pdf_to_rect(ctx, obj);

	unit = pdf_dict_get_real_default(ctx, pageref, PDF_NAME(UserUnit), 1);
	bbox.x0 *= unit;
	bbox.y0 *= unit;
	bbox.x1 *= unit;
	bbox.y1 *= unit;

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
gatherresourceinfo(fz_context *ctx, pdf_mark_list *mark_list, globals *glo, int page, pdf_obj *obj, int show)
{
	pdf_obj *rsrc;
	pdf_obj *pageref;
	pdf_obj *font;
	pdf_obj *xobj;
	pdf_obj *shade;
	pdf_obj *pattern;
	int i;

	/* stop on cyclic resource dependencies */
	if (pdf_mark_list_push(ctx, mark_list, obj))
		return;

	rsrc = pdf_dict_get(ctx, obj, PDF_NAME(Resources));

	pageref = pdf_lookup_page_obj(ctx, glo->doc, page-1);
	if (!pageref)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);

	font = pdf_dict_get(ctx, rsrc, PDF_NAME(Font));
	if (show & FONTS && font && !pdf_mark_list_push(ctx, mark_list, font))
	{
		int n;

		gatherfonts(ctx, glo, page, pageref, font);
		n = pdf_dict_len(ctx, font);
		for (i = 0; i < n; i++)
		{
			gatherresourceinfo(ctx, mark_list, glo, page, pdf_dict_get_val(ctx, font, i), show);
		}
	}

	xobj = pdf_dict_get(ctx, rsrc, PDF_NAME(XObject));
	if (show & (IMAGES|XOBJS) && xobj && !pdf_mark_list_push(ctx, mark_list, xobj))
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
			gatherresourceinfo(ctx, mark_list, glo, page, pdf_dict_get_val(ctx, xobj, i), show);
		}
	}

	shade = pdf_dict_get(ctx, rsrc, PDF_NAME(Shading));
	if (show & SHADINGS && shade && !pdf_mark_list_push(ctx, mark_list, shade))
		gathershadings(ctx, glo, page, pageref, shade);

	pattern = pdf_dict_get(ctx, rsrc, PDF_NAME(Pattern));
	if (show & PATTERNS && pattern && !pdf_mark_list_push(ctx, mark_list, pattern))
	{
		int n;
		gatherpatterns(ctx, glo, page, pageref, pattern);
		n = pdf_dict_len(ctx, pattern);
		for (i = 0; i < n; i++)
		{
			gatherresourceinfo(ctx, mark_list, glo, page, pdf_dict_get_val(ctx, pattern, i), show);
		}
	}
	
	if (show & LAYERS)
		gatherlayersinfo(ctx, glo);
}

static void
gatherpageinfo(fz_context *ctx, globals *glo, int page, int show)
{
	pdf_mark_list mark_list;
	pdf_obj *pageref;

	pageref = pdf_lookup_page_obj(ctx, glo->doc, page - 1);

	if (!pageref)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot retrieve info from page %d", page);

	gatherdimensions(ctx, glo, page, pageref);

	pdf_mark_list_init(ctx, &mark_list);
	fz_try(ctx)
		gatherresourceinfo(ctx, &mark_list, glo, page, pageref, show);
	fz_always(ctx)
		pdf_mark_list_free(ctx, &mark_list);
	fz_catch(ctx)
		fz_rethrow(ctx);
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
			fz_write_printf(ctx, out, PAGE_FMT_zu "[ %R ]\n",
				glo->dim[i].page,
				pdf_to_num(ctx, glo->dim[i].pageref),
				glo->dim[i].u.dim.bbox);
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
showinfo(fz_context *ctx, globals *glo, int show, const char *pagelist)
{
	int page, spage, epage;
	int allpages;
	int pagecount;
	fz_output *out = glo->out;

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
}

static void
showzugferd(fz_context *ctx, globals *glo)
{
	float version;
	fz_output *out = glo->out;
	enum pdf_zugferd_profile profile = pdf_zugferd_profile(ctx, glo->doc, &version);
	fz_buffer *buf;

	if (profile == PDF_NOT_ZUGFERD)
	{
		fz_write_printf(ctx, out, "Not a ZUGFeRD file.\n");
		return;
	}

	fz_write_printf(ctx, out, "ZUGFeRD version %g\n", version);
	fz_write_printf(ctx, out, "%s profile\n", pdf_zugferd_profile_to_string(ctx, profile));

	fz_write_printf(ctx, out, "Embedded XML:\n");
	buf = pdf_zugferd_xml(ctx, glo->doc);
	fz_write_buffer(ctx, out, buf);
	fz_drop_buffer(ctx, buf);
	fz_write_printf(ctx, out, "\n\n");
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
						fz_throw(glo.ctx, FZ_ERROR_ARGUMENT, "cannot authenticate password: %s", filename);
					}
				}
				glo.pagecount = pdf_count_pages(ctx, glo.doc);

				showglobalinfo(ctx, &glo);
				state = NO_INFO_GATHERED;

				if (show & ZUGFERD)
					showzugferd(ctx, &glo);
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

int pdfinfo_main(int argc, const char** argv)
{
	const char* password = "";
	const char* output = NULL;
	int show = ALL;
	int c;
	int ret;
	fz_output* out = NULL;

	ctx = NULL;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "FISPXyMZo:p:h")) != -1)
	{
		switch (c)
		{
		case 'F': if (show == ALL) show = FONTS; else show |= FONTS; break;
		case 'I': if (show == ALL) show = IMAGES; else show |= IMAGES; break;
		case 'S': if (show == ALL) show = SHADINGS; else show |= SHADINGS; break;
		case 'P': if (show == ALL) show = PATTERNS; else show |= PATTERNS; break;
		case 'X': if (show == ALL) show = XOBJS; else show |= XOBJS; break;
		case 'y': if (show == ALL) show = LAYERS; else show |= LAYERS; break;
		case 'M': if (show == ALL) show = DIMENSIONS; else show |= DIMENSIONS; break;
		case 'Z': if (show == ALL) show = ZUGFERD; else show |= ZUGFERD; break;
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

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();


	ret = EXIT_SUCCESS;
	fz_try(ctx)
	{
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

			pdfinfo_info(ctx, out, password, show, &argv[fz_optind], argc - fz_optind);
		}
		fz_catch(ctx)
		{
			// when the data isn't dumped to stdout, echo the fatal error to the output file.
			//
			// This must be done *before* the output file is flushed & closed!
			if (output && out)
			{
				fz_write_printf(ctx, out, "\n\n\n==========================================================\n!Fatal Error: %s\n\n", fz_caught_message(ctx));
			}
			fz_rethrow(ctx);
		}

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
