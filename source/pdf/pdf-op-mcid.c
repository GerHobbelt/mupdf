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

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <string.h>

#if FZ_ENABLE_PDF

typedef struct
{
	pdf_processor super;
	pdf_font_desc *font;
	pdf_mcid_table *table;
	pdf_mcid_entry *cursor;
	int mcid;
} pdf_mcid_processor;

static pdf_mcid_entry *
mcid_new_entry(fz_context *ctx, pdf_mcid_table *table, int mcid)
{
	if (table->len + 1 >= table->cap)
	{
		int new_cap = table->cap ? table->cap : 16;
		while (table->len >= new_cap)
			new_cap = new_cap * 2;
		table->entry = fz_realloc_array(ctx, table->entry, new_cap, pdf_mcid_entry);
		table->cap = new_cap;
	}
	table->entry[table->len].mcid = mcid;
	table->entry[table->len].text = fz_new_buffer(ctx, 16);
	return &table->entry[table->len++];
}

static void
mcid_putc(fz_context *ctx, pdf_mcid_processor *p, int c)
{
	if (!p->cursor || p->cursor->mcid != p->mcid)
		p->cursor = mcid_new_entry(ctx, p->table, p->mcid);
	fz_append_rune(ctx, p->cursor->text, c);
}

static void
mcid_show_newline(fz_context *ctx, pdf_mcid_processor *p)
{
	mcid_putc(ctx, p, '\n');
}

static void
mcid_show_space(fz_context *ctx, pdf_mcid_processor *p, float adj)
{
	if (adj > 250)
		mcid_putc(ctx, p, ' ');
}

static void
mcid_show_char(fz_context *ctx, pdf_mcid_processor *p, int cid)
{
	pdf_font_desc *fontdesc = p->font;
	int buf[8];
	int i, n;

	n = 0;
	if (fontdesc->to_unicode)
		n = pdf_lookup_cmap_full(fontdesc->to_unicode, cid, buf);
	if (n == 0 && (size_t)cid < fontdesc->cid_to_ucs_len)
	{
		buf[0] = fontdesc->cid_to_ucs[cid];
		n = 1;
	}
	if (n == 0 || (n == 1 && buf[0] == 0))
	{
		buf[0] = FZ_REPLACEMENT_CHARACTER;
		n = 1;
	}

	for (i = 0; i < n; ++i)
		mcid_putc(ctx, p, buf[i]);
}

static void
mcid_show_string(fz_context *ctx, pdf_mcid_processor *p, const unsigned char *buf, size_t len)
{
	pdf_font_desc *fontdesc = p->font;
	const unsigned char *end = buf + len;
	unsigned int cpt;
	int cid;

	if (!fontdesc)
	{
		fz_warn(ctx, "font not set");
		return;
	}

	while (buf < end)
	{
		int w = pdf_decode_cmap(fontdesc->encoding, buf, end, &cpt);
		buf += w;

		cid = pdf_lookup_cmap(fontdesc->encoding, cpt);
		if (cid >= 0)
			mcid_show_char(ctx, p, cid);
		else
			fz_warn(ctx, "cannot encode character");
	}
}

static void
mcid_show_text(fz_context *ctx, pdf_mcid_processor *p, pdf_obj *text)
{
	if (pdf_is_array(ctx, text))
	{
		int i, n = pdf_array_len(ctx, text);
		for (i = 0; i < n; ++i)
		{
			pdf_obj *item = pdf_array_get(ctx, text, i);
			if (pdf_is_string(ctx, item))
				mcid_show_string(ctx, p,
					(unsigned char *)pdf_to_str_buf(ctx, item),
					pdf_to_str_len(ctx, item));
			else
				mcid_show_space(ctx, p, -pdf_to_real(ctx, item));
		}
	}
	else if (pdf_is_string(ctx, text))
	{
		mcid_show_string(ctx, p,
			(unsigned char *)pdf_to_str_buf(ctx, text),
			pdf_to_str_len(ctx, text));
	}
}

/* text state */

