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

#include "mupdf/fitz.h"

#if FZ_ENABLE_RENDER_CORE 

#include "draw-imp.h"
#include "glyph-imp.h"
#include "pixmap-imp.h"

#include <string.h>
#include "mupdf/assertions.h"

#if FZ_ENABLE_GAMMA

const fz_gamma_table *
fz_colorspace_gamma_tables(fz_colorspace *colorspace)
{
	return colorspace ? colorspace->gamma : NULL;
}

#endif

/*

The functions in this file implement various flavours of Porter-Duff blending.

We take the following as definitions:

	Cx = Color (from plane x)
	ax = Alpha (from plane x)
	cx = Cx.ax = Premultiplied color (from plane x)

The general PorterDuff blending equation is:

	Blend Z = X op Y	cz = Fx.cx + Fy.cy	where Fx and Fy depend on op

The two operations we use in this file are: '(X in Y) over Z' and
'S over Z'. The definitions of the 'over' and 'in' operations are as
follows:

	For S over Z,	Fs = 1, Fz = 1-as
	For X in Y,	Fx = ay, Fy = 0

We have 2 choices; we can either work with premultiplied data, or non
premultiplied data. Our

First the premultiplied case:

	Let S = (X in Y)
	Let R = (X in Y) over Z = S over Z

	cs	= cx.Fx + cy.Fy	(where Fx = ay, Fy = 0)
		= cx.ay
	as	= ax.Fx + ay.Fy
		= ax.ay

	cr	= cs.Fs + cz.Fz	(where Fs = 1, Fz = 1-as)
		= cs + cz.(1-as)
		= cx.ay + cz.(1-ax.ay)
	ar	= as.Fs + az.Fz
		= as + az.(1-as)
		= ax.ay + az.(1-ax.ay)

This has various nice properties, like not needing any divisions, and
being symmetric in color and alpha, so this is what we use. Because we
went through the pain of deriving the non premultiplied forms, we list
them here too, though they are not used.

Non Pre-multiplied case:

	Cs.as	= Fx.Cx.ax + Fy.Cy.ay	(where Fx = ay, Fy = 0)
		= Cx.ay.ax
	Cs	= (Cx.ay.ax)/(ay.ax)
		= Cx
	Cr.ar	= Fs.Cs.as + Fz.Cz.az	(where Fs = 1, Fz = 1-as)
		= Cs.as	+ (1-as).Cz.az
		= Cx.ax.ay + Cz.az.(1-ax.ay)
	Cr	= (Cx.ax.ay + Cz.az.(1-ax.ay))/(ax.ay + az.(1-ax.ay))

Much more complex, it seems. However, if we could restrict ourselves to
the case where we were always plotting onto an opaque background (i.e.
az = 1), then:

	Cr	= Cx.(ax.ay) + Cz.(1-ax.ay)
		= (Cx-Cz)*(1-ax.ay) + Cz	(a single MLA operation)
	ar	= 1

Sadly, this is not true in the general case, so we abandon this effort
and stick to using the premultiplied form.

*/

#if FZ_ENABLE_GAMMA

static const int16_t unpremultiply[256] = {
	0, 65280, 32640, 21760, 16320, 13056, 10880, 9325, 8160, 7253, 6528, 5934, 5440, 5021, 4662, 4352,
	4080, 3840, 3626, 3435, 3264, 3108, 2967, 2838, 2720, 2611, 2510, 2417, 2331, 2251, 2176, 2105,
	2040, 1978, 1920, 1865, 1813, 1764, 1717, 1673, 1632, 1592, 1554, 1518, 1483, 1450, 1419, 1388,
	1360, 1332, 1305, 1280, 1255, 1231, 1208, 1186, 1165, 1145, 1125, 1106, 1088, 1070, 1052, 1036,
	1020, 1004, 989, 974, 960, 946, 932, 919, 906, 894, 882, 870, 858, 847, 836, 826,
	816, 805, 796, 786, 777, 768, 759, 750, 741, 733, 725, 717, 709, 701, 694, 687,
	680, 672, 666, 659, 652, 646, 640, 633, 627, 621, 615, 610, 604, 598, 593, 588,
	582, 577, 572, 567, 562, 557, 553, 548, 544, 539, 535, 530, 526, 522, 518, 514,
	510, 506, 502, 498, 494, 490, 487, 483, 480, 476, 473, 469, 466, 462, 459, 456,
	453, 450, 447, 444, 441, 438, 435, 432, 429, 426, 423, 421, 418, 415, 413, 410,
	408, 405, 402, 400, 398, 395, 393, 390, 388, 386, 384, 381, 379, 377, 375, 373,
	370, 368, 366, 364, 362, 360, 358, 356, 354, 352, 350, 349, 347, 345, 343, 341,
	340, 338, 336, 334, 333, 331, 329, 328, 326, 324, 323, 321, 320, 318, 316, 315,
	313, 312, 310, 309, 307, 306, 305, 303, 302, 300, 299, 298, 296, 295, 294, 292,
	291, 290, 288, 287, 286, 285, 283, 282, 281, 280, 278, 277, 276, 275, 274, 273,
	272, 270, 269, 268, 267, 266, 265, 264, 263, 262, 261, 260, 259, 258, 257, 256
};

static fz_forceinline int FZ_BLEND_GAMMA(int src_srgb, int dst_srgb, int alpha, const fz_gamma_table *gamma)
{
	// alpha is 0..256
	// src_srgb, dst_srgb is 0..255
	// src_lin, dst_lin is 0..X
	int src_lin = gamma->to_linear[src_srgb];
	int dst_lin = gamma->to_linear[dst_srgb];
	int result = (((src_lin - dst_lin) * alpha) + (dst_lin << 8)) >> 8;
	return gamma->from_linear[result];
}

static fz_forceinline int FZ_BLEND_GAMMA_DA(int src_srgb, int dst_srgb_a, int dst_alpha, int src_alpha, const fz_gamma_table *gamma)
{
	// over = asrc * src + adest * ddst + asrc * adst * src
	// src_alpha is 0..256
	// dst_alpha is 0..255
	// out_alpha is 0..256
	int dst_srgb = (dst_srgb_a * unpremultiply[dst_alpha]) >> 8;
	int src_lin = gamma->to_linear[src_srgb];
	int dst_lin = gamma->to_linear[dst_srgb];
	// FIXME: THIS IS ALL WRONG!
	int out_alpha = (dst_alpha * src_alpha) >> 8;
	int out_lin = ((src_alpha * src_lin) + (dst_alpha * dst_lin) + (out_alpha * src_lin)) >> 8;
	int out_srgb = gamma->from_linear[out_lin];
	return (out_srgb * out_alpha) >> 8;
}

#endif /* FZ_ENABLE_GAMMA */

typedef void (fz_span_color_gamma_painter_t)(unsigned char * FZ_RESTRICT dp, const unsigned char * FZ_RESTRICT mp, int n, int w, const unsigned char * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma);

typedef unsigned char byte;

/* These are used by the non-aa scan converter */

static fz_forceinline void
template_solid_color_1_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int sa = FZ_EXPAND(color[1]);
	TRACK_FN();
	if (sa == 0)
		return;
	if (sa == 256)
	{
		do
		{
			dp[0] = color[0];
			dp[1] = 255;
			dp += 2;
		}
		while (--w);
	}
	else
	{
		do
		{
			dp[0] = FZ_BLEND(color[0], dp[0], sa);
			dp[1] = FZ_BLEND(255, dp[1], sa);
			dp += 2;
		}
		while (--w);
	}
}

static inline int isbigendian(void)
{
	union { int i; char c[sizeof(int)]; } u = {1};
	return u.c[0] != 1;
}

static fz_forceinline void
template_solid_color_3_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	unsigned int rgba = *(int *)color;
	int sa = FZ_EXPAND(color[3]);
	TRACK_FN();
	if (sa == 0)
		return;
	if (isbigendian())
		rgba |= 0x000000FF;
	else
		rgba |= 0xFF000000;
	if (sa == 256)
	{
		do
		{
			*(unsigned int *)dp = rgba;
			dp += 4;
		}
		while (--w);
	}
	else
	{
		unsigned int mask = 0xFF00FF00;
		unsigned int rb = rgba & (mask>>8);
		unsigned int ga = (rgba & mask)>>8;
		do
		{
			unsigned int RGBA = *(unsigned int *)dp;
			unsigned int RB = (RGBA<<8) & mask;
			unsigned int GA = RGBA & mask;
			RB += (rb-(RB>>8))*sa;
			GA += (ga-(GA>>8))*sa;
			RB &= mask;
			GA &= mask;
			*(unsigned int *)dp = (RB>>8) | GA;
			dp += 4;
		}
		while (--w);
	}
}

static fz_forceinline void
template_solid_color_4_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int sa = FZ_EXPAND(color[4]);
	TRACK_FN();
	if (sa == 0)
		return;
	if (sa == 256)
	{
#ifdef ARCH_UNALIGNED_OK
		if (w > 4)
		{
			if (isbigendian())
			{
				const uint32_t a = *(uint32_t*)color;
				const uint32_t b = 0xFF000000|(a>>8);
				const uint32_t c = 0x00FF0000|(a>>16)|(a<<24);
				const uint32_t d = 0x0000FF00|(a>>24)|(a<<16);
				const uint32_t e = 0x000000FF|(a<<8);
				w -= 3;
				do
				{
					((uint32_t *)(void *)dp)[0] = a;
					((uint32_t *)(void *)dp)[1] = b;
					((uint32_t *)(void *)dp)[2] = c;
					((uint32_t *)(void *)dp)[3] = d;
					((uint32_t *)(void *)dp)[4] = e;
					dp += 20;
					w -= 4;
				}
				while (w > 0);
			}
			else
			{
				const uint32_t a = *(uint32_t*)color;
				const uint32_t b = 0x000000FF|(a<<8);
				const uint32_t c = 0x0000FF00|(a<<16)|(a>>24);
				const uint32_t d = 0x00FF0000|(a<<24)|(a>>16);
				const uint32_t e = 0xFF000000|(a>>8);
				w -= 3;
				do
				{
					((uint32_t *)(void *)dp)[0] = a;
					((uint32_t *)(void *)dp)[1] = b;
					((uint32_t *)(void *)dp)[2] = c;
					((uint32_t *)(void *)dp)[3] = d;
					((uint32_t *)(void *)dp)[4] = e;
					dp += 20;
					w -= 4;
				}
				while (w > 0);
			}
			w += 3;
			if (w == 0)
				return;
		}
#endif
		do
		{
			dp[0] = color[0];
			dp[1] = color[1];
			dp[2] = color[2];
			dp[3] = color[3];
			dp[4] = 255;
			dp += 5;
		}
		while (--w);
	}
	else
	{
		do
		{
			dp[0] = FZ_BLEND(color[0], dp[0], sa);
			dp[1] = FZ_BLEND(color[1], dp[1], sa);
			dp[2] = FZ_BLEND(color[2], dp[2], sa);
			dp[3] = FZ_BLEND(color[3], dp[3], sa);
			dp[4] = FZ_BLEND(255, dp[5], sa);
			dp += 5;
		}
		while (--w);
	}
}

