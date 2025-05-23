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

/*
	This file is #included by draw-paint.c multiple times to
	produce optimised plotters.
*/

#ifdef ALPHA
#define NAME alpha
#else
#define NAME solid
#endif

#ifdef DA
#define NAME2 _da
#else
#define NAME2
#endif

#ifdef EOP
#define NAME3 _op
#else
#define NAME3
#endif

#ifdef GAMMA
#define NAME4 _gamma
#else
#define NAME4
#endif

#define FUNCTION_NAMER(NAME,N,NAME2,NAME3,NAME4) fz_paint_glyph_##NAME##_##N##NAME2##NAME3##NAME4
#define FUNCTION_NAME(NAME,N,NAME2,NAME3,NAME4) FUNCTION_NAMER(NAME,N,NAME2,NAME3,NAME4)

#ifdef EOP
#define IF_OVERPRINT_COMPONENT(k) if (fz_overprint_component(eop, k))
#else
#define IF_OVERPRINT_COMPONENT(k) if (1)
#endif

#ifdef GAMMA
#define BLEND(A,B,C,G) FZ_BLEND_GAMMA(A,B,C,G)
#else
#define BLEND(A,B,C,G) FZ_BLEND(A,B,C)
#endif

static fz_forceinline void
FUNCTION_NAME(NAME,N,NAME2,NAME3,NAME4)(const unsigned char * FZ_RESTRICT colorbv,
#ifndef N
				const int n1,
#endif
				int span, unsigned char * FZ_RESTRICT dp, const fz_glyph *glyph, int w, int h, int skip_x, int skip_y
#ifdef EOP
				, const fz_overprint *eop
#endif
#ifdef GAMMA
				, const fz_gamma_table *gamma
#endif
				)
{
#ifdef N
	const int n1 = N;
#endif
#ifdef DA
	const int n = n1 + 1;
#else
	const int n = n1;
#endif
#ifdef ALPHA
	int sa = FZ_EXPAND(colorbv[n1]);
#else
#if defined(N) && N == 1 && defined(DA)
	const uint16_t color = *(const uint16_t *)colorbv;
#elif defined(N) && N == 3 && defined(DA)
	const uint32_t color = *(const uint32_t *)colorbv;
#elif defined(N) && N == 4 && !defined(DA)
	const uint32_t color = *(const uint32_t *)colorbv;
#endif
#endif
	TRACK_FN();
	while (h--)
	{
		int skip_xx, ww, len, extend;
		int v = 0;
		const unsigned char *runp;
		unsigned char *ddp = dp;
		int offset = ((int *)(glyph->data))[skip_y++];
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
					ddp += len * n;
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
#ifdef ALPHA
#if defined(N) && N == 1
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = BLEND(colorbv[0], ddp[0], sa, gamma);
#ifdef DA
						ddp[1] = FZ_BLEND(0xFF, ddp[1], sa);
						ddp += 2;
#else
						ddp++;
#endif
#elif defined(N) && N == 3
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = BLEND(colorbv[0], ddp[0], sa, gamma);
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = BLEND(colorbv[1], ddp[1], sa, gamma);
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = BLEND(colorbv[2], ddp[2], sa, gamma);
#ifdef DA
						ddp[3] = FZ_BLEND(0xFF, ddp[3], sa);
						ddp += 4;
#else
						ddp += 3;
#endif
#elif defined(N) && N == 4
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = BLEND(colorbv[0], ddp[0], sa, gamma);
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = BLEND(colorbv[1], ddp[1], sa, gamma);
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = BLEND(colorbv[2], ddp[2], sa, gamma);
						IF_OVERPRINT_COMPONENT(3)
							ddp[3] = BLEND(colorbv[3], ddp[3], sa, gamma);
#ifdef DA
						ddp[4] = FZ_BLEND(0xFF, ddp[4], sa);
						ddp += 5;
#else
						ddp += 4;
#endif
#else
						int k = 0;
						do
						{
							IF_OVERPRINT_COMPONENT(k)
								*ddp = BLEND(colorbv[k], *ddp, sa, gamma);
							k++;
							ddp++;
						}
						while (k != n1);
#ifdef DA
						*ddp = FZ_BLEND(0xFF, *ddp, sa);
						ddp++;
#endif
#endif
#else
#if defined(N) && N == 1
#ifdef DA
#ifdef EOP
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = colorbv[0];
						IF_OVERPRINT_COMPONENT(1)
							*ddp[1] = colorbv[1];
#else
						*(uint16_t *)ddp = color;
#endif
						ddp += 2;
#else
						*ddp++ = colorbv[0];
#endif
#elif defined(N) && N == 3
#ifdef DA
#ifdef EOP
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = colorbv[0];
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = colorbv[1];
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = colorbv[2];
						IF_OVERPRINT_COMPONENT(3)
#else
						*(uint32_t *)ddp = color;
#endif
						ddp += 4;
#else
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = colorbv[0];
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = colorbv[1];
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = colorbv[2];
						ddp += 3;
#endif
#elif defined(N) && N == 4
#ifdef DA
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = colorbv[0];
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = colorbv[1];
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = colorbv[2];
						IF_OVERPRINT_COMPONENT(3)
							ddp[3] = colorbv[3];
						ddp[4] = colorbv[4];
						ddp += 5;
#else
#ifdef EOP
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = colorbv[0];
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = colorbv[1];
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = colorbv[2];
						ddp[3] = colorbv[3];
#else
						*(uint32_t *)ddp = color;
#endif
						ddp += 4;
#endif
#else
						int k = 0;
						do
						{
							IF_OVERPRINT_COMPONENT(k)
								*ddp = colorbv[k];
							k++;
							ddp++;
						}
						while (k != n);
#endif
#endif
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
						int k = 0;
						int a = *runp++;
#ifdef ALPHA
						a = FZ_COMBINE(sa, FZ_EXPAND(a));
#else
						a = FZ_EXPAND(a);
#endif
						(void)k;
#if defined(N) && N == 1
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = BLEND(colorbv[0], ddp[0], a, gamma);
#ifdef DA
						ddp[1] = FZ_BLEND(0xFF, ddp[1], a);
						ddp += 2;
#else
						ddp++;
#endif
#elif defined(N) && N == 3
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = BLEND(colorbv[0], ddp[0], a, gamma);
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = BLEND(colorbv[1], ddp[1], a, gamma);
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = BLEND(colorbv[2], ddp[2], a, gamma);
#ifdef DA
						ddp[3] = FZ_BLEND(0xFF, ddp[3], a);
						ddp += 4;
#else
						ddp += 3;
#endif
#elif defined(N) && N == 4
						IF_OVERPRINT_COMPONENT(0)
							ddp[0] = BLEND(colorbv[0], ddp[0], a, gamma);
						IF_OVERPRINT_COMPONENT(1)
							ddp[1] = BLEND(colorbv[1], ddp[1], a, gamma);
						IF_OVERPRINT_COMPONENT(2)
							ddp[2] = BLEND(colorbv[2], ddp[2], a, gamma);
						IF_OVERPRINT_COMPONENT(3)
							ddp[3] = BLEND(colorbv[3], ddp[3], a, gamma);
#ifdef DA
						ddp[4] = FZ_BLEND(0xFF, ddp[4], a);
						ddp += 5;
#else
						ddp += 4;
#endif
#else
						do
						{
							IF_OVERPRINT_COMPONENT(k)
								*ddp = BLEND(colorbv[k], *ddp, a, gamma);
							k++;
							ddp++;
						}
						while (k != n1);
#ifdef DA
						*ddp = FZ_BLEND(0xFF, *ddp, a);
						ddp++;
#endif
#endif
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

#undef GAMMA
#undef BLEND
#undef NAME
#undef ALPHA
#undef NAME2
#undef NAME3
#undef NAME4
#undef EOP
#undef DA
#undef N
#undef FUNCTION_NAMER
#undef FUNCTION_NAME
#undef IF_OVERPRINT_COMPONENT
