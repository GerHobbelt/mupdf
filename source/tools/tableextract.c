#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <string.h>
#include <memory.h>

#define VERBOSE

typedef struct
{
	float min;
	float pos;
	float max;
	int refined;
} divider;

typedef struct
{
	int n;
	divider *div;
} dividers;

typedef struct
{
	int full;
	int x_crossed;
	int y_crossed;
	int rowspan;
	int colspan;
	int allocated;
	int col_idx;
	fz_rect content_rect;
	fz_buffer *buf;
} cell;

#define MAX_CELL_DIM 1024

typedef struct
{
	int max_w;
	int max_h;
	int w;
	int h;
	cell *c;
} cells;

typedef struct
{
	pdf_obj *obj;
	int x;
	int y;
} td_obj;

typedef struct
{
	fz_buffer *buf;
	fz_rect r;
	int page;
	int overlap;
	int broken;
	int out_of_order;
	int has_attrs;
	dividers xs;
	dividers ys;
	cells cs;
	fz_stext_block *top_block;
	int num_tds;
	td_obj *tds;
} table;

typedef struct
{
	int page;
	fz_rect bbox;
	dividers xs;
	dividers ys;
	cells cs;
	fz_output *out;
	int num_tds;
	td_obj tds[MAX_CELL_DIM * MAX_CELL_DIM];
	int use_attrs;
	int x;
	int y;
	int num_tables;
	table tables[256];
	fz_flotilla *flotilla;
	fz_stext_block *stext;
} walk_data;

static cell *
get_cell(fz_context *ctx, cells *cs, int x, int y)
{
	if (x >= cs->max_w || y >= cs->max_h)
	{
		int old_max_w = cs->max_w;
		int old_max_h = cs->max_h;
		int new_max_w = cs->max_w;
		int new_max_h = cs->max_h;
		int xx, yy;
		cell *c;
		if (new_max_w == 0)
			new_max_w = 8;
		if (new_max_h == 0)
			new_max_h = 8;
		while (x >= new_max_w)
			new_max_w *= 2;
		while (y >= new_max_h)
			new_max_h *= 2;

		c = fz_realloc(ctx, cs->c, sizeof(*c) * new_max_w * new_max_h);

		for (yy = old_max_h-1; yy >= 0; yy--)
			for (xx = old_max_w-1; xx >= 0; xx--)
				c[yy*new_max_w + xx] = c[yy*old_max_w + xx];
		for (yy = old_max_h; yy < new_max_h; yy++)
			for (xx = 0; xx < new_max_w; xx++)
			{
				memset(&c[yy*new_max_w + xx], 0, sizeof(*c));
				c[yy*new_max_w + xx].rowspan = 1;
				c[yy*new_max_w + xx].colspan = 1;
			}
		for (yy = 0; yy < old_max_h; yy++)
			for (xx = old_max_w; xx < new_max_w; xx++)
			{
				memset(&c[yy*new_max_w + xx], 0, sizeof(*c));
				c[yy*new_max_w + xx].rowspan = 1;
				c[yy*new_max_w + xx].colspan = 1;
			}

		cs->c = c;
		cs->max_w = new_max_w;
		cs->max_h = new_max_h;
	}

	return &cs->c[x + y*cs->max_w];
}

