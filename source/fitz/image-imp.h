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

#ifndef FITZ_IMAGE_IMP_H
#define FITZ_IMAGE_IMP_H

#if BUILDING_MUPDF_MINIMAL_CORE < 2

fz_pixmap *fz_load_jpeg(fz_context *ctx, const unsigned char *data, size_t size);
fz_pixmap *fz_load_png(fz_context *ctx, const unsigned char *data, size_t size);
fz_pixmap *fz_load_psd(fz_context *ctx, const unsigned char *data, size_t size);
#if FZ_ENABLE_TIFF
fz_pixmap *fz_load_tiff(fz_context *ctx, const unsigned char *data, size_t size);
#endif
fz_pixmap *fz_load_jxr(fz_context *ctx, const unsigned char *data, size_t size);
#if FZ_ENABLE_WEBP
fz_pixmap *fz_load_webp(fz_context *ctx, const unsigned char *data, size_t size);
#endif
fz_pixmap *fz_load_gif(fz_context *ctx, const unsigned char *data, size_t size);
fz_pixmap *fz_load_bmp(fz_context *ctx, const unsigned char *data, size_t size);
fz_pixmap *fz_load_pnm(fz_context *ctx, const unsigned char *data, size_t size);
fz_pixmap *fz_load_jbig2(fz_context *ctx, const unsigned char *data, size_t size);
#if FZ_ENABLE_JPEGXL
fz_pixmap *fz_load_jpegxl(fz_context *ctx, const unsigned char *data, size_t size);
#endif

void fz_load_jpeg_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#if FZ_ENABLE_JPX
void fz_load_jpx_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#endif
void fz_load_png_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
void fz_load_psd_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#if FZ_ENABLE_TIFF
void fz_load_tiff_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#endif
void fz_load_jxr_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#if FZ_ENABLE_WEBP
void fz_load_webp_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#endif
void fz_load_gif_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
void fz_load_bmp_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
void fz_load_pnm_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
void fz_load_jbig2_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#if FZ_ENABLE_JPEGXL
void fz_load_jpegxl_info(fz_context *ctx, const unsigned char *data, size_t size, int *w, int *h, int *xres, int *yres, fz_colorspace **cspace, uint8_t *orientation);
#endif

#endif

#endif
