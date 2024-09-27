// Copyright (C) 2024 Artifex Software, Inc.
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

#include <assert.h>

/* #define DEBUG_WRITE_AS_PS */

static fz_stext_block *
add_grid_block(fz_context *ctx, fz_stext_page *page, fz_stext_block **first_block)
{
	fz_stext_block *block = fz_pool_alloc(ctx, page->pool, sizeof(**first_block));
	memset(block, 0, sizeof(*block));
	block->type = FZ_STEXT_BLOCK_GRID;
	block->bbox = fz_empty_rect; /* Fixes bug 703267. */
	block->next = *first_block;
	if (*first_block)
		(*first_block)->prev = block;
	*first_block = block;
	return block;
}

static void
insert_block_before(fz_stext_block *block, fz_stext_block *before, fz_stext_page *page, fz_stext_struct *dest)
{
	if (before)
	{
		/* We have a block to insert it before, so we know it's not the last. */
		block->next = before;
		block->prev = before->prev;
		if (before->prev)
			before->prev->next = block;
		else if (dest)
			dest->first_block = block;
		else
			page->first_block = block;
		before->prev = block;
	}
	else if (dest)
	{
		/* Will be the last block. */
		block->next = NULL;
		block->prev = dest->last_block;
		if (dest->last_block)
			dest->last_block->next = block;
		if (dest->first_block == NULL)
			dest->first_block = block;
		dest->last_block = block;
	}
	else
	{
		/* Will be the last block. */
		block->next = NULL;
		block->prev = page->last_block;
		if (page->last_block)
			page->last_block->next = block;
		if (page->first_block)
			page->first_block = block;
		page->last_block = block;
	}
}

static fz_stext_struct *
add_struct_block_before(fz_context *ctx, fz_stext_block *before, fz_stext_page *page, fz_stext_struct *parent, fz_structure std, const char *raw)
{
	fz_stext_block *block;
	fz_stext_block *prev = NULL;
	int idx = 0;
	size_t z;
	fz_stext_struct *newstruct;

	if (raw == NULL)
		raw = "";
	z = strlen(raw);

	/* We're going to insert a struct block. We need an idx, so walk the list */
	for (block = parent ? parent->first_block : page->first_block; block != before; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_STRUCT)
		{
			assert(block->u.s.index >= idx);
			idx = block->u.s.index + 1;
		}
	}
	/* So we'll add our block as idx. But all the other struct blocks that follow us need to have
	 * larger values. */

	/* Update all the subsequent structs to have a higher idx */
	if (before)
	{
		int idx2 = idx+1;
		for (block = before->next; block != NULL; block = block->next)
		{
			if (block->type != FZ_STEXT_BLOCK_STRUCT)
				continue;
			if (block->u.s.index > idx2)
				break;
			block->u.s.index = idx2++;
		}
	}

	/* Now make our new struct block and insert it. */
	block = fz_pool_alloc(ctx, page->pool, sizeof(*block));
	block->type = FZ_STEXT_BLOCK_STRUCT;
	block->bbox = fz_empty_rect; /* Fixes bug 703267. */
	insert_block_before(block, before, page, parent);

	block->u.s.down = newstruct = fz_pool_alloc(ctx, page->pool, sizeof(*block->u.s.down) + z);
	block->u.s.index = idx;
	newstruct->parent = parent;
	newstruct->standard = std;
	memcpy(newstruct->raw, raw, z);
	newstruct->raw[z] = 0;
	newstruct->up = block;

	return newstruct;
}

typedef struct
{
	int len;
	int max;
	struct {
		int left;
		float pos;
		int freq;
	} *list;
} div_list;

static void
div_list_push(fz_context *ctx, div_list *div, int left, float pos)
{
	int i;

	/* FIXME: Could be bsearch. */
	for (i = 0; i < div->len; i++)
	{
		if (div->list[i].pos > pos)
			break;
		else if (div->list[i].pos == pos && div->list[i].left == left)
		{
			div->list[i].freq++;
			return;
		}
	}

	if (div->len == div->max)
	{
		int newmax = div->max * 2;
		if (newmax == 0)
			newmax = 32;
		div->list = fz_realloc(ctx, div->list, sizeof(div->list[0]) * newmax);
		div->max = newmax;
	}

	if (i < div->len)
		memmove(&div->list[i+1], &div->list[i], sizeof(div->list[0]) * (div->len - i));
	div->len++;
	div->list[i].left = left;
	div->list[i].pos = pos;
	div->list[i].freq = 1;
}

