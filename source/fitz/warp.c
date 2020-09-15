#include "mupdf/fitz.h"

#include <string.h>
#include <math.h>

typedef struct
{
	int x;
	int y;
} fz_ipoint;

typedef struct
{
	int i;
	int f;
	int di;
	int df;
} fz_bresenham_core;

typedef struct
{
	fz_bresenham_core c;
	int n;
} fz_bresenham;

typedef struct
{
	fz_bresenham_core x;
	fz_bresenham_core y;
	int n;
} fz_ipoint_bresenham;

typedef struct
{
	fz_bresenham_core sx;
	fz_bresenham_core sy;
	fz_bresenham_core ex;
	fz_bresenham_core ey;
	int n;
} fz_ipoint2_bresenham;

static inline fz_bresenham_core
init_bresenham_core(int start, int end, int n)
{
	fz_bresenham_core b;
	int delta = end-start;

	b.di = n == 0 ? 0 : delta/(n-1);
	b.df = delta - (n-1)*b.di; /* 0 <= b.df < n */
	/* Starts with bi.i = start, bi.f = n, and then does a half
	 * step. */
	b.i = start + (b.di>>1);
	b.f = n - (((b.di & 1) * n + b.df)>>1);

	return b;
}

#ifdef CURRENTLY_UNUSED
static inline fz_bresenham
init_bresenham(int start, int end, int n)
{
	fz_bresenham b;

	b.c = init_bresenham_core(start, end, n);
	b.n = n;

	return b;
}

static inline void
step(fz_bresenham *b)
{
	step_core(&b->c, b->n);
}
#endif

static inline void
step_core(fz_bresenham_core *b, int n)
{
	b->i += b->di;
	b->f -= b->df;
	if (b->f <= 0)
	{
		b->f += n;
		b->i++;
	}
}

static inline fz_ipoint_bresenham
init_ip_bresenham(fz_ipoint start, fz_ipoint end, int n)
{
	fz_ipoint_bresenham b;

	b.x = init_bresenham_core(start.x, end.x, n);
	b.y = init_bresenham_core(start.y, end.y, n);
	b.n = n;

	return b;
}

static inline void
step_ip(fz_ipoint_bresenham *b)
{
	step_core(&b->x, b->n);
	step_core(&b->y, b->n);
}

static inline fz_ipoint
current_ip(const fz_ipoint_bresenham b)
{
	fz_ipoint ip;

	ip.x = b.x.i;
	ip.y = b.y.i;

	return ip;
}

static inline fz_ipoint2_bresenham
init_ip2_bresenham(fz_ipoint ss, fz_ipoint se, fz_ipoint es, fz_ipoint ee, int n)
{
	fz_ipoint2_bresenham b;

	b.sx = init_bresenham_core(ss.x, se.x, n);
	b.sy = init_bresenham_core(ss.y, se.y, n);
	b.ex = init_bresenham_core(es.x, ee.x, n);
	b.ey = init_bresenham_core(es.y, ee.y, n);
	b.n = n;

	return b;
}

static inline void
step_ip2(fz_ipoint2_bresenham *b)
{
	step_core(&b->sx, b->n);
	step_core(&b->sy, b->n);
	step_core(&b->ex, b->n);
	step_core(&b->ey, b->n);
}

static inline fz_ipoint
start_ip(const fz_ipoint2_bresenham b)
{
	fz_ipoint ip;

	ip.x = b.sx.i;
	ip.y = b.sy.i;

	return ip;
}

static inline fz_ipoint
end_ip(const fz_ipoint2_bresenham b)
{
	fz_ipoint ip;

	ip.x = b.ex.i;
	ip.y = b.ey.i;

	return ip;
}

static void
interp_n(unsigned char *d, const unsigned char *s0,
	const unsigned char *s1, int f, int n)
{
	do
	{
		int a = *s0++;
		int b = *s1++ - a;
		*d++ = ((a<<8) + b*f + 128)>>8;
	}
	while (--n);
}

static void
interp2_n(unsigned char *d, const unsigned char *s0,
	const unsigned char *s1, const unsigned char *s2,
	int f0, int f1, int n)
{
	do
	{
		int a = *s0++;
		int b = *s1++ - a;
		int c;
		a = (a<<8) + b*f0;
		c = (*s2++<<8) - a;
		*d++ = ((a<<8) + c*f1 + (1<<15))>>16;
	}
	while (--n);
}

