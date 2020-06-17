#ifndef MUPDF_FITZ_COMPRESS_H
#define MUPDF_FITZ_COMPRESS_H

#include "mupdf/fitz/system.h"

typedef enum
{
	FZ_DEFLATE_NONE = 0,
	FZ_DEFLATE_BEST_SPEED = 1,
	FZ_DEFLATE_BEST = 9,
	FZ_DEFLATE_DEFAULT = -1
} fz_deflate_level;

/**
	Compress input data using the Deflate/Zip algorithm.
	Returns a new buffer containing the compressed data.
 */
fz_buffer *fz_deflate(fz_context *ctx, fz_buffer *input);

/**
	Compress bitmap data as CCITT Group 3 1D fax image.
	Creates a stream assuming the default PDF parameters,
	except the number of columns.
*/
fz_buffer *fz_compress_ccitt_fax_g3(fz_context *ctx, const unsigned char *data, int columns, int rows);

/**
	Compress bitmap data as CCITT Group 4 2D fax image.
	Creates a stream assuming the default PDF parameters, except
	K=-1 and the number of columns.
*/
fz_buffer *fz_compress_ccitt_fax_g4(fz_context *ctx, const unsigned char *data, int columns, int rows);

#endif