static void
scribble_box(fz_context *ctx, fz_pixmap *pix, fz_rect bbox, uint32_t col, float alpha, int stroke)
{
	float cols[3] = { ((col>>16) & 0xFF)/255.0f, ((col>>8) & 0xFF)/255.0f, ((col) & 0xFF)/255.0f };
	fz_device *dev = NULL;
	fz_path *path = NULL;

	fz_var(dev);
	fz_var(path);

	fz_try(ctx)
	{
		dev = fz_new_draw_device(ctx, fz_identity, pix);
		path = fz_new_path(ctx);
		fz_rectto(ctx, path, bbox.x0, bbox.y0, bbox.x1, bbox.y1);
		if (stroke)
			fz_stroke_path(ctx, dev, path, &fz_default_stroke_state, fz_identity, fz_device_rgb(ctx), cols, alpha, fz_default_color_params);
		else
			fz_fill_path(ctx, dev, path, 0, fz_identity, fz_device_rgb(ctx), cols, alpha, fz_default_color_params);
		fz_close_device(ctx, dev);
	}
	fz_always(ctx)
	{
		fz_drop_device(ctx, dev);
		fz_drop_path(ctx, path);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
output_divs(fz_context *ctx, fz_pixmap *pix, table *table)
{
	dividers *ds = &table->xs;
	int i, n = ds->n;
	fz_rect bbox;

	for (i = 0; i < n; i++)
	{
		bbox.x0 = ds->div[i].min;
		bbox.x1 = ds->div[i].max;
		bbox.y0 = table->ys.div[0].min;
		bbox.y1 = table->ys.div[table->ys.n-1].max;
		scribble_box(ctx, pix, bbox, 0x00ff00, 0.2f, 0);
		bbox.x0 = ds->div[i].pos;
		bbox.x1 = ds->div[i].pos;
		scribble_box(ctx, pix, bbox, 0x00ff00, 1, 1);
	}

	ds = &table->ys;
	n = ds->n;
	for (i = 0; i < n; i++)
	{
		bbox.y0 = ds->div[i].min;
		bbox.y1 = ds->div[i].max;
		bbox.x0 = table->xs.div[0].min;
		bbox.x1 = table->xs.div[table->xs.n-1].max;
		scribble_box(ctx, pix, bbox, 0x00ff00, 0.2f, 0);
		bbox.y0 = ds->div[i].pos;
		bbox.y1 = ds->div[i].pos;
		scribble_box(ctx, pix, bbox, 0x00ff00, 1, 1);
	}
}

static void
add_cell(fz_context *ctx, walk_data *wd, fz_rect r)
{
	int i, x, y;
	int w = wd->xs.n;
	int h = wd->ys.n;
	int left, right, top, bottom;

	if (w == 0 || h == 0)
		return;

	for (i = 0; i < w; i++)
		if (wd->xs.div[i].min > r.x0)
			break;
	left = i > 0 ? i-1 : i;

	for (; i < w; i++)
		if (r.x1 <= wd->xs.div[i].max)
			break;
	right = i < w ? i-1 : w-1;

	for (i = 0; i < h; i++)
		if (wd->ys.div[i].min > r.y0)
			break;
	top = i > 0 ? i-1 : i;

	for (; i < h; i++)
		if (r.y1 <= wd->ys.div[i].max)
			break;
	bottom = i < h ? i-1 : h-1;

	//printf("x %g->%g = %d->%d\n", r.x0, r.x1, left, right);
	//printf("y %g->%g = %d->%d\n", r.y0, r.y1, top, bottom);

	for (y = top; y <= bottom; y++)
	{
		for (x = left; x <= right; x++)
		{
			cell *c = get_cell(ctx, &wd->cs, x, y);
			c->full = 1;
			if (x != left)
				c->x_crossed = 1;
			if (y != top)
				c->y_crossed = 1;
			if (x == left && y == top)
				c->content_rect = r;
			else
				c->content_rect = fz_empty_rect;
		}
	}
}

static void
walk_for_cells2(fz_context *ctx, walk_data *wd, fz_stext_block *block)
{
	for (; block != NULL; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_STRUCT && block->u.s.down != NULL)
		{
			if (block->u.s.down->standard == FZ_STRUCTURE_TH ||
				block->u.s.down->standard == FZ_STRUCTURE_TD)
			{
				add_cell(ctx, wd, block->bbox);
			}
			else
				walk_for_cells2(ctx, wd, block->u.s.down->first_block);
		}
	}
}

static void
asciiart_table(fz_context *ctx, fz_output *out, cells *cs)
{
#ifdef VERBOSE
	int w = cs->w;
	int h = cs->h;
	int x, y;

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w-1; x++)
		{
			cell *c = get_cell(ctx, cs, x, y);
			fz_write_printf(ctx, out, "+");
			if (c->y_crossed)
				fz_write_printf(ctx, out, "v");
			else
				fz_write_printf(ctx, out, "-");
		}
		fz_write_printf(ctx, out, "+\n");
		if (y < h-1)
		{
			for (x = 0; x < w-1; x++)
			{
				cell *c = get_cell(ctx, cs, x, y);
				if (c->x_crossed)
					fz_write_printf(ctx, out, ">");
				else
					fz_write_printf(ctx, out, "|");
				if (c->full)
					fz_write_printf(ctx, out, "#");
				else if (c->allocated)
					fz_write_printf(ctx, out, ".");
				else
					fz_write_printf(ctx, out, " ");
			}
			fz_write_printf(ctx, out, "|\n");
		}
	}
#endif
}

#if 0
static void
make_cells(walk_data *wd)
{
	memset(&wd->cs, 0, sizeof(wd->cs));
}

