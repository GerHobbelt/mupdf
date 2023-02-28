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

#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

#include "mupdf/fitz.h"
#include "mupdf/ucdn.h"
#include "mupdf/helpers/dir.h"

#include "glyphbox.h"

#include <math.h>
#include <float.h>
#include <string.h>

#if FZ_ENABLE_RENDER_CORE 

/* Simple layout structure */

fz_layout_block *fz_new_layout(fz_context *ctx)
{
	fz_pool *pool = fz_new_pool(ctx);
	fz_layout_block *block;
	fz_try(ctx)
	{
		block = fz_pool_alloc(ctx, pool, sizeof (fz_layout_block));
		block->pool = pool;
		block->head = NULL;
		block->tailp = &block->head;
	}
	fz_catch(ctx)
	{
		fz_drop_pool(ctx, pool);
		fz_rethrow(ctx);
	}
	return block;
}

void fz_drop_layout(fz_context *ctx, fz_layout_block *block)
{
	if (block)
		fz_drop_pool(ctx, block->pool);
}

void fz_add_layout_line(fz_context *ctx, fz_layout_block *block, float x, float y, float font_size, const char *p)
{
	fz_layout_line *line = fz_pool_alloc(ctx, block->pool, sizeof (fz_layout_line));
	line->x = x;
	line->y = y;
	line->font_size = font_size;
	line->p = p;
	line->text = NULL;
	line->next = NULL;
	*block->tailp = line;
	block->tailp = &line->next;
	block->text_tailp = &line->text;
}

void fz_add_layout_char(fz_context *ctx, fz_layout_block *block, float x, float advance, const char *p)
{
	fz_layout_char *ch = fz_pool_alloc(ctx, block->pool, sizeof (fz_layout_char));
	ch->x = x;
	ch->advance = advance;
	ch->p = p;
	ch->next = NULL;
	*block->text_tailp = ch;
	block->text_tailp = &ch->next;
}

/* Extract text into blocks and lines. */

#define PARAGRAPH_DIST 1.5f
#define SPACE_DIST 0.15f
#define SPACE_MAX_DIST 0.8f

typedef struct
{
	fz_device super;
	fz_stext_page *page;
	fz_point pen, start;
	fz_matrix trm;
	fz_stext_options opts;
	int new_obj;
	int curdir;
	int lastchar;
	//unsigned int flags;
	int color;
	const fz_text *lasttext;
} fz_stext_device;

const char *fz_stext_options_usage =
	"Text output options:\n"
	"  inhibit-spaces:       don't add spaces between gaps in the text\n"
	"  preserve-images:      keep images in output\n"
	"  reference-images[=filepath_template]\n"
	"                        store images in separate files and reference these\n"
	"                        from the generated text file. (Uses a default\n"
	"                        path template when used without argument or when '=yes')\n"
	"  preserve-ligatures:   do not expand ligatures into constituent characters\n"
	"  preserve-whitespace:  do not convert all whitespace into space characters\n"
	"  preserve-spans:       do not merge spans on the same line\n"
	"  reference-images=no:  (default) output images as data URIs\n"
	"                  =yes: output image reference URI only\n"
	"  reuse-images:         duplicate images share the same URI\n"
	"  dehyphenate:          attempt to join up hyphenated words\n"
	"  mediabox-clip=no:     include characters outside mediabox\n"
	"  glyph-bbox:           use painted area of glyphs instead of font size for bounding boxes\n"
	"  text-as-path:         (SVG: default) output text as curves\n"
	"  external-styles       store the CSS page styles in a separate file instead of inlining\n"
	"  resolution=<scale>    render and position everything at the specified scale\n"
	"                        (HTML base resolution is 96ppi)\n"
    "\n";

fz_stext_page *
fz_new_stext_page(fz_context *ctx, fz_rect mediabox)
{
	fz_pool *pool = fz_new_pool(ctx);
	fz_stext_page *page = NULL;
	fz_try(ctx)
	{
		page = fz_pool_alloc(ctx, pool, sizeof(*page));
		page->pool = pool;
		page->mediabox = mediabox;
		page->first_block = NULL;
		page->last_block = NULL;
	}
	fz_catch(ctx)
	{
		fz_drop_pool(ctx, pool);
		fz_rethrow(ctx);
	}
	return page;
}

void
fz_drop_stext_page(fz_context *ctx, fz_stext_page *page)
{
	if (page)
	{
		fz_stext_block *block;
		fz_stext_line *line;
		fz_stext_char *ch;
		for (block = page->first_block; block; block = block->next)
		{
			if (block->type == FZ_STEXT_BLOCK_IMAGE)
				fz_drop_image(ctx, block->u.i.image);
			else
				for (line = block->u.t.first_line; line; line = line->next)
					for (ch = line->first_char; ch; ch = ch->next)
						fz_drop_font(ctx, ch->font);
		}
		fz_drop_pool(ctx, page->pool);
	}
}

