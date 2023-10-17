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
 * pdfdump -- dumps all embedded files to disk
 * User-defined function--does not come with mupdf.
 */

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/mutool.h"

#include <stdlib.h>
#include <stdio.h>

static pdf_document *doc = NULL;
static fz_context *ctx = NULL;


int pdfdump_main(int argc, const char **argv)
{
	const char *infile;
	const char *password = "";
	int c;
	const char *output = ".";

	while ((c = fz_getopt(argc, argv, "o:O:")) != -1)
	{
		switch (c)
		{
		case 'o': 
			output = fz_optarg; 
			break;
		}
	}

	// if (fz_optind == argc)
	// 	return usage();

	infile = argv[fz_optind++];

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fprintf(stderr, "cannot initialise context\n");
		return 1;
	}

	doc = pdf_open_document(ctx, infile);
	if (pdf_needs_password(ctx, doc))
		if (!pdf_authenticate_password(ctx, doc, password))
			fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", infile);

	// int n = pdf_count_portfolio_entries(ctx, doc);
	// printf("Portfolio entries: %i\n", n);

	pdf_obj *embeddedfiles;
	fz_buffer *buf;
	pdf_obj *embeddedfile;
	pdf_obj *name;

	int i;
	int len;
	FILE *file;
	const char *filename;
	unsigned char *data;

	embeddedfiles = pdf_load_name_tree(ctx, doc, PDF_NAME(EmbeddedFiles));
	for (i = 0; i < pdf_dict_len(ctx, embeddedfiles); ++i)
	{
		char filepath[1024] = "";

		// Get name of embedded file with index i
		name = pdf_dict_get_key(ctx, embeddedfiles, i);
		filename = pdf_to_name(ctx, name);

		// Get content of embedded file with index i
		embeddedfile = pdf_dict_get_val(ctx, embeddedfiles, i);
		buf = pdf_load_stream(ctx, pdf_dict_getl(ctx, embeddedfile, PDF_NAME(EF), PDF_NAME(F), NULL));
		len = fz_buffer_storage(ctx, buf, &data);

		strcat(filepath, output);
		strcat(filepath, "/");
		strcat(filepath, filename);

		// Write content to a file with the name extracted
		file = fopen(filepath, "wb");
		if (file == NULL)
		{
			fprintf(stderr, "Failed to open '%s' for writing\n", filepath);
			return 1;
		}
		fwrite(data, 1, len, file);
		fclose(file);

		pdf_drop_obj(ctx, embeddedfile);
		pdf_drop_obj(ctx, name);
		fz_drop_buffer(ctx, buf);
	}

	pdf_drop_document(ctx, doc);
	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return 0;
}