static fz_forceinline void
template_solid_color_N_256(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int k;
	int n1 = n - da;
	if (n == 3 && da == 0 && w >= 7)
	{
		union {uint32_t w[3]; byte b[12];} u;

		u.b[0] = u.b[3] = u.b[6] = u.b[9] = color[0];
		u.b[1] = u.b[4] = u.b[7] = u.b[10] = color[1];
		u.b[2] = u.b[5] = u.b[8] = u.b[11] = color[2];

		switch (((intptr_t)dp) & 3)
		{
		case 3:
			*dp++ = color[0];
			*(uint32_t *)dp = u.w[1];
			dp += 4;
			*(uint32_t *)dp = u.w[2];
			dp += 4;
			w -= 3;
			break;
		case 2:
			*dp++ = color[0];
			*dp++ = color[1];
			*(uint32_t *)dp = u.w[2];
			dp += 4;
			w -= 2;
			break;
		case 1:
			*dp++ = color[0];
			*dp++ = color[1];
			*dp++ = color[2];
			w -= 1;
			break;
		}
		w -= 4;
		do
		{
			*(uint32_t *)dp = u.w[0];
			dp += 4;
			*(uint32_t *)dp = u.w[1];
			dp += 4;
			*(uint32_t *)dp = u.w[2];
			dp += 4;
			w -= 4;
		}
		while (w > 0);
		w += 4;
		if (w == 0)
			return;
	}
	do
	{
		dp[0] = color[0];
		if (n1 > 1)
			dp[1] = color[1];
		if (n1 > 2)
			dp[2] = color[2];
		for (k = 3; k < n1; k++)
			dp[k] = color[k];
		if (da)
			dp[n1] = 255;
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_solid_color_N_256_op(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	int k;
	int n1 = n - da;
	do
	{
		if (fz_overprint_component(eop, 0))
			dp[0] = color[0];
		if (n1 > 1)
			if (fz_overprint_component(eop, 1))
				dp[1] = color[1];
		if (n1 > 2)
			if (fz_overprint_component(eop, 2))
				dp[2] = color[2];
		for (k = 3; k < n1; k++)
			if (fz_overprint_component(eop, k))
				dp[k] = color[k];
		if (da)
			dp[n1] = 255;
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_solid_color_N_sa(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, int sa)
{
	int k;
	int n1 = n - da;
	do
	{
		for (k = 0; k < n1; k++)
			dp[k] = FZ_BLEND(color[k], dp[k], sa);
		if (da)
			dp[k] = FZ_BLEND(255, dp[k], sa);
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_solid_color_N_sa_op(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, int sa, const fz_overprint * FZ_RESTRICT eop)
{
	int k;
	int n1 = n - da;
	do
	{
		for (k = 0; k < n1; k++)
			if (fz_overprint_component(eop, k))
				dp[k] = FZ_BLEND(color[k], dp[k], sa);
		if (da)
			dp[k] = FZ_BLEND(255, dp[k], sa);
		dp += n;
	}
	while (--w);
}

#if FZ_PLOTTERS_N
static fz_forceinline void
template_solid_color_N_general(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, int sa)
{
	int k;
	int n1 = n - da;
	if (sa == 256)
	{
		do
		{
			dp[0] = color[0];
			if (n1 > 1)
				dp[1] = color[1];
			if (n1 > 2)
				dp[2] = color[2];
			for (k = 3; k < n1; k++)
				dp[k] = color[k];
			if (da)
				dp[n1] = 255;
			dp += n;
		}
		while (--w);
	}
	else
	{
		do
		{
			for (k = 0; k < n1; k++)
				dp[k] = FZ_BLEND(color[k], dp[k], sa);
			if (da)
				dp[k] = FZ_BLEND(255, dp[k], sa);
			dp += n;
		}
		while (--w);
	}
}

static fz_forceinline void
template_solid_color_N_general_op(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, int sa, const fz_overprint * FZ_RESTRICT eop)
{
	int k;
	int n1 = n - da;
	if (sa == 256)
	{
		do
		{
			if (fz_overprint_component(eop, 0))
				dp[0] = color[0];
			if (n1 > 1)
				if (fz_overprint_component(eop, 1))
					dp[1] = color[1];
			if (n1 > 2)
				if (fz_overprint_component(eop, 2))
					dp[2] = color[2];
			for (k = 3; k < n1; k++)
				if (fz_overprint_component(eop, k))
					dp[k] = color[k];
			if (da)
				dp[n1] = 255;
			dp += n;
		}
		while (--w);
	}
	else
	{
		do
		{
			for (k = 0; k < n1; k++)
				if (fz_overprint_component(eop, k))
					dp[k] = FZ_BLEND(color[k], dp[k], sa);
			if (da)
				dp[k] = FZ_BLEND(255, dp[k], sa);
			dp += n;
		}
		while (--w);
	}
}
#endif

static fz_forceinline void
template_solid_color_0_da(byte * FZ_RESTRICT dp, int w, int sa)
{
	if (sa == 256)
	{
		memset(dp, 255, w);
	}
	else
	{
		do
		{
			*dp = FZ_BLEND(255, *dp, sa);
			dp++;
		}
		while (--w);
	}
}

#if FZ_PLOTTERS_G
static void paint_solid_color_1_alpha(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_sa(dp, 1, w, color, 0, FZ_EXPAND(color[1]));
}

static void paint_solid_color_1(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_256(dp, 1, w, color, 0);
}

static void paint_solid_color_1_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_1_da(dp, 2, w, color, 1);
}
#endif /* FZ_PLOTTERS_G */

static void paint_solid_color_0_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_0_da(dp, w, 256);
}

#if FZ_PLOTTERS_RGB
static void paint_solid_color_3_alpha(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_sa(dp, 3, w, color, 0, FZ_EXPAND(color[3]));
}

static void paint_solid_color_3(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_256(dp, 3, w, color, 0);
}

static void paint_solid_color_3_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_3_da(dp, 4, w, color, 1);
}
#endif /* FZ_PLOTTERS_RGB */

#if FZ_PLOTTERS_CMYK
static void paint_solid_color_4_alpha(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_sa(dp, 4, w, color, 0, FZ_EXPAND(color[4]));
}

static void paint_solid_color_4(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_256(dp, 4, w, color, 0);
}

static void paint_solid_color_4_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_4_da(dp, 5, w, color, 1);
}
#endif /* FZ_PLOTTERS_CMYK */

#if FZ_PLOTTERS_N
static void paint_solid_color_N_alpha(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_sa(dp, n, w, color, 0, FZ_EXPAND(color[n]));
}

static void paint_solid_color_N(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_256(dp, n, w, color, 0);
}

static void paint_solid_color_N_da(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_general(dp, n, w, color, 1, FZ_EXPAND(color[n-1]));
}
#endif /* FZ_PLOTTERS_N */

#if FZ_ENABLE_SPOT_RENDERING
static void paint_solid_color_N_alpha_op(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_sa_op(dp, n, w, color, 0, FZ_EXPAND(color[n]), eop);
}

static void paint_solid_color_N_op(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_256_op(dp, n, w, color, 0, eop);
}

static void paint_solid_color_N_da_op(byte * FZ_RESTRICT dp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_solid_color_N_general_op(dp, n, w, color, 1, FZ_EXPAND(color[n-1]), eop);
}
#endif /* FZ_ENABLE_SPOT_RENDERING */

fz_solid_color_painter_t *
fz_get_solid_color_painter(int n, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (da)
			return paint_solid_color_N_da_op;
		else if (color[n] == 255)
			return paint_solid_color_N_op;
		else
			return paint_solid_color_N_alpha_op;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch (n-da)
	{
		case 0:
			return paint_solid_color_0_da;
#if FZ_PLOTTERS_G
		case 1:
			if (da)
				return paint_solid_color_1_da;
			else if (color[1] == 255)
				return paint_solid_color_1;
			else
				return paint_solid_color_1_alpha;
#endif /* FZ_PLOTTERS_G */
#if FZ_PLOTTERS_RGB
		case 3:
			if (da)
				return paint_solid_color_3_da;
			else if (color[3] == 255)
				return paint_solid_color_3;
			else
				return paint_solid_color_3_alpha;
#endif /* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
		case 4:
			if (da)
				return paint_solid_color_4_da;
			else if (color[4] == 255)
				return paint_solid_color_4;
			else
				return paint_solid_color_4_alpha;
#endif /* FZ_PLOTTERS_CMYK */
		default:
#if FZ_PLOTTERS_N
			if (da)
				return paint_solid_color_N_da;
			else if (color[n] == 255)
				return paint_solid_color_N;
			else
				return paint_solid_color_N_alpha;
#else
			return NULL;
#endif /* FZ_PLOTTERS_N */
	}
}

/* Blend a non-premultiplied color in mask over destination */

static fz_forceinline void
template_span_with_color_1_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int g = color[0];
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			dp[0] = g;
			dp[1] = 255;
		}
		else if (ma != 0)
		{
			dp[0] = FZ_BLEND(g, dp[0], ma);
			dp[1] = FZ_BLEND(255, dp[1], ma);
		}
		dp += 2;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_1_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int sa = FZ_EXPAND(color[1]);
	int g = color[0];
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma != 0)
		{
			ma = FZ_COMBINE(ma, sa);
			dp[0] = FZ_BLEND(g, dp[0], ma);
			dp[1] = FZ_BLEND(255, dp[1], ma);
		}
		dp += 2;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_3_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	unsigned int rgba = *((const unsigned int *)color);
	unsigned int mask, rb, ga;
	if (isbigendian())
		rgba |= 0x000000FF;
	else
		rgba |= 0xFF000000;
	mask = 0xFF00FF00;
	rb = rgba & (mask>>8);
	ga = (rgba & mask)>>8;
	do
	{
		unsigned int ma = *mp++;
		dp += 4;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			((unsigned int *)dp)[-1] = rgba;
		}
		else if (ma != 0)
		{
			unsigned int RGBA = ((unsigned int *)dp)[-1];
			unsigned int RB = (RGBA<<8) & mask;
			unsigned int GA = RGBA & mask;
			RB += (rb-(RB>>8))*ma;
			GA += (ga-(GA>>8))*ma;
			RB &= mask;
			GA &= mask;
			((unsigned int *)dp)[-1] = (RB>>8) | GA;
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_3_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	unsigned int rgba = *((const unsigned int *)color);
	unsigned int mask, rb, ga;
	int sa = FZ_EXPAND(color[3]);
	if (isbigendian())
		rgba |= 0x000000FF;
	else
		rgba |= 0xFF000000;
	mask = 0xFF00FF00;
	rb = rgba & (mask>>8);
	ga = (rgba & mask)>>8;
	do
	{
		unsigned int ma = *mp++;
		ma = FZ_COMBINE(FZ_EXPAND(ma), sa);
		dp += 4;
		if (ma != 0)
			{
			unsigned int RGBA = ((unsigned int*)dp)[-1];
			unsigned int RB = (RGBA<<8) & mask;
			unsigned int GA = RGBA & mask;
			RB += (rb-(RB>>8))*ma;
			GA += (ga-(GA>>8))*ma;
			RB &= mask;
			GA &= mask;
			((unsigned int *)dp)[-1] = (RB>>8) | GA;
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_4_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int c = color[0];
	int m = color[1];
	int y = color[2];
	int k = color[3];
	TRACK_FN();
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			dp[0] = c;
			dp[1] = m;
			dp[2] = y;
			dp[3] = k;
			dp[4] = 255;
		}
		else if (ma != 0)
		{
			dp[0] = FZ_BLEND(c, dp[0], ma);
			dp[1] = FZ_BLEND(m, dp[1], ma);
			dp[2] = FZ_BLEND(y, dp[2], ma);
			dp[3] = FZ_BLEND(k, dp[3], ma);
			dp[4] = FZ_BLEND(255, dp[4], ma);
		}
		dp += 5;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_4_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int sa = FZ_EXPAND(color[4]);
	int c = color[0];
	int m = color[1];
	int y = color[2];
	int k = color[3];
	TRACK_FN();
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma != 0)
		{
			ma = FZ_COMBINE(ma, sa);
			dp[0] = FZ_BLEND(c, dp[0], ma);
			dp[1] = FZ_BLEND(m, dp[1], ma);
			dp[2] = FZ_BLEND(y, dp[2], ma);
			dp[3] = FZ_BLEND(k, dp[3], ma);
			dp[4] = FZ_BLEND(255, dp[4], ma);
		}
		dp += 5;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int k;
	int n1 = n - da;
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			if (n1 > 0)
				dp[0] = color[0];
			if (n1 > 1)
				dp[1] = color[1];
			if (n1 > 2)
				dp[2] = color[2];
			for (k = 3; k < n1; k++)
				dp[k] = color[k];
			if (da)
				dp[n1] = 255;
		}
		else if (ma != 0)
		{
			if (n1 > 0)
				dp[0] = FZ_BLEND(color[0], dp[0], ma);
			if (n1 > 1)
				dp[1] = FZ_BLEND(color[1], dp[1], ma);
			if (n1 > 2)
				dp[2] = FZ_BLEND(color[2], dp[2], ma);
			for (k = 3; k < n1; k++)
				dp[k] = FZ_BLEND(color[k], dp[k], ma);
			if (da)
				dp[n1] = FZ_BLEND(255, dp[n1], ma);
		}
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da)
{
	int k;
	int n1 = n - da;
	int sa = FZ_EXPAND(color[n1]);
	do
	{
		int ma = *mp++;
		ma = FZ_COMBINE(FZ_EXPAND(ma), sa);
		if (n1 > 0)
			dp[0] = FZ_BLEND(color[0], dp[0], ma);
		if (n1 > 1)
			dp[1] = FZ_BLEND(color[1], dp[1], ma);
		if (n1 > 2)
			dp[2] = FZ_BLEND(color[2], dp[2], ma);
		for (k = 3; k < n1; k++)
			dp[k] = FZ_BLEND(color[k], dp[k], ma);
		if (da)
			dp[n1] = FZ_BLEND(255, dp[n1], ma);
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_op_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	int k;
	int n1 = n - da;
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			if (n1 > 0)
				if (fz_overprint_component(eop, 0))
					dp[0] = color[0];
			if (n1 > 1)
				if (fz_overprint_component(eop, 1))
					dp[1] = color[1];
			if (n1 > 2)
				if (fz_overprint_component(eop, 2))
					dp[2] = color[2];
			for (k = 3; k < n1; k++)
				if (fz_overprint_component(eop, k))
					dp[k] = color[k];
			if (da)
				dp[n1] = 255;
		}
		else if (ma != 0)
		{
			for (k = 0; k < n1; k++)
				if (fz_overprint_component(eop, k))
					dp[k] = FZ_BLEND(color[k], dp[k], ma);
			if (da)
				dp[n1] = FZ_BLEND(255, dp[k], ma);
		}
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_op_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	int k;
	int n1 = n - da;
	int sa = FZ_EXPAND(color[n1]);
	do
	{
		int ma = *mp++;
		ma = FZ_COMBINE(FZ_EXPAND(ma), sa);
		for (k = 0; k < n1; k++)
			if (fz_overprint_component(eop, k))
				dp[k] = FZ_BLEND(color[k], dp[k], ma);
		if (da)
			dp[k] = FZ_BLEND(255, dp[k], ma);
		dp += n;
	}
	while (--w);
}