static fz_stext_grid_positions *
make_table_positions(fz_context *ctx, fz_stext_page *page, div_list *xs, float min, float max)
{
	int wind;
	fz_stext_grid_positions *pos;
	int len = xs->len;
	int i;
	int hi = 0;

	/* Count the number of edges */
	int local_min = 0;
	int edges = 2;

	if (len == 0)
		return NULL;

	assert(xs->list[0].left);
	for (i = 0; i < len; i++)
	{
		if (xs->list[i].left)
		{
			if (local_min)
				edges++;
		}
		else
			local_min = 1;
	}
	assert(!xs->list[i-1].left);

	pos = fz_pool_alloc(ctx, page->pool, sizeof(*pos) + (edges-1) * sizeof(pos->list[0]));
	pos->len = edges;

	/* Copy the edges in */
	wind = 0;
	local_min = 0;
	edges = 1;
	pos->list[0].pos = xs->list[0].pos;
	pos->list[0].min = min;
	pos->list[0].max = pos->list[0].pos;
	pos->list[0].uncertainty = 0;
#ifdef DEBUG_TABLE_HUNT
	printf("|%g ", post->list[0].pos);
#endif
	for (i = 0; i < len; i++)
	{
		if (xs->list[i].left)
		{
			if (local_min)
			{
				pos->list[edges].min = xs->list[i-1].pos;
				pos->list[edges].max = xs->list[i].pos;
				pos->list[edges].pos = (xs->list[i-1].pos + xs->list[i].pos)/2;
				pos->list[edges++].uncertainty = wind;
#ifdef DEBUG_TABLE_HUNT
				if (wind)
					printf("?%g(%d) ", post->list[0].pos, wind);
				else
					printf("|%g ", post->list[0].pos);
#endif
			}
			wind += xs->list[i].freq;
			if (wind > hi)
				hi = wind;
		}
		else
		{
			wind -= xs->list[i].freq;
			local_min = 1;
		}
	}
	assert(wind == 0);
	pos->list[edges].pos = xs->list[i-1].pos;
	pos->list[edges].min = xs->list[i-1].pos;
	pos->list[edges].max = max;
	pos->list[edges].uncertainty = 0;
	pos->max_uncertainty = hi;
#ifdef DEBUG_TABLE_HUNT
	printf("|%g\n", post->list[edges].pos);
#endif

	return pos;
}

static void
sanitize_positions(fz_context *ctx, div_list *xs)
{
	int i, j;

#ifdef DEBUG_TABLE_HUNT
	printf("OK:\n");
	for (i = 0; i < xs->len; i++)
	{
		if (xs->list[i].left)
			printf("[");
		printf("%g(%d)", xs->list[i].pos, xs->list[i].freq);
		if (!xs->list[i].left)
			printf("]");
		printf(" ");
	}
	printf("\n");
#endif

	/* Now, combine runs of left and right */
	for (i = 0; i < xs->len; i++)
	{
		if (xs->list[i].left)
		{
			j = i;
			while (i < xs->len-1 && xs->list[i+1].left)
			{
				i++;
				xs->list[j].freq += xs->list[i].freq;
				xs->list[i].freq = 0;
			}
		}
		else
		{
			while (i < xs->len-1 && !xs->list[i+1].left)
			{
				i++;
				xs->list[i].freq += xs->list[i-1].freq;
				xs->list[i-1].freq = 0;
			}
		}
	}

#ifdef DEBUG_TABLE_HUNT
	printf("Shrunk:\n");
	for (i = 0; i < xs->len; i++)
	{
		if (xs->list[i].left)
			printf("[");
		printf("%g(%d)", xs->list[i].pos, xs->list[i].freq);
		if (!xs->list[i].left)
			printf("]");
		printf(" ");
	}
	printf("\n");
#endif

	/* Now remove the 0 frequency ones. */
	j = 0;
	for (i = 0; i < xs->len; i++)
	{
		if (xs->list[i].freq == 0)
			continue;
		if (i != j)
			xs->list[j] = xs->list[i];
		j++;
	}
	xs->len = j;

#ifdef DEBUG_TABLE_HUNT
	printf("Compacted:\n");
	for (i = 0; i < xs->len; i++)
	{
		if (xs->list[i].left)
			printf("[");
		printf("%g(%d)", xs->list[i].pos, xs->list[i].freq);
		if (!xs->list[i].left)
			printf("]");
		printf(" ");
	}
	printf("\n");
#endif
}