static fz_stext_block *
add_block_to_page(fz_context *ctx, fz_stext_page *page)
{
	fz_stext_block *block = fz_pool_alloc(ctx, page->pool, sizeof *page->first_block);
	block->bbox = fz_empty_rect; /* Fixes bug 703267. */
	block->prev = page->last_block;
	if (!page->first_block)
		page->first_block = page->last_block = block;
	else
	{
		page->last_block->next = block;
		page->last_block = block;
	}
	return block;
}

static fz_stext_block *
add_text_block_to_page(fz_context *ctx, fz_stext_page *page)
{
	fz_stext_block *block = add_block_to_page(ctx, page);
	block->type = FZ_STEXT_BLOCK_TEXT;
	return block;
}

static fz_stext_block *
add_image_block_to_page(fz_context *ctx, fz_stext_page *page, fz_matrix ctm, fz_image *image)
{
	fz_stext_block *block = add_block_to_page(ctx, page);
	block->type = FZ_STEXT_BLOCK_IMAGE;
	block->u.i.transform = ctm;
	block->u.i.image = fz_keep_image(ctx, image);
	block->bbox = fz_transform_rect(fz_unit_rect, ctm);
	return block;
}

static fz_stext_line *
add_line_to_block(fz_context *ctx, fz_stext_page *page, fz_stext_block *block, const fz_point *dir, int wmode)
{
	fz_stext_line *line = fz_pool_alloc(ctx, page->pool, sizeof *block->u.t.first_line);
	line->prev = block->u.t.last_line;
	if (!block->u.t.first_line)
		block->u.t.first_line = block->u.t.last_line = line;
	else
	{
		block->u.t.last_line->next = line;
		block->u.t.last_line = line;
	}

	line->dir = *dir;
	line->wmode = wmode;

	return line;
}

static fz_stext_char *
add_char_to_line(fz_context *ctx, fz_stext_device *dev, fz_stext_page *page, fz_stext_line *line, fz_matrix trm, fz_font *font, float size, int c, int gid, fz_point *p, fz_point *q, int color)
{
	fz_stext_char *ch = fz_pool_alloc(ctx, page->pool, sizeof *line->first_char);
	fz_point a, d;

	if (!line->first_char)
		line->first_char = line->last_char = ch;
	else
	{
		line->last_char->next = ch;
		line->last_char = ch;
	}

	ch->c = c;
	ch->color = color;
	ch->origin = *p;
	ch->size = size;
	ch->font = fz_keep_font(ctx, font);

	if (dev->opts.flags & FZ_STEXT_GLYPH_BBOX)
	{
		ch->quad.ll = ch->quad.ul = ch->quad.ur = ch->quad.lr = ch->origin;
		if (gid >= 0)
		{
			fz_rect bbox = fz_bound_glyph(ctx, font, gid, trm);
			if (!fz_is_empty_rect(bbox))
				ch->quad = fz_quad_from_rect(bbox);
		}
	}
	else
	{
		if (line->wmode == 0)
		{
			a.x = 0;
			d.x = 0;
			a.y = fz_font_ascender(ctx, font);
			d.y = fz_font_descender(ctx, font);
		}
		else
		{
			a.x = 1;
			d.x = 0;
			a.y = 0;
			d.y = 0;
		}
		a = fz_transform_vector(a, trm);
		d = fz_transform_vector(d, trm);

		ch->quad.ll = fz_make_point(p->x + d.x, p->y + d.y);
		ch->quad.ul = fz_make_point(p->x + a.x, p->y + a.y);
		ch->quad.lr = fz_make_point(q->x + d.x, q->y + d.y);
		ch->quad.ur = fz_make_point(q->x + a.x, q->y + a.y);
	}

	return ch;
}

static void
remove_last_char(fz_context *ctx, fz_stext_line *line)
{
	if (line && line->first_char)
	{
		fz_stext_char *prev = NULL;
		fz_stext_char *ch = line->first_char;
		while (ch->next)
		{
			prev = ch;
			ch = ch->next;
		}
		if (prev)
		{
			/* the characters are pool allocated, so we don't actually leak the removed node */
			line->last_char = prev;
			line->last_char->next = NULL;
		}
	}
}

static int
direction_from_bidi_class(int bidiclass, int curdir)
{
	switch (bidiclass)
	{
	/* strong */
	case UCDN_BIDI_CLASS_L: return 1;
	case UCDN_BIDI_CLASS_R: return -1;
	case UCDN_BIDI_CLASS_AL: return -1;

	/* weak */
	case UCDN_BIDI_CLASS_EN:
	case UCDN_BIDI_CLASS_ES:
	case UCDN_BIDI_CLASS_ET:
	case UCDN_BIDI_CLASS_AN:
	case UCDN_BIDI_CLASS_CS:
	case UCDN_BIDI_CLASS_NSM:
	case UCDN_BIDI_CLASS_BN:
		return curdir;

	/* neutral */
	case UCDN_BIDI_CLASS_B:
	case UCDN_BIDI_CLASS_S:
	case UCDN_BIDI_CLASS_WS:
	case UCDN_BIDI_CLASS_ON:
		return curdir;

	/* embedding, override, pop ... we don't support them */
	default:
		return 0;
	}
}