static void
crush_cells(fz_context *ctx, walk_data *wd)
{
	int w = wd->cs.w;
	int h = wd->cs.h;
	int x, y;

	/* If all the cells in column [i] are either crossed, or
	 * are empty, or have the cell to their left being empty
	 * we can merge columns i-1 and i. Merge them if at
	 * least one cell was crossed. */
	for (x = 1; x < w-1; x++)
	{
		int was_crossed = 0;
		for (y = 0; y < h; y++)
		{
			cell *c = get_cell(ctx, &wd->cs, x, y);
			if (c->x_crossed)
			{
				was_crossed = 1;
				continue;
			}
			if (c[-1].full && c[0].full)
				break;
		}
		if (y == h && was_crossed)
		{
			/* Remove column x.*/
#ifdef VERBOSE
			fz_write_printf(ctx, wd->out, "Merging columns %d and %d\n", x-1, x);
#endif
			for (y = 0; y < h; y++)
			{
				cell *from = get_cell(ctx, &wd->cs, 0, y);
				cell *to = get_cell(ctx, &wd->cs, 0, y-1);
				memmove(to, from, sizeof(*to) * wd->cs.max_w);
				to[x-1].full |= to[x].full;
				to[x-1].y_crossed |= to[x].y_crossed;
				if (x < w-1)
					memmove(&to[x], &to[x+1], sizeof(*to) * (w-x-1));
			}
			memmove(&wd->xs.div[x], &wd->xs.div[x+1], sizeof(wd->xs.div[x]) * (w-x-1));
			wd->xs.n--;
			x--;
			wd->cs.w--;
			w--;
			asciiart_table(ctx, wd->out, &wd->cs);
		}
	}

	/* If all the cells in row [i] are either crossed, or
	 * are empty, or have the cell above them being empty
	 * we can merge rows i-1 and i. Merge them if at
	 * least one cell was crossed. */
	for (y = 1; y < h-1; y++)
	{
		int was_crossed = 0;
		for (x = 0; x < w; x++)
		{
			cell *c = get_cell(ctx, &wd->cs, x, y);
			if (c->y_crossed)
			{
				was_crossed = 1;
				continue;
			}
			if (c[-MAX_CELL_DIM].full && c[0].full)
				break;
		}
		if (x == w && was_crossed)
		{
			/* Remove row y.*/
#ifdef VERBOSE
			fz_write_printf(ctx, wd->out, "Merging rows %d and %d\n", y-1, y);
#endif
			for (x = 0; x < w; x++)
			{
				cell *c = get_cell(ctx, &wd->cs, x, y);
				c[-MAX_CELL_DIM].full |= c->full;
				c[-MAX_CELL_DIM].x_crossed |= c->x_crossed;
			}
			if (y < h-1)
			{
				cell *from = get_cell(ctx, &wd->cs, 0, y+1);
				cell *to = get_cell(ctx, &wd->cs, 0, y+1);
				memmove(to, from, sizeof(*to) * wd->cs.max_w * (h - y - 1));
			}
			memmove(&wd->ys.div[y], &wd->ys.div[y+1], sizeof(wd->ys.div[y]) * (h-y-1));
			wd->ys.n--;
			y--;
			wd->cs.h--;
			h--;
			asciiart_table(ctx, wd->out, &wd->cs);
		}
	}
}
#endif

static void
get_spans_from_attr(fz_context *ctx, pdf_obj *a, int *spans)
{
	pdf_obj *o = pdf_dict_get(ctx, a, PDF_NAME(O));

	if (o != PDF_NAME(Table))
		return;

	o = pdf_dict_get(ctx, a, PDF_NAME(RowSpan));
	if (o)
		spans[0] = pdf_to_int(ctx, o);
	o = pdf_dict_get(ctx, a, PDF_NAME(ColSpan));
	if (o)
		spans[1] = pdf_to_int(ctx, o);
}

static void
get_spans(fz_context *ctx, pdf_obj *k, int *spans)
{
	pdf_obj *p = pdf_dict_get(ctx, k, PDF_NAME(P));
	pdf_obj *a;

	if (p)
	{
		get_spans(ctx, p, spans);
	}

	a = pdf_dict_get(ctx, k, PDF_NAME(A));
	if (pdf_is_array(ctx, a))
	{
		int i, n = pdf_array_len(ctx, a);
		for (i = 0; i < n; i++)
			get_spans_from_attr(ctx, pdf_array_get(ctx, a, i), spans);
	}
	else
		get_spans_from_attr(ctx, a, spans);
}

static void
walk_td_elems(fz_context *ctx, walk_data *wd, pdf_obj *selem)
{
	pdf_obj *s;
	int spans[2] = { 0 };
	int x, y;
	cell *c;

	if (!pdf_is_dict(ctx, selem))
		return;

	s = pdf_dict_get(ctx, selem, PDF_NAME(S));
	if (s == PDF_NAME(Table))
	{
		fz_warn(ctx, "Nested tables!");
		return;
	}
	if (s != PDF_NAME(TD) && s != PDF_NAME(TH))
	{
		pdf_obj *k = pdf_dict_get(ctx, selem, PDF_NAME(K));
		if (pdf_is_array(ctx, k))
		{
			int i, n = pdf_array_len(ctx, k);
			for (i = 0; i < n; i++)
				walk_td_elems(ctx, wd, pdf_array_get(ctx, k, i));
		}
		else
			walk_td_elems(ctx, wd, k);
		return;
	}

	/* Move to the next cell. */
	wd->x++;
	//printf("TD %d\n", wd->x);
	/* Skip over allocated cells (ones that would be filled by previous rowspan/colspaned cells) */
	while (1)
	{
		c = get_cell(ctx, &wd->cs, wd->x, wd->y);
		if (c->allocated == 0)
			break;
		wd->x++;
		//printf("allocated -> %d\n", wd->x);
	}

	spans[0] = 1;
	spans[1] = 1;
	get_spans(ctx, selem, spans);
	assert(spans[0] > 0 && spans[1] > 0);
	//if (spans[0] != 1 || spans[1] != 1)
	//	printf("cs=%d rs=%d\n", spans[1], spans[0]);
	for (y = 0; y < spans[0]; y++)
		for (x = 0; x < spans[1]; x++)
		{
			cell *d = get_cell(ctx, &wd->cs, wd->x + x, wd->y + y);
			if (d->allocated == 1)
				fz_warn(ctx, "Bad rowspan/colspan setup");
			d->allocated = 1;
			if (x > 0)
			{
				d->x_crossed = 1;
				d->colspan = -1;
			}
			if (y > 0)
			{
				d->y_crossed = 1;
				d->rowspan = -1;
			}
		}
	c = get_cell(ctx, &wd->cs, wd->x, wd->y); // May have moved
	c->rowspan = spans[0];
	c->colspan = spans[1];

	wd->tds[wd->num_tds].obj = selem;
	wd->tds[wd->num_tds].x = wd->x;
	wd->tds[wd->num_tds].y = wd->y;
	wd->num_tds++;

	wd->x += spans[1]-1;

	if (wd->cs.w < wd->x+1)
		wd->cs.w = wd->x+1;
	if (wd->cs.h < wd->y+spans[0])
		wd->cs.h = wd->y+spans[0];
}