static void
paint_span_with_color_0_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_solid(dp, mp, 1, w, color, 1);
}

static void
paint_span_with_color_0_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha(dp, mp, 1, w, color, 1);
}

static void
paint_span_with_color_1_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_solid(dp, mp, 1, w, color, 0);
}

static void
paint_span_with_color_1_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha(dp, mp, 1, w, color, 0);
}

static void
paint_span_with_color_1_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_1_da_solid(dp, mp, 2, w, color, 1);
}

static void
paint_span_with_color_1_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_1_da_alpha(dp, mp, 2, w, color, 1);
}

#if FZ_PLOTTERS_RGB
static void
paint_span_with_color_3_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_solid(dp, mp, 3, w, color, 0);
}

static void
paint_span_with_color_3_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_3_da_solid(dp, mp, 4, w, color, 1);
}

static void
paint_span_with_color_3_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha(dp, mp, 3, w, color, 0);
}

static void
paint_span_with_color_3_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_3_da_alpha(dp, mp, 4, w, color, 1);
}
#endif /* FZ_PLOTTERS_RGB */

#if FZ_PLOTTERS_CMYK
static void
paint_span_with_color_4_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_solid(dp, mp, 4, w, color, 0);
}

static void
paint_span_with_color_4_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha(dp, mp, 4, w, color, 0);
}

static void
paint_span_with_color_4_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_4_da_solid(dp, mp, 5, w, color, 1);
}

static void
paint_span_with_color_4_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_4_da_alpha(dp, mp, 5, w, color, 1);
}
#endif /* FZ_PLOTTERS_CMYK */

#if FZ_PLOTTERS_N
static void
paint_span_with_color_N_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_solid(dp, mp, n, w, color, 0);
}

static void
paint_span_with_color_N_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha(dp, mp, n, w, color, 0);
}

static void
paint_span_with_color_N_da_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_solid(dp, mp, n, w, color, 1);
}

static void
paint_span_with_color_N_da_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha(dp, mp, n, w, color, 1);
}
#endif /* FZ_PLOTTERS_N */

#if FZ_ENABLE_SPOT_RENDERING
static void
paint_span_with_color_N_op_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_op_solid(dp, mp, n, w, color, 0, eop);
}

static void
paint_span_with_color_N_op_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_op_alpha(dp, mp, n, w, color, 0, eop);
}

static void
paint_span_with_color_N_da_op_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_op_solid(dp, mp, n, w, color, 1, eop);
}

static void
paint_span_with_color_N_da_op_alpha(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_color_N_general_op_alpha(dp, mp, n, w, color, 1, eop);
}
#endif /* FZ_ENABLE_SPOT_RENDERING */

fz_span_color_painter_t *
fz_get_span_color_painter(int n, int da, const byte * FZ_RESTRICT color, const fz_overprint * FZ_RESTRICT eop)
{
	byte alpha = color[n-da];
	if (alpha == 0)
		return NULL;
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (alpha == 255)
			return da ? paint_span_with_color_N_da_op_solid : paint_span_with_color_N_op_solid;
		else
			return da ? paint_span_with_color_N_da_op_alpha : paint_span_with_color_N_op_alpha;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch(n-da)
	{
	case 0:
		if (alpha == 255)
			return da ? paint_span_with_color_0_da_solid : NULL;
		else
			return da ? paint_span_with_color_0_da_alpha : NULL;
	case 1:
		if (alpha == 255)
			return da ? paint_span_with_color_1_da_solid : paint_span_with_color_1_solid;
		else
			return da ? paint_span_with_color_1_da_alpha : paint_span_with_color_1_alpha;
#if FZ_PLOTTERS_RGB
	case 3:
		if (alpha == 255)
			return da ? paint_span_with_color_3_da_solid : paint_span_with_color_3_solid;
		else
			return da ? paint_span_with_color_3_da_alpha : paint_span_with_color_3_alpha;
#endif/* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
	case 4:
		if (alpha == 255)
			return da ? paint_span_with_color_4_da_solid : paint_span_with_color_4_solid;
		else
			return da ? paint_span_with_color_4_da_alpha : paint_span_with_color_4_alpha;
#endif/* FZ_PLOTTERS_CMYK */
#if FZ_PLOTTERS_N
	default:
		if (alpha == 255)
			return da ? paint_span_with_color_N_da_solid : paint_span_with_color_N_solid;
		else
			return da ? paint_span_with_color_N_da_alpha : paint_span_with_color_N_alpha;
#else
	default: return NULL;
#endif /* FZ_PLOTTERS_N */
	}
}

#if FZ_ENABLE_GAMMA
static fz_forceinline void
template_span_with_color_1_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int g = color[0];
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			dp[0] = g;
			dp[1] = 255;
		}
		else if (ma != 0)
		{
			dp[0] = FZ_BLEND_GAMMA_DA(g, dp[0], dp[1], ma, gamma);
			dp[1] = FZ_BLEND(255, dp[1], ma);
		}
		dp += 2;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_1_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int sa = FZ_EXPAND(color[1]);
	int g = color[0];
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma != 0)
		{
			ma = FZ_COMBINE(ma, sa);
			dp[0] = FZ_BLEND_GAMMA_DA(g, dp[0], dp[1], ma, gamma);
			dp[1] = FZ_BLEND(255, dp[1], ma);
		}
		dp += 2;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_3_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int r = color[0];
	int g = color[1];
	int b = color[2];
	TRACK_FN();
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			dp[0] = r;
			dp[1] = g;
			dp[2] = b;
			dp[3] = 255;
		}
		else if (ma != 0)
		{
			dp[0] = FZ_BLEND_GAMMA_DA(r, dp[0], dp[3], ma, gamma);
			dp[1] = FZ_BLEND_GAMMA_DA(g, dp[1], dp[3], ma, gamma);
			dp[2] = FZ_BLEND_GAMMA_DA(b, dp[2], dp[3], ma, gamma);
			dp[3] = FZ_BLEND(255, dp[3], ma);
		}
		dp += 4;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_3_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int sa = FZ_EXPAND(color[3]);
	int r = color[0];
	int g = color[1];
	int b = color[2];
	TRACK_FN();
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma != 0)
		{
			ma = FZ_COMBINE(ma, sa);
			dp[0] = FZ_BLEND_GAMMA_DA(r, dp[0], dp[3], ma, gamma);
			dp[1] = FZ_BLEND_GAMMA_DA(g, dp[1], dp[3], ma, gamma);
			dp[2] = FZ_BLEND_GAMMA_DA(b, dp[2], dp[3], ma, gamma);
			dp[3] = FZ_BLEND(255, dp[3], ma);
		}
		dp += 4;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_4_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int c = color[0];
	int m = color[1];
	int y = color[2];
	int k = color[3];
	TRACK_FN();
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			dp[0] = c;
			dp[1] = m;
			dp[2] = y;
			dp[3] = k;
			dp[4] = 255;
		}
		else if (ma != 0)
		{
			dp[0] = FZ_BLEND_GAMMA_DA(c, dp[0], dp[4], ma, gamma);
			dp[1] = FZ_BLEND_GAMMA_DA(m, dp[1], dp[4], ma, gamma);
			dp[2] = FZ_BLEND_GAMMA_DA(y, dp[2], dp[4], ma, gamma);
			dp[3] = FZ_BLEND_GAMMA_DA(k, dp[3], dp[4], ma, gamma);
			dp[4] = FZ_BLEND(255, dp[4], ma);
		}
		dp += 5;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_4_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int sa = FZ_EXPAND(color[4]);
	int c = color[0];
	int m = color[1];
	int y = color[2];
	int k = color[3];
	TRACK_FN();
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma != 0)
		{
			ma = FZ_COMBINE(ma, sa);
			dp[0] = FZ_BLEND_GAMMA_DA(c, dp[0], dp[4], ma, gamma);
			dp[1] = FZ_BLEND_GAMMA_DA(m, dp[1], dp[4], ma, gamma);
			dp[2] = FZ_BLEND_GAMMA_DA(y, dp[2], dp[4], ma, gamma);
			dp[3] = FZ_BLEND_GAMMA_DA(k, dp[3], dp[4], ma, gamma);
			dp[4] = FZ_BLEND(255, dp[4], ma);
		}
		dp += 5;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int k;
	int n1 = n - da;
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			if (n1 > 0)
				dp[0] = color[0];
			if (n1 > 1)
				dp[1] = color[1];
			if (n1 > 2)
				dp[2] = color[2];
			for (k = 3; k < n1; k++)
				dp[k] = color[k];
			if (da)
				dp[n1] = 255;
		}
		else if (ma != 0)
		{
			if (da)
			{
				if (n1 > 0)
					dp[0] = FZ_BLEND_GAMMA_DA(color[0], dp[0], dp[n1], ma, gamma);
				if (n1 > 1)
					dp[1] = FZ_BLEND_GAMMA_DA(color[1], dp[1], dp[n1], ma, gamma);
				if (n1 > 2)
					dp[2] = FZ_BLEND_GAMMA_DA(color[2], dp[2], dp[n1], ma, gamma);
				for (k = 3; k < n1; k++)
					dp[k] = FZ_BLEND_GAMMA_DA(color[k], dp[k], dp[n1], ma, gamma);
				dp[n1] = FZ_BLEND(255, dp[n1], ma);
			}
			else
			{
				if (n1 > 0)
					dp[0] = FZ_BLEND_GAMMA(color[0], dp[0], ma, gamma);
				if (n1 > 1)
					dp[1] = FZ_BLEND_GAMMA(color[1], dp[1], ma, gamma);
				if (n1 > 2)
					dp[2] = FZ_BLEND_GAMMA(color[2], dp[2], ma, gamma);
				for (k = 3; k < n1; k++)
					dp[k] = FZ_BLEND_GAMMA(color[k], dp[k], ma, gamma);
			}
		}
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int k;
	int n1 = n - da;
	int sa = FZ_EXPAND(color[n1]);
	do
	{
		int ma = *mp++;
		ma = FZ_COMBINE(FZ_EXPAND(ma), sa);
		if (da)
		{
			if (n1 > 0)
				dp[0] = FZ_BLEND_GAMMA_DA(color[0], dp[0], dp[n1], ma, gamma);
			if (n1 > 1)
				dp[1] = FZ_BLEND_GAMMA_DA(color[1], dp[1], dp[n1], ma, gamma);
			if (n1 > 2)
				dp[2] = FZ_BLEND_GAMMA_DA(color[2], dp[2], dp[n1], ma, gamma);
			for (k = 3; k < n1; k++)
				dp[k] = FZ_BLEND_GAMMA_DA(color[k], dp[k], dp[n1], ma, gamma);
			dp[n1] = FZ_BLEND(255, dp[n1], ma);
		}
		else
		{
			if (n1 > 0)
				dp[0] = FZ_BLEND_GAMMA(color[0], dp[0], ma, gamma);
			if (n1 > 1)
				dp[1] = FZ_BLEND_GAMMA(color[1], dp[1], ma, gamma);
			if (n1 > 2)
				dp[2] = FZ_BLEND_GAMMA(color[2], dp[2], ma, gamma);
			for (k = 3; k < n1; k++)
				dp[k] = FZ_BLEND_GAMMA(color[k], dp[k], ma, gamma);
		}
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_op_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int k;
	int n1 = n - da;
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 256)
		{
			if (n1 > 0)
				if (fz_overprint_component(eop, 0))
					dp[0] = color[0];
			if (n1 > 1)
				if (fz_overprint_component(eop, 1))
					dp[1] = color[1];
			if (n1 > 2)
				if (fz_overprint_component(eop, 2))
					dp[2] = color[2];
			for (k = 3; k < n1; k++)
				if (fz_overprint_component(eop, k))
					dp[k] = color[k];
			if (da)
				dp[n1] = 255;
		}
		else if (ma != 0)
		{
			if (da)
			{
				for (k = 0; k < n1; k++)
					if (fz_overprint_component(eop, k))
						dp[k] = FZ_BLEND_GAMMA_DA(color[k], dp[k], dp[n1], ma, gamma);
				dp[n1] = FZ_BLEND(255, dp[n1], ma);
			}
			else
			{
				for (k = 0; k < n1; k++)
					if (fz_overprint_component(eop, k))
						dp[k] = FZ_BLEND_GAMMA(color[k], dp[k], ma, gamma);
			}
		}
		dp += n;
	}
	while (--w);
}