static int is_hyphen(int c)
{
	/* check for: hyphen-minus, soft hyphen, hyphen, and non-breaking hyphen */
	return (c == '-' || c == 0xAD || c == 0x2010 || c == 0x2011);
}

static float
vec_dot(const fz_point *a, const fz_point *b)
{
	return a->x * b->x + a->y * b->y;
}

static void
prepend_line_if_possible(fz_context *ctx, fz_stext_block *cur_block, fz_point q)
{
	fz_stext_line *cur_line;
	fz_stext_line *line;
	fz_point ndir;
	float size;
	fz_point p;
	fz_point delta;
	float spacing;

	if (cur_block == NULL || cur_block->type != FZ_STEXT_BLOCK_TEXT)
		return;

	cur_line = cur_block->u.t.last_line;
	if (cur_line == NULL)
		return;

	line = cur_line->prev;
	if (line == NULL)
		return;

	if (line->wmode != cur_line->wmode)
		return;

	ndir = cur_line->dir;
	size = cur_line->last_char->size;
	p = line->first_char->origin;
	delta.x = p.x - q.x;
	delta.y = p.y - q.y;

	spacing = ndir.x * delta.x + ndir.y * delta.y;

	if (fabsf(spacing) >= size * SPACE_MAX_DIST)
		return;

	/* cur_line plausibly finishes at the start of line. */
	/* Move all the chars from cur_line onto the start of line */
	cur_line->last_char->next = line->first_char;
	line->first_char = cur_line->first_char;
	cur_line->first_char = NULL;
	cur_line->last_char = NULL;

	/* Merge the bboxes */
	line->bbox = fz_union_rect(line->bbox, cur_line->bbox);

	/* Unlink cur_line from the block. */
	cur_block->u.t.last_line = cur_block->u.t.last_line->prev;
	cur_block->u.t.last_line->next = NULL;

	/* Can't bin the line storage as it's from a pool. */
}

