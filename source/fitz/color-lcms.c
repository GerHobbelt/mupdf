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

#include "mupdf/fitz.h"

#include "color-imp.h"

#include <string.h>
#include <math.h>

#if BUILDING_MUPDF_MINIMAL_CORE < 2

#if FZ_ENABLE_ICC

#ifndef LCMS_USE_FLOAT
#define LCMS_USE_FLOAT 0
#endif

#define GLOINIT() cmsContext glo = (cmsContext)ctx->colorspace->icc_instance

#include "lcms2mt.h"
#include "lcms2mt_plugin.h"

static void fz_premultiply_row(fz_context *ctx, int n, int c, int w, unsigned char *s)
{
	unsigned char a;
	int k;
	int n1 = n-1;
	for (; w > 0; w--)
	{
		a = s[n1];
		if (a == 0)
			memset(s, 0, c);
		else if (a != 255)
			for (k = 0; k < c; k++)
				s[k] = fz_mul255(s[k], a);
		s += n;
	}
}

static void fz_premultiply_row_0or1(fz_context *ctx, int n, int c, int w, unsigned char *s)
{
	unsigned char a;
	int n1 = n-1;
	for (; w > 0; w--)
	{
		a = s[n1];
		if (a == 0)
			memset(s, 0, c);
		s += n;
	}
}

/* Returns 0 for all the alphas being 0, 1 for them being 0 or 255, 2 otherwise. */
static int fz_unmultiply_row(fz_context *ctx, int n, int c, int w, unsigned char *s, const unsigned char *in)
{
	int a, inva;
	int k;
	int n1 = n-1;
	for (; w > 0; w--)
	{
		a = in[n1];
		if (a != 0)
			goto nonzero;
		for (k = 0; k < c; k++)
			s[k] = 0;
		for (;k < n1; k++)
			s[k] = in[k];
		s[n1] = 0;
		s += n;
		in += n;
	}
	return 0;
	for (; w > 0; w--)
	{
		a = in[n1];
nonzero:
		if (a != 0 && a != 255)
			goto varying;
		k = 0;
		if (a == 0)
			for (; k < c; k++)
				s[k] = 0;
		for (;k < n; k++)
			s[k] = in[k];
		s += n;
		in += n;
	}
	return 1;
	for (; w > 0; w--)
	{
		a = in[n1];
varying:
		if (a == 0)
		{
			for (k = 0; k < c; k++)
				s[k] = 0;
			for (;k < n1; k++)
				s[k] = in[k];
			s[k] = 0;
		}
		else if (a == 255)
		{
			memcpy(s, in, n);
		}
		else
		{
			inva = 255 * 256 / a;
			for (k = 0; k < c; k++)
				s[k] = (in[k] * inva) >> 8;
			for (;k < n1; k++)
				s[k] = in[k];
			s[n1] = a;
		}
		s += n;
		in += n;
	}
	return 2;
}

struct fz_icc_link
{
	fz_storable storable;
	void *handle;
};

#if defined(HAVE_LCMS2MT)

static void fz_lcms_log_error(cmsContext id, cmsUInt32Number error_code, const char *error_text)
{
	fz_context *ctx = (fz_context *)cmsGetContextUserData(id);
	fz_warn(ctx, "lcms: %s.", error_text);
}

static void *fz_lcms_malloc(cmsContext id, unsigned int size)
{
	fz_context *ctx = (fz_context *)cmsGetContextUserData(id);
	return Memento_label(fz_malloc_no_throw(ctx, size), "lcms");
}

static void *fz_lcms_realloc(cmsContext id, void *ptr, unsigned int size)
{
	fz_context *ctx = (fz_context*)cmsGetContextUserData(id);
	return Memento_label(fz_realloc_no_throw(ctx, ptr, size), "lcms");
}

static void fz_lcms_free(cmsContext id, void *ptr)
{
	fz_context *ctx = (fz_context*)cmsGetContextUserData(id);
	fz_free(ctx, ptr);
}

static cmsPluginMemHandler fz_lcms_memhandler =
{
	{
		cmsPluginMagicNumber,
		LCMS_VERSION,
		cmsPluginMemHandlerSig,
		NULL
	},
	fz_lcms_malloc,
	fz_lcms_free,
	fz_lcms_realloc,
	NULL,
	NULL,
	NULL,
};