static void
walk_tr_elems(fz_context *ctx, walk_data *wd, pdf_obj *selem)
{
	pdf_obj *s = pdf_dict_get(ctx, selem, PDF_NAME(S));
	pdf_obj *k;

	if (s == PDF_NAME(Table))
	{
		fz_warn(ctx, "Nested tables!");
		return;
	}
	if (s == NULL)
	{
		fz_warn(ctx, "Walked off struct tree!");
		return;
	}

	k = pdf_dict_get(ctx, selem, PDF_NAME(K));
	if (s != PDF_NAME(TR))
	{
		if (pdf_is_array(ctx, k))
		{
			int i, n = pdf_array_len(ctx, k);
			for (i = 0; i < n; i++)
				walk_tr_elems(ctx, wd, pdf_array_get(ctx, k, i));
		}
		else
			walk_tr_elems(ctx, wd, k);
		return;
	}

	/* Move to the next line. */
	wd->x = -1;
	wd->y++;
	//printf("TR %d\n", wd->y);

	k = pdf_dict_get(ctx, selem, PDF_NAME(K));
	if (pdf_is_array(ctx, k))
	{
		int i, n = pdf_array_len(ctx, k);
		for (i = 0; i < n; i++)
			walk_td_elems(ctx, wd, pdf_array_get(ctx, k, i));
	}
	else
		walk_td_elems(ctx, wd, k);
}

static void
walk_table_elems(fz_context *ctx, walk_data *wd, pdf_obj *selem)
{
	pdf_obj *k = pdf_dict_get(ctx, selem, PDF_NAME(K));

	if (pdf_is_array(ctx, k))
	{
		int i, n = pdf_array_len(ctx, k);
		for (i = 0; i < n; i++)
		{
			walk_tr_elems(ctx, wd, pdf_array_get(ctx, k, i));
		}
	}
	else
	{
		walk_tr_elems(ctx, wd, k);
	}
}

static void
collect_content(fz_context *ctx, fz_buffer **bufp, fz_stext_block *block)
{
	fz_buffer *buf;
	fz_stext_line *line;
	fz_stext_char *ch;

	for (; block; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_STRUCT)
		{
			if (block->u.s.down)
				collect_content(ctx, bufp, block->u.s.down->first_block);
			continue;
		}
		if (block->type != FZ_STEXT_BLOCK_TEXT)
			continue;
		if (*bufp == NULL)
			*bufp = fz_new_buffer(ctx, 256);
		buf = *bufp;
		for (line = block->u.t.first_line; line; line = line->next)
			for (ch = line->first_char; ch; ch = ch->next)
				if (ch->c > 0)
					fz_append_rune(ctx, buf, ch->c);
	}
}


static void
do_stretch(fz_context *ctx, walk_data *wd, pdf_obj *selem, fz_stext_block *block)
{
	if (selem == NULL)
		return;
	for ( ; block != NULL; block = block->next)
	{
		pdf_obj *k;
		if (block->type != FZ_STEXT_BLOCK_STRUCT)
			continue;
		if (block->u.s.down == NULL)
			continue;
		k = pdf_dict_get(ctx, selem, PDF_NAME(K));
		if (pdf_is_array(ctx, k))
			k = pdf_array_get(ctx, k, block->u.s.index);
		else if (block->u.s.index != 0)
			k = NULL;
		if (block->u.s.down->standard == FZ_STRUCTURE_TD ||
			block->u.s.down->standard == FZ_STRUCTURE_TH)
		{
			cell *c;
			int i, x, y, w, h;

			for (i = 0; i < wd->num_tds; i++)
			{
				if (k == wd->tds[i].obj)
					break;
			}
			if (i == wd->num_tds)
			{
				fz_warn(ctx, "Unknown TD found!");
			}
			x = wd->tds[i].x;
			y = wd->tds[i].y;
			c = get_cell(ctx, &wd->cs, x, y);
			c->content_rect = block->bbox;
			w = c->colspan;
			h = c->rowspan;
			if (wd->xs.div[x].max > block->bbox.x0)
				wd->xs.div[x].max = block->bbox.x0;
			if (wd->xs.div[x+w].min < block->bbox.x1)
				wd->xs.div[x+w].min = block->bbox.x1;
			if (wd->ys.div[y].max > block->bbox.y0)
				wd->ys.div[y].max = block->bbox.y0;
			if (wd->ys.div[y+h].min < block->bbox.y1)
				wd->ys.div[y+h].min = block->bbox.y1;
			collect_content(ctx, &c->buf, block->u.s.down->first_block);
		}
		else
		{
			do_stretch(ctx, wd, k, block->u.s.down->first_block);
		}
	}
}