static void
walk_blocks(fz_context *ctx, div_list *xs, div_list *ys, fz_stext_block *first_block, int descend)
{
	fz_stext_block *block;
	fz_stext_line *line;
	fz_stext_char *ch;

	for (block = first_block; block != NULL; block = block->next)
	{
		switch (block->type)
		{
		case FZ_STEXT_BLOCK_STRUCT:
			if (descend && block->u.s.down)
				walk_blocks(ctx, xs, ys, block->u.s.down->first_block, descend);
			break;
		case FZ_STEXT_BLOCK_VECTOR:
			break;
		case FZ_STEXT_BLOCK_TEXT:
			for (line = block->u.t.first_line; line != NULL; line = line->next)
			{
				float rpos;
				int left = 1;
				int right = 0;
				div_list_push(ctx, ys, 1, line->bbox.y0);
				div_list_push(ctx, ys, 0, line->bbox.y1);
				for (ch = line->first_char; ch != NULL; ch = ch->next)
				{
					if (ch->c == ' ' && (ch->flags & FZ_STEXT_SYNTHETIC) != 0)
					{
						if (right)
						{
							div_list_push(ctx, xs, 0, rpos);
							right = 0;
							left = 1;
						}
					}
					else
					{
						if (left)
						{
							float lpos = fz_min(ch->quad.ll.x, ch->quad.ul.x);
							div_list_push(ctx, xs, 1, lpos);
							left = 0;
						}
						rpos = fz_max(ch->quad.lr.x, ch->quad.ur.x);
						right = 1;
					}
				}
				if (right)
					div_list_push(ctx, xs, 0, rpos);
			}
			break;
		}
	}
}

typedef struct
{
	int w;
	int h;
	int pos[1];
} grid_lines;

typedef struct
{
	grid_lines *xlines;
	grid_lines *ylines;
	fz_stext_grid_positions *xpos;
	fz_stext_grid_positions *ypos;
} grid_walker_data;

static void
set_line(grid_lines *lines, int x, int y)
{
	lines->pos[x + y * lines->w]++;
}

static int
get_line(grid_lines *lines, int x, int y)
{
	return lines->pos[x + y * lines->w];
}

static int
find_grid_pos_with_reinforcement(fz_context *ctx, fz_stext_grid_positions *pos, float x)
{
	int i;

	for (i = 0; i < pos->len; i++)
	{
		int r;
		if (x > pos->list[i].max)
			continue;
		if (x < pos->list[i].min)
			return -1;
		r = pos->list[i].reinforcement++;
		pos->list[i].pos = (pos->list[i].pos * r + x) / (r+1);
		return i;
	}

	return -1;
}

static void
add_h_line(fz_context *ctx, grid_walker_data *gd, float x0, float x1, float y)
{
	int start = find_grid_pos_with_reinforcement(ctx, gd->xpos, x0);
	int end = find_grid_pos_with_reinforcement(ctx, gd->xpos, x1);
	int yidx = find_grid_pos_with_reinforcement(ctx, gd->ypos, y);
	int i;

	if (start < 0 || end < 0 || yidx < 0)
		return;

	for (i = start; i < end; i++)
		set_line(gd->xlines, i, yidx);
}

static void
add_v_line(fz_context *ctx, grid_walker_data *gd, float y0, float y1, float x)
{
	int start = find_grid_pos_with_reinforcement(ctx, gd->ypos, y0);
	int end = find_grid_pos_with_reinforcement(ctx, gd->ypos, y1);
	int xidx = find_grid_pos_with_reinforcement(ctx, gd->xpos, x);
	int i;

	if (start < 0 || end < 0 || xidx < 0)
		return;

	for (i = start; i < end; i++)
		set_line(gd->ylines, i, xidx);
}