void fz_new_icc_context(fz_context *ctx)
{
	cmsContext glo = cmsCreateContext(&fz_lcms_memhandler, ctx);
	if (!glo)
		fz_throw(ctx, FZ_ERROR_LIBRARY, "cmsCreateContext failed");
	ctx->colorspace->icc_instance = glo;
	cmsSetLogErrorHandler(glo, fz_lcms_log_error);
}

void fz_drop_icc_context(fz_context *ctx)
{
	cmsContext glo = (cmsContext)ctx->colorspace->icc_instance;
	if (glo)
		cmsDeleteContext(glo);
	ctx->colorspace->icc_instance = NULL;
}

#else  // defined(HAVE_LCMS2MT)

static fz_context *glo_ctx = NULL;

static void fz_lcms_log_error(cmsContext id, cmsUInt32Number error_code, const char *error_text)
{
	fz_warn(glo_ctx, "lcms: %s.", error_text);
}

void fz_new_icc_context(fz_context *ctx)
{
	if (glo_ctx != NULL)
		fz_throw(ctx, FZ_ERROR_ARGUMENT, "Stock LCMS2 library cannot be used in multiple contexts!");
	glo_ctx = ctx;
	cmsSetLogErrorHandler(fz_lcms_log_error);
}

void fz_drop_icc_context(fz_context *ctx)
{
	glo_ctx = NULL;
	cmsSetLogErrorHandler(NULL);
}

#endif  // defined(HAVE_LCMS2MT)

fz_icc_profile *fz_new_icc_profile(fz_context *ctx, unsigned char *data, size_t size)
{
	GLOINIT();
	fz_icc_profile *profile;
	profile = (fz_icc_profile *)cmsOpenProfileFromMem(glo, data, (cmsUInt32Number)size);
	if (profile == NULL)
		fz_throw(ctx, FZ_ERROR_FORMAT, "cmsOpenProfileFromMem failed");
	return profile;
}

int fz_icc_profile_is_lab(fz_context *ctx, fz_icc_profile *profile)
{
	GLOINIT();
	if (profile == NULL)
		return 0;
	return (cmsGetColorSpace(glo, profile) == cmsSigLabData);
}

void fz_drop_icc_profile(fz_context *ctx, fz_icc_profile *profile)
{
	GLOINIT();
	if (profile)
		cmsCloseProfile(glo, profile);
}

void fz_icc_profile_name(fz_context *ctx, fz_icc_profile *profile, char *name, size_t size)
{
	GLOINIT();
	cmsMLU *descMLU;
	descMLU = cmsReadTag(glo, profile, cmsSigProfileDescriptionTag);
	name[0] = 0;
	cmsMLUgetASCII(glo, descMLU, "en", "US", name, (cmsUInt32Number)size);
}

int fz_icc_profile_components(fz_context *ctx, fz_icc_profile *profile)
{
	GLOINIT();
	return cmsChannelsOf(glo, cmsGetColorSpace(glo, profile));
}

void fz_drop_icc_link_imp(fz_context *ctx, fz_storable *storable)
{
	GLOINIT();
	fz_icc_link *link = (fz_icc_link*)storable;
	cmsDeleteTransform(glo, link->handle);
	fz_free(ctx, link);
}

void fz_drop_icc_link(fz_context *ctx, fz_icc_link *link)
{
	fz_drop_storable(ctx, &link->storable);
}