static fz_forceinline void
template_span_with_color_N_general_op_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	int k;
	int n1 = n - da;
	int sa = FZ_EXPAND(color[n1]);
	do
	{
		int ma = *mp++;
		ma = FZ_COMBINE(FZ_EXPAND(ma), sa);
		if (da)
		{
			for (k = 0; k < n1; k++)
				if (fz_overprint_component(eop, k))
					dp[k] = FZ_BLEND_GAMMA_DA(color[k], dp[k], dp[n1], ma, gamma);
			dp[n1] = FZ_BLEND(255, dp[n1], ma);
		}
		else
		{
			for (k = 0; k < n1; k++)
				if (fz_overprint_component(eop, k))
					dp[k] = FZ_BLEND_GAMMA(color[k], dp[k], ma, gamma);
		}
		dp += n;
	}
	while (--w);
}

static void
paint_span_with_color_0_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_solid_gamma(dp, mp, 1, w, color, 1, gamma);
}

static void
paint_span_with_color_0_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha_gamma(dp, mp, 1, w, color, 1, gamma);
}

static void
paint_span_with_color_1_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_solid_gamma(dp, mp, 1, w, color, 0, gamma);
}

static void
paint_span_with_color_1_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha_gamma(dp, mp, 1, w, color, 0, gamma);
}

static void
paint_span_with_color_1_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_1_da_solid_gamma(dp, mp, 2, w, color, 1, gamma);
}

static void
paint_span_with_color_1_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_1_da_alpha_gamma(dp, mp, 2, w, color, 1, gamma);
}

#if FZ_PLOTTERS_RGB
static void
paint_span_with_color_3_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_solid_gamma(dp, mp, 3, w, color, 0, gamma);
}

static void
paint_span_with_color_3_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_3_da_solid_gamma(dp, mp, 4, w, color, 1, gamma);
}

static void
paint_span_with_color_3_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha_gamma(dp, mp, 3, w, color, 0, gamma);
}

static void
paint_span_with_color_3_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_3_da_alpha_gamma(dp, mp, 4, w, color, 1, gamma);
}
#endif /* FZ_PLOTTERS_RGB */

#if FZ_PLOTTERS_CMYK
static void
paint_span_with_color_4_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_solid_gamma(dp, mp, 4, w, color, 0, gamma);
}

static void
paint_span_with_color_4_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha_gamma(dp, mp, 4, w, color, 0, gamma);
}

static void
paint_span_with_color_4_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_4_da_solid_gamma(dp, mp, 5, w, color, 1, gamma);
}

static void
paint_span_with_color_4_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_4_da_alpha_gamma(dp, mp, 5, w, color, 1, gamma);
}
#endif /* FZ_PLOTTERS_CMYK */

#if FZ_PLOTTERS_N
static void
paint_span_with_color_N_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_solid_gamma(dp, mp, n, w, color, 0, gamma);
}

static void
paint_span_with_color_N_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha_gamma(dp, mp, n, w, color, 0, gamma);
}

static void
paint_span_with_color_N_da_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_solid_gamma(dp, mp, n, w, color, 1, gamma);
}

static void
paint_span_with_color_N_da_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_alpha_gamma(dp, mp, n, w, color, 1, gamma);
}
#endif /* FZ_PLOTTERS_N */

#ifdef FZ_ENABLE_SPOT_RENDERING
static void
paint_span_with_color_N_op_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_op_solid_gamma(dp, mp, n, w, color, 0, eop, gamma);
}

static void
paint_span_with_color_N_op_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_op_alpha_gamma(dp, mp, n, w, color, 0, eop, gamma);
}

static void
paint_span_with_color_N_da_op_solid_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_op_solid_gamma(dp, mp, n, w, color, 1, eop, gamma);
}

static void
paint_span_with_color_N_da_op_alpha_gamma(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT mp, int n, int w, const byte * FZ_RESTRICT color, int da, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table * FZ_RESTRICT gamma)
{
	TRACK_FN();
	template_span_with_color_N_general_op_alpha_gamma(dp, mp, n, w, color, 1, eop, gamma);
}
#endif /* FZ_ENABLE_SPOT_RENDERING */

fz_span_color_gamma_painter_t *
fz_get_span_color_painter_gamma(int n, int da, const byte * FZ_RESTRICT color, const fz_overprint * FZ_RESTRICT eop)
{
	byte alpha = color[n-da];
	if (alpha == 0)
		return NULL;
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (alpha == 255)
			return da ? paint_span_with_color_N_da_op_solid_gamma : paint_span_with_color_N_op_solid_gamma;
		else
			return da ? paint_span_with_color_N_da_op_alpha_gamma : paint_span_with_color_N_op_alpha_gamma;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch(n-da)
	{
	case 0:
		if (alpha == 255)
			return da ? paint_span_with_color_0_da_solid_gamma : NULL;
		else
			return da ? paint_span_with_color_0_da_alpha_gamma : NULL;
	case 1:
		if (alpha == 255)
			return da ? paint_span_with_color_1_da_solid_gamma : paint_span_with_color_1_solid_gamma;
		else
			return da ? paint_span_with_color_1_da_alpha_gamma : paint_span_with_color_1_alpha_gamma;
#if FZ_PLOTTERS_RGB
	case 3:
		if (alpha == 255)
			return da ? paint_span_with_color_3_da_solid_gamma : paint_span_with_color_3_solid_gamma;
		else
			return da ? paint_span_with_color_3_da_alpha_gamma : paint_span_with_color_3_alpha_gamma;
#endif/* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
	case 4:
		if (alpha == 255)
			return da ? paint_span_with_color_4_da_solid_gamma : paint_span_with_color_4_solid_gamma;
		else
			return da ? paint_span_with_color_4_da_alpha_gamma : paint_span_with_color_4_alpha_gamma;
#endif/* FZ_PLOTTERS_CMYK */
#if FZ_PLOTTERS_N
	default:
		if (alpha == 255)
			return da ? paint_span_with_color_N_da_solid_gamma : paint_span_with_color_N_solid_gamma;
		else
			return da ? paint_span_with_color_N_da_alpha_gamma : paint_span_with_color_N_alpha_gamma;
#else
	default: return NULL;
#endif /* FZ_PLOTTERS_N */
	}
}
#endif /* FZ_GLYPH_GAMMA_BLEND */

/* Blend source in mask over destination */

