#include "mupdf/fitz.h"

#include <zlib.h> /* for compressBound() and compress() */

fz_buffer *fz_deflate(fz_context *ctx, fz_buffer *input)
{
	unsigned char *input_p = input->data;
	uLong input_n = (uLong) input->len;
	unsigned char *output_p;
	uLongf output_n;
	int result;

	/* check possible size_t / uLong precision mismatch */
	if (input->len != (size_t)input_n)
		fz_throw(ctx, FZ_ERROR_GENERIC, "buffer is too large to deflate");

	output_n = compressBound(input_n);
	output_p = Memento_label(fz_malloc(ctx, output_n, __FILE__, __LINE__), "fz_deflate");
	result = compress(output_p, &output_n, input_p, input_n);
	if (result != Z_OK)
	{
		fz_free(ctx, output_p);
		fz_throw(ctx, FZ_ERROR_GENERIC, "zlib error when deflating data");
	}

	fz_try(ctx)
		fz_realloc(ctx, output_p, output_n, __FILE__, __LINE__);
	fz_catch(ctx)
	{
		fz_free(ctx, output_p);
		fz_rethrow(ctx);
	}

	return fz_new_buffer_from_data(ctx, output_p, output_n);
}
