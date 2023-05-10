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

#if FZ_ENABLE_RENDER_CORE 

#define SUBSCRIPT_OFFSET 0.2f
#define SUPERSCRIPT_OFFSET -0.2f

#include <ft2build.h>
#include FT_FREETYPE_H

// Text black color when converted from DeviceCMYK to RGB
#define CMYK_BLACK 0x221f1f

#include "boxer.h"

static void fz_scale_stext_page(fz_context *ctx, fz_stext_page *page, float scale)
{
	fz_matrix m = fz_scale(scale, scale);
	fz_stext_block *block;
	fz_stext_line *line;
	fz_stext_char *ch;

	for (block = page->first_block; block; block = block->next)
	{
		block->bbox = fz_transform_rect(block->bbox, m);
		switch (block->type)
		{
		case FZ_STEXT_BLOCK_TEXT:
			for (line = block->u.t.first_line; line; line = line->next)
			{
				line->bbox = fz_transform_rect(block->bbox, m);
				for (ch = line->first_char; ch; ch = ch->next)
				{
					ch->origin = fz_transform_point(ch->origin, m);
					ch->quad = fz_transform_quad(ch->quad, m);
					ch->size = ch->size * scale;
				}
			}
			break;

		case FZ_STEXT_BLOCK_IMAGE:
			block->u.i.transform = fz_post_scale(block->u.i.transform, scale, scale);
			break;
		}
	}
}

/* HTML output (visual formatting with preserved layout) */

static int
detect_super_script(fz_stext_line *line, fz_stext_char *ch)
{
	if (line->wmode == 0 && line->dir.x == 1 && line->dir.y == 0)
		return ch->origin.y < line->first_char->origin.y - ch->size * 0.1f;
	return 0;
}

static const char *
font_full_name(fz_context *ctx, fz_font *font)
{
	const char *name = fz_font_name(ctx, font);
	const char *s = strchr(name, '+');
	return s ? s + 1 : name;
}

static const char *
html_clean_font_name(const char *fontname)
{
	if (strstr(fontname, "Times"))
		return "Times New Roman";
	if (strstr(fontname, "Arial") || strstr(fontname, "Helvetica"))
	{
		if (strstr(fontname, "Narrow") || strstr(fontname, "Condensed"))
			return "Arial Narrow";
		return "Arial";
	}
	if (strstr(fontname, "Courier"))
		return "Courier";
	return fontname;
}

static void
font_family_name(fz_context *ctx, fz_font *font, char *buf, int size, int is_mono, int is_serif)
{
	const char *name = html_clean_font_name(font_full_name(ctx, font));
	char *s;
	fz_strncpy_s(ctx, buf, name, size);
	s = strrchr(buf, '-');
	if (s)
		*s = 0;
	if (is_mono)
		fz_strlcat(buf, ",monospace", size);
	else
		fz_strlcat(buf, is_serif ? ",serif" : ",sans-serif", size);
}

static void
fz_print_style_begin_html(fz_context *ctx, fz_output *out, fz_font *font, float size, int sup, int color)
{
	char family[80];

	int is_bold = fz_font_is_bold(ctx, font);
	int is_italic = fz_font_is_italic(ctx, font);
	int is_serif = fz_font_is_serif(ctx, font);
	int is_mono = fz_font_is_monospaced(ctx, font);

	font_family_name(ctx, font, family, sizeof family, is_mono, is_serif);

	if (sup) fz_write_string(ctx, out, "<sup>");
	if (is_mono) fz_write_string(ctx, out, "<tt>");
	if (is_bold) fz_write_string(ctx, out, "<b>");
	if (is_italic) fz_write_string(ctx, out, "<i>");
	fz_write_printf(ctx, out, "<span style=\"font-family:%s;font-size:%.1fpt", family, size);
	if (color != 0 && color != CMYK_BLACK)
		fz_write_printf(ctx, out, ";color:#%06x", color);
	fz_write_printf(ctx, out, "\">");
}

static void
fz_print_style_end_html(fz_context *ctx, fz_output *out, fz_font *font, float size, int sup, int color)
{
	int is_mono = fz_font_is_monospaced(ctx, font);
	int is_bold = fz_font_is_bold(ctx,font);
	int is_italic = fz_font_is_italic(ctx, font);

	fz_write_string(ctx, out, "</span>");
	if (is_italic) fz_write_string(ctx, out, "</i>");
	if (is_bold) fz_write_string(ctx, out, "</b>");
	if (is_mono) fz_write_string(ctx, out, "</tt>");
	if (sup) fz_write_string(ctx, out, "</sup>");
}