static void
fz_add_stext_char_imp(fz_context *ctx, fz_stext_device *dev, fz_font *font, int c, int glyph, fz_matrix trm, float adv, int wmode, int force_new_line)
{
	fz_stext_page *page = dev->page;
	fz_stext_block *cur_block;
	fz_stext_line *cur_line;

	int new_para = 0;
	int new_line = 1;
	int add_space = 0;
	fz_point dir, ndir, p, q;
	float size;
	fz_point delta;
	float spacing = 0;
	float base_offset = 0;

	dev->curdir = direction_from_bidi_class(ucdn_get_bidi_class(c), dev->curdir);

	/* dir = direction vector for motion. ndir = normalised(dir) */
	if (wmode == 0)
	{
		dir.x = 1;
		dir.y = 0;
	}
	else
	{
		dir.x = 0;
		dir.y = -1;
	}
	dir = fz_transform_vector(dir, trm);
	ndir = fz_normalize_vector(dir);

	size = fz_matrix_expansion(trm);

	/* We need to identify where glyphs 'start' (p) and 'stop' (q).
	 * Each glyph holds its 'start' position, and the next glyph in the
	 * span (or span->max if there is no next glyph) holds its 'end'
	 * position.
	 *
	 * For both horizontal and vertical motion, trm->{e,f} gives the
	 * origin (usually the bottom left) of the glyph.
	 *
	 * In horizontal mode:
	 *   + p is bottom left.
	 *   + q is the bottom right
	 * In vertical mode:
	 *   + p is top left (where it advanced from)
	 *   + q is bottom left
	 */
	if (wmode == 0)
	{
		p.x = trm.e;
		p.y = trm.f;
		q.x = trm.e + adv * dir.x;
		q.y = trm.f + adv * dir.y;
	}
	else
	{
		p.x = trm.e - adv * dir.x;
		p.y = trm.f - adv * dir.y;
		q.x = trm.e;
		q.y = trm.f;
	}

	/* Find current position to enter new text. */
	cur_block = page->last_block;
	if (cur_block && cur_block->type != FZ_STEXT_BLOCK_TEXT)
		cur_block = NULL;
	cur_line = cur_block ? cur_block->u.t.last_line : NULL;

	if (cur_line && glyph < 0)
	{
		/* Don't advance pen or break lines for no-glyph characters in a cluster */
		add_char_to_line(ctx, dev, page, cur_line, trm, font, size, c, -1, &dev->pen, &dev->pen, dev->color);
		dev->lastchar = c;
		return;
	}

	if (cur_line == NULL || cur_line->wmode != wmode || vec_dot(&ndir, &cur_line->dir) < 0.999f)
	{
		/* If the matrix has changed rotation, or the wmode is different (or if we don't have a line at all),
		 * then we can't append to the current block/line. */
		new_para = 1;
		new_line = 1;
	}
	else
	{
		/* Detect fake bold where text is printed twice in the same place. */
		delta.x = fabsf(q.x - dev->pen.x);
		delta.y = fabsf(q.y - dev->pen.y);
		if (delta.x < FLT_EPSILON && delta.y < FLT_EPSILON && c == dev->lastchar)
			return;

		/* Calculate how far we've moved since the last character. */
		delta.x = p.x - dev->pen.x;
		delta.y = p.y - dev->pen.y;

		/* The transform has not changed, so we know we're in the same
		 * direction. Calculate 2 distances; how far off the previous
		 * baseline we are, together with how far along the baseline
		 * we are from the expected position. */
		spacing = ndir.x * delta.x + ndir.y * delta.y;
		base_offset = -ndir.y * delta.x + ndir.x * delta.y;

		/* Only a small amount off the baseline - we'll take this */
		if (fabsf(base_offset) < size * 0.8f)
		{
			/* LTR or neutral character */
			if (dev->curdir >= 0)
			{
				if (fabsf(spacing) < size * SPACE_DIST)
				{
					/* Motion is in line and small enough to ignore. */
					new_line = 0;
				}
				else if (fabsf(spacing) > size * SPACE_MAX_DIST)
				{
					/* Motion is in line and large enough to warrant splitting to a new line */
					new_line = 1;
				}
				else if (spacing < 0)
				{
					/* Motion is backward in line! Ignore this odd spacing. */
					new_line = 0;
				}
				else
				{
					/* Motion is forward in line and large enough to warrant us adding a space. */
					if (dev->lastchar != ' ' && wmode == 0)
						add_space = 1;
					new_line = 0;
				}
			}

			/* RTL character -- disable space character and column detection heuristics */
			else
			{
				new_line = 0;
#if 0 /* TODO: handle RTL visual/logical ordering */
				if (spacing > size * SPACE_DIST || spacing < 0)
					rtl = 0; /* backward (or big jump to 'right' side) means logical order */
				else
					rtl = 1; /* visual order, we need to reverse in a post process pass */
#endif
			}
		}

		/* Enough for a new line, but not enough for a new paragraph */
		else if (fabsf(base_offset) <= size * PARAGRAPH_DIST)
		{
			/* Check indent to spot text-indent style paragraphs */
			if (wmode == 0 && cur_line && dev->new_obj)
				if (fabsf(p.x - dev->start.x) > size * 0.5f)
					new_para = 1;
			new_line = 1;
		}

		/* Way off the baseline - open a new paragraph */
		else
		{
			new_para = 1;
			new_line = 1;
		}
	}

	/* Start a new block (but only at the beginning of a text object) */
	if (new_para || !cur_block)
	{
		cur_block = add_text_block_to_page(ctx, page);
		cur_line = cur_block->u.t.last_line;
	}

	if (new_line && (dev->opts.flags & FZ_STEXT_DEHYPHENATE) && is_hyphen(dev->lastchar))
	{
		remove_last_char(ctx, cur_line);
		new_line = 0;
	}

	if (new_line)
	{
		/* We are about to start a new line. This means we've finished with this
		 * one. Can this be prepended to a previous line in this block? */
		/* dev->pen records the previous stopping point - so where cur_line ends. */
		prepend_line_if_possible(ctx, cur_block, dev->pen);
	}

	/* Start a new line */
	if (new_line || !cur_line || force_new_line)
	{
		cur_line = add_line_to_block(ctx, page, cur_block, &ndir, wmode);
		dev->start = p;
	}

	/* Add synthetic space */
	if (add_space && !(dev->opts.flags & FZ_STEXT_INHIBIT_SPACES))
		add_char_to_line(ctx, dev, page, cur_line, trm, font, size, ' ', -1, &dev->pen, &p, dev->color);

	add_char_to_line(ctx, dev, page, cur_line, trm, font, size, c, glyph, &p, &q, dev->color);
	dev->lastchar = c;
	dev->pen = q;

	dev->new_obj = 0;
	dev->trm = trm;
}

