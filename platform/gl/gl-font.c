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
 * A very simple font cache and rasterizer that uses FreeType
 * to draw fonts from a single OpenGL texture. The code uses
 * a linear-probe hashtable, and writes new glyphs into
 * the texture using glTexSubImage2D. When the texture fills
 * up, or the hash table gets too crowded, the cache is emptied.
 *
 * This is designed to be used for horizontal text only,
 * and draws unhinted text with subpixel accurate metrics
 * and kerning. As such, you should always call the drawing
 * function with an orthogonal transform that maps units
 * to pixels accurately.
 */

#include "gl-app.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PADDING 1		/* set to 0 to save some space but disallow arbitrary transforms */

#define MAXGLYPHS 4093	/* prime number for hash table goodness */
#define CACHESIZE 1024
#define XPRECISION 4
#define YPRECISION 1

struct key
{
	fz_font *font;
	float size;
	short gid;
	unsigned char subx;
	unsigned char suby;
};

struct glyph
{
	char lsb, top, w, h;
	short s, t;
};

struct table
{
	struct key key;
	struct glyph glyph;
};

static struct table g_table[MAXGLYPHS];
static int g_table_load = 0;
static unsigned int g_cache_tex = 0;
static int g_cache_w = CACHESIZE;
static int g_cache_h = CACHESIZE;
static int g_cache_row_y = 0;
static int g_cache_row_x = 0;
static int g_cache_row_h = 0;

static fz_font *g_font = NULL;

static void clear_font_cache(void)
{
#if PADDING > 0
	unsigned char *zero = malloc((size_t)g_cache_w * g_cache_h);
	memset(zero, 0, (size_t)g_cache_w * g_cache_h);
	glBindTexture(GL_TEXTURE_2D, g_cache_tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, g_cache_w, g_cache_h, GL_ALPHA, GL_UNSIGNED_BYTE, zero);
	free(zero);
#endif

	memset(g_table, 0, sizeof(g_table));
	g_table_load = 0;

	g_cache_row_y = PADDING;
	g_cache_row_x = PADDING;
	g_cache_row_h = 0;
}

void ui_init_fonts(void)
{
	const unsigned char *data;
	int size;

	glGenTextures(1, &g_cache_tex);
	glBindTexture(GL_TEXTURE_2D, g_cache_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g_cache_w, g_cache_h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);

	clear_font_cache();

	data = fz_lookup_builtin_font(ctx, "Charis SIL", 0, 0, &size);
	if (!data)
		data = fz_lookup_builtin_font(ctx, "Times", 0, 0, &size);
	g_font = fz_new_font_from_memory(ctx, NULL, data, size, 0, 0);
}

void ui_finish_fonts(void)
{
	clear_font_cache();
	fz_drop_font(ctx, g_font);
}

static unsigned int hashfunc(struct key *key)
{
	unsigned char *buf = (unsigned char *)key;
	unsigned int len = sizeof(struct key);
	unsigned int h = 0;
	while (len--)
		h = *buf++ + (h << 6) + (h << 16) - h;
	return h;
}

static unsigned int lookup_table(struct key *key)
{
	unsigned int pos = hashfunc(key) % MAXGLYPHS;
	while (1)
	{
		if (!g_table[pos].key.font) /* empty slot */
			return pos;
		if (!memcmp(key, &g_table[pos].key, sizeof(struct key))) /* matching slot */
			return pos;
		pos = (pos + 1) % MAXGLYPHS;
	}
}

static struct glyph *lookup_glyph(fz_font *font, float size, int gid, float *xp, float *yp)
{
	fz_matrix trm, subpix_trm;
	unsigned char subx, suby;
	fz_pixmap *pixmap;
	struct key key;
	unsigned int pos;
	int w, h;

	/* match fitz's glyph cache quantization */
	trm = fz_scale(size, -size);
	trm.e = *xp;
	trm.f = *yp;
	fz_subpixel_adjust(ctx, &trm, &subpix_trm, &subx, &suby);
	*xp = trm.e;
	*yp = trm.f;

	/*
	 * Look it up in the table
	 */

	memset(&key, 0, sizeof key);
	key.font = font;
	key.size = size;
	key.gid = gid;
	key.subx = subx;
	key.suby = suby;

	pos = lookup_table(&key);
	if (g_table[pos].key.font)
		return &g_table[pos].glyph;

	/*
	 * Render the bitmap
	 */

	glEnd();

	pixmap = fz_render_glyph_pixmap(ctx, font, gid, &subpix_trm, NULL, 8);
	w = pixmap->w;
	h = pixmap->h;

	/*
	 * Find an empty slot in the texture
	 */

	if (g_table_load == (MAXGLYPHS * 3) / 4)
	{
		puts("font cache table full, clearing cache");
		clear_font_cache();
		pos = lookup_table(&key);
	}

	if (h + PADDING > g_cache_h || w + PADDING > g_cache_w)
		return NULL;

	if (g_cache_row_x + w + PADDING > g_cache_w)
	{
		g_cache_row_y += g_cache_row_h + PADDING;
		g_cache_row_x = PADDING;
		g_cache_row_h = 0;
	}
	if (g_cache_row_y + h + PADDING > g_cache_h)
	{
		puts("font cache texture full, clearing cache");
		clear_font_cache();
		pos = lookup_table(&key);
	}

	/*
	 * Copy bitmap into texture
	 */