static void
fz_print_stext_image_as_html(fz_context *ctx, fz_output *out, fz_stext_block *block, int pagenum, int object_index, fz_matrix page_ctm, const fz_stext_options* options)
{
	fz_rect mediabox = fz_transform_rect(block->bbox, page_ctm);
	fz_matrix ctm = block->u.i.transform;

#define USE_CSS_MATRIX_TRANSFORMS
#ifdef USE_CSS_MATRIX_TRANSFORMS
	/* Matrix maths notes.
	 * When we get here ctm maps the unit square to the position in device
	 * space occupied by the image.
	 *
	 * That is to say that mapping the 4 corners of the unit square through
	 * the transform, give us the 4 target corners. We extend the corners
	 * by adding an extra '1' into them to allow transforms to work. Thus
	 * (x,y) maps through ctm = (a b c d e f) as:
	 *
	 * (x y 1) (a b 0) = (X Y 1)
	 *         (c d 0)
	 *         (e f 1)
	 *
	 * To simplify reading of matrix maths, we use the trick where we
	 * 'drop' the first matrix down the page. Thus the corners c0=(0,0),
	 * c1=(1,0), c2=(1,1), c3=(0,1) map to C0, C1, C2, C3 respectively:
	 *
	 *         (    a     b 0)
	 *         (    c     d 0)
	 *         (    e     f 1)
	 * (0 0 1) (    e     f 1)
	 * (0 1 1) (  c+e   d+f 1)
	 * (1 1 1) (a+c+e b+d+f 1)
	 * (1 0 1) (  a+e   b+f 1)
	 *
	 * where C0 = (e,f), C1=(c+e, d+f) C2=(a+c+e, b+d+f), C3=(a+e, b+f)
	 *
	 * Unfortunately, the CSS matrix transform, does not map the unit square.
	 * Rather it does something moderately mad. As far as I can work out, the
	 * top left corner of a (0,0) -> (w, h) box is transformed using the .e
	 * and .f entries of the matrix. Then the image from within that square
	 * is transformed using the centre of that square as the origin.
	 *
	 * So, an image placed at (0,0) in destination space with 1:1 transform
	 * will result in an image a (0,0) as you'd expect. But an image at (0,0)
	 * with a scale of 2, will result in 25% of the image off the left of the
	 * screen, and 25% off the top.
	 *
	 * Accordingly, we have to adjust the ctm in several steps.
	 */
	/* Move to moving the centre of the image. */
	ctm.e += (ctm.a+ctm.c) / 2;
	ctm.f += (ctm.b+ctm.d) / 2;
	/* Move from transforming the unit square to w/h */
	ctm.a /= block->u.i.image->w;
	ctm.b /= block->u.i.image->w;
	ctm.c /= block->u.i.image->h;
	ctm.d /= block->u.i.image->h;
	/* Move from points to pixels */
	ctm.a *= 96.0f / 72;
	ctm.b *= 96.0f / 72;
	ctm.c *= 96.0f / 72;
	ctm.d *= 96.0f / 72;
	ctm.e *= 96.0f / 72;
	ctm.f *= 96.0f / 72;
	/* Move to moving the top left of the untransformed image box, cos HTML is bonkers. */
	ctm.e -= block->u.i.image->w / 2;
	ctm.f -= block->u.i.image->h / 2;

	fz_write_printf(ctx, out, "<img style=\"position:absolute;transform:matrix(%g,%g,%g,%g,%g,%g)\" data-mediabox=\"%R\" data-dimensions=\"%R\" src=\"",
		ctm.a, ctm.b, ctm.c, ctm.d, ctm.e, ctm.f, &block->bbox, &mediabox);
#else
	/* Alternative version of the code that uses scaleX/Y and rotate
	 * instead, but only copes with axis aligned cases. */
	int t;

	int x = block->bbox.x0;
	int y = block->bbox.y0;
	int w = block->bbox.x1 - block->bbox.x0;
	int h = block->bbox.y1 - block->bbox.y0;

	const char *flip = "";

	if (ctm.b == 0 && ctm.c == 0)
	{
		if (ctm.a < 0 && ctm.d < 0)
			flip = "transform: scaleX(-1) scaleY(-1);";
		else if (ctm.a < 0)
		{
			flip = "transform: scaleX(-1);";
		}
		else if (ctm.d < 0)
		{
			flip = "transform: scaleY(-1);";
		}
	} else if (ctm.a == 0 && ctm.d == 0) {
		if (ctm.b < 0 && ctm.c < 0)
		{
			flip = "transform: scaleY(-1) rotate(90deg);";
			x += (w-h)/2;
			y -= (w-h)/2;
			t = w; w = h; h = t;
		}
		else if (ctm.b < 0)
		{
			flip = "transform: scaleX(-1) scaleY(-1) rotate(90deg);";
			x += (w-h)/2;
			y -= (w-h)/2;
			t = w; w = h; h = t;
		}
		else if (ctm.c < 0)
		{
			flip = "transform: scaleX(-1) scaleY(-1) rotate(270deg);";
			x += (w-h)/2;
			y -= (w-h)/2;
			t = w; w = h; h = t;
		}
		else
		{
			flip = "transform: scaleY(-1) rotate(270deg);";
			x += (w-h)/2;
			y -= (w-h)/2;
			t = w; w = h; h = t;
		}
	}

	fz_write_printf(ctx, out, "<img style=\"position:absolute;%stop:%gpt;left:%gpt;width:%gpt;height:%gpt\" data-mediabox=\"%R\" data-dimensions=\"%R\" src=\"", flip, y, x, w, h, &block->bbox, &mediabox);
#endif	
	if (options->flags & FZ_STEXT_REFERENCE_IMAGES)
	{
		if (options->print_image_object)
		{
			(*options->print_image_object)(ctx, out, block, pagenum, object_index, page_ctm, options);
		}
		else
		{
			fz_write_string(ctx, out, "(undefined-reference-images-handler)\">\n");
			fz_throw(ctx, FZ_ERROR_GENERIC, "stext reference-images option used but no image handler has been provided.");
		}
	}
	else
	{
		fz_write_image_as_data_uri(ctx, out, block->u.i.image);
	}
	fz_write_string(ctx, out, "\">\n");
}