/* FIXME: There is potential for SWAR optimisation here */
static fz_forceinline void
template_span_with_mask_1_general(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, int a, const byte * FZ_RESTRICT mp, int w)
{
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 0 || (a && sp[1] == 0))
		{
			dp += 1 + a;
			sp += 1 + a;
		}
		else if (ma == 256)
		{
			*dp++ = *sp++;
			if (a)
				*dp++ = *sp++;
		}
		else
		{
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
			if (a)
			{
				*dp = FZ_BLEND(*sp, *dp, ma);
				sp++; dp++;
			}
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_with_mask_3_general(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, int a, const byte * FZ_RESTRICT mp, int w)
{
#ifndef NDEBUG
	const int bigendian_p = isbigendian();
#endif

	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 0 || (a && sp[3] == 0))
		{
			dp += 3 + a;
			sp += 3 + a;
		}
		else if (ma == 256)
		{
			if (a)
			{
				*(int32_t *)dp = *(int32_t *)sp;
				sp += 4; dp += 4;
			}
			else
			{
				*dp++ = *sp++;
				*dp++ = *sp++;
				*dp++ = *sp++;
			}
		}
		else if (a)
		{
			const uint32_t mask = 0x00ff00ff;
			uint32_t d0 = *(uint32_t *)dp;
			uint32_t d1 = d0>>8;
			uint32_t s0 = *(uint32_t *)sp;
			uint32_t s1 = s0>>8;
			d0 &= mask;
			d1 &= mask;
			s0 &= mask;
			s1 &= mask;
			d0 = (((d0<<8) + (s0-d0)*ma)>>8) & mask;
			d1 = ((d1<<8) + (s1-d1)*ma) & ~mask;
			d0 |= d1;

#ifndef NDEBUG
			if (bigendian_p)
			{
				ASSERT0((d0 & 0xff) >= (d0>>24));
				ASSERT0((d0 & 0xff) >= ((d0>>16) & 0xff));
				ASSERT0((d0 & 0xff) >= ((d0>>8) & 0xff));
			}
			else
			{
				ASSERT0((d0>>24) >= (d0 & 0xff));
				ASSERT0((d0>>24) >= ((d0>>8) & 0xff));
				ASSERT0((d0>>24) >= ((d0>>16) & 0xff));
			}
#endif

			*(uint32_t *)dp = d0;
			sp += 4;
			dp += 4;
		}
		else
		{
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_with_mask_4_general(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, int a, const byte * FZ_RESTRICT mp, int w)
{
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 0 || (a && sp[4] == 0))
		{
			dp += 4 + a;
			sp += 4 + a;
		}
		else if (ma == 256)
		{
			if (!a)
			{
				*(uint32_t *)dp = *(uint32_t *)sp;
				dp += 4;
				sp += 4;
			}
			else
			{
				*dp++ = *sp++;
				*dp++ = *sp++;
				*dp++ = *sp++;
				*dp++ = *sp++;
				*dp++ = *sp++;
			}
		}
		else if (a)
		{
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
			*dp = FZ_BLEND(*sp, *dp, ma);
			sp++; dp++;
		}
		else
		{
			const uint32_t mask = 0x00ff00ff;
			uint32_t d0 = *(uint32_t *)dp;
			uint32_t d1 = d0>>8;
			uint32_t s0 = *(uint32_t *)sp;
			uint32_t s1 = s0>>8;
			sp += 4;
			d0 &= mask;
			d1 &= mask;
			s0 &= mask;
			s1 &= mask;
			d0 = (((d0<<8) + (s0-d0)*ma)>>8) & mask;
			d1 = ((d1<<8) + (s1-d1)*ma) & ~mask;
			d0 |= d1;
			*(uint32_t *)dp = d0;
			dp += 4;
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_with_mask_N_general(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, int a, const byte * FZ_RESTRICT mp, int n, int w)
{
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 0 || (a && sp[n] == 0))
		{
			dp += n + a;
			sp += n + a;
		}
		else if (ma == 256)
		{
			int k;
			for (k = 0; k < n; k++)
				*dp++ = *sp++;
			if (a)
				*dp++ = *sp++;
		}
		else
		{
			int k;
			for (k = 0; k < n; k++)
			{
				*dp = FZ_BLEND(*sp, *dp, ma);
				sp++; dp++;
			}
			if (a)
			{
				*dp = FZ_BLEND(*sp, *dp, ma);
				sp++; dp++;
			}
		}
	}
	while (--w);
}

static void
paint_span_with_mask_0_a(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_N_general(dp, sp, 1, mp, 0, w);
}

static void
paint_span_with_mask_1_a(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_1_general(dp, sp, 1, mp, w);
}

static void
paint_span_with_mask_1(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_1_general(dp, sp, 0, mp, w);
}

#if FZ_PLOTTERS_RGB
static void
paint_span_with_mask_3_a(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_3_general(dp, sp, 1, mp, w);
}

static void
paint_span_with_mask_3(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_3_general(dp, sp, 0, mp, w);
}
#endif /* FZ_PLOTTERS_RGB */

#if FZ_PLOTTERS_CMYK
static void
paint_span_with_mask_4_a(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_4_general(dp, sp, 1, mp, w);
}

static void
paint_span_with_mask_4(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_4_general(dp, sp, 0, mp, w);
}
#endif /* FZ_PLOTTERS_CMYK */

#if FZ_PLOTTERS_N
static void
paint_span_with_mask_N_a(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_N_general(dp, sp, 1, mp, n, w);
}

static void
paint_span_with_mask_N(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_with_mask_N_general(dp, sp, 0, mp, n, w);
}
#endif /* FZ_PLOTTERS_N */

typedef void (fz_span_mask_painter_t)(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w, int n, int a, const fz_overprint * FZ_RESTRICT eop);

static fz_span_mask_painter_t *
fz_get_span_mask_painter(int a, int n)
{
	switch(n)
	{
		case 0:
			/* ASSERT0(a); */
			return paint_span_with_mask_0_a;
		case 1:
			if (a)
				return paint_span_with_mask_1_a;
			else
				return paint_span_with_mask_1;
#if FZ_PLOTTERS_RGB
		case 3:
			if (a)
				return paint_span_with_mask_3_a;
			else
				return paint_span_with_mask_3;
#endif /* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
		case 4:
			if (a)
				return paint_span_with_mask_4_a;
			else
				return paint_span_with_mask_4;
#endif /* FZ_PLOTTERS_CMYK */
		default:
		{
#if FZ_PLOTTERS_N
			if (a)
				return paint_span_with_mask_N_a;
			else
				return paint_span_with_mask_N;
#else
			return NULL;
#endif /* FZ_PLOTTERS_N */
		}
	}
}

/* Blend source in constant alpha over destination */

static fz_forceinline void
template_span_1_with_alpha_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int w, int alpha)
{
	if (sa)
		alpha = FZ_EXPAND(alpha);
	do
	{
		int masa = (sa ? FZ_COMBINE(sp[1], alpha) : alpha);
		int t = FZ_EXPAND(255-masa);
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		dp++; sp++;
		if (da)
		{
			*dp = masa + FZ_COMBINE(*dp, t);
			dp++;
		}
		if (sa)
			 sp++;
	}
	while (--w);
}

static fz_forceinline void
template_span_3_with_alpha_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int w, int alpha)
{
	if (sa)
		alpha = FZ_EXPAND(alpha);
	do
	{
		int masa = (sa ? FZ_COMBINE(sp[3], alpha) : alpha);
		int t = FZ_EXPAND(255-masa);
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		sp++; dp++;
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		sp++; dp++;
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		sp++; dp++;
		if (da)
		{
			*dp = masa + FZ_COMBINE(*dp, t);
			dp++;
		}
		if (sa)
			sp++;
	}
	while (--w);
}

static fz_forceinline void
template_span_4_with_alpha_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int w, int alpha)
{
	if (sa)
		alpha = FZ_EXPAND(alpha);
	do
	{
		int masa = (sa ? FZ_COMBINE(sp[4], alpha) : alpha);
		int t = FZ_EXPAND(255-masa);
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		sp++; dp++;
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		sp++; dp++;
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		sp++; dp++;
		*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
		sp++; dp++;
		if (da)
		{
			*dp = masa + FZ_COMBINE(*dp, t);
			dp++;
		}
		if (sa)
			sp++;
	}
	while (--w);
}

#if FZ_PLOTTERS_N
static fz_forceinline void
template_span_N_with_alpha_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n1, int w, int alpha)
{
	if (sa)
		alpha = FZ_EXPAND(alpha);
	do
	{
		int masa = (sa ? FZ_COMBINE(sp[n1], alpha) : alpha);
		int t = FZ_EXPAND(255-masa);
		int k;
		for (k = 0; k < n1; k++)
		{
			*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
			sp++; dp++;
		}
		if (da)
		{
			*dp = masa + FZ_COMBINE(*dp, t);
			dp++;
		}
		if (sa)
			sp++;
	}
	while (--w);
}

static fz_forceinline void
template_span_N_with_alpha_general_op(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n1, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	if (sa)
		alpha = FZ_EXPAND(alpha);
	do
	{
		int masa = (sa ? FZ_COMBINE(sp[n1], alpha) : alpha);
		int t = FZ_EXPAND(255-masa);
		int k;
		for (k = 0; k < n1; k++)
		{
			if (fz_overprint_component(eop, k))
				*dp = FZ_COMBINE(*sp, alpha) + FZ_COMBINE(*dp, t);
			sp++;
			dp++;
		}
		if (da)
		{
			*dp = masa + FZ_COMBINE(*dp, t);
			dp++;
		}
		if (sa)
			sp++;
	}
	while (--w);
}
#endif

/* Blend source over destination */

static fz_forceinline void
template_span_1_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int w)
{
	do
	{
		int t = (sa ? FZ_EXPAND(sp[1]): 256);
		if (t == 0)
		{
			dp += 1 + da; sp += 1 + sa;
		}
		else
		{
			t = 256 - t;
			if (t == 0)
			{
				*dp++ = *sp++;
				if (da)
					*dp++ = (sa ? *sp : 255);
				if (sa)
					sp++;
			}
			else
			{
				*dp = *sp + FZ_COMBINE(*dp, t);
				sp++;
				dp++;
				if (da)
				{
					*dp = *sp + FZ_COMBINE(*dp, t);
					dp++;
				}
				sp++;
			}
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_3_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int w)
{
	do
	{
		int t = (sa ? FZ_EXPAND(sp[3]) : 256);
		if (t == 0)
		{
			dp += 3 + da; sp += 3 + sa;
		}
		else
		{
			t = 256 - t;
			if (t == 0)
			{
				if (da && sa)
					*(int32_t *)dp = *(const int32_t *)sp;
				else
				{
					dp[0] = sp[0];
					dp[1] = sp[1];
					dp[2] = sp[2];
					if (da)
						dp[3] = 255;
				}
				dp += 3+da; sp += 3+sa;
			}
			else
			{
				/* sa != 0 as t != 0 */
				*dp = *sp++ + FZ_COMBINE(*dp, t);
				dp++;
				*dp = *sp++ + FZ_COMBINE(*dp, t);
				dp++;
				*dp = *sp++ + FZ_COMBINE(*dp, t);
				dp++;
				if (da)
				{
					*dp = *sp + FZ_COMBINE(*dp, t);
					dp++;
				}
				sp++;
			}
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_4_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int w)
{
	do
	{
		int t = (sa ? FZ_EXPAND(sp[4]) : 256);
		if (t == 0)
		{
			dp += 4+da; sp += 4+sa;
		}
		else
		{
			t = 256 - t;
			if (t == 0)
			{
				dp[0] = sp[0];
				dp[1] = sp[1];
				dp[2] = sp[2];
				dp[3] = sp[3];
				if (da)
					dp[4] = (sa ? sp[4] : 255);
				dp += 4+da; sp += 4 + sa;
			}
			else
			{
				/* sa != 0 as t != 0 */
				*dp = *sp++ + FZ_COMBINE(*dp, t);
				dp++;
				*dp = *sp++ + FZ_COMBINE(*dp, t);
				dp++;
				*dp = *sp++ + FZ_COMBINE(*dp, t);
				dp++;
				*dp = *sp++ + FZ_COMBINE(*dp, t);
				dp++;
				if (da)
				{
					*dp = *sp + FZ_COMBINE(*dp, t);
					dp++;
				}
				sp++;
			}
		}
	}
	while (--w);
}

#if FZ_PLOTTERS_N
static fz_forceinline void
template_span_N_general(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n1, int w)
{
	do
	{
		int t = (sa ? FZ_EXPAND(sp[n1]) : 256);
		if (t == 0)
		{
			dp += n1 + da; sp += n1 + sa;
		}
		else
		{
			t = 256 - t;
			if (t == 0)
			{
				int k;
				for (k = 0; k < n1; k++)
					*dp++ = *sp++;
				if (da)
					*dp++ = (sa ? *sp : 255);
				if (sa)
					sp++;
			}
			else
			{
				/* sa != 0, as t != 0 */
				int k;
				for (k = 0; k < n1; k++)
				{
					*dp = *sp + FZ_COMBINE(*dp, t);
					sp++;
					dp++;
				}
				if (da)
				{
					*dp = *sp + FZ_COMBINE(*dp, t);
					dp++;
				}
				sp++;
			}
		}
	}
	while (--w);
}

static fz_forceinline void
template_span_N_general_op(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n1, int w, const fz_overprint * FZ_RESTRICT eop)
{
	do
	{
		int t = (sa ? FZ_EXPAND(sp[n1]) : 256);
		if (t == 0)
		{
			dp += n1 + da; sp += n1 + sa;
		}
		else
		{
			t = 256 - t;
			if (t == 0)
			{
				int k;
				for (k = 0; k < n1; k++)
				{
					if (fz_overprint_component(eop, k))
						*dp = *sp;
					dp++;
					sp++;
				}
				if (da)
					*dp++ = (sa ? *sp : 255);
				if (sa)
					sp++;
			}
			else
			{
				int k;
				/* sa can never be 0 here, as t != 0. */
				for (k = 0; k < n1; k++)
				{
					if (fz_overprint_component(eop, k))
						*dp = *sp + FZ_COMBINE(*dp, t);
					sp++;
					dp++;
				}
				if (da)
				{
					*dp = *sp + FZ_COMBINE(*dp, t);
					dp++;
				}
				sp++;
			}
		}
	}
	while (--w);
}
#endif

static void
paint_span_0_da_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	do
	{
		int s = *sp++;
		int t = FZ_EXPAND(255 - s);
		*dp = s + FZ_COMBINE(*dp, t);
		dp ++;
	}
	while (--w);
}

static void
paint_span_0_da_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	alpha = FZ_EXPAND(alpha);
	do
	{
		int masa = FZ_COMBINE(sp[0], alpha);
		int t = FZ_EXPAND(255-masa);
		*dp = masa + FZ_COMBINE(*dp, t);
		dp++;
		sp++;
	}
	while (--w);
}

static void
paint_span_1_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_general(dp, 0, sp, 1, w);
}

static void
paint_span_1_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_with_alpha_general(dp, 0, sp, 1, w, alpha);
}

