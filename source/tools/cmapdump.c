/* cmapdump.c -- parse a CMap file and dump it as a c-struct */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/helpers/jmemcust.h"

#include <stdio.h>
#include <string.h>

static fz_context *ctx = NULL;

static void
clean(char *p)
{
	while (*p)
	{
		if ((*p == '/') || (*p == '.') || (*p == '\\') || (*p == '-'))
			*p = '_';
		p ++;
	}
}

static void usage(void)
{
	fz_info(ctx, "usage: mutool cmapdump [lots of cmap files] > out.c");
}

static void mu_drop_context(void)
{
	// WARNING: as we point `ctx` at the GLOBAL context in the app init phase, it MAY already be an INVALID
	// pointer reference by now!
	// 
	// WARNING: this assert fires when you run `mutool raster` (and probably other tools as well) and hit Ctrl+C
	// to ABORT/INTERRUPT the running application: the MSVC RTL calls this function in the atexit() handler
	// and the assert fires due to (ctx->error.top != ctx->error.stack).
	//
	// We are okay with that, as that scenario is an immediate abort anyway and the OS will be responsible
	// for cleaning up. That our fz_try/throw/catch exception stack hasn't been properly rewound at such times
	// is obvious, I suppose...
	ASSERT_AND_CONTINUE(!ctx || !fz_has_global_context() || (ctx->error.top == ctx->error.stack_base));

	if (ctx != __fz_get_RAW_global_context())
	{
		fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	}
	ctx = NULL;
}

int
cmapdump_main(int argc, const char** argv)
{
	pdf_cmap *cmap;
	fz_stream *fi;
	char name[256];
	int k, c;

	//ctx = NULL;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "h")) != -1)
	{
		switch (c)
		{
		default: usage(); return EXIT_FAILURE;
		}
	}
	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		usage();
		return EXIT_FAILURE;
	}

	ASSERT(fz_has_global_context());
	if (!fz_has_global_context())
	{
		ASSERT(ctx == NULL);
		
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}

	// registeer a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	atexit(mu_drop_context);

	if (ctx != __fz_get_RAW_global_context())
	{
		fz_drop_context(ctx); // drop our previous context IFF this happens to be a re-run in monolithic mode.
		ctx = NULL;
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_output* out = NULL;
	fz_try(ctx)
	{
		out = fz_stdout(ctx);

		fz_write_printf(ctx, out, "/* This is an automatically generated file. Do not edit. */\n");

		while (fz_optind < argc)
		{
			const char* filename = argv[fz_optind++];
			fi = fz_open_file(ctx, filename);
			cmap = pdf_load_cmap(ctx, fi);
			fz_drop_stream(ctx, fi);

			strcpy(name, cmap->cmap_name);
			clean(name);

			fz_write_printf(ctx, out, "\n/* %s */\n\n", cmap->cmap_name);

			if (cmap->rlen)
			{
				fz_write_printf(ctx, out, "static const pdf_range cmap_%s_ranges[] = {", name);
				for (k = 0; k < cmap->rlen; k++)
				{
					fz_write_printf(ctx, out, "\n{%u,%u,%u},", cmap->ranges[k].low, cmap->ranges[k].high, cmap->ranges[k].out);
				}
				fz_write_printf(ctx, out, "\n};\n\n");
			}

			if (cmap->xlen)
			{
				fz_write_printf(ctx, out, "static const pdf_xrange cmap_%s_xranges[] = {", name);
				for (k = 0; k < cmap->xlen; k++)
				{
					fz_write_printf(ctx, out, "\n{%u,%u,%u},", cmap->xranges[k].low, cmap->xranges[k].high, cmap->xranges[k].out);
				}
				fz_write_printf(ctx, out, "\n};\n\n");
			}

			if (cmap->mlen > 0)
			{
				fz_write_printf(ctx, out, "static const pdf_mrange cmap_%s_mranges[] = {", name);
				for (k = 0; k < cmap->mlen; k++)
				{
					fz_write_printf(ctx, out, "\n{%u,%u},", cmap->mranges[k].low, cmap->mranges[k].out);
				}
				fz_write_printf(ctx, out, "\n};\n\n");
			}

			if (cmap->dlen > 0)
			{
				int ti = 0, tn = cmap->dict[0];
				fz_write_printf(ctx, out, "static const int cmap_%s_table[] = {\n", name);
				for (k = 0; k < cmap->dlen; k++)
				{
					if (ti > tn)
					{
						fz_write_printf(ctx, out, "\n");
						ti = 1;
						tn = cmap->dict[k];
					}
					else
						++ti;
					fz_write_printf(ctx, out, "%u,", cmap->dict[k]);
				}
				fz_write_printf(ctx, out, "\n};\n\n");
			}

			fz_write_printf(ctx, out, "static pdf_cmap cmap_%s = {\n", name);
			fz_write_printf(ctx, out, "\t{ -1, pdf_drop_cmap_imp },\n");
			fz_write_printf(ctx, out, "\t/* cmapname */ \"%s\",\n", cmap->cmap_name);
			fz_write_printf(ctx, out, "\t/* usecmap */ \"%s\", NULL,\n", cmap->usecmap_name);
			fz_write_printf(ctx, out, "\t/* wmode */ %u,\n", cmap->wmode);
			fz_write_printf(ctx, out, "\t/* codespaces */ %u, {\n", cmap->codespace_len);
			if (cmap->codespace_len == 0)
			{
				fz_write_printf(ctx, out, "\t\t{ 0, 0, 0 },\n");
			}
			for (k = 0; k < cmap->codespace_len; k++)
			{
				int n = cmap->codespace[k].n;
				fz_write_printf(ctx, out, "\t\t{ %u, 0x%0*x, 0x%0*x },\n", n,
					n * 2, cmap->codespace[k].low,
					n * 2, cmap->codespace[k].high);
			}
			fz_write_printf(ctx, out, "\t},\n");

			if (cmap->rlen)
				fz_write_printf(ctx, out, "\t%u, %u, (pdf_range*)cmap_%s_ranges,\n", cmap->rlen, cmap->rlen, name);
			else
				fz_write_printf(ctx, out, "\t0, 0, NULL, /* ranges */\n");
			if (cmap->xlen)
				fz_write_printf(ctx, out, "\t%u, %u, (pdf_xrange*)cmap_%s_xranges,\n", cmap->xlen, cmap->xlen, name);
			else
				fz_write_printf(ctx, out, "\t0, 0, NULL, /* xranges */\n");
			if (cmap->mlen)
				fz_write_printf(ctx, out, "\t%u, %u, (pdf_mrange*)cmap_%s_mranges,\n", cmap->mlen, cmap->mlen, name);
			else
				fz_write_printf(ctx, out, "\t0, 0, NULL, /* mranges */\n");
			if (cmap->dict)
				fz_write_printf(ctx, out, "\t%u, %u, (int*)cmap_%s_table,\n", cmap->dlen, cmap->dlen, name);
			else
				fz_write_printf(ctx, out, "\t0, 0, NULL, /* table */\n");
			fz_write_printf(ctx, out, "\t0, 0, 0, NULL /* splay tree */\n");
			fz_write_printf(ctx, out, "};\n");

			pdf_drop_cmap(ctx, cmap);
		}

		fz_close_output(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "fatal: %s\n", fz_caught_message(ctx));
	}
	mu_drop_context();
	return EXIT_SUCCESS;
}
