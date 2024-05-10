// Copyright (C) 2023 Artifex Software, Inc.
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

#if FZ_ENABLE_RENDER_CORE 

typedef struct struct_stack
{
	struct struct_stack *parent;
	fz_structure s;
	int idx;
	int char_count;
} struct_stack;

typedef struct
{
	fz_device super;
	fz_point p;
	struct_stack *ss;
	char *cfi;
} fz_cfi_device;

static void make_cfi(fz_context *ctx, fz_cfi_device *dev)
{
	struct_stack *ss = dev->ss;
	char *cfi;

	if (ss == NULL)
	{
		/* Top level! */
		dev->cfi = NULL;
	}
	else
	{
		size_t len = strlen("mupdfcfi()") + 1;
		/* Count the len we need. */
		while (ss)
		{
			char text[64];
			size_t n = sprintf(text, "/%d[%s]", ss->idx, fz_structure_to_string(ss->s));
			len += n;
			ss = ss->parent;
		}

		cfi = fz_malloc(ctx, len);
		cfi[--len] = 0;
		cfi[--len] = ')';

		/* Now fill it, backwards */
		ss = dev->ss;
		while (ss)
		{
			char text[64];
			size_t n = sprintf(text, "/%d[%s]", ss->idx, fz_structure_to_string(ss->s));
			len -= n;
			memcpy(cfi + len, text, n);
			ss = ss->parent;
		}
		memcpy(cfi, "mupdfcfi(", strlen("mupdfcfi("));
		dev->cfi = cfi;
	}
}

static void
fz_cfi_fill_text(fz_context *ctx, fz_device *dev_, const fz_text *text, fz_matrix ctm, fz_colorspace *cs, const float *color, float alpha, fz_color_params cp)
{
	fz_cfi_device *dev = (fz_cfi_device *)dev_;
	fz_text_span *span;

	/* Already found one. No more work to do! */
	if (dev->cfi)
		return;

	for (span = text->head; span != NULL; span = span->next)
	{
		int i;
		fz_matrix tm = span->trm;
		fz_matrix trm;
		for (i = 0; i < span->len; i++)
		{
			fz_rect gbox;

			if (span->items[i].gid >= 0)
			{
				tm.e = span->items[i].x;
				tm.f = span->items[i].y;
				trm = fz_concat(tm, ctm);
				gbox = fz_bound_glyph(ctx, span->font, span->items[i].gid, trm);
				if (gbox.x0 <= dev->p.x && gbox.y0 <= dev->p.y && dev->p.x <= gbox.x1 && dev->p.y <= gbox.y1)
				{
					make_cfi(ctx, dev);
					return;
				}
			}
			if (dev->ss)
				dev->ss->char_count++;
		}
	}
}

static void
fz_cfi_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *ss, fz_matrix ctm, fz_colorspace *cs, const float *color, float alpha, fz_color_params cp)
{
	fz_cfi_fill_text(ctx, dev, text, ctm, cs, color, alpha, cp);
}

static void
fz_cfi_clip_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm, fz_rect scissor)
{
	fz_cfi_fill_text(ctx, dev, text, ctm, NULL, NULL, 0, fz_default_color_params);
}

static void
fz_cfi_clip_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *ss, fz_matrix ctm, fz_rect scissor)
{
	fz_cfi_fill_text(ctx, dev, text, ctm, NULL, NULL, 0, fz_default_color_params);
}

static void
fz_cfi_ignore_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm)
{
	fz_cfi_fill_text(ctx, dev, text, ctm, NULL, NULL, 0, fz_default_color_params);
}

static void
fz_cfi_begin_structure(fz_context *ctx, fz_device *dev_, fz_structure standard, const char *raw, int idx)
{
	fz_cfi_device *dev = (fz_cfi_device *)dev_;
	struct_stack *ss = fz_malloc_struct(ctx, struct_stack);

	ss->idx = idx;
	ss->s = standard;
	ss->parent = dev->ss;
	ss->char_count = 0;
	dev->ss = ss;
}

static void
pop_struct_stack(fz_context *ctx, fz_cfi_device *dev)
{
	struct_stack *ss = dev->ss;

	if (ss == NULL)
		return;
	dev->ss = ss->parent;
	fz_free(ctx, ss);
}

static void
fz_cfi_end_structure(fz_context *ctx, fz_device *dev_)
{
	fz_cfi_device *dev = (fz_cfi_device *)dev_;

	pop_struct_stack(ctx, dev);
}

static void
fz_cfi_drop_device(fz_context *ctx, fz_device *dev_)
{
	fz_cfi_device *dev = (fz_cfi_device *)dev_;

	while (dev->ss)
		pop_struct_stack(ctx, dev);
}

char *
fz_cfi_from_point(fz_context *ctx, fz_page *page, fz_matrix ctm, fz_point p)
{
	fz_cfi_device *dev = fz_new_derived_device(ctx, fz_cfi_device);
	char *cfi = NULL;

	dev->super.fill_text = fz_cfi_fill_text;
	dev->super.stroke_text = fz_cfi_stroke_text;
	dev->super.clip_text = fz_cfi_clip_text;
	dev->super.clip_stroke_text = fz_cfi_clip_stroke_text;
	dev->super.ignore_text = fz_cfi_ignore_text;
	dev->super.begin_structure = fz_cfi_begin_structure;
	dev->super.end_structure = fz_cfi_end_structure;

	dev->super.drop_device = fz_cfi_drop_device;

	dev->p = p;
	dev->cfi = NULL;

	fz_var(cfi);

	fz_try(ctx)
	{
		fz_run_page_contents(ctx, page, &dev->super, ctm);
		fz_close_device(ctx, &dev->super);
		cfi = dev->cfi;
	}
	fz_always(ctx)
		fz_drop_device(ctx, &dev->super);
	fz_catch(ctx)
		fz_rethrow(ctx);

	return cfi;
}

/**
	Return a reference to the page that contains a given CFI.
*/
fz_page *fz_page_from_cfi(fz_context *ctx, fz_document *doc, char *cfi)
{
	return NULL;
}

/**
	Return the bbox represented by a given CFI.

	If the CFI is not on the given page, an exception will be thrown.
*/
fz_rect fz_rect_from_cfi(fz_context *ctx, fz_page *page, char *cfi)
{
	return fz_empty_rect;
}

#endif  // FZ_ENABLE_RENDER_CORE 