static const struct liga {
	int unicode;
	const char* expansion;
} ligatures[] = {
	// table taken from https://en.wikipedia.org/wiki/Ligature_(writing)
	//{ 0x0057, /* W */ "UU" },
	//{ 0x0077, /* w */ "uu" },
	{ 0x00C6, /* Æ */ "AE" },
	{ 0x00DF, /* ß */ "ſz" },
	{ 0x00E6, /* æ */ "ae" },
	{ 0x0152, /* Œ */ "OE" },
	{ 0x0153, /* œ */ "oe" },
	{ 0x0195, /* ƕ */ "hv" },
	{ 0x01F6, /* Ƕ */ "Hv" },
	{ 0x0238, /* ȸ */ "db" },
	{ 0x0239, /* ȹ */ "qp" },
	{ 0x026E, /* ɮ */ "lʒ" },
	{ 0x026F, /* ɯ */ "uu" },
	{ 0x02A3, /* ʣ */ "dz" },
	{ 0x02A4, /* ʤ */ "dʒ" },
	{ 0x02A5, /* ʥ */ "dʑ" },
	{ 0x02A6, /* ʦ */ "ts" },
	{ 0x02A7, /* ʧ */ "tʃ" },
	{ 0x02A8, /* ʨ */ "tɕ" },
	{ 0x02A9, /* ʩ */ "fŋ" },
	{ 0x02AA, /* ʪ */ "ls" },
	{ 0x02AB, /* ʫ */ "lz" },
	{ 0x1D6B, /* ᵫ */ "ue" },
	{ 0x1E9E, /* ẞ */ "ſs" },
	{ 0x1EFA, /* Ỻ */ "lL" },
	{ 0x1EFB, /* ỻ */ "ll" },
	{ 0x2114, /* ℔ */ "lb" },
	{ 0xA728, /* Ꜩ */ "TZ" },
	{ 0xA729, /* ꜩ */ "tz" },
	{ 0xA732, /* Ꜳ */ "AA" },
	{ 0xA733, /* ꜳ */ "aa" },
	{ 0xA734, /* Ꜵ */ "AO" },
	{ 0xA735, /* ꜵ */ "ao" },
	{ 0xA736, /* Ꜷ */ "AU" },
	{ 0xA737, /* ꜷ */ "au" },
	{ 0xA738, /* Ꜹ */ "AV" },
	{ 0xA739, /* ꜹ */ "av" },
	{ 0xA73A, /* Ꜻ */ "AV" },
	{ 0xA73B, /* ꜻ (with bar) */ "av" },
	{ 0xA73C, /* Ꜽ */ "AY" },
	{ 0xA73D, /* ꜽ */ "ay" },
	{ 0xA74E, /* Ꝏ */ "OO" },
	{ 0xA74F, /* ꝏ */ "oo" },
	{ 0xA760, /* Ꝡ */ "VY" },
	{ 0xA761, /* ꝡ */ "vy" },
	{ 0xAB31, /* ꬱ */ "aə" },
	{ 0xAB41, /* ꭁ */ "əø" },
	{ 0xAB50, /* ꭐ */ "ui" },
	{ 0xAB51, /* ꭑ (turned) */ "ui" },
	{ 0xAB62, /* ꭢ */ "ɔe" },
	{ 0xAB63, /* ꭣ */ "uo" },
	{ 0xAB66, /* ꭦ */ "dʐ" },
	{ 0xAB67, /* ꭧ */ "tʂ" },
	{ 0xFB00, /* ﬀ */ "ff" },
	{ 0xFB01, /* ﬁ */ "fi" },
	{ 0xFB02, /* ﬂ */ "fl" },
	{ 0xFB03, /* ﬃ */ "ffi" },
	{ 0xFB04, /* ﬄ */ "ffl" },
	{ 0xFB05, /* ﬅ */ "ſt" },
	{ 0xFB06, /* ﬆ */ "st" },
	{ 0x1F670, /* 🙰 */ "et" },
};
static const int ligature_count = sizeof(ligatures) / sizeof(ligatures[0]);


static inline const char* get_ligature_replacement(int c)
{
	// top and bot are the boundary indexes, which still must be tested.
	int top = ligature_count - 1;
	int bot = 0;
	int i = (c & 0x1FFFFF) / 0xFFFF;   // initial index guess
	const struct liga* el = NULL;

	if (i > top)
		i = top;

	while (top >= bot)
	{
		el = &ligatures[i];
		if (el->unicode == c)
			break;
		if (el->unicode > c)
		{
			// next guess: below
			top = i - 1;
			i = (top + bot) / 2;
		}
		else
		{
			// next guess: above
			bot = i + 1;
			i = (top + bot) / 2;
		}
		el = NULL;
	}

	if (el)
	{
		return el->expansion;
	}
	return NULL;
}


const char* fz_get_ligature_replacement(int c)
{
	return get_ligature_replacement(c);
}


static inline int is_whitespace(int c)
{
	switch (c)
	{
	case 0x0009: /* tab */
	case 0x0020: /* space */
	case 0x00A0: /* no-break space */
	case 0x1680: /* ogham space mark */
	case 0x180E: /* mongolian vowel separator */
	case 0x2000: /* en quad */
	case 0x2001: /* em quad */
	case 0x2002: /* en space */
	case 0x2003: /* em space */
	case 0x2004: /* three-per-em space */
	case 0x2005: /* four-per-em space */
	case 0x2006: /* six-per-em space */
	case 0x2007: /* figure space */
	case 0x2008: /* punctuation space */
	case 0x2009: /* thin space */
	case 0x200A: /* hair space */
	case 0x202F: /* narrow no-break space */
	case 0x205F: /* medium mathematical space */
	case 0x3000: /* ideographic space */
		return 1;

	default:
		return 0;
	}
}


int fz_is_whitespace(int c)
{
	return is_whitespace(c);
}


static void
flush_text(fz_context *ctx, fz_stext_device *dev)
{
	fz_stext_page *page = dev->page;

	/* Find current position to enter new text. */
	prepend_line_if_possible(ctx, page->last_block, dev->pen);
}

static void
fz_add_stext_char(fz_context *ctx, fz_stext_device *dev, fz_font *font, int c, int glyph, fz_matrix trm, float adv, int wmode, int force_new_line)
{
	/* ignore when one unicode character maps to multiple glyphs */
	if (c == -1)
		return;

	if (!(dev->opts.flags & FZ_STEXT_PRESERVE_LIGATURES))
	{
		const char* replacement = get_ligature_replacement(c);

		if (replacement)
		{
			fz_add_stext_char_imp(ctx, dev, font, *replacement++, glyph, trm, adv, wmode, force_new_line);
			while (*replacement)
			{
				fz_add_stext_char_imp(ctx, dev, font, *replacement++, -1, trm, 0, wmode, 0);
			}
			return;
		}
	}

	if (!(dev->opts.flags & FZ_STEXT_PRESERVE_WHITESPACE))
	{
		if (is_whitespace(c))
		{
			c = ' ';
		}
	}

	fz_add_stext_char_imp(ctx, dev, font, c, glyph, trm, adv, wmode, force_new_line);
}

