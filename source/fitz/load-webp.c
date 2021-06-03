#include "mupdf/fitz.h"

#include <webp/decode.h>

fz_pixmap *
fz_load_webp(fz_context *ctx, const unsigned char *rbuf, size_t rlen)
{
	WebPBitstreamFeatures feat;
	fz_pixmap *pix;
	uint8_t *data;
	int error;

	error = WebPGetFeatures(rbuf, rlen, &feat);
	if (error)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode WebP image info");

	pix = fz_new_pixmap(ctx, fz_device_rgb(ctx), feat.width, feat.height, NULL, feat.has_alpha);
	if (feat.has_alpha)
		data = WebPDecodeRGBAInto(rbuf, rlen, pix->samples, pix->h * pix->stride, pix->stride);
	else
		data = WebPDecodeRGBInto(rbuf, rlen, pix->samples, pix->h * pix->stride, pix->stride);
	if (!data)
	{
		fz_drop_pixmap(ctx, pix);
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode WebP image data");
	}
	return pix;
}

void
fz_load_webp_info(fz_context *ctx, const unsigned char *rbuf, size_t rlen,
		int *wp, int *hp, int *xresp, int *yresp, fz_colorspace **cspacep)
{
	WebPBitstreamFeatures feat;
	int error;

	error = WebPGetFeatures(rbuf, rlen, &feat);
	if (error)
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot decode WebP image info");

	if (wp) *wp = feat.width;
	if (hp) *hp = feat.height;
	if (xresp) *xresp = 96;
	if (yresp) *yresp = 96;
	if (cspacep) *cspacep = fz_keep_colorspace(ctx, fz_device_rgb(ctx));
}