void
fz_print_stext_block_as_html(fz_context *ctx, fz_output *out, fz_stext_block *block)
{
	fz_stext_line *line;
	fz_stext_char *ch;
	float x, y, h;

	fz_font *font = NULL;
	float size = 0;
	int sup = 0;
	int color = 0;

	for (line = block->u.t.first_line; line; line = line->next)
	{
		x = line->bbox.x0;
		y = line->bbox.y0;
		h = line->bbox.y1 - line->bbox.y0;

		if (line->first_char)
		{
			h = line->first_char->size;
			y = line->first_char->origin.y - h * 0.8f;
		}

		fz_write_printf(ctx, out, "<p style=\"position:absolute;white-space:pre;margin:0;padding:0;top:%gpt;left:%gpt;line-height:%.1fpt\" data-mediabox=\"%R\">", y, x, h, &line->bbox);
		font = NULL;

		for (ch = line->first_char; ch; ch = ch->next)
		{
			int ch_sup = detect_super_script(line, ch);
			if (ch->font != font || ch->size != size || ch_sup != sup || ch->color != color)
			{
				if (font)
					fz_print_style_end_html(ctx, out, font, size, sup, color);
				font = ch->font;
				size = ch->size;
				color = ch->color;
				sup = ch_sup;
				fz_print_style_begin_html(ctx, out, font, size, sup, color);
			}

			switch (ch->c)
			{
			default:
				if (ch->c >= 32 && ch->c < 127)
					fz_write_byte(ctx, out, ch->c);
				else
					fz_write_printf(ctx, out, "&#x%x;", ch->c);
				break;
			case '<': fz_write_string(ctx, out, "&lt;"); break;
			case '>': fz_write_string(ctx, out, "&gt;"); break;
			case '&': fz_write_string(ctx, out, "&amp;"); break;
			case '"': fz_write_string(ctx, out, "&quot;"); break;
			case '\'': fz_write_string(ctx, out, "&apos;"); break;
			}
		}

		if (font)
			fz_print_style_end_html(ctx, out, font, size, sup, color);

		fz_write_string(ctx, out, "</p>\n");
	}
}

void
fz_print_stext_page_as_html(fz_context *ctx, fz_output *out, fz_stext_page *page, int pagenum, fz_matrix ctm, const fz_stext_options *options)
{
	fz_stext_block *block;
	fz_rect mediabox = fz_transform_rect(page->mediabox, ctm);
	int index;

	float w = page->mediabox.x1 - page->mediabox.x0;
	float h = page->mediabox.y1 - page->mediabox.y0;

	fz_write_printf(ctx, out, "<div id=\"page%d\" style=\"position:relative;width:%gpt;height:%gpt;background-color:white;\" data-mediabox=\"%R\">\n",
		pagenum,
		w, h,
		&mediabox);

	index = 0;
	for (block = page->first_block; block; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_IMAGE && (!options || (options->flags & FZ_STEXT_PRESERVE_IMAGES)))
		{
			fz_print_stext_image_as_html(ctx, out, block, pagenum, index, ctm, options);
		}
		else if (block->type == FZ_STEXT_BLOCK_TEXT)
		{
			fz_print_stext_block_as_html(ctx, out, block);
		}
		index++;
	}

	fz_write_string(ctx, out, "</div>\n");
}

void
fz_print_stext_header_as_html(fz_context *ctx, fz_output *out)
{
	fz_write_string(ctx, out, "<!DOCTYPE html>\n");
	fz_write_string(ctx, out, "<html>\n");
	fz_write_string(ctx, out, "<head>\n");
	fz_write_string(ctx, out, "<style>\n");
	fz_write_string(ctx, out, "body{background-color:slategray}\n");
	fz_write_string(ctx, out, "div{position:relative;background-color:white;margin:1em auto;box-shadow:1px 1px 8px -2px black}\n");
	fz_write_string(ctx, out, "p{position:absolute;white-space:pre;margin:0}\n");
	fz_write_string(ctx, out, "</style>\n");
	fz_write_string(ctx, out, "</head>\n");
	fz_write_string(ctx, out, "<body>\n");
}

void
fz_print_stext_trailer_as_html(fz_context *ctx, fz_output *out)
{
	fz_write_string(ctx, out, "</body>\n");
	fz_write_string(ctx, out, "</html>\n");
}

/* XHTML output (semantic, little layout, suitable for reflow) */

