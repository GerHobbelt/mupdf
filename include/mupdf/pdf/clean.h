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

#ifndef MUPDF_PDF_CLEAN_H
#define MUPDF_PDF_CLEAN_H

#include "mupdf/pdf/document.h"
#include "mupdf/pdf/image-rewriter.h"

#if FZ_ENABLE_RENDER_CORE 

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	pdf_write_options write;
	pdf_image_rewriter_options image;

	/* Experimental option. Subject to change. */
	int subset_fonts;
} pdf_clean_options;

/*
	Read infile, and write selected pages to outfile with the given options.
*/
void pdf_clean_file(fz_context *ctx, const char *infile, const char *outfile, const char *password, pdf_clean_options *opts, int retainlen, const char *retainlist[]);

/*
	Recreate page tree to include only the pages listed in the array, in the order listed.
*/
void pdf_rearrange_pages(fz_context *ctx, pdf_document *doc, int count, const int *pages);


#ifdef __cplusplus
}
#endif

#endif

#endif