static void
paint_span_1_da_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_general(dp, 1, sp, 1, w);
}

static void
paint_span_1_da_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_with_alpha_general(dp, 1, sp, 1, w, alpha);
}

#if FZ_PLOTTERS_G
static void
paint_span_1_da(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_general(dp, 1, sp, 0, w);
}

static void
paint_span_1_da_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_with_alpha_general(dp, 1, sp, 0, w, alpha);
}

static void
paint_span_1(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_general(dp, 0, sp, 0, w);
}

static void
paint_span_1_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_1_with_alpha_general(dp, 0, sp, 0, w, alpha);
}
#endif /* FZ_PLOTTERS_G */

#if FZ_PLOTTERS_RGB
static void
paint_span_3_da_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_general(dp, 1, sp, 1, w);
}

static void
paint_span_3_da_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_with_alpha_general(dp, 1, sp, 1, w, alpha);
}

static void
paint_span_3_da(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_general(dp, 1, sp, 0, w);
}

static void
paint_span_3_da_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_with_alpha_general(dp, 1, sp, 0, w, alpha);
}

static void
paint_span_3_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_general(dp, 0, sp, 1, w);
}

static void
paint_span_3_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_with_alpha_general(dp, 0, sp, 1, w, alpha);
}

static void
paint_span_3(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_general(dp, 0, sp, 0, w);
}

static void
paint_span_3_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_3_with_alpha_general(dp, 0, sp, 0, w, alpha);
}
#endif /* FZ_PLOTTERS_RGB */

#if FZ_PLOTTERS_CMYK
static void
paint_span_4_da_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_general(dp, 1, sp, 1, w);
}

static void
paint_span_4_da_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_with_alpha_general(dp, 1, sp, 1, w, alpha);
}

static void
paint_span_4_da(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_general(dp, 1, sp, 0, w);
}

static void
paint_span_4_da_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_with_alpha_general(dp, 1, sp, 0, w, alpha);
}

static void
paint_span_4_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_general(dp, 0, sp, 1, w);
}

static void
paint_span_4_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_with_alpha_general(dp, 0, sp, 1, w, alpha);
}

static void
paint_span_4(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_general(dp, 0, sp, 0, w);
}

static void
paint_span_4_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_4_with_alpha_general(dp, 0, sp, 0, w, alpha);
}
#endif /* FZ_PLOTTERS_CMYK */

#if FZ_PLOTTERS_N
static void
paint_span_N_da_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_general(dp, 1, sp, 1, n, w);
}

static void
paint_span_N_da_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_with_alpha_general(dp, 1, sp, 1, n, w, alpha);
}

static void
paint_span_N_da(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_general(dp, 1, sp, 0, n, w);
}

static void
paint_span_N_da_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_with_alpha_general(dp, 1, sp, 0, n, w, alpha);
}

static void
paint_span_N_sa(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_general(dp, 0, sp, 1, n, w);
}

static void
paint_span_N_sa_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_with_alpha_general(dp, 0, sp, 1, n, w, alpha);
}

static void
paint_span_N(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_general(dp, 0, sp, 0, n, w);
}

static void
paint_span_N_alpha(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_with_alpha_general(dp, 0, sp, 0, n, w, alpha);
}
#endif /* FZ_PLOTTERS_N */

#if FZ_ENABLE_SPOT_RENDERING
static void
paint_span_N_general_op(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_general_op(dp, da, sp, sa, n, w, eop);
}

static void
paint_span_N_general_alpha_op(byte * FZ_RESTRICT dp, int da, const byte * FZ_RESTRICT sp, int sa, int n, int w, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
	TRACK_FN();
	template_span_N_with_alpha_general_op(dp, da, sp, sa, n, w, alpha, eop);
}
#endif /* FZ_ENABLE_SPOT_RENDERING */

fz_span_painter_t *
fz_get_span_painter(int da, int sa, int n, int alpha, const fz_overprint * FZ_RESTRICT eop)
{
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (alpha == 255)
			return paint_span_N_general_op;
		else if (alpha > 0)
			return paint_span_N_general_alpha_op;
		else
			return NULL;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch (n)
	{
	case 0:
		if (alpha == 255)
			return paint_span_0_da_sa;
		else if (alpha > 0)
			return paint_span_0_da_sa_alpha;
		break;
	case 1:
		if (sa)
			if (da)
			{
				if (alpha == 255)
					return paint_span_1_da_sa;
				else if (alpha > 0)
					return paint_span_1_da_sa_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_1_sa;
				else if (alpha > 0)
					return paint_span_1_sa_alpha;
			}
		else
#if FZ_PLOTTERS_G
			if (da)
			{
				if (alpha == 255)
					return paint_span_1_da;
				else if (alpha > 0)
					return paint_span_1_da_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_1;
				else if (alpha > 0)
					return paint_span_1_alpha;
			}
#else
			goto fallback;
#endif /* FZ_PLOTTERS_G */
		break;
#if FZ_PLOTTERS_RGB
	case 3:
		if (da)
			if (sa)
			{
				if (alpha == 255)
					return paint_span_3_da_sa;
				else if (alpha > 0)
					return paint_span_3_da_sa_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_3_da;
				else if (alpha > 0)
					return paint_span_3_da_alpha;
			}
		else
			if (sa)
			{
				if (alpha == 255)
					return paint_span_3_sa;
				else if (alpha > 0)
					return paint_span_3_sa_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_3;
				else if (alpha > 0)
					return paint_span_3_alpha;
			}
		break;
#endif /* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
	case 4:
		if (da)
			if (sa)
			{
				if (alpha == 255)
					return paint_span_4_da_sa;
				else if (alpha > 0)
					return paint_span_4_da_sa_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_4_da;
				else if (alpha > 0)
					return paint_span_4_da_alpha;
			}
		else
			if (sa)
			{
				if (alpha == 255)
					return paint_span_4_sa;
				else if (alpha > 0)
					return paint_span_4_sa_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_4;
				else if (alpha > 0)
					return paint_span_4_alpha;
			}
		break;
#endif /* FZ_PLOTTERS_CMYK */
	default:
	{
#if !FZ_PLOTTERS_G
fallback:{}
#endif /* FZ_PLOTTERS_G */
#if FZ_PLOTTERS_N
		if (da)
			if (sa)
			{
				if (alpha == 255)
					return paint_span_N_da_sa;
				else if (alpha > 0)
					return paint_span_N_da_sa_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_N_da;
				else if (alpha > 0)
					return paint_span_N_da_alpha;
			}
		else
			if (sa)
			{
				if (alpha == 255)
					return paint_span_N_sa;
				else if (alpha > 0)
					return paint_span_N_sa_alpha;
			}
			else
			{
				if (alpha == 255)
					return paint_span_N;
				else if (alpha > 0)
					return paint_span_N_alpha;
			}
#endif /* FZ_PLOTTERS_N */
		break;
	}
	}
	return NULL;
}

/*
 * Pixmap blending functions
 */

void
fz_paint_pixmap_with_bbox(fz_pixmap * FZ_RESTRICT dst, const fz_pixmap * FZ_RESTRICT src, int alpha, fz_irect bbox)
{
	const unsigned char *sp;
	unsigned char *dp;
	int x, y, w, h, n, da, sa;
	fz_span_painter_t *fn;

	ASSERT0(dst->n - dst->alpha == src->n - src->alpha);

	if (alpha == 0)
		return;

	bbox = fz_intersect_irect(bbox, fz_pixmap_bbox_no_ctx(dst));
	bbox = fz_intersect_irect(bbox, fz_pixmap_bbox_no_ctx(src));

	x = bbox.x0;
	y = bbox.y0;
	w = fz_irect_width(bbox);
	h = fz_irect_height(bbox);
	if (w == 0 || h == 0)
		return;

	n = src->n;
	sp = src->samples + (y - src->y) * (size_t)src->stride + (x - src->x) * (size_t)src->n;
	sa = src->alpha;
	dp = dst->samples + (y - dst->y) * (size_t)dst->stride + (x - dst->x) * (size_t)dst->n;
	da = dst->alpha;

	n -= sa;
	fn = fz_get_span_painter(da, sa, n, alpha, 0);
	if (fn == NULL)
		return;

	while (h--)
	{
		(*fn)(dp, da, sp, sa, n, w, alpha, 0);
		sp += src->stride;
		dp += dst->stride;
	}
}

void
fz_paint_pixmap(fz_pixmap * FZ_RESTRICT dst, const fz_pixmap * FZ_RESTRICT src, int alpha)
{
	const unsigned char *sp;
	unsigned char *dp;
	fz_irect bbox;
	int x, y, w, h, n, da, sa;
	fz_span_painter_t *fn;

	if (alpha == 0)
		return;

	if (dst->n - dst->alpha != src->n - src->alpha)
	{
		// fz_error(ctx, "fz_paint_pixmap - FIXME\n");
		return;
	}
	ASSERT0(dst->n - dst->alpha == src->n - src->alpha);

	bbox = fz_intersect_irect(fz_pixmap_bbox_no_ctx(src), fz_pixmap_bbox_no_ctx(dst));
	x = bbox.x0;
	y = bbox.y0;
	w = fz_irect_width(bbox);
	h = fz_irect_height(bbox);
	if (w == 0 || h == 0)
		return;

	n = src->n;
	sp = src->samples + (y - src->y) * (size_t)src->stride + (x - src->x) * (size_t)src->n;
	sa = src->alpha;
	dp = dst->samples + (y - dst->y) * (size_t)dst->stride + (x - dst->x) * (size_t)dst->n;
	da = dst->alpha;

	n -= sa;
	fn = fz_get_span_painter(da, sa, n, alpha, 0);
	if (fn == NULL)
		return;

	while (h--)
	{
		(*fn)(dp, da, sp, sa, n, w, alpha, 0);
		sp += src->stride;
		dp += dst->stride;
	}
}

static fz_forceinline void
paint_span_alpha_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, int n, int w)
{
	TRACK_FN();
	sp += n-1;
	do
	{
		int s = *sp;
		int t = FZ_EXPAND(255 - s);
		sp += n;
		*dp = s + FZ_COMBINE(*dp, t);
		dp ++;
	}
	while (--w);
}

static fz_forceinline void
paint_span_alpha_not_solid(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, int n, int w, int alpha)
{
	TRACK_FN();
	sp += n-1;
	alpha = FZ_EXPAND(alpha);
	do
	{
		int masa = FZ_COMBINE(sp[0], alpha);
		sp += n;
		*dp = FZ_BLEND(*sp, *dp, masa);
		dp++;
	}
	while (--w);
}

void
fz_paint_pixmap_alpha(fz_pixmap * FZ_RESTRICT dst, const fz_pixmap * FZ_RESTRICT src, int alpha)
{
	const unsigned char *sp;
	unsigned char *dp;
	fz_irect bbox;
	int x, y, w, h, n;

	if (alpha == 0)
		return;

	ASSERT0(dst->n == 1 && dst->alpha == 1 && src->n >= 1 && src->alpha == 1);

	bbox = fz_intersect_irect(fz_pixmap_bbox_no_ctx(src), fz_pixmap_bbox_no_ctx(dst));
	x = bbox.x0;
	y = bbox.y0;
	w = fz_irect_width(bbox);
	h = fz_irect_height(bbox);
	if (w == 0 || h == 0)
		return;

	n = src->n;
	sp = src->samples + (y - src->y) * (size_t)src->stride + (x - src->x) * (size_t)src->n;
	dp = dst->samples + (y - dst->y) * (size_t)dst->stride + (x - dst->x) * (size_t)dst->n;

	if (alpha == 255)
	{
		while (h--)
		{
			paint_span_alpha_solid(dp, sp, n, w);
			sp += src->stride;
			dp += dst->stride;
		}
	}
	else
	{
		while (h--)
		{
			paint_span_alpha_not_solid(dp, sp, n, w, alpha);
			sp += src->stride;
			dp += dst->stride;
		}
	}
}