static void
fz_stext_extract(fz_context *ctx, fz_stext_device *dev, fz_text_span *span, fz_matrix ctm)
{
	fz_font *font = span->font;
	fz_matrix tm = span->trm;
	fz_matrix trm;
	float adv;
	int i;

	if (span->len == 0)
		return;

	for (i = 0; i < span->len; i++)
	{
		if (dev->opts.flags & FZ_STEXT_MEDIABOX_CLIP)
			if (fz_glyph_entirely_outside_box(ctx, &ctm, span, &span->items[i], &dev->page->mediabox))
				continue;

		/* Calculate new pen location and delta */
		tm.e = span->items[i].x;
		tm.f = span->items[i].y;
		trm = fz_concat(tm, ctm);

		/* Calculate bounding box and new pen position based on font metrics */
		if (span->items[i].gid >= 0)
			adv = fz_advance_glyph(ctx, font, span->items[i].gid, span->wmode);
		else
			adv = 0;

		/* Work-around for embedded fonts missing vertical mode advance widths. */
		if (adv == 0 && span->wmode)
			adv = 1;

		fz_add_stext_char(ctx, dev, font,
			span->items[i].ucs,
			span->items[i].gid,
			trm,
			adv,
			span->wmode,
			(i == 0) && (dev->opts.flags & FZ_STEXT_PRESERVE_SPANS));
	}
}

static int hexrgb_from_color(fz_context *ctx, fz_colorspace *colorspace, const float *color)
{
	float rgb[3];
	fz_convert_color(ctx, colorspace, color, fz_device_rgb(ctx), rgb, NULL, fz_default_color_params);
	return
		(fz_clampi(rgb[0] * 255, 0, 255) << 16) |
		(fz_clampi(rgb[1] * 255, 0, 255) << 8) |
		(fz_clampi(rgb[2] * 255, 0, 255));
}

static void
fz_stext_fill_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm,
	fz_colorspace *colorspace, const float *color, float alpha, fz_color_params color_params)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;
	fz_text_span *span;
	if (text == tdev->lasttext)
		return;
	tdev->color = hexrgb_from_color(ctx, colorspace, color);
	tdev->new_obj = 1;
	for (span = text->head; span; span = span->next)
		fz_stext_extract(ctx, tdev, span, ctm);
	fz_drop_text(ctx, tdev->lasttext);
	tdev->lasttext = fz_keep_text(ctx, text);
}

static void
fz_stext_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *stroke, fz_matrix ctm,
	fz_colorspace *colorspace, const float *color, float alpha, fz_color_params color_params)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;
	fz_text_span *span;
	if (text == tdev->lasttext)
		return;
	tdev->color = hexrgb_from_color(ctx, colorspace, color);
	tdev->new_obj = 1;
	for (span = text->head; span; span = span->next)
		fz_stext_extract(ctx, tdev, span, ctm);
	fz_drop_text(ctx, tdev->lasttext);
	tdev->lasttext = fz_keep_text(ctx, text);
}

static void
fz_stext_clip_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm, fz_rect scissor)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;
	fz_text_span *span;
	if (text == tdev->lasttext)
		return;
	tdev->color = 0;
	tdev->new_obj = 1;
	for (span = text->head; span; span = span->next)
		fz_stext_extract(ctx, tdev, span, ctm);
	fz_drop_text(ctx, tdev->lasttext);
	tdev->lasttext = fz_keep_text(ctx, text);
}

static void
fz_stext_clip_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *stroke, fz_matrix ctm, fz_rect scissor)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;
	fz_text_span *span;
	if (text == tdev->lasttext)
		return;
	tdev->color = 0;
	tdev->new_obj = 1;
	for (span = text->head; span; span = span->next)
		fz_stext_extract(ctx, tdev, span, ctm);
	fz_drop_text(ctx, tdev->lasttext);
	tdev->lasttext = fz_keep_text(ctx, text);
}

static void
fz_stext_ignore_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;
	fz_text_span *span;
	if (text == tdev->lasttext)
		return;
	tdev->color = 0;
	tdev->new_obj = 1;
	for (span = text->head; span; span = span->next)
		fz_stext_extract(ctx, tdev, span, ctm);
	fz_drop_text(ctx, tdev->lasttext);
	tdev->lasttext = fz_keep_text(ctx, text);
}

/* Images and shadings */

static void
fz_stext_fill_image(fz_context *ctx, fz_device *dev, fz_image *img, fz_matrix ctm, float alpha, fz_color_params color_params)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;

	/* If the alpha is less than 50% then it's probably a watermark or effect or something. Skip it. */
	if (alpha < 0.5f)
	{
		return;
	}

	add_image_block_to_page(ctx, tdev->page, ctm, img);
}