fz_icc_link *
fz_new_icc_link(fz_context *ctx,
	fz_colorspace *src, int src_extras,
	fz_colorspace *dst, int dst_extras,
	fz_colorspace *prf,
	fz_color_params rend,
	int format,
	int copy_spots,
	int premult)
{
	GLOINIT();
	cmsHPROFILE src_pro = src->u.icc.profile;
	cmsHPROFILE dst_pro = dst->u.icc.profile;
	cmsHPROFILE prf_pro = prf ? prf->u.icc.profile : NULL;
	int src_bgr = (src->type == FZ_COLORSPACE_BGR);
	int dst_bgr = (dst->type == FZ_COLORSPACE_BGR);
	cmsColorSpaceSignature src_cs, dst_cs;
	cmsUInt32Number src_fmt, dst_fmt;
	cmsUInt32Number flags;
	cmsHTRANSFORM transform;
	fz_icc_link *link = NULL;

	flags = cmsFLAGS_LOWRESPRECALC;

	src_cs = cmsGetColorSpace(glo, src_pro);
	src_fmt = COLORSPACE_SH(_cmsLCMScolorSpace(glo, src_cs));
	src_fmt |= CHANNELS_SH(cmsChannelsOf(glo, src_cs));
	src_fmt |= DOSWAP_SH(src_bgr);
	src_fmt |= SWAPFIRST_SH(src_bgr && (src_extras > 0));
#if LCMS_USE_FLOAT
	src_fmt |= BYTES_SH(format ? 4 : 1);
	src_fmt |= FLOAT_SH(format ? 1 : 0)
#else
	src_fmt |= BYTES_SH(format ? 2 : 1);
#endif
	src_fmt |= EXTRA_SH(src_extras);

	dst_cs = cmsGetColorSpace(glo, dst_pro);
	dst_fmt = COLORSPACE_SH(_cmsLCMScolorSpace(glo, dst_cs));
	dst_fmt |= CHANNELS_SH(cmsChannelsOf(glo, dst_cs));
	dst_fmt |= DOSWAP_SH(dst_bgr);
	dst_fmt |= SWAPFIRST_SH(dst_bgr && (dst_extras > 0));
#if LCMS_USE_FLOAT
	dst_fmt |= BYTES_SH(format ? 4 : 1);
	dst_fmt |= FLOAT_SH(format ? 1 : 0);
#else
	dst_fmt |= BYTES_SH(format ? 2 : 1);
#endif
	dst_fmt |= EXTRA_SH(dst_extras);

	/* flags */
	if (rend.bp)
		flags |= cmsFLAGS_BLACKPOINTCOMPENSATION;

	if (copy_spots)
		flags |= cmsFLAGS_COPY_ALPHA;

#ifdef cmsFLAGS_PREMULT
	if (premult)
		flags |= cmsFLAGS_PREMULT;
#endif

	if (prf_pro == NULL)
	{
		transform = cmsCreateTransform(glo, src_pro, src_fmt, dst_pro, dst_fmt, rend.ri, flags);
		if (!transform)
			fz_throw(ctx, FZ_ERROR_LIBRARY, "cmsCreateTransform(%s,%s) failed", src->name, dst->name);
	}

	/* LCMS proof creation links don't work properly with the Ghent test files. Handle this in a brutish manner. */
	else if (src_pro == prf_pro)
	{
		transform = cmsCreateTransform(glo, src_pro, src_fmt, dst_pro, dst_fmt, INTENT_RELATIVE_COLORIMETRIC, flags);
		if (!transform)
			fz_throw(ctx, FZ_ERROR_LIBRARY, "cmsCreateTransform(src=proof,dst) failed");
	}
	else if (prf_pro == dst_pro)
	{
		transform = cmsCreateTransform(glo, src_pro, src_fmt, prf_pro, dst_fmt, rend.ri, flags);
		if (!transform)
			fz_throw(ctx, FZ_ERROR_LIBRARY, "cmsCreateTransform(src,proof=dst) failed");
	}
	else
	{
		cmsHPROFILE src_to_prf_pro;
		cmsHTRANSFORM src_to_prf_link;
		cmsColorSpaceSignature prf_cs;
		cmsUInt32Number prf_fmt;
		cmsHPROFILE hProfiles[3];

		prf_cs = cmsGetColorSpace(glo, prf_pro);
		prf_fmt = COLORSPACE_SH(_cmsLCMScolorSpace(glo, prf_cs));
		prf_fmt |= CHANNELS_SH(cmsChannelsOf(glo, prf_cs));
#if LCMS_USE_FLOAT
		prf_fmt |= BYTES_SH(format ? 4 : 1);
		prf_fmt |= FLOAT_SH(format ? 1 : 0);
#else
		prf_fmt |= BYTES_SH(format ? 2 : 1);
#endif

		src_to_prf_link = cmsCreateTransform(glo, src_pro, src_fmt, prf_pro, prf_fmt, rend.ri, flags);
		if (!src_to_prf_link)
			fz_throw(ctx, FZ_ERROR_LIBRARY, "cmsCreateTransform(src,proof) failed");
		src_to_prf_pro = cmsTransform2DeviceLink(glo, src_to_prf_link, 3.4, flags);
		cmsDeleteTransform(glo, src_to_prf_link);
		if (!src_to_prf_pro)
			fz_throw(ctx, FZ_ERROR_LIBRARY, "cmsTransform2DeviceLink(src,proof) failed");

		hProfiles[0] = src_to_prf_pro;
		hProfiles[1] = prf_pro;
		hProfiles[2] = dst_pro;
		transform = cmsCreateMultiprofileTransform(glo, hProfiles, 3, src_fmt, dst_fmt, INTENT_RELATIVE_COLORIMETRIC, flags);
		cmsCloseProfile(glo, src_to_prf_pro);
		if (!transform)
			fz_throw(ctx, FZ_ERROR_LIBRARY, "cmsCreateMultiprofileTransform(src,proof,dst) failed");
	}

	fz_try(ctx)
	{
		link = fz_malloc_struct(ctx, fz_icc_link);
		FZ_INIT_STORABLE(link, 1, fz_drop_icc_link_imp);
		link->handle = transform;
	}
	fz_catch(ctx)
	{
		cmsDeleteTransform(glo, transform);
		fz_rethrow(ctx);
	}
	return link; //-V614
}