static void pdf_mcid_Tf(fz_context *ctx, pdf_processor *proc, const char *name, pdf_font_desc *font, float size)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	pdf_drop_font(ctx, p->font);
	p->font = NULL;
	p->font = pdf_keep_font(ctx, font);
}

/* text positioning */

static void
pdf_mcid_Td(fz_context *ctx, pdf_processor *proc, float tx, float ty)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	mcid_show_newline(ctx, p);
}

static void
pdf_mcid_TD(fz_context *ctx, pdf_processor *proc, float tx, float ty)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	mcid_show_newline(ctx, p);
}

static void
pdf_mcid_Tstar(fz_context *ctx, pdf_processor *proc)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	mcid_show_newline(ctx, p);
}

/* text showing */

static void
pdf_mcid_TJ(fz_context *ctx, pdf_processor *proc, pdf_obj *array)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	mcid_show_text(ctx, p, array);
}

static void
pdf_mcid_Tj(fz_context *ctx, pdf_processor *proc, const char *str, size_t len)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	mcid_show_string(ctx, p, (const unsigned char *)str, len);
}

static void
pdf_mcid_squote(fz_context *ctx, pdf_processor *proc, const char *str, size_t len)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	mcid_show_newline(ctx, p);
	mcid_show_string(ctx, p, (const unsigned char *)str, len);
}

static void
pdf_mcid_dquote(fz_context *ctx, pdf_processor *proc, float aw, float ac, const char *str, size_t len)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	mcid_show_newline(ctx, p);
	mcid_show_string(ctx, p, (const unsigned char*)str, len);
}

/* shadings, images, xobjects */

static void
pdf_mcid_Do_form(fz_context *ctx, pdf_processor *proc, const char *name, pdf_obj *xobj)
{
	// TODO
}

/* marked content */

static void
pdf_mcid_BMC(fz_context *ctx, pdf_processor *proc, const char *tag)
{
	// TODO: nesting
}

static void
pdf_mcid_BDC(fz_context *ctx, pdf_processor *proc, const char *tag, pdf_obj *raw, pdf_obj *cooked)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	pdf_obj *mcid = pdf_dict_get(ctx, cooked, PDF_NAME(MCID));
	if (pdf_is_number(ctx, mcid))
		p->mcid = pdf_to_int(ctx, mcid);
	// TODO: nesting
}

static void
pdf_mcid_EMC(fz_context *ctx, pdf_processor *proc)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	p->mcid = -1;
	// TODO: nesting
}

static void
pdf_close_mcid_processor(fz_context *ctx, pdf_processor *proc)
{
}

static void
pdf_drop_mcid_processor(fz_context *ctx, pdf_processor *proc)
{
	pdf_mcid_processor *p = (pdf_mcid_processor*)proc;
	pdf_drop_font(ctx, p->font);
}

pdf_processor *
pdf_new_mcid_processor(fz_context *ctx, pdf_mcid_table *table)
{
	pdf_mcid_processor *proc = (pdf_mcid_processor *)pdf_new_processor(ctx, sizeof *proc);
	{
		proc->super.close_processor = pdf_close_mcid_processor;
		proc->super.drop_processor = pdf_drop_mcid_processor;

		/* text state */
		proc->super.op_Tf = pdf_mcid_Tf;

		/* text positioning */
		proc->super.op_Td = pdf_mcid_Td;
		proc->super.op_TD = pdf_mcid_TD;
		proc->super.op_Tstar = pdf_mcid_Tstar;

		/* text showing */
		proc->super.op_TJ = pdf_mcid_TJ;
		proc->super.op_Tj = pdf_mcid_Tj;
		proc->super.op_squote = pdf_mcid_squote;
		proc->super.op_dquote = pdf_mcid_dquote;

		/* shadings, images, xobjects */
		proc->super.op_Do_form = pdf_mcid_Do_form;

		/* marked content */
		proc->super.op_BMC = pdf_mcid_BMC;
		proc->super.op_BDC = pdf_mcid_BDC;
		proc->super.op_EMC = pdf_mcid_EMC;
	}

	// TODO: output table
	proc->table = table;
	proc->cursor = NULL;
	proc->mcid = -1;

	return (pdf_processor*)proc;
}

#endif
