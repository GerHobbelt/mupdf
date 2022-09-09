#include "mupdf/fitz.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#if FZ_ENABLE_JPEGXL

#include "jxl/decode.h"

extern "C"
{

static void *
my_alloc(void *ctx_, size_t size)
{
	fz_context *ctx = (fz_context *)ctx_;

	return fz_malloc_no_throw(ctx, size);
}

static void
my_free(void *ctx_, void *ptr)
{
	fz_context *ctx = (fz_context *)ctx_;

	fz_free(ctx, ptr);
}

fz_pixmap *
fz_load_jpegxl(fz_context *ctx, const unsigned char *rbuf, size_t rlen)
{
	JxlDecoder *decoder;
	JxlMemoryManager mem_mgr;
	JxlDecoderStatus err;
	JxlBasicInfo info = { 0 };
	fz_pixmap *pix = NULL;
	fz_colorspace *colorspace = NULL;
	JxlPixelFormat format;
	int alpha = 0;
	fz_buffer *profile = NULL;
	size_t profile_size;

	mem_mgr.opaque = ctx;
	mem_mgr._alloc = my_alloc;
	mem_mgr._free = my_free;

	decoder = JxlDecoderCreate(&mem_mgr);
	if (decoder == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to create JPEGXL Decoder");

	fz_var(profile_size);
	fz_var(colorspace);
	fz_var(pix);
	fz_try(ctx)
	{
		err = JxlDecoderSetInput(decoder, rbuf, rlen);
		if (err != JXL_DEC_SUCCESS)
			fz_throw(ctx, FZ_ERROR_GENERIC, "JXL SetInput failed");

		err = JxlDecoderSubscribeEvents(decoder,
						JXL_DEC_BASIC_INFO |
						JXL_DEC_FULL_IMAGE |
						JXL_DEC_COLOR_ENCODING);
		if (err != JXL_DEC_SUCCESS)
			fz_throw(ctx, FZ_ERROR_GENERIC, "JXL Subscribe failed");

		while (err != JXL_DEC_FULL_IMAGE)
		{
			err = JxlDecoderProcessInput(decoder);
			switch (err)
			{
			case JXL_DEC_BASIC_INFO:
				err = JxlDecoderGetBasicInfo(decoder, &info);
				if (err != JXL_DEC_SUCCESS)
					fz_throw(ctx, FZ_ERROR_GENERIC, "JXL GetBasicInfo failed");

				alpha = info.num_extra_channels > 0;
				format.num_channels = info.num_color_channels + alpha;
				format.data_type = JXL_TYPE_UINT8;
				format.endianness = JXL_NATIVE_ENDIAN;
				format.align = 0; /* No alignment, same as 1. */

				break;

			case JXL_DEC_COLOR_ENCODING:
				err = JxlDecoderGetICCProfileSize(decoder,
								&format,
								JXL_COLOR_PROFILE_TARGET_DATA,
								&profile_size);

				if (err != JXL_DEC_SUCCESS)
					break;
				profile = fz_new_buffer(ctx, profile_size);

				err = JxlDecoderGetColorAsICCProfile(decoder,
								&format,
								JXL_COLOR_PROFILE_TARGET_DATA,
								profile->data,
								profile_size);
				if (err != JXL_DEC_SUCCESS)
					break;
				profile->len = profile_size;

				colorspace = fz_new_icc_colorspace(ctx,
								info.num_color_channels == 3 ?
										FZ_COLORSPACE_RGB :
										FZ_COLORSPACE_GRAY,
								0, "JPEGXL", profile);
				fz_drop_buffer(ctx, profile);

				pix = fz_new_pixmap(ctx, colorspace, info.xsize, info.ysize,
						NULL, alpha);

				err = JxlDecoderSetImageOutBuffer(decoder, &format, pix->samples,
								pix->w * pix->h * pix->n);
				break;

			case JXL_DEC_FULL_IMAGE:
				break;

			default:
				fz_throw(ctx, FZ_ERROR_GENERIC, "JXL Decode failed");
			}
		}

		(void)JxlDecoderReleaseInput(decoder);
	}
	fz_always(ctx)
	{
		JxlDecoderDestroy(decoder);
		fz_drop_colorspace(ctx, colorspace);
	}
	fz_catch(ctx)
	{
		fz_drop_buffer(ctx, profile);
		fz_drop_pixmap(ctx, pix);
		fz_rethrow(ctx);
	}

	return pix;
}

void
fz_load_jpegxl_info(fz_context *ctx, const unsigned char *rbuf, size_t rlen, int *xp, int *yp, int *xresp, int *yresp, fz_colorspace **cspacep, uint8_t *orientation)
{
	JxlDecoder *decoder;
	JxlMemoryManager mem_mgr;
	JxlDecoderStatus err;
	JxlBasicInfo info;
	JxlPixelFormat format;
	int alpha;
	fz_buffer *profile = NULL;
	size_t profile_size;

	mem_mgr.opaque = ctx;
	mem_mgr._alloc = my_alloc;
	mem_mgr._free = my_free;

	decoder = JxlDecoderCreate(&mem_mgr);
	if (decoder == NULL)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to create JPEGXL Decoder");

	*xresp = 96;
	*yresp = 96;
	if (cspacep)
		*cspacep = NULL;

	fz_var(profile_size);
	fz_try(ctx)
	{
		int done = 0;

		err = JxlDecoderSetInput(decoder, rbuf, rlen);
		if (err != JXL_DEC_SUCCESS)
			fz_throw(ctx, FZ_ERROR_GENERIC, "JXL SetInput failed");

		err = JxlDecoderSubscribeEvents(decoder,
						JXL_DEC_BASIC_INFO |
						(cspacep ? JXL_DEC_COLOR_ENCODING : 0));
		if (err != JXL_DEC_SUCCESS)
			fz_throw(ctx, FZ_ERROR_GENERIC, "JXL Subscribe failed");

		while (!done)
		{
			err = JxlDecoderProcessInput(decoder);
			switch (err)
			{
			case JXL_DEC_BASIC_INFO:
				err = JxlDecoderGetBasicInfo(decoder, &info);
				if (err != JXL_DEC_SUCCESS)
					fz_throw(ctx, FZ_ERROR_GENERIC, "JXL GetBasicInfo failed");

				alpha = info.num_extra_channels > 0;
				format.num_channels = info.num_color_channels + alpha;
				format.data_type = JXL_TYPE_UINT8;
				format.endianness = JXL_NATIVE_ENDIAN;
				format.align = 0; /* No alignment, same as 1. */

				*xp = info.xsize;
				*yp = info.ysize;

				done = (cspacep == NULL);

				break;

			case JXL_DEC_COLOR_ENCODING:
				err = JxlDecoderGetICCProfileSize(decoder,
								&format,
								JXL_COLOR_PROFILE_TARGET_DATA,
								&profile_size);

				if (err != JXL_DEC_SUCCESS)
					break;
				profile = fz_new_buffer(ctx, profile_size);

				err = JxlDecoderGetColorAsICCProfile(decoder,
								&format,
								JXL_COLOR_PROFILE_TARGET_DATA,
								profile->data,
								profile_size);
				if (err != JXL_DEC_SUCCESS)
					break;
				profile->len = profile_size;

				*cspacep = fz_new_icc_colorspace(ctx,
								info.num_color_channels == 3 ?
										FZ_COLORSPACE_RGB :
										FZ_COLORSPACE_GRAY,
								0, "JPEGXL", profile);
				done = 1;
				break;
			default:
				fz_throw(ctx, FZ_ERROR_GENERIC, "JXL Decode failed");
			}
		}

		(void)JxlDecoderReleaseInput(decoder);
	}
	fz_always(ctx)
	{
		fz_drop_buffer(ctx, profile);
		if (cspacep)
			fz_drop_colorspace(ctx, *cspacep);
		JxlDecoderDestroy(decoder);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

};

#endif
