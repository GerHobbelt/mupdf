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

#include "mupdf/fitz.h"

#include "mupdf/assertions.h"
#include <limits.h>
#include <string.h>

#if FZ_ENABLE_RENDER_CORE 

#define DEF_WIDTH 612
#define DEF_HEIGHT 792
#define DEF_FONTSIZE 12

typedef struct
{
	fz_document base;

	fz_document *underdoc;
	fz_stext_options opts;
	float w;
	float h;
	float em;
} reflow_document;

typedef struct {
	fz_page base;

	fz_document *html_doc;
	fz_page *html_page;
} reflow_page;

static void
reflow_drop_document_imp(fz_context *ctx, fz_document *_doc)
{
	reflow_document* doc = (reflow_document*)_doc;
	fz_defer_reap_start(ctx);

	fz_drop_document(ctx, doc->underdoc);
	fz_defer_reap_end(ctx);
}

static fz_colorspace *
reflow_document_output_intent(fz_context *ctx, fz_document *_doc)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_document_output_intent(ctx, doc->underdoc);
}

static int
reflow_needs_password(fz_context *ctx, fz_document *_doc)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_needs_password(ctx, doc->underdoc);
}

static int
reflow_authenticate_password(fz_context *ctx, fz_document *_doc, const char *password)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_authenticate_password(ctx, doc->underdoc, password);
}

static int
reflow_has_permission(fz_context *ctx, fz_document *_doc, fz_permission permission)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_has_permission(ctx, doc->underdoc, permission);
}

/* FIXME: Need to translate page targets somehow. */
static fz_outline *
reflow_load_outline(fz_context *ctx, fz_document *_doc)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_load_outline(ctx, doc->underdoc);
}

/* FIXME: Need to translate page targets somehow. */
static fz_outline_iterator *
reflow_outline_iterator(fz_context *ctx, fz_document *_doc)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_new_outline_iterator(ctx, doc->underdoc);
}

static fz_link_dest
reflow_resolve_link_dest(fz_context *ctx, fz_document *_doc, const char *uri)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_resolve_link_dest(ctx, doc->underdoc, uri);
}

static int
reflow_count_pages(fz_context *ctx, fz_document *_doc, int chapter)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_count_chapter_pages(ctx, doc->underdoc, chapter);
}

static fz_rect
reflow_bound_page(fz_context *ctx, fz_page *page_, fz_box_type box)
{
	reflow_page *page = (reflow_page *)page_;

	return fz_bound_page(ctx, page->html_page);
}

static void
reflow_drop_page(fz_context *ctx, fz_page *page_)
{
	reflow_page *page = (reflow_page *)page_;

	fz_drop_page(ctx, page->html_page);
	fz_drop_document(ctx, page->html_doc);
}

static void
reflow_run_page_contents(fz_context *ctx, fz_page *page_, fz_device *dev, fz_matrix transform)
{
	reflow_page *page = (reflow_page *)page_;

	fz_run_page_contents(ctx, page->html_page, dev, transform);
}

static void
reflow_run_page_annots(fz_context *ctx, fz_page *page_, fz_device *dev, fz_matrix transform)
{
	reflow_page *page = (reflow_page *)page_;

	fz_run_page_annots(ctx, page->html_page, dev, transform);
}