static inline void
copy_pixel(unsigned char *d, const fz_pixmap *src, fz_ipoint p)
{
	int u = p.x>>8;
	int v = p.y>>8;
	int fu = p.x & 255;
	int fv = p.y & 255;
	int n = src->n;
	const unsigned char *s;
	ptrdiff_t stride = src->stride;

	if (u < 0)
		u = 0, fu = 0;
	else if (u >= src->w-1)
		u = src->w-1, fu = 0;

	if (v < 0)
		v = 0, fv = 0;
	else if (v >= src->h-1)
		v = src->h-1, fv = 0;

	s = &src->samples[u * n + v * stride];
	if (fu == 0)
	{
		if (fv == 0)
		{
			/* Copy single pixel */
			memcpy(d, s, n);
			return;
		}
		/* interpolate y pixels */
		interp_n(d, s, s + stride, fv, n);
		return;
	}
	if (fv == 0)
	{
		/* interpolate x pixels */
		interp_n(d, s, s+n, fu, n);
		return;
	}

	if (fu <= fv)
	{
		/* Top half of the trapezoid. */
		interp2_n(d, s, s+n, s+stride, fu, fv, n);
	}
	else
	{
		/* Bottom half of the trapezoid. */
		interp2_n(d, s+n, s+stride, s+stride+n, fv, fu, n);
	}
}

static void
warp_core(unsigned char *d, int n, int width, int height, int stride,
	const fz_ipoint corner[4], const fz_pixmap *src)
{
	fz_ipoint2_bresenham row_bres;
	int x;

	/* We have a bresenham pair for how to move the start
	 * and end of the row each y step. */
	row_bres = init_ip2_bresenham(corner[0], corner[2],
					corner[1], corner[3], height);
	stride -= width * n;

	for (; height > 0; height--)
	{
		/* We have a bresenham for how to move the
		 * current pixel across the row. */
		fz_ipoint_bresenham pix_bres;
			pix_bres = init_ip_bresenham(start_ip(row_bres),
					end_ip(row_bres),
					width);
		for (x = width; x > 0; x--)
		{
			/* Copy pixel */
			copy_pixel(d, src, current_ip(pix_bres));
			d += n;
			step_ip(&pix_bres);
		}

		/* step to the next line. */
		step_ip2(&row_bres);
		d += stride;
	}
}

/*
	points are clockwise from NW.

	This performs simple affine warping.
*/
fz_pixmap *
fz_warp_pixmap(fz_context *ctx, fz_pixmap *src, const fz_point points[4], int width, int height)
{
	fz_pixmap *dst;

	if (src == NULL)
		return NULL;

	if (width >= (1<<24) || width < 0 || height >= (1<<24) || height < 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Bad width/height");

	dst = fz_new_pixmap(ctx, src->colorspace, width, height,
			src->seps, src->alpha);
	dst->xres = src->xres;
	dst->yres = src->yres;

	fz_try(ctx)
	{
		unsigned char *d = dst->samples;
		int n = dst->n;
		fz_ipoint corner[4];

		/* Find the corner texture positions as fixed point */
		corner[0].x = (int)(points[0].x * 256 + 128);
		corner[0].y = (int)(points[0].y * 256 + 128);
		corner[1].x = (int)(points[1].x * 256 + 128);
		corner[1].y = (int)(points[1].y * 256 + 128);
		corner[2].x = (int)(points[3].x * 256 + 128);
		corner[2].y = (int)(points[3].y * 256 + 128);
		corner[3].x = (int)(points[2].x * 256 + 128);
		corner[3].y = (int)(points[2].y * 256 + 128);

		warp_core(d, n, width, height, width * n, corner, src);
	}
	fz_catch(ctx)
	{
		fz_drop_pixmap(ctx, dst);
		fz_rethrow(ctx);
	}

	return dst;
}

static float
dist(fz_point a, fz_point b)
{
	float x = a.x-b.x;
	float y = a.y-b.y;

	return sqrtf(x*x+y*y);
}

/* Again, affine warping, but this time where the destination width/height
 * are chosen automatically. */
fz_pixmap *
fz_autowarp_pixmap(fz_context *ctx, fz_pixmap *src, const fz_point points[4])
{
	float w0 = dist(points[1], points[0]);
	float w1 = dist(points[2], points[3]);
	float h0 = dist(points[3], points[0]);
	float h1 = dist(points[2], points[1]);
	int w = (w0+w1+0.5)/2;
	int h = (h0+h1+0.5)/2;

	return fz_warp_pixmap(ctx, src, points, w, h);
}
