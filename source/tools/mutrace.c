#include "mupdf/fitz.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static fz_context* ctx = NULL;

static void usage(void)
{
	fz_info(ctx,
		"Usage: mutool trace [options] file [pages]\n"
		"\t-o -\toutput file name (default is stdout)\n"
		"\t-p -\tpassword\n"
		"\n"
		"\t-W -\tpage width for EPUB layout\n"
		"\t-H -\tpage height for EPUB layout\n"
		"\t-S -\tfont size for EPUB layout\n"
		"\t-U -\tfile name of user stylesheet for EPUB layout\n"
		"\t-X\tdisable document styles for EPUB layout\n"
		"\n"
		"\t-d\tuse display list\n"
		"\n"
		"\tpages\tcomma separated list of page numbers and ranges\n"
	);
}

static float layout_w = FZ_DEFAULT_LAYOUT_W;
static float layout_h = FZ_DEFAULT_LAYOUT_H;
static float layout_em = FZ_DEFAULT_LAYOUT_EM;
static const char *layout_css = NULL;
static int layout_use_doc_css = 1;

static int use_display_list = 0;

static void runpage(fz_context *ctx, fz_document *doc, fz_output *out, int number)
{
	fz_page *page = NULL;
	fz_display_list *list = NULL;
	fz_device *dev = NULL;
	fz_rect mediabox;

	fz_var(page);
	fz_var(list);
	fz_var(dev);
	fz_try(ctx)
	{
		page = fz_load_page(ctx, doc, number - 1);
		mediabox = fz_bound_page(ctx, page);
		fz_write_printf(ctx, out, "<page number=\"%d\" mediabox=\"%g %g %g %g\">\n",
				number, mediabox.x0, mediabox.y0, mediabox.x1, mediabox.y1);
		dev = fz_new_trace_device(ctx, out);
		if (use_display_list)
		{
			list = fz_new_display_list_from_page(ctx, page);
			fz_run_display_list(ctx, list, dev, fz_identity, fz_infinite_rect, NULL);
		}
		else
		{
			fz_run_page(ctx, page, dev, fz_identity, NULL);
		}
		fz_write_printf(ctx, out, "</page>\n");
	}
	fz_always(ctx)
	{
		fz_flush_output(ctx, out);
		fz_drop_display_list(ctx, list);
		fz_drop_page(ctx, page);
		fz_drop_device(ctx, dev);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void runrange(fz_context *ctx, fz_document *doc, int count, fz_output *out, const char *range)
{
	int start, end, i;

	while ((range = fz_parse_page_range(ctx, range, &start, &end, count)))
	{
		if (start < end)
			for (i = start; i <= end; ++i)
				runpage(ctx, doc, out, i);
		else
			for (i = start; i >= end; --i)
				runpage(ctx, doc, out, i);
	}
}

int mutrace_main(int argc, const char **argv)
{
	fz_document *doc = NULL;
	fz_output* out = NULL;
	const char *password = "";
	const char* output = NULL;
	int i, c, count;
	int errored = 0;

	ctx = NULL;

	layout_w = FZ_DEFAULT_LAYOUT_W;
	layout_h = FZ_DEFAULT_LAYOUT_H;
	layout_em = FZ_DEFAULT_LAYOUT_EM;
	layout_css = NULL;
	layout_use_doc_css = 1;

	use_display_list = 0;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:p:W:H:S:U:Xd")) != -1)
	{
		switch (c)
		{
		default: usage(); return EXIT_FAILURE;
		case 'o': output = fz_optarg; break;
		case 'p': password = fz_optarg; break;

		case 'W': layout_w = fz_atof(fz_optarg); break;
		case 'H': layout_h = fz_atof(fz_optarg); break;
		case 'S': layout_em = fz_atof(fz_optarg); break;
		case 'U': layout_css = fz_optarg; break;
		case 'X': layout_use_doc_css = 0; break;

		case 'd': use_display_list = 1; break;
		}
	}

	if (fz_optind == argc)
	{
		usage();
		return EXIT_FAILURE;
	}

	if (!fz_has_global_context())
	{
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_try(ctx)
	{
		fz_register_document_handlers(ctx);
		if (layout_css)
		{
			fz_buffer *buf = fz_read_file(ctx, layout_css);
			fz_set_user_css(ctx, fz_string_from_buffer(ctx, buf));
			fz_drop_buffer(ctx, buf);
		}
		fz_set_use_document_css(ctx, layout_use_doc_css);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "cannot initialize mupdf: %s",  fz_caught_message(ctx));
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	fz_var(doc);
	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
			out = fz_stdout(ctx);
		else
		{
			char fbuf[4096];
			fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		for (i = fz_optind; i < argc; ++i)
		{
			doc = fz_open_document(ctx, argv[i]);
			if (fz_needs_password(ctx, doc))
				if (!fz_authenticate_password(ctx, doc, password))
					fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", argv[i]);
			fz_layout_document(ctx, doc, layout_w, layout_h, layout_em);
			fz_write_printf(ctx, out, "<document filename=\"%s\">\n", argv[i]);
			count = fz_count_pages(ctx, doc);
			if (i+1 < argc && fz_is_page_range(ctx, argv[i+1]))
				runrange(ctx, doc, count, out, argv[++i]);
			else
				runrange(ctx, doc, count, out, "1-N");
			fz_write_printf(ctx, out, "</document>\n");
			fz_drop_document(ctx, doc);
			doc = NULL;
			fz_flush_output(ctx, out);
		}
	}
	fz_always(ctx)
	{
		fz_close_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "cannot run document: %s", fz_caught_message(ctx));
		fz_drop_document(ctx, doc);
		errored = 1;
	}

	fz_drop_output(ctx, out);
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return errored;
}
