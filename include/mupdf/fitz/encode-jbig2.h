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

#ifndef MUPDF_FITZ_ENCODE_JBIG2_H
#define MUPDF_FITZ_ENCODE_JBIG2_H

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#if BUILDING_MUPDF_MINIMAL_CORE < 2

/*
	An active instance of a jbig2encode process will be represented by
	an fz_jbig2e.
*/
typedef struct fz_jbig2e fz_jbig2e;

/*
	fz_jbig2e_init creates an instance of whichever jbig 2 encoder
	has been registered with the system. This will throw an exception
	if no encoder has been registered.

	This instance should be dropped once finished with.
*/
fz_jbig2e *fz_new_jbig2e(fz_context *ctx);

/*
	Feed a bitmap to an inited created. Call this repeatedly with
	all the different 'pages' to be encoded. The bitmap can safely
	be destroyed after calling this function.
*/
void fz_jbig2e_feed_bitmap(fz_context *ctx, fz_jbig2e *inst, fz_bitmap *bit);

/*
	Once all the bitmaps have been fed in, call this function. This will
	return a buffer with the symbols data.
*/
fz_buffer *
fz_jbig2e_pages_complete(fz_context *ctx, fz_jbig2e *inst);

/*
	Call this repeatedly to get the encoded versions of the bitmaps out.
	If the caller hasn't kept a count of how many bitmaps were fed in,
	they may call this until it returns NULL.
*/
fz_buffer *
fz_jbig2e_get_page(fz_context *ctx, fz_jbig2e *inst);

/*
	Destroy a jbig2e instance.
*/
void
fz_drop_jbig2e(fz_context *ctx, fz_jbig2e *inst);

/*
	Because we don't always link in the jbig2encoder, we have to jump through
	some hoops. Basically, the encoder will never be included unless we call
	fz_register_jbig2_encoder() with the address of an encoder structure.

	Different implementations are therefore possible. Each different
	implementation will provide its own fz_jbig2_encoder structure.
*/

/*
	The functions that make up the encoder are provided in a structure.
*/
typedef struct
{
	fz_jbig2e *(*init)(fz_context *);
	void (*feed_bitmap)(fz_context *, fz_jbig2e *, fz_bitmap *);
	fz_buffer *(*pages_complete)(fz_context *, fz_jbig2e *);
	fz_buffer *(*get_page)(fz_context *, fz_jbig2e *);
	void (*fin)(fz_context *, fz_jbig2e *);
} fz_jbig2_encoder;

/*
	Register a jbig2encoder with the system. Only one may be registered
	at a time. There is no reference counting etc, so the encoder
	passed in must be a static structure. It will never be finalised.
*/
void fz_register_jbig2_encoder(fz_context *ctx, const fz_jbig2_encoder *enc);

/*
	Retreive a pointer to the jbig2enc based encoder.
*/
const fz_jbig2_encoder *fz_jbig2enc_encoder(fz_context *ctx);

/*
	A function to check whether a jbig2encoder has been registered
	with the system.
*/
int fz_jbig2e_enabled(fz_context *ctx);

#endif   // BUILDING_MUPDF_MINIMAL_CORE < 2

#endif