static void
walk_grid_lines(fz_context *ctx, grid_walker_data *gd, fz_stext_block *block)
{
	for (; block != NULL; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_STRUCT)
		{
			if (block->u.s.down)
				walk_grid_lines(ctx, gd, block->u.s.down->first_block);
			continue;
		}
		else if (block->type == FZ_STEXT_BLOCK_VECTOR)
		{
			fz_rect r = block->bbox;
			float w = r.x1 - r.x0;
			float h = r.y1 - r.y0;
			if (w > h && h < 1)
			{
				/* Thin, wide line */
				add_h_line(ctx, gd, r.x0, r.x1, (r.y0 + r.y1)/2);
			}
			else if (w < h && w < 1)
			{
				/* Thin, wide line */
				add_v_line(ctx, gd, r.y0, r.y1, (r.x0 + r.x1)/2);
			}
			else
			{
				/* Rectangle */
				add_h_line(ctx, gd, r.x0, r.x1, r.y0);
				add_h_line(ctx, gd, r.x0, r.x1, r.y1);
				add_v_line(ctx, gd, r.y0, r.y1, r.x0);
				add_v_line(ctx, gd, r.y0, r.y1, r.x1);
			}
		}
	}
}

static grid_lines *new_grid_lines(fz_context *ctx, int w, int h)
{
	grid_lines *lines = fz_calloc(ctx, 1, sizeof(grid_lines) + sizeof(lines->pos[0]) * (w * h - 1));
	lines->w = w;
	lines->h = h;

	return lines;
}

static void
asciiart_table(fz_context *ctx, grid_walker_data *gd, fz_stext_page *page, fz_stext_block **first_block)
{
	int w = gd->xpos->len;
	int h = gd->ypos->len;
	int x, y;

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w-1; x++)
		{
			printf("+");
			if (get_line(gd->xlines, x, y))
			{
				printf("-");
			}
			else
			{
				printf(" ");
			}
		}
		printf("+\n");
		if (y == h-1)
			break;
		for (x = 0; x < w; x++)
		{
			if (get_line(gd->ylines, y, x))
			{
				printf("|");
			}
			else
			{
				printf(" ");
			}
			if (x < w-1)
				printf(" ");
			else
				printf("\n");
		}
	}
}

static void
recalc_bbox(fz_stext_block *block)
{
	fz_rect bbox = fz_empty_rect;
	fz_stext_line *line;

	for (line = block->u.t.first_line; line != NULL; line = line->next)
		bbox = fz_union_rect(bbox, line->bbox);

	block->bbox = bbox;
}

static void
unlink_line_from_block(fz_stext_line *line, fz_stext_block *block)
{
	fz_stext_line *next_line = line->next;

	if (line->prev)
		line->prev->next = next_line;
	else
		block->u.t.first_line = next_line;
	if (next_line)
		next_line->prev = line->prev;
	else
		block->u.t.last_line = line->prev;
}

static void
append_line_to_block(fz_stext_line *line, fz_stext_block *block)
{
	if (block->u.t.last_line == NULL)
	{
		block->u.t.first_line = block->u.t.last_line = line;
		line->prev = NULL;
	}
	else
	{
		line->prev = block->u.t.last_line;
		block->u.t.last_line->next = line;
		block->u.t.last_line = line;
	}
	line->next = NULL;
}

static void
unlink_block(fz_stext_block *block, fz_stext_block **first, fz_stext_block **last)
{
	if (block->prev)
		block->prev->next = block->next;
	else
		*first = block->next;
	if (block->next)
		block->next->prev = block->prev;
	else
		*last = block->prev;
}