static void
stretch_cells_with_content(fz_context *ctx, walk_data *wd, pdf_obj *selem, fz_rect bounds, fz_stext_block *block)
{
	int i;

	/* Set the dividers up based on the cells. */
	wd->xs.n = wd->cs.w;
	wd->ys.n = wd->cs.h;
	wd->xs.div = fz_malloc_array(ctx, wd->xs.n, divider);
	wd->ys.div = fz_malloc_array(ctx, wd->ys.n, divider);
	for (i = 0; i < wd->xs.n; i++)
	{
		wd->xs.div[i].min = bounds.x0;
		wd->xs.div[i].max = bounds.x1;
		wd->xs.div[i].refined = 0;
	}
	wd->xs.div[0].max = bounds.x0;
	wd->xs.div[wd->xs.n-1].min = bounds.x1;
	for (i = 0; i < wd->ys.n; i++)
	{
		wd->ys.div[i].min = bounds.y0;
		wd->ys.div[i].max = bounds.y1;
		wd->ys.div[i].refined = 0;
	}
	wd->ys.div[0].max = bounds.y0;
	wd->ys.div[wd->ys.n-1].min = bounds.y1;

	do_stretch(ctx, wd, selem, block);

	for (i = 0; i < wd->xs.n; i++)
	{
		wd->xs.div[i].pos = (wd->xs.div[i].min + wd->xs.div[i].max)/2;
	}
	for (i = 0; i < wd->ys.n; i++)
	{
		wd->ys.div[i].pos = (wd->ys.div[i].min + wd->ys.div[i].max)/2;
	}
}

typedef struct
{
	fz_output *out;
	int x;
	int y;
	table *table;
} walk_children_data;

static cell *
next_x_cell(fz_context *ctx, walk_children_data *wcd)
{
	cell *c;
	table *table = wcd->table;
	while (1)
	{
		c = get_cell(ctx, &table->cs, wcd->x, wcd->y);
		assert(c->rowspan != 0 && c->colspan != 0);
		if (c->rowspan >= 1)
			break;
		wcd->x++;
	}
	return c;
}

static void
walk_children(fz_context *ctx, walk_children_data *wcd, fz_stext_block *block)
{
	int comma = 0;
	fz_output *out = wcd->out;
	table *table = wcd->table;

	if (!block)
		return;

	fz_write_printf(ctx, out, ", \"children\": [\n");

	for (; block; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_STRUCT)
		{
			if (block->u.s.down != NULL)
			{
				if (comma)
				{
					fz_write_printf(ctx, out, ",");
				}
				if (block->u.s.down->standard == FZ_STRUCTURE_TH ||
					block->u.s.down->standard == FZ_STRUCTURE_TD)
				{
					int i;
					cell *c = next_x_cell(ctx, wcd);
					int x = wcd->x;
					int y = wcd->y;

					fz_write_printf(ctx, out, "{\"type\": \"%s\", \"page\": %d, \"rect\": [%g, %g, %g, %g]",
						fz_structure_to_string(block->u.s.down->standard), table->page, block->bbox.x0, block->bbox.y0, block->bbox.x1, block->bbox.y1);

					for (i = 0; i < table->num_tds; i++)
					{
						if (table->tds[i].x == wcd->x && table->tds[i].y == wcd->y)
							break;
					}
					if (i == table->num_tds)
					{
						table->broken = 1;
						fz_warn(ctx, "TH/TDs don't match up!");
						printf("TH/TDs don't match up!");
					}
					else
					{
						if (c->buf)
						{
							size_t j, m = c->buf->len;
							char *s = c->buf->data;
							fz_write_printf(ctx, out, ", \"text\": \"");
							for (j = 0; j < m; j++)
							{
								char ch = s[j];
								if (ch == '\"')
									ch = '\'';
								fz_write_printf(ctx, out, "%c", ch);
							}
							fz_write_printf(ctx, out, "\"");
						}
						wcd->x += c->rowspan-1;
						wcd->x += c->rowspan-1;
					}

					if (c->rowspan != 1)
						fz_write_printf(ctx, out, ", \"rowSpan\": %d", c->rowspan);
					if (c->colspan != 1)
						fz_write_printf(ctx, out, ", \"colSpan\": %d", c->colspan);

					fz_write_printf(ctx, out, "}\n");
				}
				else
				{
					if (block->u.s.down->standard == FZ_STRUCTURE_TR)
					{
						wcd->y++;
						wcd->x = 0;
					}
					fz_write_printf(ctx, out, "{ \"type\": \"%s\"\n", fz_structure_to_string(block->u.s.down->standard));
					walk_children(ctx, wcd, block->u.s.down->first_block);
					fz_write_printf(ctx, out, "}");
				}
				comma = 1;
			}
		}
	}

	fz_write_printf(ctx, out, "]\n");
}

