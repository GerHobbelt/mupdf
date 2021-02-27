#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#include "annot-imp.h"

#include <string.h>

static int are_outline_nodes_equal(fz_outline* a, fz_outline* b)
{
	if (!a || !b)
		return 0;
	const char* ta = a->title;
	const char* tb = b->title;
	const char* ua = a->uri;
	const char* ub = b->uri;
	return (a->page == b->page &&
		!strcmp(ta ? ta : "", tb ? tb : "") &&
		!strcmp(ua ? ua : "", ub ? ub : ""));
}

static fz_outline *
pdf_load_outline_imp(fz_context *ctx, pdf_document *doc, pdf_obj *dict, fz_outline* grandparent, fz_outline* parent, int depth)
{
	fz_outline *node, **prev, *first = NULL;
	pdf_obj *obj;
	pdf_obj *odict = dict;

	fz_var(dict);
	fz_var(first);

	fz_try(ctx)
	{
		prev = &first;
		while (dict && pdf_is_dict(ctx, dict))
		{
			if (pdf_mark_obj(ctx, dict))
				break;
			node = fz_new_outline(ctx);
			*prev = node;
			prev = &node->next;

			obj = pdf_dict_get(ctx, dict, PDF_NAME(Title));
			if (obj)
				node->title = Memento_label(fz_strdup(ctx, pdf_to_text_string(ctx, obj, NULL)), "outline_title");

			if ((obj = pdf_dict_get(ctx, dict, PDF_NAME(Dest))) != NULL)
				node->uri = Memento_label(pdf_parse_link_dest(ctx, doc, obj), "outline_uri");
			else if ((obj = pdf_dict_get(ctx, dict, PDF_NAME(A))) != NULL)
				node->uri = Memento_label(pdf_parse_link_action(ctx, doc, obj, -1), "outline_uri");
			else
				node->uri = NULL;

			if (node->uri && !fz_is_external_link(ctx, node->uri))
				node->page = pdf_resolve_link(ctx, doc, node->uri, &node->x, &node->y);
			else
				node->page = -1;

			obj = pdf_dict_get(ctx, dict, PDF_NAME(First));
			pdf_obj* next_dict = pdf_dict_get(ctx, dict, PDF_NAME(Next));
			pdf_obj* last_obj = pdf_dict_get(ctx, dict, PDF_NAME(Last));
			if (obj)
			{
				int d1 = pdf_to_num(ctx, obj);
				int d2 = pdf_to_num(ctx, last_obj);

				// when this node has the same datums as its parent AND its grandparent AND it has only a single child,
				// then we declare this node *superfluous* and do NOT include it in the outline.
				//
				// This specifically folds the > 260(!) levels deep outline of https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4835977/
				// but is hoped to be both more generic and a potential fix for malicious PDFs which try to
				// exercise our call and exception stacks.
				//
				// We do not recurse then as this would be a pure tail call.
				if (d1 == d2 &&
					node == first &&
					are_outline_nodes_equal(parent, node) &&
					are_outline_nodes_equal(parent, grandparent))
				{
					// discard `node`
					for (dict = odict; dict && pdf_obj_marked(ctx, dict); dict = pdf_dict_get(ctx, dict, PDF_NAME(Next)))
						pdf_unmark_obj(ctx, dict);
					fz_drop_outline(ctx, first);
					first = NULL;
					odict = dict = obj;
					prev = &first;
					continue;
				}

				node->down = pdf_load_outline_imp(ctx, doc, obj, parent, node, depth + 1);

				obj = pdf_dict_get(ctx, dict, PDF_NAME(Count));
				if (pdf_to_int(ctx, obj) > 0)
					node->is_open = 1;
			}

			dict = next_dict;
		}
	}
	fz_always(ctx)
	{
		for (dict = odict; dict && pdf_obj_marked(ctx, dict); dict = pdf_dict_get(ctx, dict, PDF_NAME(Next)))
			pdf_unmark_obj(ctx, dict);
	}
	fz_catch(ctx)
	{
		fz_drop_outline(ctx, first);
		fz_rethrow(ctx);
	}

	return first;
}

fz_outline *
pdf_load_outline(fz_context *ctx, pdf_document *doc)
{
	pdf_obj *root, *obj, *first;
	fz_outline *outline = NULL;

	root = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Root));
	obj = pdf_dict_get(ctx, root, PDF_NAME(Outlines));
	first = pdf_dict_get(ctx, obj, PDF_NAME(First));
	if (first)
	{
		/* cache page tree for fast link destination lookups */
		pdf_load_page_tree(ctx, doc);
		fz_try(ctx)
			outline = pdf_load_outline_imp(ctx, doc, first, NULL, NULL, 0);
		fz_always(ctx)
			pdf_drop_page_tree(ctx, doc);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}

	return outline;
}