static fz_rect
move_contained_content(fz_context *ctx, fz_stext_page *page, fz_stext_struct *dest, fz_stext_struct *src, fz_rect r)
{
	fz_stext_block *before = dest ? dest->first_block : page->first_block;
	fz_stext_block **sfirst = src ? &src->first_block : &page->first_block;
	fz_stext_block **slast = src ? &src->last_block : &page->last_block;
	fz_stext_block *block, *next;

	for (block = *sfirst; block != NULL; block = next)
	{
		fz_rect bbox = fz_intersect_rect(block->bbox, r);
		next = block->next;
		/* Don't use fz_is_empty_rect here, as that will exclude zero height areas like spaces. */
		if (bbox.x0 > bbox.x1 || bbox.y0 > bbox.y1)
			continue; /* Trivially excluded */
		if (bbox.x0 == block->bbox.x0 && bbox.y0 == block->bbox.y0 && bbox.x1 == block->bbox.x1 && bbox.y1 == block->bbox.y1)
		{
			/* Trivially included */
			unlink_block(block, sfirst, slast);
			insert_block_before(block, before, page, dest);
			before = block->next;
			continue;
		}
		if (block->type == FZ_STEXT_BLOCK_TEXT)
		{
			/* Partially included text block */
			fz_stext_line *line, *next_line;
			fz_stext_block *newblock = NULL;

			for (line = block->u.t.first_line; line != NULL; line = next_line)
			{
				fz_rect lrect = fz_intersect_rect(line->bbox, r);
				next_line = line->next;

				/* Don't use fz_is_empty_rect here, as that will exclude zero height areas like spaces. */
				if (lrect.x0 > lrect.x1 || lrect.y0 > lrect.y1)
					continue; /* Trivial exclusion */
				if (line->bbox.x0 == lrect.x0 && line->bbox.y0 == lrect.y0 && line->bbox.x1 == lrect.x1 && line->bbox.y1 == lrect.y1)
				{
					/* Trivial inclusion */
					if (newblock == NULL)
					{
						newblock = fz_pool_alloc(ctx, page->pool, sizeof(fz_stext_block));
						insert_block_before(newblock, before, page, dest);
						before = block->next;
					}

					unlink_line_from_block(line, block);
					append_line_to_block(line, newblock);
				}
				else
				{
					/* Need to walk the line and just take parts */
					fz_stext_line *newline = NULL;
					fz_stext_char *ch, *next_ch, *prev_ch = NULL;

					for (ch = line->first_char; ch != NULL; ch = next_ch)
					{
						fz_rect crect = fz_rect_from_quad(ch->quad);
						fz_rect intersect = fz_intersect_rect(crect, r);
						next_ch = ch->next;
						/* Don't use fz_is_empty_rect here, as that will exclude zero height areas like spaces. */
						if (intersect.x0 > intersect.x1 || intersect.y0 > intersect.y1)
						{
							prev_ch = ch;
							continue;
						}
						/* Take this char */
						if (newline == NULL)
						{
							newline = fz_pool_alloc(ctx, page->pool, sizeof(*newline));
							newline->dir = line->dir;
							newline->wmode = line->wmode;
							newline->bbox = fz_empty_rect;
						}
						/* Unlink char */
						if (prev_ch == NULL)
							line->first_char = next_ch;
						else
							prev_ch->next = next_ch;
						if (next_ch == NULL)
							line->last_char = prev_ch;
						/* Relink char */
						ch->next = NULL;
						if (newline->last_char == NULL)
							newline->first_char = ch;
						else
							newline->last_char->next = ch;
						newline->last_char = ch;
						newline->bbox = fz_union_rect(newline->bbox, crect);
					}
					if (newline)
					{
						if (newblock == NULL)
						{
							newblock = fz_pool_alloc(ctx, page->pool, sizeof(fz_stext_block));

							/* Add the block onto our target list */
							insert_block_before(newblock, before, page, dest);
							before = newblock->next;
						}
						append_line_to_block(newline, newblock);
					}
				}
			}
			if (newblock)
			{
				recalc_bbox(block);
				recalc_bbox(newblock);
			}
		}
	}

	return r;
}

