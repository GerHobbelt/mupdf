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
 * pdfextract -- the ultimate way to extract images and fonts from pdfs
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/jmemcust.h"

#include <stdlib.h>
#include <stdio.h>

#if FZ_ENABLE_RENDER_CORE 

static pdf_document *doc = NULL;
static fz_context *ctx = NULL;
static int dorgb = 0;
static int doalpha = 0;
static int doicc = 1;
static const char *output_template_path = "";
static int count = 0;

static int usage(void)
{
	fz_info(ctx,
		"usage: mutool extract [options] file.pdf [object numbers]\n"
		"\t-o -\toutput files name template: generated filenames are appended\n"
		"\t-p\tpassword\n"
		"\t-r\tconvert images to rgb\n"
        "\t-a\tembed SMasks as alpha channel\n"
		"\t-N\tdo not use ICC color conversions\n");

	return EXIT_FAILURE;
}

static int isimage(pdf_obj *obj)
{
	pdf_obj *type = pdf_dict_get(ctx, obj, PDF_NAME(Subtype));
	return pdf_name_eq(ctx, type, PDF_NAME(Image));
}

static int isfontdesc(pdf_obj *obj)
{
	pdf_obj *type = pdf_dict_get(ctx, obj, PDF_NAME(Type));
	return pdf_name_eq(ctx, type, PDF_NAME(FontDescriptor));
}

static void writepixmap(fz_pixmap *pix, const char *file)
{
	char buf[PATH_MAX];
	fz_pixmap *rgb = NULL;

	if (!pix)
		return;

	if (dorgb && pix->colorspace && pix->colorspace != fz_device_rgb(ctx))
	{
		rgb = fz_convert_pixmap(ctx, pix, fz_device_rgb(ctx), NULL, NULL, fz_default_color_params /* FIXME */, 1);
		pix = rgb;
	}

	if (!pix->colorspace || pix->colorspace->type == FZ_COLORSPACE_GRAY || pix->colorspace->type == FZ_COLORSPACE_RGB)
	{
		fz_snprintf(buf, sizeof(buf), "%s%s.png", output_template_path, file);
		fz_info(ctx, "extracting %s\n", buf);
		fz_save_pixmap_as_png(ctx, pix, buf);
	}
	else
	{
		fz_snprintf(buf, sizeof(buf), "%s%s.pam", output_template_path, file);
		fz_info(ctx, "extracting %s\n", buf);
		fz_save_pixmap_as_pam(ctx, pix, buf);
	}

	fz_drop_pixmap(ctx, rgb);
}