static void
generate_table_json(fz_context *ctx, fz_output *out, table *table)
{
	int i, n;
	walk_children_data wcd = { 0 };

	fz_write_printf(ctx, out, "{ \"type\": \"Table\",\n");
	fz_write_printf(ctx, out, "\"page\": %d,\n", table->page);
	fz_write_printf(ctx, out, "\"rect\": [%g, %g, %g, %g],\n", table->r.x0, table->r.y0, table->r.x1, table->r.y1);
	if (table->overlap)
		fz_write_printf(ctx, out, "\"overlap\" : true, \n");
	if (table->broken)
		fz_write_printf(ctx, out, "\"broken\" : true, \n");
	if (table->out_of_order)
		fz_write_printf(ctx, out, "\"out_of_order\" : true, \n");
	if (table->has_attrs)
		fz_write_printf(ctx, out, "\"has_attrs\" : true, \n");

	//printf("Table\n");
	fz_write_printf(ctx, out, "\"h_lines\": {");
	n = table->ys.n;
	for (i = 0; i < n; i++)
	{
		if (i != 0)
			fz_write_printf(ctx, out, ",");
		fz_write_printf(ctx, out, "\"%d\": %g", i, table->ys.div[i].pos);
	}
	fz_write_printf(ctx, out, "},\n");

	fz_write_printf(ctx, out, "\"v_lines\": {");
	n = table->xs.n;
	for (i = 0; i < n; i++)
	{
		if (i != 0)
			fz_write_printf(ctx, out, ",");
		fz_write_printf(ctx, out, "\"%d\": %g", i, table->xs.div[i].pos);
	}
	fz_write_printf(ctx, out, "}\n");
	wcd.out = out;
	wcd.x = 0;
	wcd.y = -1;
	wcd.table = table;
	walk_children(ctx, &wcd, table->top_block);
	fz_write_printf(ctx, out, "}\n");
}

static int
detect_out_of_order(fz_context *ctx, table* table)
{
	int ooo = 0;
	int i, n = table->xs.n;

	for (i = 0; i < n; i++)
	{
		if (table->xs.div[i].min > table->xs.div[i].max)
			ooo = 1;
	}
	for (i = 0; i < n-1; i++)
	{
		if (table->xs.div[i].max > table->xs.div[i+1].min)
			ooo = 1;
	}
	n = table->ys.n;
	for (i = 0; i < n; i++)
	{
		if (table->ys.div[i].min > table->ys.div[i].max)
			ooo = 1;
	}
	for (i = 0; i < n-1; i++)
	{
		if (table->ys.div[i].max > table->ys.div[i+1].min)
			ooo = 1;
	}

	return ooo;
}

static void
refine_divider(dividers *ds, float x)
{
	int i, n = ds->n;

	for (i = 0; i < n; i++)
	{
		if (ds->div[i].min <= x && ds->div[i].max >= x)
			break;
	}
	if (i == n)
		return;

	x += (ds->div[i].pos * ds->div[i].refined);
	ds->div[i].refined++;
	ds->div[i].pos = x / ds->div[i].refined;
}

static void
refine_dividers_with_vectors(fz_context *ctx, walk_data *wd, fz_stext_block *block)
{
	for (; block != NULL; block = block->next)
	{
		float w, h;
		if (!fz_overlaps_rect(wd->bbox, block->bbox))
			continue;
		if (block->type == FZ_STEXT_BLOCK_STRUCT)
		{
			if (block->u.s.down)
				refine_dividers_with_vectors(ctx, wd, block->u.s.down->first_block);
		}
		if (block->type != FZ_STEXT_BLOCK_VECTOR)
			continue;
		if ((block->u.v.flags & FZ_STEXT_VECTOR_IS_RECTANGLE) == 0)
			continue;
		w = block->bbox.x1 - block->bbox.x0;
		h = block->bbox.y1 - block->bbox.y0;
		if (block->u.v.flags & FZ_STEXT_VECTOR_IS_STROKED)
		{
			if (w > 4 && h > 4)
				continue;
			if (w < 4)
				refine_divider(&wd->xs, block->bbox.x0 + w/2);
			if (h < 4)
				refine_divider(&wd->ys, block->bbox.y0 + h/2);
		}
		else
		{
			if (w > 10)
			{
				refine_divider(&wd->xs, block->bbox.x0);
				refine_divider(&wd->xs, block->bbox.x1);
			}
			if (h > 10)
			{
				refine_divider(&wd->ys, block->bbox.y0);
				refine_divider(&wd->ys, block->bbox.y1);
			}
		}
	}
}


