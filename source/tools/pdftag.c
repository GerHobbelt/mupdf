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
 * PDF tag; tag dumper and rewriter.
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#undef DEBUG_STRUCTURE

#if FZ_ENABLE_RENDER_CORE 

static int usage(void)
{
	fprintf(stderr,
		"usage: mutool tag [options] input.pdf\n"
		"  -c    include contents\n"
		"  -f -  tag file\n"
		"  -i    indent the XML for easy reading\n"
		"  -p -  password\n"

		);
	return 1;
}

typedef struct
{
	fz_device base;

	fz_xml *root;
	fz_xml *current;

#ifdef DEBUG_STRUCTURE
	int depth;
#endif
	int sequence;

	size_t struct_path_max;
	char *struct_path;
} fz_struct_gatherer;

static int
is_TAG_node(fz_xml *tag)
{
	const char *t = fz_xml_tag(tag);
	return (t && strcmp(t, "TAG") == 0);
}

static int
is_ROOT_node(fz_xml *tag)
{
	const char *t = fz_xml_tag(tag);
	return (t && strcmp(t, "ROOT") == 0);
}

static void
struct_gather_begin_structure(fz_context *ctx, fz_device *dev_, fz_structure standard, const char *raw, int idx)
{
	fz_struct_gatherer *dev = (fz_struct_gatherer *)dev_;
	fz_xml *node = NULL;
	fz_xml *down;

	if (dev == NULL)
		return;

#ifdef DEBUG_STRUCTURE
	{
		int i;
		for (i = 0; i < dev->depth; i++)
			fprintf(stderr, " ");
		fprintf(stderr, "BEGIN %s %d ", raw ? raw : "?", idx);
		assert(dev->depth >= 0);
		dev->depth++;
	}
#endif

	fz_var(node);

	/* Extend the path for this node */
	{
		size_t len = dev->struct_path ? strlen(dev->struct_path) : 0;
		size_t len2 = strlen(raw);
		char text[32];
		sprintf(text, "/%d[]", idx);
		len2 += strlen(text);

		if (len + len2 + 1> dev->struct_path_max)
		{
			dev->struct_path = fz_realloc(ctx, dev->struct_path, len + len2 + 64);
			if (dev->struct_path_max == 0)
				dev->struct_path[0] = 0;
			dev->struct_path_max = len + len2 + 64;
		}

		sprintf(dev->struct_path + len, "/%d[%s]", idx, raw);
	}

	/* We only ever extend downwards from TAGs or from the ROOT. If we're on a
	 * different type node find the enclosing one. */
	{
#ifdef DEBUG_STRUCTURE
		fz_xml *start = dev->current;
#endif

		while (!is_TAG_node(dev->current) && !is_ROOT_node(dev->current))
		{
			fz_xml *up = fz_xml_up(dev->current);
			if (up == NULL)
			{
#ifdef DEBUG_STRUCTURE
				fz_dom_add_attribute(ctx, start, "bad", "nesting");
				fz_debug_xml(dev->current, 0);
#endif
				fz_throw(ctx, FZ_ERROR_GENERIC, "Structure stack confusing");
			}
			dev->current = up;
		}
	}

	down = fz_xml_down(dev->current);
	if (down == NULL && idx == 0)
	{
		/* We have no children at the moment, and this one should be the 0th. */
		node = fz_new_dom_node(ctx, dev->root, "TAG");
		fz_dom_add_attribute(ctx, node, "raw", raw);
		fz_dom_add_attribute(ctx, node, "path", dev->struct_path);
		fz_dom_append_child(ctx, dev->current, node);
		dev->current = node;
		node = NULL;
	}
	else if (down == NULL)
	{
		/* We have no children at the moment, and there should be idx before us. */
		int i;

		node = fz_new_dom_node(ctx, dev->root, "TAG");
		fz_dom_append_child(ctx, dev->current, node);
		dev->current = node;
		node = NULL;

		for (i = 1; i < idx; i++)
		{
			node = fz_new_dom_node(ctx, dev->root, "TAG");
			fz_dom_insert_after(ctx, dev->current, node);
			dev->current = node;
			node = NULL;
		}

		node = fz_new_dom_node(ctx, dev->root, "TAG");
		fz_dom_add_attribute(ctx, node, "raw", raw);
		fz_dom_add_attribute(ctx, node, "path", dev->struct_path);
		fz_dom_insert_after(ctx, dev->current, node);
		dev->current = node;
		node = NULL;
	}
	else
	{
		/* We have children, and we want to be the idxth TAG node. (counting from 0!) */
		int i = 0;
		dev->current = down;

		/* Skip over any non TAG nodes to get to the 0th. */
		while (1)
		{
			fz_xml *next = fz_dom_next(ctx, dev->current);
			if (is_TAG_node(dev->current))
				break;
			if (next == NULL)
				break;
			dev->current = next;
		}
		/* dev->current is either the 0th tag, or the last non-tag entry. */

		if (is_TAG_node(dev->current))
		{
			for (i = 0; i < idx;)
			{
				fz_xml *next = fz_dom_next(ctx, dev->current);
				if (next == NULL)
					break;
				if (is_TAG_node(next))
					i++;
				dev->current = next;
			}
		}
		/* If i == idx, then dev->current is the idxth tag. Otherwise it's the last entry. */

		if (i < idx)
		{
			/* We need to create some new ones. */
			for (; i < idx-1; i++)
			{
				node = fz_new_dom_node(ctx, dev->root, "TAG");
				fz_dom_insert_after(ctx, dev->current, node);
				dev->current = node;
				node = NULL;
			}

			/* Add the new node */
			node = fz_new_dom_node(ctx, dev->root, "TAG");
			fz_dom_add_attribute(ctx, node, "raw", raw);
			fz_dom_add_attribute(ctx, node, "path", dev->struct_path);
			fz_dom_insert_after(ctx, dev->current, node);
			dev->current = node;
			node = NULL;
		}
		else
		{
			/* We are on the idxth one. Let's hope the tags match! */
			char *raw2 = fz_xml_att(dev->current, "raw");
			if (raw2 == NULL)
				fz_dom_add_attribute(ctx, dev->current, "raw", raw);
			else if (strcmp(raw2, raw) != 0)
				fz_warn(ctx, "Structure tree inconsistency: %s != %s", raw2, raw);
			raw2 = fz_xml_att(dev->current, "path");
			if (raw2 == NULL)
				fz_dom_add_attribute(ctx, node, "path", dev->struct_path);
			else if (strcmp(raw2, dev->struct_path) != 0)
				fz_warn(ctx, "Structure tree path inconsistency: %s != %s", raw2, dev->struct_path);
		}
	}
#ifdef DEBUG_STRUCTURE
	{
		fprintf(stderr, "%p\n", dev->current);
	}
#endif
}