static fz_stext_block *
find_table_insertion_point(fz_context *ctx, fz_rect r, fz_stext_block *block)
{
	fz_stext_block *after = NULL;

	for (; block != NULL; block = block->next)
	{
		fz_rect s = fz_intersect_rect(r, block->bbox);

		if (s.x0 > s.x1 || s.y0 > s.y1)
			continue;
		after = block;
	}

	/* Convert to before */
	if (after)
		after = after->next;

	return after;
}
static void
transcribe_table(fz_context *ctx, grid_walker_data *gd, fz_stext_page *page, fz_stext_struct *parent)
{
	int w = gd->xpos->len;
	int h = gd->ypos->len;
	int x, y;
	char *sent_tab = fz_calloc(ctx, 1, w*h);
	fz_stext_block **first_block = parent ? &parent->first_block : &page->first_block;
	fz_stext_struct *table, *tr, *td;
	fz_stext_block *before;
	fz_rect r;

	/* Where should we insert the table in the data? */
	r.x0 = gd->xpos->list[0].pos;
	r.x1 = gd->xpos->list[w-1].pos;
	r.y0 = gd->ypos->list[0].pos;
	r.y1 = gd->ypos->list[h-1].pos;
	before = find_table_insertion_point(ctx, r, *first_block);
	
	/* Make table */
	table = add_struct_block_before(ctx, before, page, parent, FZ_STRUCTURE_TABLE, "Table");

	/* Run through the cells, and guess at spanning. */
	for (y = 0; y < h-1; y++)
	{
		/* Make TR */
		tr = add_struct_block_before(ctx, NULL, page, table, FZ_STRUCTURE_TR, "TR");

		for (x = 0; x < w-1; x++)
		{
			int x2, y2;
			int xdivided = 0;
			int ydivided = 0;
			int cellw = 1;
			int cellh = 1;

			/* Have we sent this cell already? */
			if (sent_tab[x+y*w])
				continue;
			/* Do we ever see an X divider here? */
			for (y2 = 0; y2 < h-1; y2++)
			{
				if (get_line(gd->ylines, y2, x+1))
				{
					xdivided = 1;
					break;
				}
			}
			/* So, we want to make a cell starting at (x,y) that's cellw wide and cellh tall. */
			if (xdivided)
			{
				/* Search for a cell divider to find cellw (or a cell we've sent already). */
				for (x2 = x+1; x2 < w-1; x2++)
				{
					if (get_line(gd->ylines, y, x2) || sent_tab[x2+y*w] != 0)
						break;
					cellw++;
				}
			}
			/* Now, can we extend that cell downwards too? */
			/* Do we ever see a Y divider here? */
			for (x2 = 0; x2 < w-1; x2++)
			{
				if (get_line(gd->xlines, y+1, x2))
				{
					ydivided = 1;
					break;
				}
			}
			if (ydivided)
			{
				/* Search for a cell divider to find cellw. */
				for (y2 = y+1; y2 < h-1; y2++)
				{
					for (x2 = x; x2 < x+cellw; x2++)
						if (get_line(gd->xlines, x, y2))
							break;
					if (x2 != x+cellw)
						break;
					cellh++;
				}
			}
			/* Make TD */
			td = add_struct_block_before(ctx, NULL, page, tr, FZ_STRUCTURE_TD, "TD");
			r.x0 = gd->xpos->list[x].pos;
			r.x1 = gd->xpos->list[x+cellw].pos;
			r.y0 = gd->ypos->list[y].pos;
			r.y1 = gd->ypos->list[y+cellh].pos;
			/* Use r, not REAL contents bbox, as otherwise spanned rows
			 * can end up empty. */
			td->up->bbox = r;
			move_contained_content(ctx, page, td, parent, r);
			printf("(%d,%d) + (%d,%d)\n", x, y, cellw, cellh);
			for (y2 = y; y2 < y+cellh; y2++)
				for (x2 = x; x2 < x+cellw; x2++)
					sent_tab[x2+y2*w] = 1;
		}
		r.x0 = gd->xpos->list[0].pos;
		r.x1 = gd->xpos->list[gd->xpos->len-1].pos;
		r.y0 = gd->ypos->list[y].pos;
		r.y1 = gd->ypos->list[y+1].pos;
		tr->up->bbox = r;
		table->up->bbox = fz_union_rect(table->up->bbox, tr->up->bbox);
	}
	fz_free(ctx, sent_tab);
}

