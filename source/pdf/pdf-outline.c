#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

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
pdf_load_outline_imp(fz_context *ctx, pdf_document *doc, pdf_obj *dict, pdf_obj *mark_list, fz_outline* grandparent, fz_outline* parent, int depth, int corrected_depth, fz_outline_summary* outline_summary)
{
	fz_outline *node, **prev, *first = NULL;
	pdf_obj *obj;

	fz_var(first);

	if (outline_summary->hierarchy_levels < depth)
		outline_summary->hierarchy_levels = depth;
	if (outline_summary->hierarchy_levels_after_repair < corrected_depth)
		outline_summary->hierarchy_levels_after_repair = corrected_depth;

	fz_try(ctx)
	{
		prev = &first;
		while (dict && pdf_is_dict(ctx, dict))
		{
			if (pdf_mark_obj(ctx, dict))
				break;
			pdf_array_push(ctx, mark_list, dict);

			node = fz_new_outline(ctx);
			*prev = node;
			prev = &node->next;

			outline_summary->total_item_count++;
			outline_summary->total_item_count_after_repair++;

			obj = pdf_dict_get(ctx, dict, PDF_NAME(Title));
			if (obj)
				node->title = Memento_label(fz_strdup(ctx, pdf_to_text_string(ctx, obj)), "outline_title");

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

					parent->is_repaired = 1;

					outline_summary->is_repaired = 1;
					outline_summary->total_item_count_after_repair--;

					depth++;   // tail recursion: the original depth would go +1 now!
					if (outline_summary->hierarchy_levels < depth)
						outline_summary->hierarchy_levels = depth;

					continue;
				}

				node->down = pdf_load_outline_imp(ctx, doc, obj, mark_list, parent, node, depth + 1, corrected_depth + 1, outline_summary);

				obj = pdf_dict_get(ctx, dict, PDF_NAME(Count));
				if (pdf_to_int(ctx, obj) > 0)
					node->is_open = 1;
			}

			dict = next_dict;
		}
	}
	fz_catch(ctx)
	{
		fz_drop_outline(ctx, first);
		fz_rethrow(ctx);
	}

	return first;
}

fz_outline *
pdf_load_outline(fz_context *ctx, pdf_document *doc, fz_outline_summary* outline_summary)
{
	pdf_obj *root, *obj, *first, *mark_list;
	fz_outline *outline = NULL;
	int i;
	fz_outline_summary alt_summary;
	
	// always have a summary block, even if the user didn't provide us with one:
	if (!outline_summary)
		outline_summary = &alt_summary;
	memset(outline_summary, 0, sizeof(*outline_summary));

	mark_list = pdf_new_array(ctx, doc, 100);
	fz_try(ctx)
	{
		root = pdf_dict_get(ctx, pdf_trailer(ctx, doc), PDF_NAME(Root));
		obj = pdf_dict_get(ctx, root, PDF_NAME(Outlines));
		first = pdf_dict_get(ctx, obj, PDF_NAME(First));
		if (first)
		{
			/* cache page tree for fast link destination lookups */
			pdf_load_page_tree(ctx, doc);
			fz_try(ctx)
			outline = pdf_load_outline_imp(ctx, doc, mark_list, first, NULL, NULL, 1, 1, outline_summary);
			fz_always(ctx)
				pdf_drop_page_tree(ctx, doc);
			fz_catch(ctx)
				fz_rethrow(ctx);
		}
	}
	fz_always(ctx)
	{
		for (i = 0; i < pdf_array_len(ctx, mark_list); ++i)
			pdf_unmark_obj(ctx, pdf_array_get(ctx, mark_list, i));
		pdf_drop_obj(ctx, mark_list);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}

	return outline;
}