static void
struct_gather_end_structure(fz_context *ctx, fz_device *dev_)
{
	fz_struct_gatherer *dev = (fz_struct_gatherer *)dev_;
	fz_xml *up;
#ifdef DEBUG_STRUCTURE
	fz_xml *start = dev->current;
#endif

	if (dev == NULL)
		return;

#ifdef DEBUG_STRUCTURE
	{
		int i;
		dev->depth--;
		for (i = 0; i < dev->depth; i++)
			fprintf(stderr, " ");
		fprintf(stderr, "END");
		assert(dev->depth >= 0);
	}
#endif

	/* Shorten the path for this node */
	if (dev->struct_path == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Structure path underflow");
	{
		size_t len = strlen(dev->struct_path);
		while (1)
		{
			if (dev->struct_path[len] == '/')
				break;
			if (len == 0)
				fz_throw(ctx, FZ_ERROR_GENERIC, "Structure path underflow");
			len--;
		}
		dev->struct_path[len] = 0;
	}

	/* Make sure we're on a TAG node to step up from! */
	while (!is_TAG_node(dev->current))
	{
		fz_xml *up = fz_xml_up(dev->current);
		if (up == NULL)
		{
#ifdef DEBUG_STRUCTURE
			fz_dom_add_attribute(ctx, start, "bad", "nesting");
			fz_debug_xml(dev->root, 0);
#endif
			fz_throw(ctx, FZ_ERROR_GENERIC, "Structure stack underflow");
		}
		dev->current = up;
	}

	/* Now do the actual step up. */
	up = fz_xml_up(dev->current);
	if (up == NULL)
	{
#ifdef DEBUG_STRUCTURE
		fz_dom_add_attribute(ctx, start, "bad", "nesting");
		fz_debug_xml(dev->root, 0);
#endif
		fz_throw(ctx, FZ_ERROR_GENERIC, "Structure stack underflow");
	}
	dev->current = up;

#ifdef DEBUG_STRUCTURE
	if (up == NULL)
		fprintf(stderr, " pop to where???\n");
	else
	{
		char *raw2 = fz_xml_att(dev->current, "raw");
		fprintf(stderr, " pop to %s (%p)\n", raw2 ? raw2 : "?", dev->current);
	}
#endif
}

static void
do_seq(fz_context *ctx, fz_struct_gatherer *dev, fz_xml *node)
{
	char text[32];

	sprintf(text, "%d", dev->sequence++);
	fz_dom_add_attribute(ctx, node, "seq", text);
}

static void
do_text(fz_context *ctx, fz_device *dev_, const fz_text *text, const char *tag)
{
	fz_struct_gatherer *dev = (fz_struct_gatherer *)dev_;
	fz_xml *node;
	fz_text_span *s;
	size_t len = 1;
	char *buf;

	for (s = text->head; s; s = s->next)
	{
		int i;
		for (i = 0; i < s->len; i++)
			if (s->items[i].ucs > 0 && s->items[i].ucs != FZ_REPLACEMENT_CHARACTER)
				len += fz_runelen(s->items[i].ucs);
	}
	buf = fz_malloc(ctx, len);
	len = 0;
	for (s = text->head; s; s = s->next)
	{
		int i;
		for (i = 0; i < s->len; i++)
			if (s->items[i].ucs > 0 && s->items[i].ucs != FZ_REPLACEMENT_CHARACTER)
				len += fz_runetochar(buf + len, s->items[i].ucs);
	}
	buf[len] = 0;

	fz_try(ctx)
	{
		fz_xml *tnode = fz_new_dom_node(ctx, dev->root, tag);
		fz_dom_append_child(ctx, dev->current, tnode);
		node = fz_new_dom_text_node(ctx, dev->root, buf);
		fz_dom_append_child(ctx, tnode, node);
		do_seq(ctx, dev, tnode);
	}
	fz_always(ctx)
		fz_free(ctx, buf);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
struct_gather_fill_text(fz_context *ctx, fz_device *dev_, const fz_text *text, fz_matrix ctm, fz_colorspace *cs, const float *color, float alpha, fz_color_params cp)
{
	do_text(ctx, dev_, text, "TEXT");
}

static void
struct_gather_stroke_text(fz_context *ctx, fz_device *dev_, const fz_text *text, const fz_stroke_state *st, fz_matrix ctm, fz_colorspace *cs, const float *color, float alpha, fz_color_params cp)
{
	do_text(ctx, dev_, text, "STROKE_TEXT");
}

static void
struct_gather_clip_text(fz_context *ctx, fz_device *dev_, const fz_text *text, fz_matrix ctm, fz_rect scissor)
{
	do_text(ctx, dev_, text, "CLIP_TEXT");
}

static void
struct_gather_ignore_text(fz_context *ctx, fz_device *dev_, const fz_text *text, fz_matrix ctm)
{
	do_text(ctx, dev_, text, "IGNORE_TEXT");
}

static fz_xml *
do_node(fz_context *ctx, fz_device *dev_, const char *tag)
{
	fz_struct_gatherer *dev = (fz_struct_gatherer *)dev_;
	fz_xml *node = fz_new_dom_node(ctx, dev->root, tag);
	fz_dom_append_child(ctx, dev->current, node);
	do_seq(ctx, dev, node);

	return node;
}

static fz_xml *
do_begin(fz_context *ctx, fz_device *dev_, const char *tag)
{
	fz_struct_gatherer *dev = (fz_struct_gatherer *)dev_;

	return dev->current = do_node(ctx, dev_, tag);
}

static void
do_end(fz_context *ctx, fz_device *dev_, const char *tag)
{
	fz_struct_gatherer *dev = (fz_struct_gatherer *)dev_;

	/* Sadly, tags and content don't have to be nicely nested. In order
	 * to form our representation, we therefore may have to fake it a
	 * bit, by ending content early to preserve nesting. If we've done
	 * that, then we may not actually need to do the end here. Essentially
	 * we only want to actually do a pop if the tag matches what we expect.
	 */
	if (strcmp(fz_xml_tag(dev->current), tag) != 0)
		return;

	dev->current = fz_dom_parent(ctx, dev->current);
}

static void
do_image(fz_context *ctx, fz_device *dev, fz_image *image, const char *tag)
{
	fz_xml *node;
	char text[32];

	node = do_node(ctx, dev, tag);
	sprintf(text, "%d", image->w);
	fz_dom_add_attribute(ctx, node, "w", text);
	sprintf(text, "%d", image->h);
	fz_dom_add_attribute(ctx, node, "h", text);
}

static void
struct_gather_fill_image(fz_context *ctx, fz_device *dev_, fz_image *image, fz_matrix ctm, float alpha, fz_color_params cp)
{
	do_image(ctx, dev_, image, "IMAGE");
}

static void
struct_gather_fill_image_mask(fz_context *ctx, fz_device *dev_, fz_image *image, fz_matrix ctm, fz_colorspace *cs, const float *color, float alpha, fz_color_params cp)
{
	do_image(ctx, dev_, image, "IMAGEMASK");
}

static void
struct_gather_clip_image_mask(fz_context *ctx, fz_device *dev_, fz_image *image, fz_matrix ctm, fz_rect scissor)
{
	do_image(ctx, dev_, image, "IMAGECLIP");
}

static void
struct_gather_fill_path(fz_context *ctx, fz_device *dev_, const fz_path *path, int even_odd, fz_matrix ctm, fz_colorspace *cs, const float *color, float alpha, fz_color_params cp)
{
	do_node(ctx, dev_, "FILL_PATH");
}

static void
struct_gather_stroke_path(fz_context *ctx, fz_device *dev_, const fz_path *path, const fz_stroke_state *st, fz_matrix ctm, fz_colorspace *cs, const float *color, float alpha, fz_color_params cp)
{
	do_node(ctx, dev_, "STROKE_PATH");
}

static void
struct_gather_clip_path(fz_context *ctx, fz_device *dev_, const fz_path *path, int even_odd, fz_matrix ctm, fz_rect scissor)
{
	do_begin(ctx, dev_, "CLIP_PATH");
}

static void
struct_gather_clip_stroke_path(fz_context *ctx, fz_device *dev_, const fz_path *path, const fz_stroke_state *st, fz_matrix ctm, fz_rect scissor)
{
	do_begin(ctx, dev_, "CLIP_STROKE_PATH");
}

static void
struct_gather_fill_shade(fz_context *ctx, fz_device *dev_, fz_shade *shd, fz_matrix ctm, float alpha, fz_color_params color_params)
{
	do_node(ctx, dev_, "FILL_SHADE");
}

static void
struct_gather_pop_clip(fz_context *ctx, fz_device *dev_)
{
	fz_struct_gatherer *dev = (fz_struct_gatherer *)dev_;

	/* Sadly, tags and content don't have to be nicely nested. In order
	 * to form our representation, we therefore may have to fake it a
	 * bit, by ending content early to preserve nesting. If we've done
	 * that, then we may not actually need to do the end here. Essentially
	 * we only want to actually do a pop if the tag matches what we expect.
	 */
	if (strcmp(fz_xml_tag(dev->current), "CLIP_PATH") != 0 &&
		strcmp(fz_xml_tag(dev->current), "CLIP_STROKE_PATH") != 0 &&
		strcmp(fz_xml_tag(dev->current), "CLIP_MASK") != 0)
		return;

	dev->current = fz_dom_parent(ctx, dev->current);
}

static void
struct_gather_begin_mask(fz_context *ctx, fz_device *dev, fz_rect area, int luminosity, fz_colorspace *cs, const float *bc, fz_color_params cp)
{
	do_begin(ctx, dev, "MASK");
}

static void
struct_gather_end_mask(fz_context *ctx, fz_device *dev, fz_function *fn)
{
	do_end(ctx, dev, "MASK");
	do_begin(ctx, dev, "CLIP_MASK");
}

static void
struct_gather_begin_group(fz_context *ctx, fz_device *dev, fz_rect area, fz_colorspace *cs, int isolated, int knockout, int blendmode, float alpha)
{
	char text[32];
	fz_xml *node = do_begin(ctx, dev, "GROUP");

	if (isolated)
		fz_dom_add_attribute(ctx, node, "i", "1");
	if (knockout)
		fz_dom_add_attribute(ctx, node, "k", "1");
	if (blendmode)
	{
		sprintf(text, "%d", blendmode);
		fz_dom_add_attribute(ctx, node, "bm", text);
	}
	if (alpha != 1)
	{
		sprintf(text, "%f", alpha);
		fz_dom_add_attribute(ctx, node, "alpha", text);
	}
}

static void
struct_gather_end_group(fz_context *ctx, fz_device *dev)
{
	do_end(ctx, dev, "GROUP");
}

static int
struct_gather_begin_tile(fz_context *ctx, fz_device *dev, fz_rect area, fz_rect view, float xstep, float ystep, fz_matrix ctm, int id)
{
	do_begin(ctx, dev, "TILE");
	return 0;
}

static void
struct_gather_end_tile(fz_context *ctx, fz_device *dev)
{
	do_end(ctx, dev, "TILE");
}

static void
struct_gather_begin_layer(fz_context *ctx, fz_device *dev, const char *layer_name)
{
	fz_xml *node = do_begin(ctx, dev, "LAYER");

	if (layer_name)
		fz_dom_add_attribute(ctx, node, "layer", layer_name);
}

static void
struct_gather_end_layer(fz_context *ctx, fz_device *dev)
{
	do_end(ctx, dev, "LAYER");
}

static void
struct_gather_begin_metatext(fz_context *ctx, fz_device *dev, fz_metatext meta, const char *text)
{
	fz_xml *node = do_begin(ctx, dev, "METATEXT");
	char buf[32];

	sprintf(buf, "%d", meta);
	fz_dom_add_attribute(ctx, node, "meta", buf);
	fz_dom_add_attribute(ctx, node, "text", text);
}

static void
struct_gather_end_metatext(fz_context *ctx, fz_device *dev)
{
	do_end(ctx, dev, "METATEXT");
}

fz_device *
fz_new_struct_gatherer(fz_context *ctx)
{
	fz_struct_gatherer *dev = fz_new_derived_device(ctx, fz_struct_gatherer);

	//dev->base.close_device = NULL;
	//dev->base.drop_device = NULL;
	dev->base.fill_text = struct_gather_fill_text;
	dev->base.stroke_text = struct_gather_stroke_text;
	dev->base.clip_text = struct_gather_clip_text;
	dev->base.ignore_text = struct_gather_ignore_text;

	dev->base.fill_path = struct_gather_fill_path;
	dev->base.stroke_path = struct_gather_stroke_path;
	dev->base.clip_path = struct_gather_clip_path;
	dev->base.clip_stroke_path = struct_gather_clip_stroke_path;

	dev->base.fill_shade = struct_gather_fill_shade;
	dev->base.fill_image = struct_gather_fill_image;
	dev->base.fill_image_mask = struct_gather_fill_image_mask;
	dev->base.clip_image_mask = struct_gather_clip_image_mask;

	dev->base.pop_clip = struct_gather_pop_clip;

	dev->base.begin_mask = struct_gather_begin_mask;
	dev->base.end_mask = struct_gather_end_mask;
	dev->base.begin_group = struct_gather_begin_group;
	dev->base.end_group = struct_gather_end_group;

	dev->base.begin_tile = struct_gather_begin_tile;
	dev->base.end_tile = struct_gather_end_tile;

	//void (*render_flags)(fz_context *, fz_device *, int set, int clear);
	//void (*set_default_colorspaces)(fz_context *, fz_device *, fz_default_colorspaces *);

	dev->base.begin_layer = struct_gather_begin_layer;
	dev->base.end_layer = struct_gather_end_layer;

	dev->base.begin_structure = struct_gather_begin_structure;
	dev->base.end_structure = struct_gather_end_structure;

	dev->base.begin_metatext = struct_gather_begin_metatext;
	dev->base.end_metatext = struct_gather_end_metatext;

	fz_try(ctx)
	{
		dev->root = fz_new_dom(ctx, "ROOT");
		dev->current = dev->root;
	}
	fz_catch(ctx)
	{
		fz_drop_device(ctx, &dev->base);
		fz_rethrow(ctx);
	}

	return (fz_device *)dev;
}

static void
pdf_structure_to_xml(fz_context *ctx, pdf_document *doc, const char *tagfile, int pages, int indent)
{
	fz_device *dev = fz_new_struct_gatherer(ctx);

	fz_run_document_structure(ctx, (fz_document *)doc, dev, NULL);

	if (pages)
	{
		int i, n = pdf_count_pages(ctx, doc);
		pdf_page *page = NULL;

		fz_var(page);

		fz_try(ctx)
		{
			for (i = 0; i < n; i++)
			{
				page = pdf_load_page(ctx, doc, i);
				fz_run_page(ctx, (fz_page *)page, dev, fz_identity);
				pdf_drop_page(ctx, page);
				page = NULL;
			}
		}
		fz_catch(ctx)
		{
			pdf_drop_page(ctx, page);
			fz_rethrow(ctx);
		}
	}

	fz_save_xml(ctx, ((fz_struct_gatherer *)dev)->root, tagfile, indent);

	fz_drop_device(ctx, dev);
}

int pdftag_main(int argc, const char **argv)
{
	const char *infile;
	const char *tagfile = NULL;
	const char *password = "";
	int c;
	pdf_document *doc = NULL;
	fz_context *ctx = NULL;
	int ret = 0;
	int contents = 0;
	int indent = 0;

	while ((c = fz_getopt(argc, argv, "icf:p:")) != -1)
	{
		switch (c)
		{
		case 'p': password = fz_optarg; break;
		case 'f': tagfile = fz_optarg; break;
		case 'c': contents = 1; break;
		case 'i': indent = 1; break;
		default: return usage();
		}
	}

	if (argc - fz_optind < 1)
		return usage();

	infile = argv[fz_optind++];

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fprintf(stderr, "cannot initialise context\n");
		return 1;
	}

	fz_var(doc);

	fz_try(ctx)
	{
		if (tagfile == NULL)
			fz_throw(ctx, FZ_ERROR_ARGUMENT, "Must specify a tag file");

		doc = pdf_open_document(ctx, infile);
		if (pdf_needs_password(ctx, doc))
			if (!pdf_authenticate_password(ctx, doc, password))
				fz_throw(ctx, FZ_ERROR_ARGUMENT, "cannot authenticate password: %s", infile);

		pdf_structure_to_xml(ctx, doc, tagfile, contents, indent);
	}
	fz_always(ctx)
	{
		pdf_drop_document(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_report_error(ctx);
		ret = 1;
	}

	fz_flush_warnings(ctx);
	fz_drop_context(ctx);

	return ret;
}

#endif