static void
check_for_grid_lines(fz_context *ctx, fz_stext_grid_positions *xps, fz_stext_grid_positions *yps, fz_stext_page *page, fz_stext_struct *parent)
{
	fz_stext_block **first_blockp = parent ? &parent->first_block : &page->first_block;
	grid_walker_data gd = { 0 };

	gd.xpos = xps;
	gd.ypos = yps;

	fz_var(gd);

	fz_try(ctx)
	{
		gd.xlines = new_grid_lines(ctx, xps->len-1, yps->len);
		gd.ylines = new_grid_lines(ctx, yps->len-1, xps->len);

		walk_grid_lines(ctx, &gd, *first_blockp);

		/* Now we should have the entire table calculated. */
		asciiart_table(ctx, &gd, page, first_blockp);
		transcribe_table(ctx, &gd, page, parent);
	}
	fz_always(ctx)
	{
		fz_free(ctx, gd.xlines);
		fz_free(ctx, gd.ylines);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static fz_rect
bbox_of_blocks(fz_stext_block *block)
{
	fz_rect r = fz_empty_rect;

	while (block)
	{
		r = fz_union_rect(r, block->bbox);
		block = block->next;
	}

	return r;
}

static void
do_table_hunt(fz_context *ctx, fz_stext_page *page, fz_stext_struct *parent)
{
	div_list xs = { 0 };
	div_list ys = { 0 };
	fz_stext_block *block;
	int count;
	fz_stext_block **first_block = parent ? &parent->first_block : &page->first_block;

	/* No content? Just bale. */
	if (*first_block == NULL)
		return;

	/* First off, descend into any children to see if those look like tables. */
	count = 0;
	for (block = *first_block; block != NULL; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_STRUCT)
		{
			if (block->u.s.down)
			{
				do_table_hunt(ctx, page, block->u.s.down);
				count++;
			}
		}
		else if (block->type == FZ_STEXT_BLOCK_TEXT)
			count++;
	}

	/* If all we have is a single child, no more to hunt. */
	if (count <= 1)
		return;

	fz_try(ctx)
	{
		/* Now see whether the content looks like tables.
		 * Currently, we pass descend == 0, which means we only consider content at
		 * this level. If we pass 1, then we'll consider all the content at this
		 * level, plus the children. This might allow for where we have oversegmented,
		 * but really needs us to fixup the content. */
		walk_blocks(ctx, &xs, &ys, *first_block, 0);

		sanitize_positions(ctx, &xs);
		sanitize_positions(ctx, &ys);

		/* Run across the line, counting 'winding' */
		if (xs.len > 2 && ys.len > 2)
		{
			fz_rect rect = bbox_of_blocks(*first_block);
			fz_stext_grid_positions *xps = make_table_positions(ctx, page, &xs, rect.x0, rect.x1);
			fz_stext_grid_positions *yps = make_table_positions(ctx, page, &ys, rect.y0, rect.y1);
			check_for_grid_lines(ctx, xps, yps, page, parent);

			if (0)
			{
				fz_stext_block *block;
				block = add_grid_block(ctx, page, first_block);
				block->u.b.xs = xps;
				block->u.b.ys = yps;
				block->bbox.x0 = block->u.b.xs->list[0].pos;
				block->bbox.y0 = block->u.b.ys->list[0].pos;
				block->bbox.x1 = block->u.b.xs->list[block->u.b.xs->len-1].pos;
				block->bbox.y1 = block->u.b.ys->list[block->u.b.ys->len-1].pos;
			}
#ifdef DEBUG_WRITE_AS_PS
			{
				int i;
				printf("%% TABLE\n");
				for (i = 0; i < block->u.b.xs->len; i++)
				{
					if (block->u.b.xs->list[i].uncertainty)
						printf("0 1 0 setrgbcolor\n");
					else
						printf("0 0.5 0 setrgbcolor\n");
					printf("%g %g moveto %g %g lineto stroke\n",
						block->u.b.xs->list[i].pos, block->bbox.y0,
						block->u.b.xs->list[i].pos, block->bbox.y1);
				}
				for (i = 0; i < block->u.b.ys->len; i++)
				{
					if (block->u.b.ys->list[i].uncertainty)
						printf("0 1 0 setrgbcolor\n");
					else
						printf("0 0.5 0 setrgbcolor\n");
					printf("%g %g moveto %g %g lineto stroke\n",
						block->bbox.x0, block->u.b.ys->list[i].pos,
						block->bbox.x1, block->u.b.ys->list[i].pos);
				}
			}
#endif
		}
	}
	fz_always(ctx)
	{
		fz_free(ctx, xs.list);
		fz_free(ctx, ys.list);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

void
fz_table_hunt(fz_context *ctx, fz_stext_page *page)
{
	if (page == NULL)
		return;

	do_table_hunt(ctx, page, NULL);
}
