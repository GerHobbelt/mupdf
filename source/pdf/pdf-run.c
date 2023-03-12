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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "pdf-annot-imp.h"

#if FZ_ENABLE_PDF

static void
pdf_run_annot_with_usage(fz_context *ctx, pdf_document *doc, pdf_page *page, pdf_annot *annot, fz_device *dev, fz_matrix ctm, const char *usage)
{
	fz_matrix page_ctm;
	fz_rect mediabox;
	pdf_processor *proc = NULL;
	fz_default_colorspaces *default_cs = NULL;
	int flags;
	int resources_pushed = 0;
	int struct_parent_num = -1;
	pdf_obj *struct_parent;
	fz_cookie* cookie = ctx->cookie;

	fz_var(proc);
	fz_var(default_cs);
	fz_var(resources_pushed);

	if (cookie && page->super.incomplete)
		cookie->d.incomplete = 1;

	pdf_annot_push_local_xref(ctx, annot);

	pdf_obj* subtype = pdf_dict_get(ctx, annot->obj, PDF_NAME(Subtype));
	enum pdf_annot_type type = pdf_annot_type_from_string(ctx, pdf_to_name(ctx, subtype));

	/* Widgets only get displayed if they have both a T and a TF flag,
	 * apparently */
	if (type == PDF_ANNOT_WIDGET)
	{
		pdf_obj *ft = pdf_dict_get_inheritable(ctx, annot->obj, PDF_NAME(FT));
		pdf_obj *t = pdf_dict_get_inheritable(ctx, annot->obj, PDF_NAME(T));

		if (ft == NULL || t == NULL)
		{
			pdf_annot_pop_local_xref(ctx, annot);
			return;
		}
	}

	// is this particular type rejected from the render run?
	if (cookie)
	{
		int test_index = 1 + type;
		assert(test_index >= 0);
		assert(test_index < sizeof(cookie->d.run_annotations_reject_mask));
		if (cookie->d.run_annotations_reject_mask[test_index])
		{
			pdf_annot_pop_local_xref(ctx, annot);
			return;
		}
	}

	fz_try(ctx)
	{
		default_cs = pdf_load_default_colorspaces(ctx, doc, page);
		if (default_cs)
			fz_set_default_colorspaces(ctx, dev, default_cs);

		pdf_page_transform(ctx, page, &mediabox, &page_ctm);

		flags = pdf_dict_get_int(ctx, annot->obj, PDF_NAME(F));
		if (flags & PDF_ANNOT_IS_NO_ROTATE)
		{
			int rotate = pdf_to_int(ctx, pdf_dict_get_inheritable(ctx, page->obj, PDF_NAME(Rotate)));
			fz_rect rect = pdf_dict_get_rect(ctx, annot->obj, PDF_NAME(Rect));
			fz_point tp = fz_transform_point_xy(rect.x0, rect.y1, page_ctm);
			page_ctm = fz_concat(page_ctm, fz_translate(-tp.x, -tp.y));
			page_ctm = fz_concat(page_ctm, fz_rotate(-rotate));
			page_ctm = fz_concat(page_ctm, fz_translate(tp.x, tp.y));
		}

		ctm = fz_concat(page_ctm, ctm);

		struct_parent = pdf_dict_getl(ctx, page->obj, PDF_NAME(StructParent));
		if (pdf_is_number(ctx, struct_parent))
			struct_parent_num = pdf_to_int(ctx, struct_parent);

        assert(doc == page->doc); //[GHo]
		proc = pdf_new_run_processor(ctx, page->doc, dev, ctm, struct_parent_num, usage, NULL, default_cs, page->transparency);
		pdf_processor_push_resources(ctx, proc, pdf_page_resources(ctx, annot->page));
		resources_pushed = 1;
		pdf_process_annot(ctx, proc, annot);
		pdf_close_processor(ctx, proc);
	}
	fz_always(ctx)
	{
		if (resources_pushed)
			pdf_processor_pop_resources(ctx, proc);
		pdf_drop_processor(ctx, proc);
		fz_drop_default_colorspaces(ctx, default_cs);
		pdf_annot_pop_local_xref(ctx, annot);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
pdf_run_page_contents_with_usage_imp(fz_context *ctx, pdf_document *doc, pdf_page *page, fz_device *dev, fz_matrix ctm, const char *usage)
{
	fz_matrix page_ctm;
	pdf_obj *resources;
	pdf_obj *contents;
	fz_rect mediabox;
	pdf_processor *proc = NULL;
	fz_default_colorspaces *default_cs = NULL;
	fz_colorspace *colorspace = NULL;
	int struct_parent_num = -1;
	pdf_obj *struct_parent;
	fz_cookie* cookie = ctx->cookie;

	fz_var(proc);
	fz_var(colorspace);
	fz_var(default_cs);

	if (cookie && page->super.incomplete)
		cookie->d.incomplete = 1;

	fz_try(ctx)
	{
		default_cs = pdf_load_default_colorspaces(ctx, doc, page);
		if (default_cs)
			fz_set_default_colorspaces(ctx, dev, default_cs);

		pdf_page_transform(ctx, page, &mediabox, &page_ctm);
		ctm = fz_concat(page_ctm, ctm);
		mediabox = fz_transform_rect(mediabox, ctm);

		resources = pdf_page_resources(ctx, page);
		contents = pdf_page_contents(ctx, page);

		if (page->transparency)
		{
			pdf_obj *group = pdf_page_group(ctx, page);

			if (group)
			{
				pdf_obj *cs = pdf_dict_get(ctx, group, PDF_NAME(CS));
				if (cs)
				{
					fz_try(ctx)
						colorspace = pdf_load_colorspace(ctx, cs);
					fz_catch(ctx)
					{
						fz_rethrow_if(ctx, FZ_ERROR_TRYLATER);
						fz_warn(ctx, "Ignoring Page blending colorspace.");
					}
					if (!fz_is_valid_blend_colorspace(ctx, colorspace))
					{
						fz_warn(ctx, "Ignoring invalid Page blending colorspace: %s.", colorspace->name);
						fz_drop_colorspace(ctx, colorspace);
						colorspace = NULL;
					}
				}
			}
			else
				colorspace = fz_keep_colorspace(ctx, fz_default_output_intent(ctx, default_cs));

			fz_begin_group(ctx, dev, mediabox, colorspace, 1 /* isolated */, 0, FZ_BLEND_NORMAL, 1.0);
		}

		struct_parent = pdf_dict_get(ctx, page->obj, PDF_NAME(StructParents));
		if (pdf_is_number(ctx, struct_parent))
			struct_parent_num = pdf_to_int(ctx, struct_parent);

        assert(doc == page->doc); //[GHo]
		proc = pdf_new_run_processor(ctx, page->doc, dev, ctm, struct_parent_num, usage, NULL, default_cs, page->transparency);
		pdf_process_contents(ctx, proc, doc, resources, contents, NULL);
		pdf_close_processor(ctx, proc);

		if (page->transparency)
		{
			fz_end_group(ctx, dev);
		}
	}
	fz_always(ctx)
	{
		pdf_drop_processor(ctx, proc);
		fz_drop_colorspace(ctx, colorspace);
		fz_drop_default_colorspaces(ctx, default_cs);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void pdf_run_page_contents_with_usage(fz_context *ctx, pdf_page *page, fz_device *dev, fz_matrix ctm, const char *usage)
{
	pdf_document *doc = page->doc;
	int nocache;
	fz_cookie* cookie = ctx->cookie;

	if (cookie)
	{
		enum fz_run_flags mode = cookie->d.run_mode;
		if (mode != FZ_RUN_EVERYTHING && !(mode & FZ_RUN_CONTENT))
			return;
	}

	nocache = !!(dev->hints & FZ_NO_CACHE);
	if (nocache)
		pdf_mark_xref(ctx, doc);

	fz_try(ctx)
	{
		pdf_run_page_contents_with_usage_imp(ctx, doc, page, dev, ctm, usage);
	}
	fz_always(ctx)
	{
		if (nocache)
			pdf_clear_xref_to_mark(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void pdf_run_page_contents(fz_context *ctx, pdf_page *page, fz_device *dev, fz_matrix ctm)
{
	pdf_run_page_contents_with_usage(ctx, page, dev, ctm, "View");
}

void pdf_run_annot(fz_context *ctx, pdf_annot *annot, fz_device *dev, fz_matrix ctm)
{
	pdf_page *page = annot->page;
	pdf_document *doc;
	int nocache;
	fz_cookie* cookie = ctx->cookie;

	if (cookie)
	{
		enum fz_run_flags mode = cookie->d.run_mode;
		if (mode != FZ_RUN_EVERYTHING && !(mode & FZ_RUN_ANNOTATIONS))
			return;
	}

	if (!page)
		fz_throw(ctx, FZ_ERROR_GENERIC, "annotation not bound to any page");

	doc = page->doc;

	nocache = !!(dev->hints & FZ_NO_CACHE);
	if (nocache)
		pdf_mark_xref(ctx, doc);

	fz_try(ctx)
	{
		pdf_run_annot_with_usage(ctx, doc, page, annot, dev, ctm, "View");
	}
	fz_always(ctx)
	{
		if (nocache)
			pdf_clear_xref_to_mark(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

static void
pdf_run_page_annots_with_usage_imp(fz_context *ctx, pdf_document *doc, pdf_page *page, fz_device *dev, fz_matrix ctm, const char *usage)
{
	pdf_annot *annot;
	fz_cookie* cookie = ctx->cookie;

	if (cookie && cookie->d.progress_max != (size_t)-1)
	{
		int count = 0;
		for (annot = page->annots; annot; annot = annot->next)
			count++;
		cookie->d.progress_max += count;
	}

	for (annot = page->annots; annot; annot = annot->next)
	{
		/* Check the cookie for aborting */
		if (cookie)
		{
			if (cookie->d.abort)
				break;

			cookie->check_back(ctx, FZ_PROGRESS_ANNOT_RUN_PROCEEDING, 1);

			cookie->d.progress++;
		}

		pdf_run_annot_with_usage(ctx, doc, page, annot, dev, ctm, usage);
	}
}

void pdf_run_page_annots_with_usage(fz_context *ctx, pdf_page *page, fz_device *dev, fz_matrix ctm, const char *usage)
{
	pdf_document *doc = page->doc;
	int nocache;
	fz_cookie* cookie = ctx->cookie;

	if (cookie)
	{
		enum fz_run_flags mode = cookie->d.run_mode;
		if (mode != FZ_RUN_EVERYTHING && !(mode & FZ_RUN_ANNOTATIONS))
			return;
	}

	nocache = !!(dev->hints & FZ_NO_CACHE);
	if (nocache)
		pdf_mark_xref(ctx, doc);

	fz_try(ctx)
	{
		pdf_run_page_annots_with_usage_imp(ctx, doc, page, dev, ctm, usage);
	}
	fz_always(ctx)
	{
		if (nocache)
			pdf_clear_xref_to_mark(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void pdf_run_page_annots(fz_context *ctx, pdf_page *page, fz_device *dev, fz_matrix ctm)
{
	pdf_run_page_annots_with_usage(ctx, page, dev, ctm, "View");
}

void
pdf_run_page_with_usage(fz_context *ctx, pdf_page *page, fz_device *dev, fz_matrix ctm, const char *usage)
{
	pdf_document *doc = page->doc;
	int nocache = !!(dev->hints & FZ_NO_CACHE);
	fz_cookie* cookie = ctx->cookie;

	if (nocache)
		pdf_mark_xref(ctx, doc);
	fz_try(ctx)
	{
		enum fz_run_flags mode = FZ_RUN_EVERYTHING;
		if (cookie)
		{
			mode = cookie->d.run_mode;
		}
		if (mode == FZ_RUN_EVERYTHING || (mode & FZ_RUN_CONTENT))
		{
			pdf_run_page_contents_with_usage_imp(ctx, doc, page, dev, ctm, usage);
		}

		if (mode == FZ_RUN_EVERYTHING || (mode & FZ_RUN_ANNOTATIONS))
		{
			pdf_run_page_annots_with_usage_imp(ctx, doc, page, dev, ctm, usage);
		}
	}
	fz_always(ctx)
	{
		if (nocache)
			pdf_clear_xref_to_mark(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void
pdf_run_page(fz_context *ctx, pdf_page *page, fz_device *dev, fz_matrix ctm)
{
	pdf_run_page_with_usage(ctx, page, dev, ctm, "View");
}

void
pdf_run_glyph(fz_context *ctx, pdf_document *doc, pdf_obj *resources, fz_buffer *contents, fz_device *dev, fz_matrix ctm, void *gstate, fz_default_colorspaces *default_cs, int has_transparency)
{
	pdf_processor *proc;

	proc = pdf_new_run_processor(ctx, doc, dev, ctm, -1, "View", gstate, default_cs, has_transparency);
	fz_try(ctx)
	{
		pdf_process_glyph(ctx, proc, doc, resources, contents);
		pdf_close_processor(ctx, proc);
	}
	fz_always(ctx)
		pdf_drop_processor(ctx, proc);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

#endif