void
fz_paint_pixmap_with_overprint(fz_pixmap * FZ_RESTRICT dst, const fz_pixmap * FZ_RESTRICT src, const fz_overprint * FZ_RESTRICT eop)
{
	const unsigned char *sp;
	unsigned char *dp;
	fz_irect bbox;
	int x, y, w, h, n, da, sa;
	fz_span_painter_t *fn;

	if (dst->n - dst->alpha != src->n - src->alpha)
	{
		// fz_error(ctx, "fz_paint_pixmap_with_overprint - FIXME\n");
		return;
	}
	ASSERT0(dst->n - dst->alpha == src->n - src->alpha);

	bbox = fz_intersect_irect(fz_pixmap_bbox_no_ctx(src), fz_pixmap_bbox_no_ctx(dst));
	x = bbox.x0;
	y = bbox.y0;
	w = fz_irect_width(bbox);
	h = fz_irect_height(bbox);
	if (w == 0 || h == 0)
		return;

	n = src->n;
	sp = src->samples + (y - src->y) * (size_t)src->stride + (x - src->x) * (size_t)src->n;
	sa = src->alpha;
	dp = dst->samples + (y - dst->y) * (size_t)dst->stride + (x - dst->x) * (size_t)dst->n;
	da = dst->alpha;

	n -= sa;
	fn = fz_get_span_painter(da, sa, n, 255, eop);
	if (fn == NULL)
		return;

	while (h--)
	{
		(*fn)(dp, da, sp, sa, n, w, 255, eop);
		sp += src->stride;
		dp += dst->stride;
	}
}

void
fz_paint_pixmap_with_mask(fz_pixmap * FZ_RESTRICT dst, const fz_pixmap * FZ_RESTRICT src, const fz_pixmap * FZ_RESTRICT msk)
{
	const unsigned char *sp, *mp;
	unsigned char *dp;
	fz_irect bbox;
	int x, y, w, h, n, sa, da;
	fz_span_mask_painter_t *fn;

	ASSERT0(dst->n == src->n);
	ASSERT0(msk->n == 1);

	bbox = fz_pixmap_bbox_no_ctx(dst);
	bbox = fz_intersect_irect(bbox, fz_pixmap_bbox_no_ctx(src));
	bbox = fz_intersect_irect(bbox, fz_pixmap_bbox_no_ctx(msk));

	x = bbox.x0;
	y = bbox.y0;
	w = fz_irect_width(bbox);
	h = fz_irect_height(bbox);
	if (w == 0 || h == 0)
		return;

	n = src->n;
	sp = src->samples + (y - src->y) * (size_t)src->stride + (x - src->x) * (size_t)src->n;
	sa = src->alpha;
	mp = msk->samples + (y - msk->y) * (size_t)msk->stride + (x - msk->x) * (size_t)msk->n;
	dp = dst->samples + (y - dst->y) * (size_t)dst->stride + (x - dst->x) * (size_t)dst->n;
	da = dst->alpha;

	/* sa == da, or something has gone very wrong! */
	ASSERT0(sa == da);

	n -= sa;
	fn = fz_get_span_mask_painter(da, n);
	if (fn == NULL)
		return;

	while (h--)
	{
		(*fn)(dp, sp, mp, w, n, sa, NULL);
		sp += src->stride;
		dp += dst->stride;
		mp += msk->stride;
	}
}

static fz_forceinline void
paint_over_span_with_mask(byte * FZ_RESTRICT dp, const byte * FZ_RESTRICT sp, const byte * FZ_RESTRICT mp, int w)
{
	do
	{
		int ma = *mp++;
		ma = FZ_EXPAND(ma);
		if (ma == 0 || *sp == 0)
		{
			dp++;
			sp++;
		}
		else
		{
			int a = *sp++;
			if (ma != 256)
				a = fz_mul255(ma, a);
			*dp = 255 - fz_mul255(255 - a, 255 - *dp);
			dp++;
		}
	}
	while (--w);
}

void
fz_paint_over_pixmap_with_mask(fz_pixmap * FZ_RESTRICT dst, const fz_pixmap * FZ_RESTRICT src, const fz_pixmap * FZ_RESTRICT msk)
{
	const unsigned char *sp, *mp;
	unsigned char *dp;
	fz_irect bbox;
	int x, y, w, h;

	ASSERT0(dst->n == src->n);
	ASSERT0(msk->n == 1);

	bbox = fz_pixmap_bbox_no_ctx(dst);
	bbox = fz_intersect_irect(bbox, fz_pixmap_bbox_no_ctx(src));
	bbox = fz_intersect_irect(bbox, fz_pixmap_bbox_no_ctx(msk));

	x = bbox.x0;
	y = bbox.y0;
	w = fz_irect_width(bbox);
	h = fz_irect_height(bbox);
	if (w == 0 || h == 0)
		return;

	/* sa == da, or something has gone very wrong! */
	ASSERT0(src->alpha == dst->alpha && dst->alpha == 1 && src->n == 1);
	sp = src->samples + (y - src->y) * (size_t)src->stride + (size_t)(x - src->x);
	mp = msk->samples + (y - msk->y) * (size_t)msk->stride + (size_t)(x - msk->x);
	dp = dst->samples + (y - dst->y) * (size_t)dst->stride + (size_t)(x - dst->x);


	while (h--)
	{
		paint_over_span_with_mask(dp, sp, mp, w);
		sp += src->stride;
		dp += dst->stride;
		mp += msk->stride;
	}
}

static inline void
fz_paint_glyph_mask(int span, unsigned char *dp, int da, const fz_glyph *glyph, int w, int h, int skip_x, int skip_y)
{
	while (h--)
	{
		int skip_xx, ww, len, extend;
		int v = 0;
		const unsigned char *runp;
		unsigned char *ddp = dp;
		int offset = ((const int *)(glyph->data))[skip_y++];
		if (offset >= 0)
		{
			int eol = 0;
			runp = &glyph->data[offset];
			extend = 0;
			ww = w;
			skip_xx = skip_x;
			while (skip_xx)
			{
				v = *runp++;
				switch (v & 3)
				{
				case 0: /* Extend */
					extend = v>>2;
					len = 0;
					break;
				case 1: /* Transparent */
					len = (v>>2) + 1 + (extend<<6);
					extend = 0;
					if (len > skip_xx)
					{
						len -= skip_xx;
						goto transparent_run;
					}
					break;
				case 2: /* Solid */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
					if (len > skip_xx)
					{
						len -= skip_xx;
						goto solid_run;
					}
					break;
				default: /* Intermediate */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
					if (len > skip_xx)
					{
						runp += skip_xx;
						len -= skip_xx;
						goto intermediate_run;
					}
					runp += len;
					break;
				}
				if (eol)
				{
					ww = 0;
					break;
				}
				skip_xx -= len;
			}
			while (ww > 0)
			{
				v = *runp++;
				switch(v & 3)
				{
				case 0: /* Extend */
					extend = v>>2;
					break;
				case 1: /* Transparent */
					len = (v>>2) + 1 + (extend<<6);
					extend = 0;
transparent_run:
					if (len > ww)
						len = ww;
					ww -= len;
					ddp += len;
					break;
				case 2: /* Solid */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
solid_run:
					if (len > ww)
						len = ww;
					ww -= len;
					do
					{
						*ddp++ = 0xFF;
					}
					while (--len);
					break;
				default: /* Intermediate */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
intermediate_run:
					if (len > ww)
						len = ww;
					ww -= len;
					do
					{
						int v = *ddp;
						int a = *runp++;
						if (v == 0)
						{
							*ddp++ = a;
						}
						else
						{
							a = FZ_EXPAND(a);
							*ddp = FZ_BLEND(0xFF, v, a);
							ddp++;
						}
					}
					while (--len);
					break;
				}
				if (eol)
					break;
			}
		}
		dp += span;
	}
}

static inline void
fz_paint_glyph_mask_alpha(int span, unsigned char *dp, int da, const fz_glyph *glyph, int w, int h, int skip_x, int skip_y, unsigned char alpha)
{
	while (h--)
	{
		int skip_xx, ww, len, extend;
		const unsigned char *runp;
		unsigned char *ddp = dp;
		int offset = ((const int *)(glyph->data))[skip_y++];
		if (offset >= 0)
		{
			int eol = 0;
			runp = &glyph->data[offset];
			extend = 0;
			ww = w;
			skip_xx = skip_x;
			while (skip_xx)
			{
				int v = *runp++;
				switch (v & 3)
				{
				case 0: /* Extend */
					extend = v>>2;
					len = 0;
					break;
				case 1: /* Transparent */
					len = (v>>2) + 1 + (extend<<6);
					extend = 0;
					if (len > skip_xx)
					{
						len -= skip_xx;
						goto transparent_run;
					}
					break;
				case 2: /* Solid */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
					if (len > skip_xx)
					{
						len -= skip_xx;
						goto solid_run;
					}
					break;
				default: /* Intermediate */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
					if (len > skip_xx)
					{
						runp += skip_xx;
						len -= skip_xx;
						goto intermediate_run;
					}
					runp += len;
					break;
				}
				if (eol)
				{
					ww = 0;
					break;
				}
				skip_xx -= len;
			}
			while (ww > 0)
			{
				int v = *runp++;
				switch(v & 3)
				{
				case 0: /* Extend */
					extend = v>>2;
					break;
				case 1: /* Transparent */
					len = (v>>2) + 1 + (extend<<6);
					extend = 0;
transparent_run:
					if (len > ww)
						len = ww;
					ww -= len;
					ddp += len;
					break;
				case 2: /* Solid */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
solid_run:
					if (len > ww)
						len = ww;
					ww -= len;
					do
					{
						*ddp++ = alpha;
					}
					while (--len);
					break;
				default: /* Intermediate */
					eol = v & 4;
					len = (v>>3) + 1 + (extend<<5);
					extend = 0;
intermediate_run:
					if (len > ww)
						len = ww;
					ww -= len;
					do
					{
						int v = *ddp;
						int a = *runp++;
						if (v == 0)
						{
							*ddp++ = fz_mul255(a, alpha);
						}
						else
						{
							a = FZ_EXPAND(a);
							*ddp = FZ_BLEND(alpha, v, a);
							ddp++;
						}
					}
					while (--len);
					break;
				}
				if (eol)
					break;
			}
		}
		dp += span;
	}
}

#define N 1
#include "paint-glyph.h"

#define ALPHA
#define N 1
#include "paint-glyph.h"

#if FZ_PLOTTERS_G
#define DA
#define N 1
#include "paint-glyph.h"

#define DA
#define ALPHA
#define N 1
#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_G */

#if FZ_PLOTTERS_RGB
#define DA
#define N 3
#include "paint-glyph.h"

#define DA
#define ALPHA
#define N 3
#include "paint-glyph.h"

#define N 3
#include "paint-glyph.h"

#define ALPHA
#define N 3
#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_RGB */

#if FZ_PLOTTERS_CMYK
#define DA
#define N 4
#include "paint-glyph.h"

#define DA
#define ALPHA
#define N 4
#include "paint-glyph.h"

#define ALPHA
#define N 4
#include "paint-glyph.h"

#define N 4
#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_CMYK */

#if FZ_PLOTTERS_N
#define ALPHA
#include "paint-glyph.h"

#define DA
#include "paint-glyph.h"

#define DA
#define ALPHA
#include "paint-glyph.h"

#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_N */

#if FZ_ENABLE_SPOT_RENDERING
#define ALPHA
#define EOP
#include "paint-glyph.h"

#define DA
#define EOP
#include "paint-glyph.h"

#define DA
#define ALPHA
#define EOP
#include "paint-glyph.h"

#define EOP
#include "paint-glyph.h"
#endif /* FZ_ENABLE_SPOT_RENDERING */