static int
extract_table_details(fz_context *ctx, walk_data *wd, pdf_obj *selem, fz_stext_struct *str)
{
	fz_stext_block *table = str->up;
	fz_buffer *buf = NULL;
	int n, i;

	//fz_write_printf(ctx, wd->out, "Page=%d Table bbox=%g %g %g %g\n", wd->page+1, table->bbox.x0, table->bbox.y0, table->bbox.x1, table->bbox.y1);

	wd->bbox = table->bbox;

	/* Increase the bbox to allow for the raft it's on. */

	n = fz_flotilla_size(ctx, wd->flotilla);
	for (i = 0; i < n; i++)
	{
		fz_rect r = fz_flotilla_raft_area(ctx, wd->flotilla, i);

		if (!fz_overlaps_rect(r, wd->bbox))
			continue;
		wd->bbox = fz_union_rect(wd->bbox, r);
	}

	/* Create a cell structure based upon the structure element tree */
	memset(&wd->cs, 0, sizeof(wd->cs));
	wd->cs.w = 0;
	wd->cs.h = 0;
	wd->x = -1;
	wd->y = -1;
	walk_table_elems(ctx, wd, selem);

	if (wd->cs.w && wd->cs.h)
	{
		cell *c = get_cell(ctx, &wd->cs, wd->cs.w, wd->cs.h);
		wd->cs.w++;
		wd->cs.h++;
	}

	/* Now look at the content */
	stretch_cells_with_content(ctx, wd, selem, wd->bbox, str->first_block);

	/* Refine dividers with vectors. */
	refine_dividers_with_vectors(ctx, wd, wd->stext);

	//asciiart_table(ctx, wd->out, &wd->cs);

	wd->tables[wd->num_tables].buf = buf;
	wd->tables[wd->num_tables].r = wd->bbox;
	wd->tables[wd->num_tables].xs = wd->xs;
	wd->tables[wd->num_tables].ys = wd->ys;
	wd->tables[wd->num_tables].page = wd->page;
	wd->tables[wd->num_tables].cs = wd->cs;
	wd->tables[wd->num_tables].top_block = str->first_block;
	wd->tables[wd->num_tables].out_of_order = detect_out_of_order(ctx, &wd->tables[wd->num_tables]);
	memset(&wd->cs, 0, sizeof(wd->cs));
	wd->tables[wd->num_tables].tds = fz_malloc_array(ctx, wd->num_tds, td_obj);
	memcpy(wd->tables[wd->num_tables].tds, wd->tds, wd->num_tds * sizeof(td_obj));
	wd->tables[wd->num_tables].num_tds = wd->num_tds;
	wd->tables[wd->num_tables].has_attrs = wd->use_attrs;
	wd->num_tds = 0;
	wd->num_tables++;

	return 1;
}

/* Returns the number of tables found. */
static int
find_table_in_struct(fz_context *ctx, walk_data *wd, pdf_obj *parent_selem, fz_stext_struct *str)
{
	fz_stext_block *block = str->up;
	pdf_obj *k;
	int got = 0;

	if (!str)
		return 0;

	k = pdf_dict_get(ctx, parent_selem, PDF_NAME(K));
	if (pdf_is_array(ctx, k))
	{
		k = pdf_array_get(ctx, k, block->u.s.index);
	}
	else if (block->u.s.index != 0)
	{
		k = NULL;
	}

	if (str->standard == FZ_STRUCTURE_TABLE)
	{
		//printf("Table\n");
		return extract_table_details(ctx, wd, k, str);
	}

	/* Keep walking */
	for (block = str->first_block; block != NULL; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_STRUCT)
		{
			got += find_table_in_struct(ctx, wd, k, block->u.s.down);
		}
	}

	return got;
}

static void
detect_overlapping_tables(walk_data *wd)
{
	int i, j, n;

	n = wd->num_tables;

	for (i = 0; i < n-1; i++)
	{
		for (j = i+1; j < n; j++)
		{
			if (wd->tables[i].page != wd->tables[j].page)
				continue;
			if (fz_overlaps_rect(wd->tables[i].r, wd->tables[j].r))
			{
				wd->tables[i].overlap = 1;
				wd->tables[j].overlap = 1;
			}
		}
	}
}

static fz_pixmap *pix = NULL;
static int pix_for_page = -1;
bitmap_debugging(fz_context *ctx, fz_document *doc, table *table, const char *rootname, int table_num)
{
	int x, y;
	fz_pixmap *pix2;
	char png[4096];

	if (pix_for_page != table->page)
	{
		pix = fz_new_pixmap_from_page_number(ctx, doc, table->page, fz_identity, fz_device_rgb(ctx), 0);
		pix_for_page = table->page;
	}

	pix2 = fz_clone_pixmap(ctx, pix);
	scribble_box(ctx, pix2, table->r, 0xff0000, 0.2f, 0);
	sprintf(png, "%s.%d_0.png", rootname, table_num);
	fz_save_pixmap_as_png(ctx, pix2, png);
	fz_drop_pixmap(ctx, pix2);

	pix2 = fz_clone_pixmap(ctx, pix);
	for (y = 0; y < table->cs.h; y++)
	{
		for (x = 0; x < table->cs.w; x++)
		{
			fz_rect r = get_cell(ctx, &table->cs, x, y)->content_rect;
			if (!fz_is_empty_rect(r))
				scribble_box(ctx, pix2, r, 0x0000ff, 0.2f, 0);
		}
	}
	sprintf(png, "%s.%d_1.png", rootname, table_num);
	fz_save_pixmap_as_png(ctx, pix2, png);
	fz_drop_pixmap(ctx, pix2);

	pix2 = fz_clone_pixmap(ctx, pix);
	output_divs(ctx, pix2, table);
	sprintf(png, "%s.%d_2.png", rootname, table_num);
	fz_save_pixmap_as_png(ctx, pix2, png);
	fz_drop_pixmap(ctx, pix2);
	pix2 = NULL;
}

static walk_data wd = { 0 };