void
fz_icc_transform_color(fz_context *ctx, fz_color_converter *cc, const float *src, float *dst)
{
	GLOINIT();
#if LCMS_USE_FLOAT
	cmsDoTransform(glo, cc->link->handle, src, dst, 1);
#else
	uint16_t s16[FZ_MAX_COLORS];
	uint16_t d16[FZ_MAX_COLORS];
	int dn = cc->dst_n;
	int i;
	if (cc->ss->type == FZ_COLORSPACE_LAB)
	{
		s16[0] = src[0] * 655.35f;
		s16[1] = (src[1] + 128) * 257;
		s16[2] = (src[2] + 128) * 257;
	}
	else
	{
		int sn = cc->ss->n;
		for (i = 0; i < sn; ++i)
			s16[i] = src[i] * 65535;
	}
	cmsDoTransform(glo, cc->link->handle, s16, d16, 1);
	for (i = 0; i < dn; ++i)
		dst[i] = d16[i] / 65535.0f;
#endif
}

void
fz_icc_transform_pixmap(fz_context *ctx, fz_icc_link *link, const fz_pixmap *src, fz_pixmap *dst, int copy_spots)
{
	GLOINIT();
	int cmm_num_src, cmm_num_dst, cmm_extras;
	unsigned char *inputpos, *outputpos, *buffer;
	int ss = src->stride;
	int ds = dst->stride;
	int sw = src->w;
	int dw = dst->w;
	int sn = src->n;
	int dn = dst->n;
	int sa = src->alpha;
	int da = dst->alpha;
	int ssp = src->s;
	int dsp = dst->s;
	int sc = sn - ssp - sa;
	int dc = dn - dsp - da;
	int h = src->h;
	cmsUInt32Number src_format, dst_format;

	/* check the channels. */
	src_format = cmsGetTransformInputFormat(glo, link->handle);
	dst_format = cmsGetTransformOutputFormat(glo, link->handle);
	cmm_num_src = T_CHANNELS(src_format);
	cmm_num_dst = T_CHANNELS(dst_format);
	cmm_extras = T_EXTRA(src_format);
	if (cmm_num_src != sc || cmm_num_dst != dc || cmm_extras != ssp+sa || sa != da || (copy_spots && ssp != dsp))
		fz_throw(ctx, FZ_ERROR_ARGUMENT, "bad setup in ICC pixmap transform: src: %d vs %d+%d+%d, dst: %d vs %d+%d+%d", cmm_num_src, sc, ssp, sa, cmm_num_dst, dc, dsp, da);

	inputpos = src->samples;
	outputpos = dst->samples;

#ifdef cmsFLAGS_PREMULT
	/* LCMS2MT can only handle premultiplied data if the number of 'extra'
	 * channels is the same. If not, do it by steam. */
	if (sa && cmm_extras != (int)T_EXTRA(dst_format))
#else
	/* Vanilla LCMS2 cannot handle premultiplied data. If present, do it by steam. */
	if (sa)
#endif
	{
		buffer = fz_malloc(ctx, ss);
		for (; h > 0; h--)
		{
			int mult = fz_unmultiply_row(ctx, sn, sc, sw, buffer, inputpos);
			if (mult == 0)
			{
				/* Solid transparent row. No point in doing the transform
				 * because it will premultiplied back to 0. */
				memset(outputpos, 0, ds);
			}
			else
			{
				cmsDoTransform(glo, link->handle, buffer, outputpos, sw);
				if (!copy_spots)
				{
					/* Copy the alpha by steam */
					unsigned char *d = outputpos + dn - 1;
					const unsigned char *s = inputpos + sn -1;
					int w = sw;

					while (w--)
					{
						*d = *s;
						d += dn;
						s += sn;
					}
				}
				if (mult == 1)
					fz_premultiply_row_0or1(ctx, dn, dc, dw, outputpos);
				else if (mult == 2)
					fz_premultiply_row(ctx, dn, dc, dw, outputpos);
			}
			inputpos += ss;
			outputpos += ds;
		}
		fz_free(ctx, buffer);
	}
	else
	{
		for (; h > 0; h--)
		{
			cmsDoTransform(glo, link->handle, inputpos, outputpos, sw);
			inputpos += ss;
			outputpos += ds;
		}
	}
}