static void
fz_print_stext_image_as_xhtml(fz_context *ctx, fz_output *out, fz_stext_block *block, int pagenum, int object_index, fz_matrix ctm, const fz_stext_options *options)
{
	float w = block->bbox.x1 - block->bbox.x0;
	float h = block->bbox.y1 - block->bbox.y0;
	fz_rect mediabox = fz_transform_rect(block->bbox, ctm);

	fz_write_printf(ctx, out, "<p><img width=\"%gpt\" height=\"%gpt\" data-mediabox=\"%R\" data-dimensions=\"%R\" src=\"", w, h, &block->bbox, &mediabox);
	if (options->flags & FZ_STEXT_REFERENCE_IMAGES)
	{
		if (options->print_image_object)
		{
			(*options->print_image_object)(ctx, out, block, pagenum, object_index, ctm, options);
		}
		else
		{
			fz_write_string(ctx, out, "(undefined-reference-images-handler)\"/></p>\n");
			fz_throw(ctx, FZ_ERROR_GENERIC, "stext reference-images option used but no image handler has been provided.");
		}
	}
	else
	{
		fz_write_image_as_data_uri(ctx, out, block->u.i.image);
	}
	fz_write_string(ctx, out, "\"/></p>\n");
}

static void
fz_print_style_begin_xhtml(fz_context *ctx, fz_output *out, fz_font *font, int sup)
{
	int is_mono = fz_font_is_monospaced(ctx, font);
	int is_bold = fz_font_is_bold(ctx, font);
	int is_italic = fz_font_is_italic(ctx, font);

	if (sup)
		fz_write_string(ctx, out, "<sup>");
	if (is_mono)
		fz_write_string(ctx, out, "<tt>");
	if (is_bold)
		fz_write_string(ctx, out, "<b>");
	if (is_italic)
		fz_write_string(ctx, out, "<i>");
}

static void
fz_print_style_end_xhtml(fz_context *ctx, fz_output *out, fz_font *font, int sup)
{
	int is_mono = fz_font_is_monospaced(ctx, font);
	int is_bold = fz_font_is_bold(ctx, font);
	int is_italic = fz_font_is_italic(ctx, font);

	if (is_italic)
		fz_write_string(ctx, out, "</i>");
	if (is_bold)
		fz_write_string(ctx, out, "</b>");
	if (is_mono)
		fz_write_string(ctx, out, "</tt>");
	if (sup)
		fz_write_string(ctx, out, "</sup>");
}

static float avg_font_size_of_line(fz_stext_char *ch)
{
	float size = 0;
	int n = 0;
	if (!ch)
		return 0;
	while (ch)
	{
		size += ch->size;
		++n;
		ch = ch->next;
	}
	return size / n;
}

static const char *tag_from_font_size(float size)
{
	if (size >= 20) return "h1";
	if (size >= 15) return "h2";
	if (size >= 12) return "h3";
	return "p";
}

static void fz_print_stext_block_as_xhtml(fz_context *ctx, fz_output *out, fz_stext_block *block)
{
	fz_stext_line *line;
	fz_stext_char *ch;

	fz_font *font = NULL;
	int sup = 0;
	int sp = 1;
	const char *tag = NULL;
	const char *new_tag;

	for (line = block->u.t.first_line; line; line = line->next)
	{
		new_tag = tag_from_font_size(avg_font_size_of_line(line->first_char));
		if (tag != new_tag)
		{
			if (tag)
			{
				if (font)
					fz_print_style_end_xhtml(ctx, out, font, sup);
				fz_write_printf(ctx, out, "</%s>", tag);
			}
			tag = new_tag;
			fz_write_printf(ctx, out, "<%s>", tag);
			if (font)
				fz_print_style_begin_xhtml(ctx, out, font, sup);
		}

		if (!sp)
			fz_write_byte(ctx, out, ' ');

		for (ch = line->first_char; ch; ch = ch->next)
		{
			int ch_sup = detect_super_script(line, ch);
			if (ch->font != font || ch_sup != sup)
			{
				if (font)
					fz_print_style_end_xhtml(ctx, out, font, sup);
				font = ch->font;
				sup = ch_sup;
				fz_print_style_begin_xhtml(ctx, out, font, sup);
			}

			sp = (ch->c == ' ');
			switch (ch->c)
			{
			default:
				if (ch->c >= 32 && ch->c < 127)
					fz_write_byte(ctx, out, ch->c);
				else
					fz_write_printf(ctx, out, "&#x%x;", ch->c);
				break;
			case '<': fz_write_string(ctx, out, "&lt;"); break;
			case '>': fz_write_string(ctx, out, "&gt;"); break;
			case '&': fz_write_string(ctx, out, "&amp;"); break;
			case '"': fz_write_string(ctx, out, "&quot;"); break;
			case '\'': fz_write_string(ctx, out, "&apos;"); break;
			}
		}
	}

	if (font)
		fz_print_style_end_xhtml(ctx, out, font, sup);
	fz_write_printf(ctx, out, "</%s>\n", tag);
}