static void
writejpeg(const unsigned char *data, size_t len, const char *file)
{
	char buf[PATH_MAX];
	fz_output *out;

	fz_snprintf(buf, sizeof(buf), "%s%s.jpg", output_template_path, file);

	out = fz_new_output_with_path(ctx, buf, 0);
	fz_try(ctx)
	{
		fz_info(ctx, "extracting %s\n", buf);
		fz_write_data(ctx, out, data, len);
		fz_close_output(ctx, out);
	}
	fz_always(ctx)
		fz_drop_output(ctx, out);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void saveimage(pdf_obj *ref)
{
	fz_image *image = NULL;
	fz_pixmap *pix = NULL;
	fz_pixmap *mask = NULL;
	char buf[40];
	fz_compressed_buffer *cbuf;
	int type;

	fz_var(image);
	fz_var(pix);

	fz_try(ctx)
	{
		image = pdf_load_image(ctx, doc, ref);
		cbuf = fz_compressed_image_buffer(ctx, image);
		fz_snprintf(buf, sizeof(buf), "image-%04d-%04d", pdf_to_num(ctx, ref), ++count);
		type = cbuf == NULL ? FZ_IMAGE_UNKNOWN : cbuf->params.type;

		if (image->use_colorkey)
			type = FZ_IMAGE_UNKNOWN;
		if (image->use_decode)
			type = FZ_IMAGE_UNKNOWN;
		if (image->mask)
			type = FZ_IMAGE_UNKNOWN;
		if (dorgb)
		{
			enum fz_colorspace_type ctype = fz_colorspace_type(ctx, image->colorspace);
			if (ctype != FZ_COLORSPACE_RGB && ctype != FZ_COLORSPACE_GRAY)
				type = FZ_IMAGE_UNKNOWN;
		}

		if (type == FZ_IMAGE_JPEG)
		{
			unsigned char *data;
			size_t len = fz_buffer_storage(ctx, cbuf->buffer, &data);
			writejpeg(data, len, buf);
		}
		else
		{
			pix = fz_get_pixmap_from_image(ctx, image, NULL, NULL, 0, 0);
			if (image->mask && doalpha)
			{
				mask = fz_get_pixmap_from_image(ctx, image->mask, NULL, NULL, 0, 0);
				if (mask->w == pix->w && mask->h == pix->h)
				{
					fz_pixmap *apix = fz_new_pixmap_from_color_and_mask(ctx, pix, mask);
					fz_drop_pixmap(ctx, pix);
					pix = apix;
				}
				else
				{
					fz_warn(ctx, "cannot combine image with smask if different resolution");
				}
			}
			writepixmap(pix, buf);
		}
	}
	fz_always(ctx)
	{
		fz_drop_image(ctx, image);
		fz_drop_pixmap(ctx, mask);
		fz_drop_pixmap(ctx, pix);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void savefont(pdf_obj *dict)
{
	char namebuf[PATH_MAX];
	fz_buffer *buf;
	pdf_obj *stream = NULL;
	pdf_obj *obj;
	char *ext = "";
	fz_output *out;
	size_t len;
	unsigned char *data;

	obj = pdf_dict_get(ctx, dict, PDF_NAME(FontFile));
	if (obj)
	{
		stream = obj;
		ext = "pfa";
	}

	obj = pdf_dict_get(ctx, dict, PDF_NAME(FontFile2));
	if (obj)
	{
		stream = obj;
		ext = "ttf";
	}

	obj = pdf_dict_get(ctx, dict, PDF_NAME(FontFile3));
	if (obj)
	{
		stream = obj;

		obj = pdf_dict_get(ctx, obj, PDF_NAME(Subtype));
		if (obj && !pdf_is_name(ctx, obj))
			fz_throw(ctx, FZ_ERROR_GENERIC, "invalid font descriptor subtype");

		if (pdf_name_eq(ctx, obj, PDF_NAME(Type1C)))
			ext = "cff";
		else if (pdf_name_eq(ctx, obj, PDF_NAME(CIDFontType0C)))
			ext = "cid";
		else if (pdf_name_eq(ctx, obj, PDF_NAME(OpenType)))
			ext = "otf";
		else
			fz_throw(ctx, FZ_ERROR_GENERIC, "unhandled font type '%s'", pdf_to_name_not_null(ctx, obj));
	}

	if (!stream)
	{
		fz_warn(ctx, "unhandled font type");
		return;
	}

	buf = pdf_load_stream(ctx, stream);
	len = fz_buffer_storage(ctx, buf, &data);
	fz_try(ctx)
	{
		fz_snprintf(namebuf, sizeof(namebuf), "%sfont-%04d-%04d.%s", output_template_path, pdf_to_num(ctx, dict), ++count, ext);
		fz_info(ctx, "extracting %s\n", namebuf);
		out = fz_new_output_with_path(ctx, namebuf, 0);
		fz_try(ctx)
		{
			fz_write_data(ctx, out, data, len);
			fz_close_output(ctx, out);
		}
		fz_always(ctx)
			fz_drop_output(ctx, out);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}
	fz_always(ctx)
		fz_drop_buffer(ctx, buf);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void extractobject(int num)
{
	pdf_obj *ref;

	if (!doc)
		fz_throw(ctx, FZ_ERROR_GENERIC, "no file specified");

	fz_try(ctx)
	{
		ref = pdf_new_indirect(ctx, doc, num, 0);
		if (isimage(ref))
			saveimage(ref);
		if (isfontdesc(ref))
			savefont(ref);

		fz_empty_store(ctx);
	}
	fz_always(ctx)
		pdf_drop_obj(ctx, ref);
	fz_catch(ctx)
		fz_warn(ctx, "ignoring object %d", num);
}

int pdfextract_main(int argc, const char** argv)
{
	const char *infile;
	const char *password = "";
	int c, o;
	int errored = 0;
    char file_tpl_buf[PATH_MAX];

	doc = NULL;
	ctx = NULL;
	dorgb = 0;
	doicc = 1;
	count = 0;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:p:raNh")) != -1)
	{
		switch (c)
		{
		case 'o': output_template_path = fz_optarg; break;
		case 'p': password = fz_optarg; break;
		case 'r': dorgb++; break;
		case 'a': doalpha++; break;
		case 'N': doicc^=1; break;
		default: return usage();
		}
	}

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		return usage();
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

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	infile = argv[fz_optind++];

	fz_var(doc);

	fz_try(ctx)
	{
        fz_format_output_path(ctx, file_tpl_buf, sizeof file_tpl_buf, output_template_path, 0);
        fz_normalize_path(ctx, file_tpl_buf, sizeof file_tpl_buf, file_tpl_buf);
        fz_sanitize_path(ctx, file_tpl_buf, sizeof file_tpl_buf, file_tpl_buf);
        // the lifetime of `file_tpl_buf` will be plenty long enough for all usage of that path to complete, so it's safe to reference it like this, via a global alias:
        output_template_path = file_tpl_buf;

		if (doicc)
			fz_enable_icc(ctx);
		else
			fz_disable_icc(ctx);

		doc = pdf_open_document(ctx, infile);
		if (pdf_needs_password(ctx, doc))
			if (!pdf_authenticate_password(ctx, doc, password))
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", infile);

		if (fz_optind == argc)
		{
			int len = pdf_count_objects(ctx, doc);
			for (o = 1; o < len; o++)
				extractobject(o);
		}
		else
		{
			while (fz_optind < argc)
			{
				extractobject(atoi(argv[fz_optind]));
				fz_optind++;
			}
		}
	}
	fz_always(ctx)
	{
		pdf_drop_document(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_log_error(ctx, fz_caught_message(ctx));
		errored = 1;
	}
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return errored;
}

#endif