static void
fz_stext_fill_image_mask(fz_context *ctx, fz_device *dev, fz_image *img, fz_matrix ctm,
		fz_colorspace *cspace, const float *color, float alpha, fz_color_params color_params)
{
	fz_stext_fill_image(ctx, dev, img, ctm, alpha, color_params);
}

static fz_image *
fz_new_image_from_shade(fz_context *ctx, fz_shade *shade, fz_matrix *in_out_ctm, fz_color_params color_params, fz_rect scissor)
{
	fz_matrix ctm = *in_out_ctm;
	fz_pixmap *pix;
	fz_image *img = NULL;
	fz_rect bounds;
	fz_irect bbox;

	bounds = fz_bound_shade(ctx, shade, ctm);
	bounds = fz_intersect_rect(bounds, scissor);
	bbox = fz_irect_from_rect(bounds);

	pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), bbox, NULL, !shade->use_background);
	fz_try(ctx)
	{
		if (shade->use_background)
			fz_fill_pixmap_with_color(ctx, pix, shade->colorspace, shade->background, color_params);
		else
			fz_clear_pixmap(ctx, pix);
		fz_paint_shade(ctx, shade, NULL, ctm, pix, color_params, bbox, NULL, NULL);
		img = fz_new_image_from_pixmap(ctx, pix, NULL);
	}
	fz_always(ctx)
		fz_drop_pixmap(ctx, pix);
	fz_catch(ctx)
		fz_rethrow(ctx);

	in_out_ctm->a = pix->w;
	in_out_ctm->b = 0;
	in_out_ctm->c = 0;
	in_out_ctm->d = pix->h;
	in_out_ctm->e = pix->x;
	in_out_ctm->f = pix->y;
	return img;
}

static void
fz_stext_fill_shade(fz_context *ctx, fz_device *dev, fz_shade *shade, fz_matrix ctm, float alpha, fz_color_params color_params)
{
	fz_matrix local_ctm = ctm;
	fz_rect scissor = fz_device_current_scissor(ctx, dev);
	fz_image *image = fz_new_image_from_shade(ctx, shade, &local_ctm, color_params, scissor);
	fz_try(ctx)
		fz_stext_fill_image(ctx, dev, image, local_ctm, alpha, color_params);
	fz_always(ctx)
		fz_drop_image(ctx, image);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
fz_stext_close_device(fz_context *ctx, fz_device *dev)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;
	fz_stext_page *page = tdev->page;
	fz_stext_block *block;
	fz_stext_line *line;
	fz_stext_char *ch;

	flush_text(ctx, tdev);

	for (block = page->first_block; block; block = block->next)
	{
		if (block->type != FZ_STEXT_BLOCK_TEXT)
			continue;
		for (line = block->u.t.first_line; line; line = line->next)
		{
			for (ch = line->first_char; ch; ch = ch->next)
			{
				fz_rect ch_box = fz_rect_from_quad(ch->quad);
				if (ch == line->first_char)
					line->bbox = ch_box;
				else
					line->bbox = fz_union_rect(line->bbox, ch_box);
			}
			block->bbox = fz_union_rect(block->bbox, line->bbox);
		}
	}

	/* TODO: smart sorting of blocks and lines in reading order */
	/* TODO: unicode NFC normalization */
}

static void
fz_stext_drop_device(fz_context *ctx, fz_device *dev)
{
	fz_stext_device *tdev = (fz_stext_device*)dev;
	fz_drop_text(ctx, tdev->lasttext);
	fz_drop_stext_options(ctx, &tdev->opts);
}