void
fz_print_stext_page_as_xhtml(fz_context *ctx, fz_output *out, fz_stext_page *page, int pagenum, fz_matrix ctm, const fz_stext_options *options)
{
	fz_stext_block *block;
	fz_rect mediabox = fz_transform_rect(page->mediabox, ctm);
	int index;

	fz_write_printf(ctx, out, "<div id=\"page%d\" width=\"%gpt\" height=\"%gpt\" data-mediabox=\"%R\">\n",
		pagenum,
		mediabox.x1 - mediabox.x0,
		mediabox.y1 - mediabox.y0,
		&page->mediabox);

	index = 0;
	for (block = page->first_block; block; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_IMAGE && (!options || (options->flags & FZ_STEXT_PRESERVE_IMAGES)))
			fz_print_stext_image_as_xhtml(ctx, out, block, pagenum, index, ctm, options);
		else if (block->type == FZ_STEXT_BLOCK_TEXT)
			fz_print_stext_block_as_xhtml(ctx, out, block);
		index++;
	}

	fz_write_string(ctx, out, "</div>\n");
}

void
fz_print_stext_header_as_xhtml(fz_context *ctx, fz_output *out)
{
	fz_write_string(ctx, out, "<?xml version=\"1.0\"?>\n");
	fz_write_string(ctx, out, "<!DOCTYPE html");
	fz_write_string(ctx, out, " PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"");
	fz_write_string(ctx, out, " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
	fz_write_string(ctx, out, "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
	fz_write_string(ctx, out, "<head>\n");
	fz_write_string(ctx, out, "<style>\n");
	fz_write_string(ctx, out, "p{white-space:pre-wrap}\n");
	fz_write_string(ctx, out, "</style>\n");
	fz_write_string(ctx, out, "</head>\n");
	fz_write_string(ctx, out, "<body>\n");
}

void
fz_print_stext_trailer_as_xhtml(fz_context *ctx, fz_output *out)
{
	fz_write_string(ctx, out, "</body>\n");
	fz_write_string(ctx, out, "</html>\n");
}

/* Detailed XML dump of the entire structured text data */

void
fz_print_stext_page_as_xml(fz_context *ctx, fz_output *out, fz_stext_page *page, int id, fz_matrix ctm)
{
	fz_stext_block *block;
	fz_stext_line *line;
	fz_stext_char *ch;
	fz_rect mediabox = fz_transform_rect(page->mediabox, ctm);

	fz_write_printf(ctx, out, "<page id=\"page%d\" pagenum=\"%d\" width=\"%g\" height=\"%g\" mediabox=\"%R\">\n", id, id,
		mediabox.x1 - mediabox.x0,
		mediabox.y1 - mediabox.y0,
		&page->mediabox);

	for (block = page->first_block; block; block = block->next)
	{
		switch (block->type)
		{
		case FZ_STEXT_BLOCK_TEXT:
			fz_write_printf(ctx, out, "<block bbox=\"%R\">\n", &block->bbox);
			for (line = block->u.t.first_line; line; line = line->next)
			{
				fz_font *font = NULL;
				float size = 0;
				const char *name = NULL;

				fz_write_printf(ctx, out, "<line bbox=\"%R\" wmode=\"%d\" dir=\"%P\">\n",
						&line->bbox,
						line->wmode,
						&line->dir);

				for (ch = line->first_char; ch; ch = ch->next)
				{
					if (ch->font != font || ch->size != size)
					{
						if (font)
							fz_write_string(ctx, out, "</font>\n");
						font = ch->font;
						size = ch->size;
						name = font_full_name(ctx, font);
						fz_write_printf(ctx, out, "<font name=\"%s\" size=\"%g\">\n", name, size);
					}
					if (fz_quad_is_axis_oriented(ch->quad))
					{
						fz_rect bb = fz_rect_from_quad(ch->quad);
						fz_write_printf(ctx, out, "<char rect=\"%R\" x=\"%g\" y=\"%g\" color=\"#%06x\" c=\"",
							&bb,
							ch->origin.x, ch->origin.y,
							ch->color);
					}
					else
					{
						fz_write_printf(ctx, out, "<char quad=\"%Z\" x=\"%g\" y=\"%g\" color=\"#%06x\" c=\"",
							&ch->quad,
							ch->origin.x, ch->origin.y,
							ch->color);
					}
					switch (ch->c)
					{
					case '<': fz_write_string(ctx, out, "&lt;"); break;
					case '>': fz_write_string(ctx, out, "&gt;"); break;
					case '&': fz_write_string(ctx, out, "&amp;"); break;
					case '"': fz_write_string(ctx, out, "&quot;"); break;
					case '\'': fz_write_string(ctx, out, "&apos;"); break;
					default:
						   if (ch->c >= 32 && ch->c < 127)
							   fz_write_printf(ctx, out, "%c", ch->c);
						   else
							   fz_write_printf(ctx, out, "&#x%x;", ch->c);  // XML NCR: Numeric Character Reference
						   break;
					}
					fz_write_string(ctx, out, "\"/>\n");
				}

				if (font)
					fz_write_string(ctx, out, "</font>\n");

				fz_write_string(ctx, out, "</line>\n");
			}
			fz_write_string(ctx, out, "</block>\n");
			break;

		case FZ_STEXT_BLOCK_IMAGE:
			fz_write_printf(ctx, out, "<image bbox=\"%R\" />\n", &block->bbox);
			break;
		}
	}
	fz_write_string(ctx, out, "</page>\n");
}

/* JSON dump */

void
fz_print_stext_page_as_json(fz_context *ctx, fz_output *out, fz_stext_page *page, int id, fz_matrix ctm)
{
	fz_stext_block *block;
	fz_stext_line *line;
	fz_stext_char *ch;
	fz_rect mediabox = fz_transform_rect(page->mediabox, ctm);
	float w = mediabox.x1 - mediabox.x0;
	float h = mediabox.y1 - mediabox.y0;

	fz_write_printf(ctx, out, "{%q:%d, %q:%g, %q:%g, %q:{%q:%g, %q:%g, %q:%g, %q:%g}, %q:[",
		"page_id", id,
		"width", w,
		"height", h,
		"mediabox",
		"x0", page->mediabox.x0,
		"y0", page->mediabox.y0,
		"x1", page->mediabox.x1,
		"y1", page->mediabox.y1,
		"blocks");

	for (block = page->first_block; block; block = block->next)
	{
		if (block != page->first_block)
			fz_write_string(ctx, out, ",");
		switch (block->type)
		{
		case FZ_STEXT_BLOCK_TEXT:
			fz_write_printf(ctx, out, "{%q:%q,", "type", "text");
			fz_write_printf(ctx, out, "%q:{", "bbox");
			fz_write_printf(ctx, out, "%q:%g,", "x", block->bbox.x0);
			fz_write_printf(ctx, out, "%q:%g,", "y", block->bbox.y0);
			fz_write_printf(ctx, out, "%q:%g,", "w", (block->bbox.x1 - block->bbox.x0));
			fz_write_printf(ctx, out, "%q:%g},", "h", (block->bbox.y1 - block->bbox.y0));
			fz_write_printf(ctx, out, "%q:[", "lines");

			for (line = block->u.t.first_line; line; line = line->next)
			{
				if (line != block->u.t.first_line)
					fz_write_string(ctx, out, ",");
				fz_write_printf(ctx, out, "{%q:%d,", "wmode", line->wmode);
				fz_write_printf(ctx, out, "%q:{", "bbox");
				fz_write_printf(ctx, out, "%q:%g,", "x", line->bbox.x0);
				fz_write_printf(ctx, out, "%q:%g,", "y", line->bbox.y0);
				fz_write_printf(ctx, out, "%q:%g,", "w", (line->bbox.x1 - line->bbox.x0));
				fz_write_printf(ctx, out, "%q:%g},", "h", (line->bbox.y1 - line->bbox.y0));

				/* Since we force preserve-spans, the first char has the style for the entire line. */
				if (line->first_char)
				{
					fz_font *font = line->first_char->font;
					const char *font_family = "sans-serif";
					const char *font_weight = "normal";
					const char *font_style = "normal";
					if (fz_font_is_monospaced(ctx, font)) font_family = "monospace";
					else if (fz_font_is_serif(ctx, font)) font_family = "serif";
					if (fz_font_is_bold(ctx, font)) font_weight = "bold";
					if (fz_font_is_italic(ctx, font)) font_style = "italic";
					fz_write_printf(ctx, out, "%q:{", "font");
					fz_write_printf(ctx, out, "%q:%q,", "name", fz_font_name(ctx, font));
					fz_write_printf(ctx, out, "%q:%q,", "family", font_family);
					fz_write_printf(ctx, out, "%q:%q,", "weight", font_weight);
					fz_write_printf(ctx, out, "%q:%q,", "style", font_style);
					fz_write_printf(ctx, out, "%q:%g},", "size", line->first_char->size);
					fz_write_printf(ctx, out, "%q:%g,", "x", line->first_char->origin.x);
					fz_write_printf(ctx, out, "%q:%g,", "y", line->first_char->origin.y);
				}

				fz_write_printf(ctx, out, "%q:\"", "text");
				for (ch = line->first_char; ch; ch = ch->next)
				{
					if (ch->c == '"' || ch->c == '\\')
						fz_write_printf(ctx, out, "\\%c", ch->c);
					else if (ch->c < 32)
						fz_write_printf(ctx, out, "\\u%04x", ch->c);
					else
						fz_write_printf(ctx, out, "%C", ch->c);
				}
				fz_write_printf(ctx, out, "\"}");
			}
			fz_write_string(ctx, out, "]}");
			break;

		case FZ_STEXT_BLOCK_IMAGE:
			fz_write_printf(ctx, out, "{%q:%q,", "type", "image");
			fz_write_printf(ctx, out, "%q:{", "bbox");
			fz_write_printf(ctx, out, "%q:%g,", "x", block->bbox.x0);
			fz_write_printf(ctx, out, "%q:%g,", "y", block->bbox.y0);
			fz_write_printf(ctx, out, "%q:%g,", "w", (block->bbox.x1 - block->bbox.x0));
			fz_write_printf(ctx, out, "%q:%g}}", "h", (block->bbox.y1 - block->bbox.y0));
			break;
		}
	}
	fz_write_string(ctx, out, "]}");
}

/* Plain text */

void
fz_print_stext_page_as_text(fz_context *ctx, fz_output *out, fz_stext_page *page)
{
	fz_stext_block *block;
	fz_stext_line *line;
	fz_stext_char *ch;
	char utf[10];
	int i, n;

	for (block = page->first_block; block; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_TEXT)
		{
			for (line = block->u.t.first_line; line; line = line->next)
			{
				for (ch = line->first_char; ch; ch = ch->next)
				{
					n = fz_runetochar(utf, ch->c);
					for (i = 0; i < n; i++)
						fz_write_byte(ctx, out, utf[i]);
				}
				fz_write_string(ctx, out, "\n");
			}
			fz_write_string(ctx, out, "\n");
		}
	}
}

#define WRITE_AS_PS

static void
gogo(fz_context *ctx, fz_output *out, boxer_t *big_boxer, int depth)
{
	fz_rect margins;
	boxer_t *boxer;
	boxer_t *boxer1;
	boxer_t *boxer2;
	int i, n;
	fz_rect *list;

	margins = boxer_margins(big_boxer);
	fz_write_printf(ctx, out, "\n\n%% MARGINS %R\n", &margins);

	boxer = boxer_subset(big_boxer, &margins);

	if (depth < 6 && boxer_subdivide(boxer, &boxer1, &boxer2)) {
		gogo(ctx, out, boxer1, depth+1);
		gogo(ctx, out, boxer2, depth+1);
		boxer_destroy(boxer1);
		boxer_destroy(boxer2);
		return;
	}

	boxer_sort(boxer);
	n = boxer_results(boxer, &list);

	fz_write_printf(ctx, out, "% SUBDIVISION\n");
	for (i = 0; i < n; i++) {
#ifndef WRITE_AS_PS
		fz_write_printf(ctx, out, "%R\n", &list[i]);
#else
		fz_write_printf(ctx, out, "%% %R\n", &list[i]);
#endif
	}

#ifdef WRITE_AS_PS
	fz_write_printf(ctx, out, "0 0 0 setrgbcolor\n");
	for (i = 0; i < n; i++) {
		fz_write_printf(ctx, out, "%g %g moveto\n%g %g lineto\n%g %g lineto\n%g %g lineto\nclosepath\nstroke\n\n",
				list[i].x0, list[i].y0,
				list[i].x0, list[i].y1,
				list[i].x1, list[i].y1,
				list[i].x1, list[i].y0);
	}

	fz_write_printf(ctx, out, "1 0 0 setrgbcolor\n");
	fz_write_printf(ctx, out, "%g %g moveto\n%g %g lineto\n%g %g lineto\n%g %g lineto\nclosepath\nstroke\n\n",
			margins.x0, margins.y0,
			margins.x0, margins.y1,
			margins.x1, margins.y1,
			margins.x1, margins.y0);
#endif
	boxer_destroy(boxer);
}

void
fz_print_stext_page_as_empty_box(fz_context *ctx, fz_output *out, fz_stext_page *page)
{
	fz_stext_block *block;
	fz_stext_line *line;
	//fz_stext_char *ch;
	//char utf[10];
	boxer_t *boxer;

	boxer = boxer_create(&page->mediabox);

	for (block = page->first_block; block; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_TEXT)
		{
			for (line = block->u.t.first_line; line; line = line->next)
			{
				boxer_feed(boxer, &line->bbox);
				//for (ch = line->first_char; ch; ch = ch->next)
				//{
				//	n = fz_runetochar(utf, ch->c);
				//	for (i = 0; i < n; i++)
				//		fz_write_byte(ctx, out, utf[i]);
				//}
				//fz_write_string(ctx, out, "\n");
			}
			//fz_write_string(ctx, out, "\n");
		}
	}

#ifdef WRITE_AS_PS
	fz_write_printf(ctx, out, "\n\n1 -1 scale 0 -%g translate\n", page->mediabox.y1-page->mediabox.y0);
#endif

	gogo(ctx, out, boxer, 0);
	boxer_destroy(boxer);

#ifdef WRITE_AS_PS
	fz_write_printf(ctx, out, "showpage\n");
#endif
}

/* Text output writer */

enum {
	FZ_FORMAT_TEXT,
	FZ_FORMAT_HTML,
	FZ_FORMAT_XHTML,
	FZ_FORMAT_STEXT_XML,
	FZ_FORMAT_STEXT_JSON,
};

typedef struct
{
	fz_document_writer super;
	int format;
	int number;
	fz_stext_options opts;
	fz_stext_page *page;
	fz_output *out;
} fz_text_writer;

static fz_device *
text_begin_page(fz_context *ctx, fz_document_writer *wri_, fz_rect mediabox)
{
	fz_text_writer *wri = (fz_text_writer*)wri_;
	float s = wri->opts.scale;

	if (wri->page)
	{
		fz_drop_stext_page(ctx, wri->page);
		wri->page = NULL;
	}

	wri->number++;

	wri->page = fz_new_stext_page(ctx, fz_transform_rect(mediabox, fz_scale(s, s)));
	return fz_new_stext_device(ctx, wri->page, &wri->opts);
}

static void
text_end_page(fz_context *ctx, fz_document_writer *wri_, fz_device *dev)
{
	fz_text_writer *wri = (fz_text_writer*)wri_;
	float s = wri->opts.scale;

	fz_scale_stext_page(ctx, wri->page, s);

	fz_try(ctx)
	{
		fz_close_device(ctx, dev);
		switch (wri->format)
		{
		default:
		case FZ_FORMAT_TEXT:
			fz_print_stext_page_as_text(ctx, wri->out, wri->page);
			break;
		case FZ_FORMAT_HTML:
			fz_print_stext_page_as_html(ctx, wri->out, wri->page, wri->number, fz_identity, &wri->opts);
			break;
		case FZ_FORMAT_XHTML:
			fz_print_stext_page_as_xhtml(ctx, wri->out, wri->page, wri->number, fz_identity, &wri->opts);
			break;
		case FZ_FORMAT_STEXT_XML:
			fz_print_stext_page_as_xml(ctx, wri->out, wri->page, wri->number, fz_identity);
			break;
		case FZ_FORMAT_STEXT_JSON:
			if (wri->number > 1)
				fz_write_string(ctx, wri->out, ",");
			fz_print_stext_page_as_json(ctx, wri->out, wri->page, wri->number, fz_identity);
			break;
		}
	}
	fz_always(ctx)
	{
		fz_drop_device(ctx, dev);
		fz_drop_stext_page(ctx, wri->page);
		wri->page = NULL;
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
text_close_writer(fz_context *ctx, fz_document_writer *wri_)
{
	fz_text_writer *wri = (fz_text_writer*)wri_;
	switch (wri->format)
	{
	case FZ_FORMAT_HTML:
		fz_print_stext_trailer_as_html(ctx, wri->out);
		break;
	case FZ_FORMAT_XHTML:
		fz_print_stext_trailer_as_xhtml(ctx, wri->out);
		break;
	case FZ_FORMAT_STEXT_XML:
		fz_write_string(ctx, wri->out, "</document>\n");
		break;
	case FZ_FORMAT_STEXT_JSON:
		fz_write_string(ctx, wri->out, "]\n");
		break;
	}
	fz_close_output(ctx, wri->out);
}

static void
text_drop_writer(fz_context *ctx, fz_document_writer *wri_)
{
	fz_text_writer *wri = (fz_text_writer*)wri_;
	fz_drop_stext_page(ctx, wri->page);
	fz_drop_output(ctx, wri->out);
	fz_drop_stext_options(ctx, &wri->opts);
}

fz_document_writer *
fz_new_text_writer_with_output(fz_context *ctx, const char *format, fz_output *out, const char *options)
{
	fz_text_writer *wri = NULL;

	fz_var(wri);

	fz_try(ctx)
	{
		wri = fz_new_derived_document_writer(ctx, fz_text_writer, text_begin_page, text_end_page, text_close_writer, text_drop_writer);
		fz_parse_stext_options(ctx, &wri->opts, options);

		wri->format = FZ_FORMAT_TEXT;
		if (!strcmp(format, "text"))
			wri->format = FZ_FORMAT_TEXT;
		else if (!strcmp(format, "html"))
			wri->format = FZ_FORMAT_HTML;
		else if (!strcmp(format, "xhtml"))
			wri->format = FZ_FORMAT_XHTML;
		else if (!strcmp(format, "stext"))
			wri->format = FZ_FORMAT_STEXT_XML;
		else if (!strcmp(format, "stext.xml"))
			wri->format = FZ_FORMAT_STEXT_XML;
		else if (!strcmp(format, "stext.json"))
		{
			wri->format = FZ_FORMAT_STEXT_JSON;
			wri->opts.flags |= FZ_STEXT_PRESERVE_SPANS;
		}

		wri->out = out;

		switch (wri->format)
		{
		case FZ_FORMAT_HTML:
			fz_print_stext_header_as_html(ctx, wri->out);
			break;
		case FZ_FORMAT_XHTML:
			fz_print_stext_header_as_xhtml(ctx, wri->out);
			break;
		case FZ_FORMAT_STEXT_XML:
			fz_write_string(ctx, wri->out, "<?xml version=\"1.0\"?>\n");
			fz_write_string(ctx, wri->out, "<document>\n");
			break;
		case FZ_FORMAT_STEXT_JSON:
			fz_write_string(ctx, wri->out, "[");
			break;
		}
	}
	fz_catch(ctx)
	{
		fz_drop_output(ctx, out);
		if (wri)
		{
			fz_drop_stext_options(ctx, &wri->opts);
		}
		fz_free(ctx, wri);
		fz_rethrow(ctx);
	}

	return (fz_document_writer*)wri;
}

fz_document_writer *
fz_new_text_writer(fz_context *ctx, const char *format, const char *path, const char *options)
{
	// make sure we provide a 'sane' output path.
	char dfltpath[64];
	fz_snprintf(dfltpath, sizeof(dfltpath), "out.%s", format ? format : "text");
	if (!path || !*path)
		path = dfltpath;

	fz_output *out = fz_new_output_with_path(ctx, path, 0);
	return fz_new_text_writer_with_output(ctx, format, out, options);
}

#endif