	memcpy(&g_table[pos].key, &key, sizeof(struct key));
	g_table[pos].glyph.w = pixmap->w;
	g_table[pos].glyph.h = pixmap->h;
	g_table[pos].glyph.lsb = pixmap->x;
	g_table[pos].glyph.top = -pixmap->y;
	g_table[pos].glyph.s = g_cache_row_x;
	g_table[pos].glyph.t = g_cache_row_y;
	g_table_load ++;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, pixmap->w);
	glTexSubImage2D(GL_TEXTURE_2D, 0, g_cache_row_x, g_cache_row_y, w, h,
			GL_ALPHA, GL_UNSIGNED_BYTE, pixmap->samples);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	fz_drop_pixmap(ctx, pixmap);

	glBegin(GL_QUADS);

	g_cache_row_x += w + PADDING;
	if (g_cache_row_h < h + PADDING)
		g_cache_row_h = h + PADDING;

	return &g_table[pos].glyph;
}

static float ui_draw_glyph(fz_font *font, float size, int gid, float x, float y)
{
	struct glyph *glyph;
	float s0, t0, s1, t1, xc, yc;
	fz_irect r;

	glyph = lookup_glyph(font, size, gid, &x, &y);
	if (!glyph)
		return 0;

	s0 = (float) glyph->s / g_cache_w;
	t0 = (float) glyph->t / g_cache_h;
	s1 = (float) (glyph->s + glyph->w) / g_cache_w;
	t1 = (float) (glyph->t + glyph->h) / g_cache_h;
	xc = floorf(x) + glyph->lsb;
	yc = floorf(y) - glyph->top + glyph->h;

	r.x0 = xc;
	r.y0 = yc - glyph->h;
	r.x1 = xc + glyph->w;
	r.y1 = yc;

	if (r.x0 > 0 && r.y0 > 0 && r.y0 <= r.y1 && r.x0 <= r.x1)
	{
		glTexCoord2f(s0, t0); glVertex2f(r.x0, r.y0);
		glTexCoord2f(s1, t0); glVertex2f(r.x1, r.y0);
		glTexCoord2f(s1, t1); glVertex2f(r.x1, r.y1);
		glTexCoord2f(s0, t1); glVertex2f(r.x0, r.y1);
	}

	return fz_advance_glyph(ctx, font, gid, 0) * size;
}

float ui_measure_character(int c)
{
	fz_font *font;
	int gid = fz_encode_character_with_fallback(ctx, g_font, c, 0, 0, &font);
	return fz_advance_glyph(ctx, font, gid, 0) * ui.fontsize;
}

static float ui_draw_character_imp(float x, float y, int c)
{
	fz_font *font;
	int gid = fz_encode_character_with_fallback(ctx, g_font, c, 0, 0, &font);
	return ui_draw_glyph(font, ui.fontsize, gid, x, y);
}

static void ui_begin_text(void)
{
	glBindTexture(GL_TEXTURE_2D, g_cache_tex);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
}

static void ui_end_text(void)
{
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void ui_draw_string(float x, float y, const char *str)
{
	int c;
	ui_begin_text();
	while (*str)
	{
		str += fz_chartorune_unsafe(&c, str);
		x += ui_draw_character_imp(x, y + ui.baseline, c);
	}
	ui_end_text();
}

void ui_draw_string_part(float x, float y, const char *s, const char *e)
{
	int c;
	ui_begin_text();
	while (s < e)
	{
		s += fz_chartorune_unsafe(&c, s);
		x += ui_draw_character_imp(x, y + ui.baseline, c);
	}
	ui_end_text();
}

void ui_draw_character(float x, float y, int c)
{
	ui_begin_text();
	ui_draw_character_imp(x, y + ui.baseline, c);
	ui_end_text();
}

float ui_measure_string(const char *str)
{
	int c;
	float x = 0;
	while (*str)
	{
		str += fz_chartorune_unsafe(&c, str);
		x += ui_measure_character(c);
	}
	return x;
}

float ui_measure_string_part(const char *s, const char *e)
{
	int c;
	float w = 0;
	while (s < e)
	{
		s += fz_chartorune_unsafe(&c, s);
		w += ui_measure_character(c);
	}
	return w;
}

int ui_break_lines(char *a, struct line *lines, int maxlines, int width, int *maxwidth)
{
	char *next, *space = NULL, *b = a;
	int c, n = 0;
	float space_x, x = 0, w = 0;

	if (maxwidth)
		*maxwidth = 0;

	while (*b)
	{
		next = b + fz_chartorune_unsafe(&c, b);
		if (c == '\r' || c == '\n')
		{
			if (lines && n < maxlines)
			{
				lines[n].a = a;
				lines[n].b = b;
			}
			++n;
			if (maxwidth && *maxwidth < x)
				*maxwidth = x;
			a = next;
			x = 0;
			space = NULL;
		}
		else
		{
			if (c == ' ' && maxlines > 1)
			{
				space = b;
				space_x = x;
			}

			w = ui_measure_character(c);
			if (x + w > width)
			{
				if (space)
				{
					if (lines && n < maxlines)
					{
						lines[n].a = a;
						lines[n].b = space;
					}
					++n;
					if (maxwidth && *maxwidth < space_x)
						*maxwidth = space_x;
					a = next = space + 1;
					x = 0;
					space = NULL;
				}
				else
				{
					if (lines && n < maxlines)
					{
						lines[n].a = a;
						lines[n].b = b;
					}
					++n;
					if (maxwidth && *maxwidth < x)
						*maxwidth = x;
					a = b;
					x = w;
					space = NULL;
				}
			}
			else
			{
				x += w;
			}
		}
		b = next;
	}

	if (lines && n < maxlines)
	{
		lines[n].a = a;
		lines[n].b = b;
	}
	++n;
	if (maxwidth && *maxwidth < x)
		*maxwidth = x;
	return n < maxlines ? n : maxlines;
}

void ui_draw_lines(float x, float y, struct line *lines, int n)
{
	int i;
	for (i = 0; i < n; ++i)
	{
		ui_draw_string_part(x, y, lines[i].a, lines[i].b);
		y += ui.lineheight;
	}
}