int main(int argc, const char *argv[])
{
	fz_context *ctx;
	fz_document *doc = NULL;
	fz_stext_page **stexts = NULL;
	fz_output *out;
	int exit_code = 0;
	int pages = 0;

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (ctx == NULL)
	{
		fprintf(stderr, "Failed to create context");
		return 1;
	}

	fz_var(doc);
	fz_var(stexts);
	fz_var(pages);

	fz_try(ctx)
	{
		int i, flags;
		fz_stext_options opts = { 0 };
		fz_stext_block *block;
		char json[4096];
		pdf_obj *str;
		pdf_document *pdf;

		opts.flags = FZ_STEXT_COLLECT_STRUCTURE | FZ_STEXT_ACCURATE_BBOXES | FZ_STEXT_COLLECT_VECTORS | FZ_STEXT_PRESERVE_IMAGES;
		opts.scale = 1;
		opts.clip = fz_infinite_rect;

		fz_register_document_handlers(ctx);

		sprintf(json, "%s.json", argv[1]);
		out = fz_new_output_with_path(ctx, json, 0);

		doc = fz_open_document(ctx, argv[1]);

		flags = pdf_check_structure_tree(ctx, pdf_specifics(ctx, doc));
		if ((flags & PDF_STRUCT_PRESENT) == 0)
		{
			fz_warn(ctx, "No structure tree present");
		}
		if (flags & PDF_STRUCT_FIXED)
		{
			fz_warn(ctx, "Structure tree repaired");
		}
		if (flags & PDF_STRUCT_HAS_CYCLE)
		{
			fz_warn(ctx, "Invalid structure tree - cycle present");
		}
		else if (flags & PDF_STRUCT_BROKEN)
		{
			fz_warn(ctx, "Invalid structure tree");
		}
		if (flags & PDF_STRUCT_HAS_TABLE_SPAN_ATTRIBUTES)
		{
			fz_warn(ctx, "StructTree has table span attributes");
			wd.use_attrs = 1;
		}
		else  if (flags & PDF_STRUCT_HAS_TABLE_ATTRIBUTES)
		{
			fz_warn(ctx, "StructTree has table attributes");
			wd.use_attrs = 1;
		}
		else if (flags & PDF_STRUCT_HAS_ATTRIBUTES)
		{
			fz_warn(ctx, "StructTree has attributes");
			wd.use_attrs = 1;
		}


		pdf = pdf_specifics(ctx, doc);
		str = pdf_dict_get(ctx, pdf_dict_get(ctx, pdf_trailer(ctx, pdf), PDF_NAME(Root)), PDF_NAME(StructTreeRoot));
		pages = fz_count_pages(ctx, doc);
		stexts = fz_malloc_array(ctx, pages, fz_stext_page *);

		for (i = 0; i < pages; i++)
		{
			int got = 0;

			wd.page = i;
			stexts[i] = fz_new_stext_page_from_page_number(ctx, doc, i, &opts);

			wd.flotilla = fz_new_flotilla_from_stext_page_vectors(ctx, stexts[i]);

			/* Walk to find the tables */
			for (block = stexts[i]->first_block; block != NULL; block = block->next)
			{
				wd.stext = stexts[i]->first_block;
				if (block->type == FZ_STEXT_BLOCK_STRUCT)
					got = find_table_in_struct(ctx, &wd, str, block->u.s.down);
			}

			fz_drop_flotilla(ctx, wd.flotilla);
			wd.flotilla = NULL;
		}

		detect_overlapping_tables(&wd);

		/* Output phase */
		fz_write_printf(ctx, out, "{ \"input\": \"%s\",\n", argv[1]);
		fz_write_printf(ctx, out, "\"attribute_flags\": \"%d\",\n", flags);
		fz_write_printf(ctx, out, "\"pages\": %d,\n", pages);
		fz_write_printf(ctx, out, "\"pages_meta\": [\n");
		for (i = 0; i < pages; i++)
		{
			fz_page *page = fz_load_page(ctx, doc, i);
			fz_rect bounds = fz_bound_page(ctx, page);
			fz_drop_page(ctx, page);
			if (i != 0)
				fz_write_printf(ctx, out, ", ");
			fz_write_printf(ctx, out, "{ \"index\": %d, ", i);
			fz_write_printf(ctx, out, "\"rotation\": 0, ", 0); // FIXME
			fz_write_printf(ctx, out, "\"mediabox\": [ %g, %g ], ", bounds.x1 - bounds.x0, bounds.y1 - bounds.y0); // FIXME
			fz_write_printf(ctx, out, "\"displayed\": [ %g, %g ] ", bounds.x1 - bounds.x0, bounds.y1 - bounds.y0); // FIXME
			fz_write_printf(ctx, out, "}\n");
		}
		fz_write_printf(ctx, out, "],\n");

		fz_write_printf(ctx, out, "\"tables\": [\n");

		for (i = 0; i < wd.num_tables; i++)
		{
			table *table = &wd.tables[i];

			if (i != 0)
				fz_write_printf(ctx, out, ",\n");

			generate_table_json(ctx, out, table);

			if (!table->has_attrs || table->broken || table->overlap || table->out_of_order || table->xs.n <= 2 || table->ys.n <= 2)
				continue;

			bitmap_debugging(ctx, doc, table, argv[1], i);

		}

		fz_write_printf(ctx, out, "]\n");
		fz_write_printf(ctx, out, "}\n");
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		int i;
		for (i = 0; i < pages; i++)
			fz_drop_stext_page(ctx, stexts[i]);
		fz_free(ctx, stexts);
		fz_drop_document(ctx, doc);
		fz_drop_output(ctx, out);
		fz_drop_pixmap(ctx, pix);
		fz_drop_flotilla(ctx, wd.flotilla);
	}
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		exit_code = 1;
	}

	fz_drop_context(ctx);

	return exit_code;
}