static void
fz_paint_glyph_alpha(const unsigned char * FZ_RESTRICT colorbv, int n, int span, unsigned char * FZ_RESTRICT dp, int da, const fz_glyph *glyph, int w, int h, int skip_x, int skip_y, const fz_overprint * FZ_RESTRICT eop)
{
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (da)
			fz_paint_glyph_alpha_N_da_op(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop);
		else
			fz_paint_glyph_alpha_N_op(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop);
		return;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch (n)
	{
	case 1:
		if (da)
#if FZ_PLOTTERS_G
			fz_paint_glyph_alpha_1_da(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
#else
			goto fallback;
#endif /* FZ_PLOTTERS_G */
		else
			fz_paint_glyph_alpha_1(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		break;
#if FZ_PLOTTERS_RGB
	case 3:
		if (da)
			fz_paint_glyph_alpha_3_da(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		else
			fz_paint_glyph_alpha_3(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		break;
#endif /* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
	case 4:
		if (da)
			fz_paint_glyph_alpha_4_da(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		else
			fz_paint_glyph_alpha_4(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		break;
#endif /* FZ_PLOTTERS_CMYK */
	default:
	{
#if !FZ_PLOTTERS_G
fallback:{}
#endif /* !FZ_PLOTTERS_G */
#if FZ_PLOTTERS_N
		if (da)
			fz_paint_glyph_alpha_N_da(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y);
		else
			fz_paint_glyph_alpha_N(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y);
#endif /* FZ_PLOTTERS_N */
		break;
	}
	}
}

static void
fz_paint_glyph_solid(const unsigned char * FZ_RESTRICT colorbv, int n, int span, unsigned char * FZ_RESTRICT dp, int da, const fz_glyph * FZ_RESTRICT glyph, int w, int h, int skip_x, int skip_y, const fz_overprint * FZ_RESTRICT eop)
{
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (da)
			fz_paint_glyph_solid_N_da_op(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop);
		else
			fz_paint_glyph_solid_N_op(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop);
		return;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch (n)
	{
	case 1:
		if (da)
#if FZ_PLOTTERS_G
			fz_paint_glyph_solid_1_da(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
#else
			goto fallback;
#endif /* FZ_PLOTTERS_G */
		else
			fz_paint_glyph_solid_1(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		break;
#if FZ_PLOTTERS_RGB
	case 3:
		if (da)
			fz_paint_glyph_solid_3_da(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		else
			fz_paint_glyph_solid_3(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		break;
#endif /* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
	case 4:
		if (da)
			fz_paint_glyph_solid_4_da(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		else
			fz_paint_glyph_solid_4(colorbv, span, dp, glyph, w, h, skip_x, skip_y);
		break;
#endif /* FZ_PLOTTERS_CMYK */
	default:
	{
#if !FZ_PLOTTERS_G
fallback:{}
#endif /* FZ_PLOTTERS_G */
#if FZ_PLOTTERS_N
		if (da)
			fz_paint_glyph_solid_N_da(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y);
		else
			fz_paint_glyph_solid_N(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y);
		break;
#endif /* FZ_PLOTTERS_N */
	}
	}
}


#if FZ_ENABLE_GAMMA

#define GAMMA
#define N 1
#include "paint-glyph.h"

#define GAMMA
#define ALPHA
#define N 1
#include "paint-glyph.h"

#if FZ_PLOTTERS_G
#define GAMMA
#define DA
#define N 1
#include "paint-glyph.h"

#define GAMMA
#define DA
#define ALPHA
#define N 1
#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_G */

#if FZ_PLOTTERS_RGB
#define GAMMA
#define DA
#define N 3
#include "paint-glyph.h"

#define GAMMA
#define DA
#define ALPHA
#define N 3
#include "paint-glyph.h"

#define GAMMA
#define N 3
#include "paint-glyph.h"

#define GAMMA
#define ALPHA
#define N 3
#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_RGB */

#if FZ_PLOTTERS_CMYK
#define GAMMA
#define DA
#define N 4
#include "paint-glyph.h"

#define GAMMA
#define DA
#define ALPHA
#define N 4
#include "paint-glyph.h"

#define GAMMA
#define ALPHA
#define N 4
#include "paint-glyph.h"

#define GAMMA
#define N 4
#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_CMYK */

#if FZ_PLOTTERS_N
#define GAMMA
#define ALPHA
#include "paint-glyph.h"

#define GAMMA
#define DA
#include "paint-glyph.h"

#define GAMMA
#define DA
#define ALPHA
#include "paint-glyph.h"

#define GAMMA
#include "paint-glyph.h"
#endif /* FZ_PLOTTERS_N */

#if FZ_ENABLE_SPOT_RENDERING
#define GAMMA
#define ALPHA
#define EOP
#include "paint-glyph.h"

#define GAMMA
#define DA
#define EOP
#include "paint-glyph.h"

#define GAMMA
#define DA
#define ALPHA
#define EOP
#include "paint-glyph.h"

#define GAMMA
#define EOP
#include "paint-glyph.h"
#endif /* FZ_ENABLE_SPOT_RENDERING */

static void
fz_paint_glyph_alpha_gamma(const unsigned char * FZ_RESTRICT colorbv, int n, int span, unsigned char * FZ_RESTRICT dp, int da, const fz_glyph *glyph, int w, int h, int skip_x, int skip_y, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table *gamma)
{
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (da)
			fz_paint_glyph_alpha_N_da_op_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop, gamma);
		else
			fz_paint_glyph_alpha_N_op_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop, gamma);
		return;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch (n)
	{
	case 1:
		if (da)
#if FZ_PLOTTERS_G
			fz_paint_glyph_alpha_1_da_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
#else
			goto fallback;
#endif /* FZ_PLOTTERS_G */
		else
			fz_paint_glyph_alpha_1_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		break;
#if FZ_PLOTTERS_RGB
	case 3:
		if (da)
			fz_paint_glyph_alpha_3_da_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		else
			fz_paint_glyph_alpha_3_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		break;
#endif /* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
	case 4:
		if (da)
			fz_paint_glyph_alpha_4_da_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		else
			fz_paint_glyph_alpha_4_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		break;
#endif /* FZ_PLOTTERS_CMYK */
	default:
	{
#if !FZ_PLOTTERS_G
fallback:{}
#endif /* !FZ_PLOTTERS_G */
#if FZ_PLOTTERS_N
		if (da)
			fz_paint_glyph_alpha_N_da_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		else
			fz_paint_glyph_alpha_N_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, gamma);
#endif /* FZ_PLOTTERS_N */
		break;
	}
	}
}

static void
fz_paint_glyph_solid_gamma(const unsigned char * FZ_RESTRICT colorbv, int n, int span, unsigned char * FZ_RESTRICT dp, int da, const fz_glyph * FZ_RESTRICT glyph, int w, int h, int skip_x, int skip_y, const fz_overprint * FZ_RESTRICT eop, const fz_gamma_table *gamma)
{
#if FZ_ENABLE_SPOT_RENDERING
	if (fz_overprint_required(eop))
	{
		if (da)
			fz_paint_glyph_solid_N_da_op_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop, gamma);
		else
			fz_paint_glyph_solid_N_op_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, eop, gamma);
		return;
	}
#endif /* FZ_ENABLE_SPOT_RENDERING */
	switch (n)
	{
	case 1:
		if (da)
#if FZ_PLOTTERS_G
			fz_paint_glyph_solid_1_da_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
#else
			goto fallback;
#endif /* FZ_PLOTTERS_G */
		else
			fz_paint_glyph_solid_1_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		break;
#if FZ_PLOTTERS_RGB
	case 3:
		if (da)
			fz_paint_glyph_solid_3_da_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		else
			fz_paint_glyph_solid_3_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		break;
#endif /* FZ_PLOTTERS_RGB */
#if FZ_PLOTTERS_CMYK
	case 4:
		if (da)
			fz_paint_glyph_solid_4_da_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		else
			fz_paint_glyph_solid_4_gamma(colorbv, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		break;
#endif /* FZ_PLOTTERS_CMYK */
	default:
	{
#if !FZ_PLOTTERS_G
fallback:{}
#endif /* FZ_PLOTTERS_G */
#if FZ_PLOTTERS_N
		if (da)
			fz_paint_glyph_solid_N_da_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		else
			fz_paint_glyph_solid_N_gamma(colorbv, n, span, dp, glyph, w, h, skip_x, skip_y, gamma);
		break;
#endif /* FZ_PLOTTERS_N */
	}
	}
}
#endif /* FZ_GLYPH_GAMMA_BLEND */

void
fz_paint_glyph(fz_context *ctx, const unsigned char * FZ_RESTRICT colorbv, fz_pixmap * FZ_RESTRICT dst, unsigned char * FZ_RESTRICT dp, const fz_glyph * FZ_RESTRICT glyph, int w, int h, int skip_x, int skip_y, const fz_overprint * FZ_RESTRICT eop)
{
	int n = dst->n - dst->alpha;
	if (dst->colorspace)
	{
#if FZ_ENABLE_GAMMA
		const fz_gamma_table *gamma;
#endif /* FZ_ENABLE_GAMMA */
		ASSERT0(n > 0);
#if FZ_ENABLE_GAMMA
		if (fz_get_gamma_blending(ctx))
		{
			gamma = fz_colorspace_gamma_tables(dst->colorspace);
			if (gamma)
			{
				if (colorbv[n] == 255)
					fz_paint_glyph_solid_gamma(colorbv, n, dst->stride, dp, dst->alpha, glyph, w, h, skip_x, skip_y, eop, gamma);
				else if (colorbv[n] != 0)
					fz_paint_glyph_alpha_gamma(colorbv, n, dst->stride, dp, dst->alpha, glyph, w, h, skip_x, skip_y, eop, gamma);
				return;
			}
		}
#endif /* FZ_ENABLE_GAMMA */
		if (colorbv[n] == 255)
			fz_paint_glyph_solid(colorbv, n, dst->stride, dp, dst->alpha, glyph, w, h, skip_x, skip_y, eop);
		else if (colorbv[n] != 0)
			fz_paint_glyph_alpha(colorbv, n, dst->stride, dp, dst->alpha, glyph, w, h, skip_x, skip_y, eop);
	}
	else
	{
		ASSERT0(dst->alpha && dst->n == 1 && dst->colorspace == NULL && !fz_overprint_required(eop));
		if (colorbv == NULL || colorbv[0] == 255)
			fz_paint_glyph_mask(dst->stride, dp, dst->alpha, glyph, w, h, skip_x, skip_y);
		else
			fz_paint_glyph_mask_alpha(dst->stride, dp, dst->alpha, glyph, w, h, skip_x, skip_y, colorbv[0]);
	}
}

void
fz_paint_glyph_pixmap(fz_context *ctx, const unsigned char * FZ_RESTRICT colorbv, fz_pixmap * FZ_RESTRICT dst, unsigned char * FZ_RESTRICT dp, const fz_pixmap * FZ_RESTRICT msk, int w, int h, int skip_x, int skip_y, const fz_overprint * FZ_RESTRICT eop)
{
	unsigned char *mp = msk->samples + skip_y * msk->stride + skip_x;
	int da = dst->alpha;

	if (dst->colorspace)
	{
#if FZ_ENABLE_GAMMA
		if (fz_get_gamma_blending(ctx))
		{
			const fz_gamma_table *gamma = fz_colorspace_gamma_tables(dst->colorspace);
			if (gamma)
			{
				fz_span_color_gamma_painter_t *fn;
				fn = fz_get_span_color_painter_gamma(dst->n, da, colorbv, eop);
				if (fn == NULL)
					return;
				while (h--)
				{
					(*fn)(dp, mp, dst->n, w, colorbv, da, eop, gamma);
					dp += dst->stride;
					mp += msk->stride;
				}
				return;
			}
		}
#endif /* FZ_ENABLE_GAMMA */
		{
			fz_span_color_painter_t *fn;
			fn = fz_get_span_color_painter(dst->n, da, colorbv, eop);
			if (fn == NULL)
				return;
			while (h--)
			{
				(*fn)(dp, mp, dst->n, w, colorbv, da, eop);
				dp += dst->stride;
				mp += msk->stride;
			}
		}
	}
	else
	{
		fz_span_painter_t *fn;
		int col = colorbv ? colorbv[0] : 255;

		fn = fz_get_span_painter(da, 1, 0, col, eop);
		if (fn == NULL)
			return;
		while (h--)
		{
			(*fn)(dp, da, mp, 1, 0, w, col, eop);
			dp += dst->stride;
			mp += msk->stride;
		}
	}
}

#endif
