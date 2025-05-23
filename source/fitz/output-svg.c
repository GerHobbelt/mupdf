// Copyright (C) 2004-2024 Artifex Software, Inc.
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
#include "mupdf/helpers/dir.h"

#include <limits.h>

#if FZ_ENABLE_RENDER_CORE 

typedef struct
{
	fz_document_writer super;
	char *path;
	int count;
	fz_output *out;
	int id;
	fz_svg_options opts;
} fz_svg_writer;

const char *fz_svg_write_options_usage =
	"SVG output options:\n"
	"  text=text: Emit text as <text> elements (inaccurate fonts).\n"
	"  text=path: Emit text as <path> elements (accurate fonts).\n"
	"  no-reuse-images: Do not reuse images using <symbol> definitions.\n"
	"\n"
	;

fz_svg_options *
fz_parse_svg_options(fz_context *ctx, fz_svg_options *opts, const char *options)
{
	const char *val;

	memset(opts, 0, sizeof *opts);

	if (fz_has_option(ctx, options, "text", &val))
	{
		if (fz_option_eq(val, "text"))
			opts->text_format = FZ_SVG_TEXT_AS_TEXT;
		else if (fz_option_eq(val, "path"))
			opts->text_format = FZ_SVG_TEXT_AS_PATH;
	}
	if (fz_has_option(ctx, options, "no-reuse-images", &val))
		if (fz_option_eq(val, "yes"))
			opts->reuse_images = 0;

	return opts;
}

static fz_device *
svg_begin_page(fz_context *ctx, fz_document_writer *wri_, fz_rect mediabox)
{
	fz_svg_writer *wri = (fz_svg_writer*)wri_;
	char path[PATH_MAX];

	float w = mediabox.x1 - mediabox.x0;
	float h = mediabox.y1 - mediabox.y0;

	wri->count += 1;

	if (wri->path)
	{
		fz_format_output_path(ctx, path, sizeof path, wri->path, wri->count);
		fz_normalize_path(ctx, path, sizeof path, path);
		fz_sanitize_path(ctx, path, sizeof path, path);
		wri->out = fz_new_output_with_path(ctx, path, 0);
	}
	else
	{
		if (!wri->out)
			fz_throw(ctx, FZ_ERROR_ARGUMENT, "cannot write multiple pages to a single SVG output");
	}

	return fz_new_svg_device_with_id(ctx, wri->out, w, h, wri->opts.text_format, wri->opts.reuse_images, &wri->id);
}

static void
svg_end_page(fz_context *ctx, fz_document_writer *wri_, fz_device *dev)
{
	fz_svg_writer *wri = (fz_svg_writer*)wri_;

	fz_try(ctx)
	{
		fz_close_device(ctx, dev);
		fz_close_output(ctx, wri->out);
	}
	fz_always(ctx)
	{
		fz_drop_device(ctx, dev);
		fz_drop_output(ctx, wri->out);
		wri->out = NULL;
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void
svg_drop_writer(fz_context *ctx, fz_document_writer *wri_)
{
	fz_svg_writer *wri = (fz_svg_writer*)wri_;
	fz_drop_output(ctx, wri->out);
	fz_free(ctx, wri->path);
}

fz_document_writer *
fz_new_svg_writer(fz_context *ctx, const char *path, const char *options)
{
	fz_svg_writer *wri = fz_new_derived_document_writer(ctx, fz_svg_writer, svg_begin_page, svg_end_page, NULL, svg_drop_writer);

	fz_try(ctx)
	{
		fz_parse_svg_options(ctx, &wri->opts, options);
		if (!wri->opts.text_format)
			wri->opts.text_format = FZ_SVG_TEXT_AS_PATH;
		if (!wri->opts.reuse_images)
			wri->opts.reuse_images = 1;
		wri->path = fz_strdup(ctx, path ? path : "out-%04d.svg");
	}
	fz_catch(ctx)
	{
		fz_free(ctx, wri);
		fz_rethrow(ctx);
	}

	return (fz_document_writer*)wri;
}

fz_document_writer *
fz_new_svg_writer_with_output(fz_context *ctx, fz_output *out, const char *options)
{
	fz_svg_writer *wri = fz_new_derived_document_writer(ctx, fz_svg_writer, svg_begin_page, svg_end_page, NULL, svg_drop_writer);

	fz_try(ctx)
	{
		fz_parse_svg_options(ctx, &wri->opts, options);
		if (!wri->opts.text_format)
			wri->opts.text_format = FZ_SVG_TEXT_AS_PATH;
		if (!wri->opts.reuse_images)
			wri->opts.reuse_images = 1;
		wri->out = out;
	}
	fz_catch(ctx)
	{
		fz_free(ctx, wri);
		fz_rethrow(ctx);
	}

	return (fz_document_writer*)wri;
}

#endif