fz_stext_options *
fz_parse_stext_options(fz_context *ctx, fz_stext_options *opts, const char *string)
{
	const char *val;

	memset(opts, 0, sizeof(*opts));
	opts->scale = 1;

	if (fz_has_option(ctx, string, "preserve-ligatures", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_PRESERVE_LIGATURES;
		if (fz_option_eq(val, "yes"))
			opts->flags |= FZ_STEXT_PRESERVE_LIGATURES;
	}
	if (fz_has_option(ctx, string, "preserve-whitespace", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_PRESERVE_WHITESPACE;
		if (fz_option_eq(val, "yes"))
			opts->flags |= FZ_STEXT_PRESERVE_WHITESPACE;
	}
	if (fz_has_option(ctx, string, "preserve-images", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_PRESERVE_IMAGES;
		if (fz_option_eq(val, "yes"))
			opts->flags |= FZ_STEXT_PRESERVE_IMAGES;
	}
	if (fz_has_option(ctx, string, "reference-images", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_REFERENCE_IMAGES;
		if (!fz_option_eq(val, "no"))
		{
			opts->flags |= FZ_STEXT_REFERENCE_IMAGES;
			if (!fz_option_eq(val, "yes"))
			{
				char pathbuf[PATH_MAX];
				fz_copy_option(ctx, val, pathbuf, sizeof(pathbuf));
				opts->reference_image_path_template = fz_strdup(ctx, pathbuf);
			}
		}
	}
	if (fz_has_option(ctx, string, "reuse-images", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_NO_REUSE_IMAGES;
		if (fz_option_eq(val, "no"))
			opts->flags |= FZ_STEXT_NO_REUSE_IMAGES;
	}
	if (fz_has_option(ctx, string, "inhibit-spaces", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_INHIBIT_SPACES;
		if (fz_option_eq(val, "yes"))
			opts->flags |= FZ_STEXT_INHIBIT_SPACES;
	}
	if (fz_has_option(ctx, string, "dehyphenate", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_DEHYPHENATE;
		if (fz_option_eq(val, "yes"))
			opts->flags |= FZ_STEXT_DEHYPHENATE;
	}
	if (fz_has_option(ctx, string, "preserve-spans", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_PRESERVE_SPANS;
		if (fz_option_eq(val, "yes"))
			opts->flags |= FZ_STEXT_PRESERVE_SPANS;
	}
	if (fz_has_option(ctx, string, "glyph-bbox", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_GLYPH_BBOX;
		if (fz_option_eq(val, "yes"))
			opts->flags |= FZ_STEXT_GLYPH_BBOX;
	}
	// default: enable MEDIABOX-CLIP:
	opts->flags |= FZ_STEXT_MEDIABOX_CLIP;
	if (fz_has_option(ctx, string, "mediabox-clip", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_MEDIABOX_CLIP;
		if (fz_option_eq(val, "no"))
			opts->flags &= ~FZ_STEXT_MEDIABOX_CLIP;
	}
	if (fz_has_option(ctx, string, "text-as-path", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_NO_TEXT_AS_PATH;
		if (fz_option_eq(val, "no"))
			opts->flags |= FZ_STEXT_NO_TEXT_AS_PATH;
	}
	if (fz_has_option(ctx, string, "external-styles", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_EXTERNAL_STYLES;
		if (!fz_option_eq(val, "no"))
		{
			opts->flags |= FZ_STEXT_EXTERNAL_STYLES;
			if (!fz_option_eq(val, "yes"))
			{
				char pathbuf[PATH_MAX];
				fz_copy_option(ctx, val, pathbuf, sizeof(pathbuf));
				opts->external_styles_path_template = fz_strdup(ctx, pathbuf);
			}
		}
	}
	if (fz_has_option(ctx, string, "resolution", &val))
	{
		opts->flags_conf_mask |= FZ_STEXT_RESOLUTION;
		float v = fz_atof(val);
		// do accept a horribly wide resolution range, but do not accept totally ludicrous values:
		if (v >= 2 && v <= 10e3)
		{
			opts->scale = v / 96.0f; /* HTML base resolution is 96ppi */
		}
	}
	return opts;
}

fz_stext_options *
fz_set_stext_options_images_handler(fz_context* ctx, fz_stext_options* opts, fz_process_stext_referenced_image_f* print_img, void* user_state)
{
	opts->user_state = user_state;
	opts->print_image_object = print_img;
	return opts;
}

fz_stext_options *
fz_copy_stext_options(fz_context* ctx, fz_stext_options* dst, const fz_stext_options* src)
{
	assert(dst != NULL);
	if (!src)
	{
		memset(dst, 0, sizeof(*dst));
	}
	else
	{
		*dst = *src;
		if (src->external_styles_path_template)
			dst->external_styles_path_template = fz_strdup(ctx, src->external_styles_path_template);
		if (src->reference_image_path_template)
			dst->reference_image_path_template = fz_strdup(ctx, src->reference_image_path_template);
	}
	return dst;
}	

void
fz_drop_stext_options(fz_context* ctx, fz_stext_options* options)
{
	if (options)
	{
		if (options->external_styles_path_template)
			fz_free(ctx, options->external_styles_path_template);
		if (options->reference_image_path_template)
			fz_free(ctx, options->reference_image_path_template);
		options->external_styles_path_template = NULL;
		options->reference_image_path_template = NULL;
	}
}

fz_device *
fz_new_stext_device(fz_context *ctx, fz_stext_page *page, const fz_stext_options *opts)
{
	fz_stext_device *dev = fz_new_derived_device(ctx, fz_stext_device);

	dev->super.close_device = fz_stext_close_device;
	dev->super.drop_device = fz_stext_drop_device;

	dev->super.fill_text = fz_stext_fill_text;
	dev->super.stroke_text = fz_stext_stroke_text;
	dev->super.clip_text = fz_stext_clip_text;
	dev->super.clip_stroke_text = fz_stext_clip_stroke_text;
	dev->super.ignore_text = fz_stext_ignore_text;

	if (opts)
	{
		if (opts->flags & FZ_STEXT_PRESERVE_IMAGES)
		{
			dev->super.fill_shade = fz_stext_fill_shade;
			dev->super.fill_image = fz_stext_fill_image;
			dev->super.fill_image_mask = fz_stext_fill_image_mask;
		}

		fz_copy_stext_options(ctx, &dev->opts, opts);
	}
	dev->page = page;
	dev->pen.x = 0;
	dev->pen.y = 0;
	dev->trm = fz_identity;
	dev->lastchar = ' ';
	dev->curdir = 1;
	dev->lasttext = NULL;

	return (fz_device*)dev;
}

#endif