static fz_page *
reflow_load_page(fz_context *ctx, fz_document *_doc, int chapter, int pagenum)
{
	reflow_document* doc = (reflow_document*)_doc;
	fz_buffer *buf = NULL;
	fz_stext_page *text = NULL;
	fz_stext_options default_opts = { FZ_STEXT_PRESERVE_IMAGES | FZ_STEXT_DEHYPHENATE };
	reflow_page *page = NULL;
	fz_stream *stm = NULL;
	fz_output *out = NULL;
	const fz_stext_options* opts = NULL;

	if (!opts)
		opts = &default_opts;

	page = fz_new_derived_page(ctx, reflow_page, _doc);
	page->base.bound_page = reflow_bound_page;
	page->base.drop_page = reflow_drop_page;
	page->base.run_page_contents = reflow_run_page_contents;
	page->base.run_page_annots = reflow_run_page_annots;

	fz_var(buf);
	fz_var(out);
	fz_var(text);
	fz_var(stm);

	fz_try(ctx)
	{
		buf = fz_new_buffer(ctx, 8192);
		out = fz_new_output_with_buffer(ctx, buf);
		fz_print_stext_header_as_xhtml(ctx, out);

		text = fz_new_stext_page_from_chapter_page_number(ctx, doc->underdoc, chapter, pagenum, opts);
		fz_print_stext_page_as_xhtml(ctx, out, text, pagenum+1, fz_identity, opts); /* pagenum is not right w.r.t chapter. */
		fz_drop_stext_page(ctx, text);
		text = NULL;

		fz_print_stext_trailer_as_xhtml(ctx, out);
		fz_close_output(ctx, out);
		fz_terminate_buffer(ctx, buf);

		stm = fz_open_buffer(ctx, buf);
		page->html_doc = fz_open_document_with_stream(ctx, "application/xhtml+xml", stm);
		fz_layout_document(ctx, page->html_doc, doc->w, 0, doc->em);
		page->html_page = fz_load_chapter_page(ctx, page->html_doc, 0, 0);
	}
	fz_always(ctx)
	{
		fz_drop_stext_page(ctx, text);
		fz_drop_output(ctx, out);
		fz_drop_stream(ctx, stm);
		fz_drop_buffer(ctx, buf);
	}
	fz_catch(ctx)
	{
		fz_drop_page(ctx, &page->base);
		fz_rethrow(ctx);
	}

	return &page->base;
}

static int reflow_lookup_metadata(fz_context *ctx, fz_document *_doc, const char *key, char *buf, size_t size)
{
	reflow_document* doc = (reflow_document*)_doc;
	return fz_lookup_metadata(ctx, doc->underdoc, key, buf, size);
}

static void *reflow_layout_page(fz_context *ctx, fz_page *page_, void *state)
{
	reflow_page *page = (reflow_page *) page_;
	reflow_document *doc = (reflow_document *) page->base.doc;
	fz_layout_document(ctx, page->html_doc, doc->w, 0, doc->em);
	return NULL;
}

static void reflow_layout(fz_context *ctx, fz_document *_doc, float w, float h, float em)
{
	reflow_document* doc = (reflow_document*)_doc;
	if (doc->w == w && doc->h == h && doc->em == em)
		return;
	doc->w = w;
	doc->h = h;
	doc->em = em;

	(void) fz_process_opened_pages(ctx, (fz_document *) doc, reflow_layout_page, NULL);
}

fz_document *
fz_open_reflowed_document(fz_context *ctx, fz_document *underdoc, const fz_stext_options *opts)
{
	reflow_document *doc = fz_new_derived_document(ctx, reflow_document);

	doc->base.drop_document = reflow_drop_document_imp;
	doc->base.get_output_intent = reflow_document_output_intent;
	doc->base.needs_password = reflow_needs_password;
	doc->base.authenticate_password = reflow_authenticate_password;
	doc->base.has_permission = reflow_has_permission;
	doc->base.load_outline = reflow_load_outline;
	doc->base.outline_iterator = reflow_outline_iterator;
	doc->base.resolve_link_dest = reflow_resolve_link_dest;
	doc->base.count_pages = reflow_count_pages;
	doc->base.load_page = reflow_load_page;
	doc->base.lookup_metadata = reflow_lookup_metadata;
	doc->base.layout = reflow_layout;

	doc->underdoc = fz_keep_document(ctx, underdoc);
	fz_copy_stext_options(ctx, &doc->opts, opts);

	doc->w = DEF_WIDTH;
	doc->h = DEF_HEIGHT;
	doc->em = DEF_FONTSIZE;

	return &doc->base;
}

#endif
