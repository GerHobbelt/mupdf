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

#ifndef TESSOCR_H
#define TESSOCR_H

#include "mupdf/fitz.h"

#ifdef __cplusplus
extern "C" {
#endif

void *ocr_init(fz_context *ctx, const char *language, const char *datadir);

void ocr_fin(fz_context *ctx, void *api);

void ocr_recognise(fz_context *ctx,
		void *api,
		fz_pixmap *pix,
		void (*callback)(fz_context *ctx,
				void *arg,
				int unicode,
				const char *font_name,
				const int *line_bbox,
				const int *word_bbox,
				const int *char_bbox,
				int pointsize),
		int (*progress)(fz_context *ctx,
				void *arg,
				int progress),
		void *arg);

// --------

#if defined(BUILD_MONOLITHIC)

int tesseract_tool_main(int argc, const char** argv);
int tesseract_get_page_gradient_main(int argc, const char** argv);
int tesseract_svpaint_main(int argc, const char** argv);
int tesseract_test_issue_845_main(int argc, const char **argv);
int tesseract_test_issue_ML_1bba6c_main(int argc, const char **argv);
int tesseract_basic_example_main(int argc, const char **argv);

#endif

#ifdef __cplusplus
}
#endif

#include "../../thirdparty/tesseract/include/tesseract/capi_training_tools.h"

#endif