#endif  // FZ_ENABLE_ICC

#if FZ_ENABLE_GAMMA

void
fz_measure_colorspace_linearity(fz_context *ctx, fz_colorspace *colorspace)
{
	fz_gamma_table *gamma;

	if (colorspace->type == FZ_COLORSPACE_LAB)
		return;
	if (colorspace->type == FZ_COLORSPACE_CMYK)
		return;
	if (colorspace->type == FZ_COLORSPACE_INDEXED)
		return;
	if (colorspace->type == FZ_COLORSPACE_SEPARATION)
		return;

	gamma = colorspace->gamma = fz_malloc_struct(ctx, fz_gamma_table);

#if FZ_ENABLE_ICC
	if (colorspace->flags & FZ_COLORSPACE_IS_ICC)
	{
		cmsContext glo = ctx->colorspace->icc_instance;
		cmsHPROFILE input_pro;
		cmsUInt32Number input_fmt;
		cmsHPROFILE xyz_pro;
		cmsHTRANSFORM xform;
		float xyz[3];
		cmsUInt8Number color[4];
		int i;

		switch (colorspace->type)
		{
		default:
			/* should never happen */
			goto fallback;
		case FZ_COLORSPACE_GRAY:
			input_fmt = TYPE_GRAY_8;
			break;
		case FZ_COLORSPACE_RGB:
			input_fmt = TYPE_RGB_8;
			break;
		case FZ_COLORSPACE_BGR:
			input_fmt = TYPE_BGR_8;
			break;
		}

		input_pro = colorspace->u.icc.profile;
		xyz_pro = cmsCreateXYZProfile(glo);
		xform = cmsCreateTransform(glo,
			input_pro,
			input_fmt,
			xyz_pro,
			TYPE_XYZ_FLT,
			INTENT_RELATIVE_COLORIMETRIC,
			cmsFLAGS_NOOPTIMIZE);

		for (i = 0; i < 256; ++i)
		{
			color[0] = color[1] = color[2] = i;

			cmsDoTransform(glo, xform, color, xyz, 1);

			gamma->to_linear[i] = xyz[1] * 4095.0f + 0.5f;
		}

		cmsDeleteTransform(glo, xform);

		xform = cmsCreateTransform(glo,
			xyz_pro,
			TYPE_XYZ_FLT,
			input_pro,
			input_fmt,
			INTENT_RELATIVE_COLORIMETRIC,
			cmsFLAGS_NOOPTIMIZE);

		for (i = 0; i < 4096; ++i)
		{
			// D50 illuminant
			xyz[0] = (i * 0.964212f) / 4095.0f;
			xyz[1] = (i * 1.000000f) / 4095.0f;
			xyz[2] = (i * 0.825188f) / 4095.0f;

			cmsDoTransform(glo, xform, xyz, color, 1);

			gamma->from_linear[i] = color[0];
		}

		cmsDeleteTransform(glo, xform);

		cmsCloseProfile(glo, xyz_pro);
	}
	else
#endif  // FZ_ENABLE_ICC

	/* Use the sRGB gamma curve if color management is not available. */
	{
		float c, y;
		int i;

		fallback:

		for (i = 0; i < 256; ++i) {
			c = i / 255.0f;
			if (c <= 0.04045f)
				y = c / 12.92f;
			else
				y = powf((c + 0.055f) / 1.055f, 2.4f);
			gamma->to_linear[i] = y * 4095.0f + 0.5f;
		}

		for (i = 0; i < 4096; ++i) {
			y = i / 4095.0f;
			if (y < 0.0031308f)
				c = 12.92f * y;
			else
				c = 1.055f * powf(y, 0.41666f) - 0.055f;
			gamma->from_linear[i] = c * 255.0f + 0.5f;
		}
	}
}

#endif  // FZ_ENABLE_GAMMA

#endif  // BUILDING_MUPDF_MINIMAL_CORE < 2

