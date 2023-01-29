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

/*
 * PDF cleaning tool: general purpose pdf syntax washer.
 *
 * Rewrite PDF with pretty printed objects.
 * Garbage collect unreachable objects.
 * Inflate compressed streams.
 * Create subset documents.
 *
 * TODO: linearize document for fast web view
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/jmemcust.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if FZ_ENABLE_RENDER_CORE

static fz_context* ctx = NULL;

static int usage(void)
{
	fz_info(ctx,
		"usage: mutool clean [options] input.pdf [output.pdf] [pages]\n"
		"\t-p -\tpassword\n"
		"\t-g\tgarbage collect unused objects\n"
		"\t-gg\tin addition to -g compact xref table\n"
		"\t-ggg\tin addition to -gg merge duplicate objects\n"
		"\t-gggg\tin addition to -ggg check streams for duplication\n"
		"\t-l\tlinearize PDF\n"
		"\t-D\tsave file without encryption\n"
		"\t-E -\tsave file with new encryption (rc4-40, rc4-128, aes-128, or aes-256)\n"
		"\t-O -\towner password (only if encrypting)\n"
		"\t-U -\tuser password (only if encrypting)\n"
		"\t-P -\tpermission flags (only if encrypting)\n"
		"\t-a\tascii hex encode binary streams\n"
		"\t-d\tdecompress streams\n"
		"\t-z\tdeflate uncompressed streams\n"
		"\t-f\tcompress font streams\n"
		"\t-i\tcompress image streams\n"
		"\t-c\tclean content streams\n"
		"\t-s\tsanitize content streams\n"
		"\t-A\tcreate appearance streams for annotations\n"
		"\t-AA\trecreate appearance streams for annotations\n"
		"\t-m\tpreserve metadata\n"
		"\tpages\tcomma separated list of page numbers and ranges\n"
	);

	return EXIT_FAILURE;
}

static int encrypt_method_from_string(const char *name)
{
	if (!strcmp(name, "rc4-40")) return PDF_ENCRYPT_RC4_40;
	if (!strcmp(name, "rc4-128")) return PDF_ENCRYPT_RC4_128;
	if (!strcmp(name, "aes-128")) return PDF_ENCRYPT_AES_128;
	if (!strcmp(name, "aes-256")) return PDF_ENCRYPT_AES_256;
	return PDF_ENCRYPT_UNKNOWN;
}

int pdfclean_main(int argc, const char** argv)
{
	const char *infile;
	const char *outfile = "out.pdf";
	const char *password = "";
	int c;
	pdf_write_options opts = pdf_default_write_options;
	int errors = 0;

	ctx = NULL;

	fz_getopt_reset();
	opts.dont_regenerate_id = 1;

	while ((c = fz_getopt(argc, argv, "adfgilp:sczDAE:O:U:P:m")) != -1)
	{
		switch (c)
		{
		case 'p': password = fz_optarg; break;

		case 'd': opts.do_decompress += 1; break;
		case 'z': opts.do_compress += 1; break;
		case 'f': opts.do_compress_fonts += 1; break;
		case 'i': opts.do_compress_images += 1; break;
		case 'a': opts.do_ascii += 1; break;
		case 'g': opts.do_garbage += 1; break;
		case 'l': opts.do_linear += 1; break;
		case 'c': opts.do_clean += 1; break;
		case 's': opts.do_sanitize += 1; break;
		case 'A': opts.do_appearance += 1; break;

		case 'D': opts.do_encrypt = PDF_ENCRYPT_NONE; break;
		case 'E': opts.do_encrypt = encrypt_method_from_string(fz_optarg); break;
		case 'P': opts.permissions = fz_atoi(fz_optarg); break;
		case 'O': fz_strncpy_s(ctx, opts.opwd_utf8, fz_optarg, sizeof opts.opwd_utf8); break;
		case 'U': fz_strncpy_s(ctx, opts.upwd_utf8, fz_optarg, sizeof opts.upwd_utf8); break;
		case 'm': opts.do_preserve_metadata = 1; break;

		default: return usage();
		}
	}

	// for example, specify '-d' decompress TWICE, together with one '-z' RECOMPRESS to get PRETTY processing too!
	if ((opts.do_ascii + opts.do_decompress) == opts.do_compress + 1)
		opts.do_pretty = 1;

	if (argc == fz_optind)
	{
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

	// registeer a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	infile = argv[fz_optind++];

	if (argc - fz_optind > 0 &&
		(strstr(argv[fz_optind], ".pdf") || strstr(argv[fz_optind], ".PDF")))
	{
		outfile = argv[fz_optind++];
	}

	fz_try(ctx)
	{
		char file_path[PATH_MAX];
		fz_format_output_path(ctx, file_path, sizeof file_path, outfile, 0);
		fz_normalize_path(ctx, file_path, sizeof file_path, file_path);
		fz_sanitize_path(ctx, file_path, sizeof file_path, file_path);

		pdf_clean_file(ctx, infile, file_path, password, &opts, argc - fz_optind, &argv[fz_optind]);
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "%s", fz_caught_message(ctx));
		errors++;
		// delete damaged/incomplete output file:
		unlink(outfile);
	}
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);

	return errors != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

#endif
