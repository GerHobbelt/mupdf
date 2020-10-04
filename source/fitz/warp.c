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

/*
	Now, suppose we want to do better than affine warping - i.e. to
	allow for perspective.

	We have a scanned quad, like, ABCD:

	*              w0
	*          A___
	*         /    ----B
	*    h0  /         |
	*       /         | h1
	*      /         |
	*     D---__    |
	*           ---C
	*        w1

	w0 = |B-A|
	w1 = |C-A|
	h0 = |D-A|
	h1 = |C-B|

	Imagine the document lies in the x/y plane with the centre of the
	document at 0,0. We look down on it from a camera at distance 'd'
	up the z axis.

	In general, when taking such pictures using a phone, the distance
	to the document is typically about the same as the the longer of
	width or height. Yes, this is a REALLY huge assumption, but we'll
	live with it for now. So d = max(w0+w1, h0+h1)/2.

	Imagine the top edge is tipped away from us, by an additional
	distance e. w0 gets smaller. The bottom tips towards us, and
	w1 gets larger.

	w0 = d.w/(d+e)		w1 = d.w/(d-e)

	i.e.	(d+e).w0 = d.w = (d-e).w1
		d.w0 + e.w0 = d.w1 - e.w1
		d.(w1-w0) = e.(w1+w0)
		e = d.(w1-w0)/(w1+w0)

	So, we can estimate 'e' from the sizes of the measured width of
	the document.

	Similarly, we can estimate 'f' (the equivalent for tip around
	the y axis as:

		f = d.(h1-h0)/(h1+h0)
*/


/*
	Corner detection: We shall steal the algorithm from the Dropbox
	Document Scanner, as decribed here:

	https://dropbox.tech/machine-learning/fast-and-accurate-document-detection-for-scanning

	A good reference to the steps involved in the canny edge
	detection process can be found here:

	https://towardsdatascience.com/canny-edge-detection-step-by-step-in-python-computer-vision-b49c3a2d8123

	This involves:
	* Canny Edge Detection
	* * Greyscale conversion
	* * Noise reduction
	* * Gradient Calculation
	* * Non-maximum suppression
	* * Double threshold
	* * Edge tracking by Hysteresis
	* Hough Transform to fix possible edges
	* Computing intersections and scoring quads
*/

static void
histeq(fz_pixmap *im)
{
	uint32_t count[256];
	unsigned char tbl[256];
	int i;
	unsigned char *s = im->samples;
	int n = im->w*im->h;
	int sigma;
	int den;

	memset(count, 0, sizeof(count));
	for (i = n; i > 0; i--)
		count[*s++]++;

	/* Rather than doing a pure histogram equalisation, we bend
	 * the table so that 0 always stays as 0, and 255 always stays
	 * as 255. */
	sigma = (count[0]>>1) - count[0];
	den = sigma + n - (count[255]>>1);
	for (i = 0; i < 256; i++)
	{
		int v = count[i];
		sigma += v - (v>>1);
		tbl[i] = (int)(255.0f * sigma / den + 0.5f);
		sigma += (v>>1);
	}

	s = im->samples;
	for (i = n; i > 0; i--)
		*s = tbl[*s], s++;
}

/* The first functions apply a 5x5 gauss filter to blur the greyscale
 * image and remove noise. The gauss filter is a convolution with
 * weights:
 *
 * 2  4  5  4  2
 * 4  9 12  9  4
 * 5 12 15 12  5
 * 4  9 12  9  4
 * 2  4  5  4  2
 *
 * As you can see, there are 3 distinct lines of weights within that
 * matrix. We walk each row of source pixels once, calculating each of
 * those convolutions, storing the result in a rolling buffer of 5x3xw
 * entries. Then we sum columns from the buffer to give us the results.
 */

/* Read across a row of pixels of width w, from s, performing
 * the horizontal portion of the convolutions, storing the results
 * in 3 lines of a buffer, starting at d, &d[w], &d[2*w].
 *
 * d[0*w] uses weights (5 12 15 12  5)
 * d[1*w] uses weights (4  9 12  9  4)
 * d[2*w] uses weights (2  4 5   4  2)
 */
static void
gauss5row(uint16_t *d, const unsigned char *s, int w)
{
	int i;
	int s0 = s[0];
	int s1 = s[1];
	int s2 = s[2];
	int s3 = s[3];

	s += 4;

	d[2*w] = 11*s0 +  4*s1 +  2*s2;
	d[1*w] = 25*s0 +  9*s1 +  4*s2;
	*d++   = 32*s0 + 12*s1 +  5*s2;

	d[2*w] =  6*s0 +  5*s1 +  4*s2 +  2*s3;
	d[1*w] = 13*s0 + 12*s1 +  9*s2 +  4*s3;
	*d++   = 17*s0 + 15*s1 + 12*s2 +  5*s3;

	for (i = w - 4; i > 0; i--)
	{
		int d2 = 2*s0 +  4*s1 +  5*s2 +  4*s3;
		int d1 = 4*s0 +  9*s1 + 12*s2 +  9*s3;
		int d0 = 5*s0 + 12*s1 + 15*s2 + 12*s3;
		s0 = s1;
		s1 = s2;
		s2 = s3;
		s3 = *s++;
		d[2*w] = d2 + 2*s3;
		d[1*w] = d1 + 4*s3;
		*d++   = d0 + 5*s3;
	}

	d[2*w] =  2*s0 +  4*s1 +  5*s2 +  6*s3;
	d[1*w] =  4*s0 +  9*s1 + 12*s2 + 13*s3;
	*d++   =  5*s0 + 12*s1 + 15*s2 + 17*s3;

	d[2*w] =  2*s1 +  4*s2 + 11*s3;
	d[1*w] =  4*s1 +  9*s2 + 25*s3;
	*d     =  5*s1 + 12*s2 + 32*s3;
}

/* Calculate the results for row y of the image, of width w, by
 * summing results from the temporary buffer s, and writing into the
 * original pixmap at d. */
static void
gauss5col(unsigned char *d, const uint16_t *s, int y, int w)
{
	const uint16_t *s0, *s1, *s2, *s3, *s4;
	y *= 3;
	s0 = &s[((y+ 9+2)%15)*w];
	s1 = &s[((y+12+1)%15)*w];
	s2 = &s[( y      %15)*w];
	s3 = &s[((y+ 3+1)%15)*w];
	s4 = &s[((y+ 6+2)%15)*w];

	for (; w > 0; w--)
		*d++ = (*s0++ + *s1++ + *s2++ + *s3++ + *s4++ + 79)/159;
}

static void
gauss5x5(fz_context *ctx, fz_pixmap *src)
{
	int w = src->w;
	int h = src->h;
	uint16_t *buf = fz_malloc(ctx, w*3*5 * sizeof(uint16_t));
	unsigned char *s = src->samples;
	int y;

	if (w < 5 || h < 5)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Pixmap too small");

	gauss5row(&buf[0*3*w], &s[0*w], w);
	gauss5row(&buf[1*3*w], &s[1*w], w);
	memcpy(&buf[3*3*w], buf, w*3 * sizeof(uint16_t)); /* row -2 */
	memcpy(&buf[4*3*w], buf, w*3 * sizeof(uint16_t)); /* row -1 */
	for (y = 2; y < h; y++)
	{
		gauss5row(&buf[((y*3)%15)*w], &s[2*w], w);

		gauss5col(s, buf, y-2, w);
		s += w;
	}
	for (; y < h+2; y++)
	{
		memcpy(&buf[(y%5)*3*w], &buf[((y+4)%5)*3*w], 3*w*sizeof(uint16_t));
		gauss5col(s, buf, y-2, w);
		s += w;
	}

	fz_free(ctx, buf);
}

/* Variant of the above that works on a single plane from rgb data */
static void
gauss5row3(uint16_t *d, const unsigned char *s, int w)
{
	int i;
	int s0 = s[0];
	int s1 = s[3];
	int s2 = s[6];
	int s3 = s[9];

	s += 4*3;

	d[2*w] = 11*s0 +  4*s1 +  2*s2;
	d[1*w] = 25*s0 +  9*s1 +  4*s2;
	*d++   = 32*s0 + 12*s1 +  5*s2;

	d[2*w] =  6*s0 +  5*s1 +  4*s2 +  2*s3;
	d[1*w] = 13*s0 + 12*s1 +  9*s2 +  4*s3;
	*d++   = 17*s0 + 15*s1 + 12*s2 +  5*s3;

	for (i = w - 4; i > 0; i--)
	{
		int d2 = 2*s0 +  4*s1 +  5*s2 +  4*s3;
		int d1 = 4*s0 +  9*s1 + 12*s2 +  9*s3;
		int d0 = 5*s0 + 12*s1 + 15*s2 + 12*s3;
		s0 = s1;
		s1 = s2;
		s2 = s3;
		s3 = *s; s += 3;
		d[2*w] = d2 + 2*s3;
		d[1*w] = d1 + 4*s3;
		*d++   = d0 + 5*s3;
	}

	d[2*w] =  2*s0 +  4*s1 +  5*s2 +  6*s3;
	d[1*w] =  4*s0 +  9*s1 + 12*s2 + 13*s3;
	*d++   =  5*s0 + 12*s1 + 15*s2 + 17*s3;

	d[2*w] =  2*s1 +  4*s2 + 11*s3;
	d[1*w] =  4*s1 +  9*s2 + 25*s3;
	*d     =  5*s1 + 12*s2 + 32*s3;
}

static void
gauss5x5_3(fz_context *ctx, fz_pixmap *dst, const fz_pixmap *src, int comp)
{
	int w = src->w;
	int h = src->h;
	uint16_t *buf = fz_malloc(ctx, w*3*5 * sizeof(uint16_t));
	unsigned char *s = src->samples + comp;
	unsigned char *d = dst->samples;
	int y;

	if (w < 5 || h < 5)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Pixmap too small");

	gauss5row3(&buf[0*3*w], s, w);
	s += w*3;
	gauss5row3(&buf[1*3*w], s, w);
	s += w*3;
	memcpy(&buf[3*3*w], buf, w*3 * sizeof(uint16_t)); /* row -2 */
	memcpy(&buf[4*3*w], buf, w*3 * sizeof(uint16_t)); /* row -1 */
	for (y = 2; y < h; y++)
	{
		gauss5row3(&buf[((y*3)%15)*w], s, w);

		gauss5col(d, buf, y-2, w);
		s += w*3;
		d += w;
	}
	for (; y < h+2; y++)
	{
		memcpy(&buf[(y%5)*3*w], &buf[((y+4)%5)*3*w], 3*w*sizeof(uint16_t));
		gauss5col(d, buf, y-2, w);
		d += w;
	}

	fz_free(ctx, buf);
}

/* The next set of functions perform the gradient calculation.
 * We convolve with Sobel kernels Kx and Ky respectively:
 *
 * Kx =  -1  0  1    Ky =  1  2  1
 *       -2  0  2          0  0  0
 *       -1  0  1         -1 -2 -1
 *
 * We do this by using a rolling temporary buffer of int16_t's to hold
 * 3 pairs of lines of weights scaled by (1 0 1) and (1 2 1).
 *
 * We can then sum entries from those lines to calculate kx and ky for
 * each pixel in the image.
 *
 * Then by examining the values of x and y, we can figure out the
 * "direction" of the edge (horizontal, vertical, or either diagonal),
 * and the magnitude of the difference across that edge. These get
 * encoded back into the original image storage using the 2 bottom bits
 * for direction, and the top 6 bits for magnitude.
 */

static void
gradrow(int16_t *d, const unsigned char *s, int w)
{
	int i;
	unsigned char s0 = *s++;
	unsigned char s1 = *s++;

	d[w] = 3*s0 + s1;
	*d++ = s1 - s0;
	for (i = w-2; i > 0; i--)
	{
		int s2 = *s++;
		d[w] = s0 + 2*s1 + s2;
		*d++ = s2 - s0;
		s0 = s1;
		s1 = s2;
	}
	d[w] = s0 + 3*s1;
	*d   = s1 - s0;
}

static void
gradcol(unsigned char *d, const int16_t *buf, int y, int w)
{
	const int16_t *s0 = &buf[((y+2)%3)*w*2];
	const int16_t *s1 = &buf[((y  )%3)*w*2];
	const int16_t *s2 = &buf[((y+1)%3)*w*2];
	int i;

	for (i = w; i > 0; i--)
	{
		int y = s0[w] - s2[w];
		int x = *s0++ + 2 * *s1++ + *s2++;
		int ax = x >= 0 ? x : -x;
		int ay = y >= 0 ? y : -y;
		int angle, mag;
		/* x and y are now both in the range -1020..1020 */
		/* Now we calculate slope and gradient.
		 *   angle = atan2(y, x);
		 *   intensity = hypot(x, y);
		 * But wait, we don't need that accuracy. We only need
		 * to distinguish 4 directions...
		 *
		 * -22.5 < angle <=  22.5 = 0
		 *  22.5 < angle <=  67.5 = 1
		 *  67.5 < angle <= 112.5 = 2
		 * 115.5 < angle <= 157.5 = 3
		 * (and the reflections)
		 *
		 * 7 0 1   (x positive right, y positive downwards)
		 * 6 * 2
		 * 5 4 3
		 *
		 * tan(22.5)*65536 = 27146.
		 * And for magnitude, we consider the magnitude just
		 * along the 8 directions we've picked. So
		 * 65536/SQR(2) = 46341.
		 * We want magnitude in the 0...63 range.
		 */
		if (ax<<16 < 27146*ay)
		{
			angle = 0;
			mag = ay>>4; /* 0 to 63 */
		}
		else if (ay<<16 < ax*27146)
		{
			angle = 2;
			mag = ax>>4; /* 0 to 63 */
		}
		else
		{
			/* 1 or 3 */
			angle = y >= 0 ? 3 : 1;
			mag = (46341*(ax+ay)) >> 20;
		}
		*d++ = (mag<<2) | angle;
	}
}

static void
grad(fz_context *ctx, fz_pixmap *src)
{
	int w = src->w;
	int h = src->h;
	unsigned char *s = src->samples;
	int16_t *buf = fz_malloc(ctx, w*2*3*sizeof(int16_t));
	int y;

	gradrow(buf, s, w); /* Line 0 */
	memcpy(&buf[w*2*2], buf, w*2*sizeof(int16_t)); /* Line 1 */
	s += w;
	for (y = 1; y < h-1; y++)
	{
		gradrow(&buf[(y%3)*w*2], s, w);
		gradcol(s, buf, y-1, w);
		s += w;
	}
	memcpy(&buf[((y+1)%3)*w*2], &buf[(y%3)*w*2], w*2*sizeof(int16_t)); /* Line h */
	gradcol(s, buf, y-1, w);

	fz_free(ctx, buf);
}

static void
combine_grad(fz_pixmap *grey, const fz_pixmap *r, const fz_pixmap *g, const fz_pixmap *b)
{
	int n;
	unsigned char *sd = grey->samples;
	const unsigned char *sr = r->samples;
	const unsigned char *sg = g->samples;
	const unsigned char *sb = b->samples;

	for (n = g->w * g->h; n > 0; n--)
	{
		unsigned char vg = *sg++;
		unsigned char vr = *sr++;
		unsigned char vb = *sb++;
		unsigned char vd = *sd++;
		if (vr > vg)
			vg = vr;
		if (vb > vg)
			vg = vb;
		if (vg > vd)
			sd[-1] = vg;
	}
}

/* Next, we perform Non-Maximum Suppression and Double Thresholding,
 * both in the same phase.
 *
 * We walk the image, looking at the magnitude of the edges. Edges below
 * the 'weak' threshold are discarded. Otherwise, neighbouring pixels in
 * the direction of the edge are considered; if other pixels are stronger
 * then this pixel is discarded. If not, we classify ourself as either
 * 'strong' or 'weak'.
 */
#define WEAK_THRESHOLD ((int)(0.05 * 63 + 0.5))
#define STRONG_THRESHOLD ((int)(0.09 * 63 + 0.5))
#define WEAK_EDGE 64
#define STRONG_EDGE 128
static void
nonmax(fz_context *ctx, fz_pixmap *src)
{
	int w = src->w;
	int h = src->h;
	unsigned char *s0 = src->samples;
	unsigned char *s1 = s0;
	unsigned char *s2 = s0+w;
	int x, y;

	for (y = h-1; y >= 0;)
	{
		int lastmag;
		int ang = *s1++;
		int mag = ang>>2;
		int q, r;

		/* Pixel 0 */
		if (mag <= WEAK_THRESHOLD)
		{
			/* Not even a weak edge. We'll never keep it. */
			s1[-1] = 0;
			s0++;
			s2++;
		}
		else
		{
			ang &= 3;
			switch (ang)
			{
				default:
				case 0:
					q = (*s0++)>>2;
					r = (*s2++)>>2;
					break;
				case 1:
					q = (*++s0)>>2;
					r = 0;
					s2++;
					break;
				case 2:
					s0++;
					s2++;
					q = 0;
					r = *s1>>2;
					break;
				case 3:
					q = 0;
					s0++;
					r = (*++s2)>>2;
					break;
			}
			if (mag < q || mag < r)
			{
				/* Neighouring edges are stronger.
				 * Lose this one. */
				s1[-1] = 0;
			}
			else if (mag < STRONG_THRESHOLD)
			{
				/* Weak edge. */
				s1[-1] = WEAK_EDGE | (ang<<4);
			}
			else
			{
				/* Strong edge */
				s1[-1] = STRONG_EDGE | (ang<<4);
			}
		}
		lastmag = mag;
		for (x = w-2; x > 0; x--)
		{
			int ang = *s1++;
			int mag = ang>>2;
			int q, r;
			if (mag <= WEAK_THRESHOLD)
			{
				/* Not even a weak edge. We'll never keep it. */
				s1[-1] = 0;
				s0++;
				s2++;
			}
			else
			{
				ang &= 3;
				switch (ang)
				{
					default:
					case 0:
						q = (*s0++)>>2;
						r = (*s2++)>>2;
						break;
					case 1:
						q = (*++s0)>>2;
						r = ((s2++)[-1])>>2;
						break;
					case 2:
						s0++;
						s2++;
						q = lastmag;
						r = *s1>>2;
						break;
					case 3:
						q = ((s0++)[-1])>>2;
						r = (*++s2)>>2;
						break;
				}
				if (mag < q || mag < r)
				{
					/* Neighouring edges are stronger.
					 * Lose this one. */
					s1[-1] = 0;
				}
				else if (mag < STRONG_THRESHOLD)
				{
					/* Weak edge. */
					s1[-1] = WEAK_EDGE | (ang<<4);
				}
				else
				{
					/* Strong edge */
					s1[-1] = STRONG_EDGE | (ang<<4);
				}
			}
			lastmag = mag;
		}
		/* Pixel w-1 */
		ang = *s1++;
		mag = ang>>2;
		if (mag <= WEAK_THRESHOLD)
		{
			/* Not even a weak edge. We'll never keep it. */
			s1[-1] = 0;
			s0++;
			s2++;
			lastmag = 0;
		}
		else
		{
			ang &= 3;
			switch (ang)
			{
				default:
				case 0:
					q = (*s0++)>>2;
					r = (*s2++)>>2;
					break;
				case 1:
					q = 0;
					s0++;
					r = ((s2++)[-1])>>2;
					break;
				case 2:
					s0++;
					s2++;
					q = 0;
					r = *s1>>2;
					break;
				case 3:
					q = ((s0++)[-1])>>2;
					r = 0;
					s2++;
					break;
			}
			if (mag < q || mag < r)
			{
				/* Neighouring edges are stronger.
				 * Lose this one. */
				s1[-1] = 0;
			}
			else if (mag < STRONG_THRESHOLD)
			{
				/* Weak edge. */
				s1[-1] = WEAK_EDGE | (ang<<4);
			}
			else
			{
				/* Strong edge */
				s1[-1] = STRONG_EDGE | (ang<<4);
			}
		}
		s0 = s1-w;
		if (--y == 0)
			s2 = s1;
	}
}

/* Next, we have the hysteresis phase. Here we bump any 'weak' pixel
 * that has at least one strong pixel around it up to being a 'strong'
 * pixel.
 *
 * On entry, strong pixels have bit 7 set, weak pixels have bit 6 set,
 * the angle is in bits 4 and 5, everything else is 0.
 *
 * We walk the rows of the image, and for each pixel we set bit 0 to be
 * the logical OR of all bit 7's of itself, and it's horizontally
 * neighbouring pixels.
 *
 * Once we have done the first 2 rows like that, we can combine the
 * operation of generating the bottom bits for row i, with the
 * calculation of row i-1. Any given pixel on row i-1 should be promoted
 * to 'strong' if it was 'weak', and if the logical OR of the matching
 * pixels in row i, i-1 or i-2 has bit 0 set.
 *
 * At the end of this process any pixel with bit 7 set is 'strong'.
 * Bits 4 and 5 still have the angle.
 */

static void
hysteresis(fz_context *ctx, fz_pixmap *src)
{
	int w = src->w;
	int h = src->h;
	unsigned char *s0 = src->samples;
	unsigned char *s1 = s0;
	unsigned char *s2 = s0;
	unsigned char v0, v1, v2, r0, r1, r2;
	int x, y;

	/* We make the bottom bit in every pixel be 1 iff the pixel
	 * or the ones to either side of it are 'strong'. */

	/* First row - just do the bottom bit. */
	/* Pixel 0 */
	v0 = *s0++;
	v1 = *s0++;
	s0[-2] = v0 | ((v0 | v1)>>7);
	/* Middle pixels */
	for (x = w-2; x > 0; x--)
	{
		v2 = *s0++;
		s0[-2] = v1 | ((v0 | v1 | v2)>>7);
		v0 = v1;
		v1 = v2;
	}
	/* Pixel w-1 */
	s0[-1] = v1 | ((v0 | v1)>>7);

	/* Second row - do the "bottom bit" for the second row, and
	 * perform hysteresis on the top row. */
	/* Pixel 0 */
	v0 = *s0++;
	v1 = *s0++;
	r0 = v0 | ((v0 | v1)>>7);
	s0[-2] = r0;
	r1 = *s1++;
	if ((r1>>6) & (r0 | r1))
		s1[-1] |= 128;
	/* Middle pixels */
	for (x = w-2; x > 0; x--)
	{
		v2 = *s0++;
		r0 = v1 | ((v0 | v1 | v2)>>7);
		s0[-2] = r0;
		r1 = *s1++;
		if ((r1>>6) & (r0 | r1))
			s1[-1] |= 128;
		v0 = v1;
		v1 = v2;
	}
	/* Pixel w-1 */
	r0 = v1 | ((v0 | v1)>>7);
	s0[-1] = r0;
	r1 = *s1++;
	if ((r1>>6) & (r0 | r1))
		s1[-1] |= 128;

	/* Now we get into the swing of things. We do the "bottom bit"
	 * for row n+1, and do the actual processing for row n. */
	for (y = h-4; y > 0; y--)
	{
		/* Pixel 0 */
		v0 = *s0++;
		v1 = *s0++;
		r0 = v0 | ((v0 | v1)>>7);
		s0[-2] = r0;
		r1 = *s1++;
		r2 = *s2++;
		if ((r1>>6) & (r0 | r1 | r2))
			s1[-1] |= 128;
		/* Middle pixels */
		for (x = w-2; x > 0; x--)
		{
			v2 = *s0++;
			r0 = v1 | ((v0 | v1 | v2)>>7);
			s0[-2] = r0;
			r1 = *s1++;
			r2 = *s2++;
			if ((r1>>6) & (r0 | r1 | r2))
				s1[-1] |= 128;
			v0 = v1;
			v1 = v2;
		}
		/* Pixel w-1 */
		r0 = v1 | ((v0 | v1)>>7);
		s0[-1] = r0;
		r1 = *s1++;
		r2 = *s2++;
		if ((r1>>6) & (r0 | r1 | r2))
			s1[-1] |= 128;
	}

	/* Final 2 rows together */
	/* Pixel 0 */
	v0 = *s0++;
	v1 = *s0++;
	r0 = v0 | ((v0 | v1)>>7);
	r1 = *s1++;
	r2 = *s2++;
	if ((r1>>6) & (r0 | r1 | r2))
		s1[-1] |= 128;
	if ((r0>>6) & (r0 | r1))
		s0[-2] |= 128;
	/* Middle pixels */
	for (x = w-2; x > 0; x--)
	{
		v2 = *s0++;
		r0 = v1 | ((v0 | v1 | v2)>>7);
		r1 = *s1++;
		r2 = *s2++;
		if ((r1>>6) & (r0 | r1 | r2))
			s1[-1] |= 128;
		if ((r0>>6) & (r0 | r1))
			s0[-2] |= 128;
		v0 = v1;
		v1 = v2;
	}
	/* Pixel w-1 */
	r0 = v1 | ((v0 | v1)>>7);
	r1 = *s1;
	r2 = *s2;
	if ((r1>>6) & (r0 | r1 | r2))
		s1[0] |= 128;
	if ((r0>>6) & (r0 | r1))
		s0[-1] |= 128;
}

/* A simple function to keep just bit 7 of the image. This 'cleans' the
 * pixmap so that a grey version can be saved out for visual checking. */
static void
clean(fz_context *ctx, fz_pixmap *src)
{
	int w = src->w;
	int h = src->h;
	unsigned char *s = src->samples;

	for (w = w*h; w > 0; w--)
		*s = *s & 128, s++;
}

#ifndef M_PI
#    define  M_PI  3.14159265358979323846
#endif
static void
mark_hough(uint32_t *d, int x, int y, int maxlen, int reduce, int ang)
{
	int theta;
	int stride = (maxlen*2)>>reduce;
	int minang, maxang;

	switch (ang)
	{
		default:
		case 0:
			/* Vertical boundary. Lines through this boundary
			 * go horizontally. So the perpendicular to them
			 * is vertical. */
			minang = 0; maxang = 22;
			break;
		case 1:
			/* NE boundary */
			minang = 22; maxang = 67;
			break;
		case 2:
			/* Horizontal boundary. */
			minang = 67; maxang = 112;
			break;
		case 3:
			/* SE boundary */
			minang = 112; maxang = 157;
			break;
		case 4:
			/* For debugging: */
			minang = 0; maxang = 180;
			break;
	}

	d += minang * stride;
	while (1)
	{
		for (theta = minang; theta < maxang; theta++)
		{
			float thetarad = (theta-90)*M_PI/180.0f;
			float p = x*cosf(thetarad) + y*sinf(thetarad);
			int v = (int)(maxlen + p + 0.5f)>>reduce;

			d[v]++;
			d += stride;
		}
		if (ang != 0)
			break;
		ang = 4;
		minang = 157;
		d += (minang - maxang) * stride;
		maxang = 180;
	}
}

typedef struct
{
	int ang;
	int dis;
	int strength;
	int x0;
	int y0;
	int x1;
	int y1;
} hough_edge_t;

typedef struct
{
	int e0;
	int e1;
	int score;
	int x;
	int y;
} hough_point_t;

static int
intersect(hough_point_t *pt, hough_edge_t *edges, int e0, int e1)
{
	int x1 = edges[e0].x0;
	int y1 = edges[e0].y0;
	int x2 = edges[e0].x1;
	int y2 = edges[e0].y1;
	int x3 = edges[e1].x0;
	int y3 = edges[e1].y0;
	int x4 = edges[e1].x1;
	int y4 = edges[e1].y1;
	int d = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
	int t = (x1-x3)*(y3-y4)-(y1-y3)*(x3-x4);
	float ft;

	if (d < 0)
	{
		if (t < d || t > 0)
			return 0;
	}
	else
	{
		if (t < 0 || t > d)
			return 0;
	}

	pt->e0 = e0;
	pt->e1 = e1;
	pt->score = edges[e0].strength + edges[e1].strength;
	ft = t / (float)d;
	pt->x = x1 + ft*(x2-x1);
	pt->y = y1 + ft*(y2-y1);

	return 1;
}

typedef struct
{
	int w;
	int h;
	int edge[4];
	int point[4];
	int best_score;
	int best_edge[4];
	int best_point[4];
} hough_route_t;

/* To test convexity, we use the dot product:
 *      B
 *     / \
 *    /   C
 *   A
 *
 * The dot product of vectors u and v is:
 *    dot(u,v) = |u|.|v|.cos(theta)
 * Helpfully, cos(theta) > 0 for  -90 < theta < 90.
 * So if we can form perp(AB) = the vector given by rotating
 * AB 90 degrees clockwise, we can then look at the sign of:
 *    dot(perp(AB),BC)
 * If we do that for each corner of the polygon, and the signs
 * of the results for all of them are the same, we have convexity.
 *
 * If any of the dot products are zero, the edges are parallel
 * (i.e. the same edge). This should never happen.
 *
 * If AB = (x,y), then perp(AB) = (y,-x)
 */
static void
score_corner(hough_point_t *points, hough_route_t *route, int p0, int *score, int *sign)
{
	int x0, x1, x2, y0, y1, y2, ux, uy, vx, vy, dot, s, len;
	float costheta;

	/* If we've already decided this is duff, then bale. */
	if (*score < 0)
		return;

	x0 = points[route->point[p0]].x;
	y0 = points[route->point[p0]].y;
	x1 = points[route->point[(p0+1)&3]].x;
	y1 = points[route->point[(p0+1)&3]].y;
	x2 = points[route->point[(p0+2)&3]].x;
	y2 = points[route->point[(p0+2)&3]].y;

	ux = y1-y0;  /* u = Perp(p0 to p1)*/
	uy = x0-x1;
	vx = x2-x1;  /* v = p1 to p2 */
	vy = y2-y1;

	dot = ux*vx + uy*vy;
	if (dot == 0)
	{
		*score = -1;
		return;
	}

	s = (dot > 0) ? 1 : -1;
	if (*sign == 0)
		*sign = s;
	else if (*sign != s)
	{
		*score = -1;
		return;
	}

	len = sqrt(ux*ux + uy*uy) * sqrt(vx*vx + vy*vy);
	costheta = dot / (float)len;
	if (costheta < 0)
		costheta = -costheta;
	costheta *= costheta;

	*score += points[route->point[(p0+1)%3]].score * costheta;
}

/* route points to 8 ints:
 * 2*i   = edge number
 * 2*i+1 = point number
 */
static int
score_route(hough_point_t *points, hough_route_t *route)
{
	int score = 0;
	int sign = 0;

	score_corner(points, route, 0, &score, &sign);
	score_corner(points, route, 1, &score, &sign);
	score_corner(points, route, 2, &score, &sign);
	score_corner(points, route, 3, &score, &sign);

	return score;
}

static float
score_by_area(const hough_point_t *points, const hough_route_t *route)
{
	int minx = points[route->point[0]].x;
	int miny = points[route->point[0]].y;
	int maxx = minx;
	int maxy = miny;

	if (minx > points[route->point[1]].x)
		minx = points[route->point[1]].x;
	else if (maxx < points[route->point[1]].x)
		maxx = points[route->point[1]].x;
	if (minx > points[route->point[2]].x)
		minx = points[route->point[2]].x;
	else if (maxx < points[route->point[2]].x)
		maxx = points[route->point[2]].x;
	if (minx > points[route->point[3]].x)
		minx = points[route->point[3]].x;
	else if (maxx < points[route->point[3]].x)
		maxx = points[route->point[3]].x;
	if (miny > points[route->point[1]].y)
		miny = points[route->point[1]].y;
	else if (maxy < points[route->point[1]].y)
		maxy = points[route->point[1]].y;
	if (miny > points[route->point[2]].y)
		miny = points[route->point[2]].y;
	else if (maxy < points[route->point[2]].y)
		maxy = points[route->point[2]].y;
	if (miny > points[route->point[3]].y)
		miny = points[route->point[3]].y;
	else if (maxy < points[route->point[3]].y)
		maxy = points[route->point[3]].y;


	return (maxx-minx)*(maxy-miny)/(float)route->w/route->h;
}

/* The first n+1 edges of the route are filled in, as are rhe first n
 * points.
 * 2*i   = edge number
 * 2*i+1 = point number
 */
static void
find_route(hough_point_t *points, int num_points, hough_route_t *route, int n)
{
	int i;

	for (i = 0; i < num_points; i++)
	{
		/* If this point continues our route (e0 == route->edge[n])
		 * then the next point in the route might be e1. */
		int e0 = points[i].e0;
		int e1 = points[i].e1;
		if (e0 == route->edge[n])
		{
		}
		else if (e1 == route->edge[n])
		{
			int t = e0; e0 = e1; e1 = t;
		}
		else
			continue; /* Doesn't fit. Keep searching. */

		/* If we've found 3 points already, then this is the
		 * fourth. */
		if (n == 3)
		{
			int score = 0;

			/* If we haven't looped back to our first edge,
			 * no good. Keep searching. */
			if (route->edge[0] != e1)
				continue;
			route->point[3] = i;

			score = score_route(points, route);

			if (score > 0)
				score *= score_by_area(points, route);

			printf("Found route: (point=%d %d %d %d) (edge %d %d %d %d) score=%d\n",
				route->point[0], route->point[1], route->point[2], route->point[3],
				route->edge[0], route->edge[1], route->edge[2], route->edge[3],
				score);

			/* We want our route to be convex */
			if (score < 0)
				continue;

			/* Score the route */
			if (route->best_score < score)
			{
				route->best_score = score;
				memcpy(route->best_edge, route->edge, sizeof(*route->edge)*4);
				memcpy(route->best_point, route->point, sizeof(*route->point)*4);
			}
		}
		else
		{
			int j;
			for (j = 0; j < n && route->edge[j] != e1; j++);
			/* If we're about to loop back to any of the
			 * previous edges, keep searching. */
			if (j != n)
				continue;

			/* Possible. Extend the route, and recurse. */
			route->point[n] = i;
			route->edge[n+1] = e1;
			find_route(points, num_points, route, n+1);
		}
	}
}

#define MAX_EDGES 32
#define BLOT_ANG 10
#define BLOT_DIS 10
static fz_pixmap *
make_hough(fz_context *ctx, fz_pixmap *src)
{
	int w = src->w;
	int h = src->h;
	int maxlen = (int)(sqrtf(w*w + h*h) + 0.5);
	uint32_t *hough;
	fz_pixmap *dst;
	const unsigned char *s = src->samples;
	unsigned char *d;
	int x, y;
	uint32_t maxval;
	uint32_t *p;
	uint32_t scale;
	int reduce;
	int stride;
	hough_edge_t edge[MAX_EDGES];
	hough_point_t points[MAX_EDGES * MAX_EDGES];
	hough_route_t route;
	int num_edges, num_points;

	/* Figure out a suitable scale for the data. */
	reduce = 0;
	while ((maxlen*2>>reduce) > 720 && reduce < 16)
		reduce++;

	stride = (maxlen*2)>>reduce;
	hough = fz_calloc(ctx, sizeof(uint32_t), 180*stride);
	dst = fz_new_pixmap(ctx, NULL, stride, 180, NULL, 0);
	d = dst->samples;

	/* Construct the hough space representation. */
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			unsigned char v = *s++;
			if (v & 128)
				mark_hough(hough, x, y, maxlen, reduce, (v>>4)&3);
		}
	}

	/* Make the image of the hough space (for debugging) */
	maxval = 1; /* Avoid possible division by zero */
	p = hough;
	for (y = 180*stride; y > 0; y--)
	{
		uint32_t v = *p++;
		if (v > maxval)
			maxval = v;
	}

	scale = 0xFFFFFFFFU/maxval;
	p = hough;
	for (y = 180*stride; y > 0; y--)
	{
		*d++ = (scale * *p++)>>24;
	}

	/* We want to find the top n edges that aren't too close to
	 * one another. */
	for (x = 0; x < MAX_EDGES; x++)
	{
		int ang, dis;
		int minang, maxang, mindis, maxdis;
		int where = 0;
		uint32_t *p = hough;
		maxval = 0;
		for (y = 180*stride; y > 0; y--)
		{
			uint32_t v = *p++;
			if (v > maxval)
				maxval = v, where = y;
		}
		where = 180*stride - where;
		ang = edge[x].ang = where/stride;
		dis = edge[x].dis = where - edge[x].ang*stride;
		edge[x].strength = hough[where];
		/* We don't want to find any other maxima that are too
		 * close to this one, so we 'blot out' stuff around this
		 * maxima. */
		printf("Maxima %d: dist=%d ang=%d strength=%d\n",
			x, (dis<<reduce)-maxlen, ang-90, edge[x].strength);
		minang = ang - BLOT_ANG;
		if (minang < 0)
			minang = 0;
		maxang = ang + BLOT_ANG;
		if (maxang > 180)
			maxang = 180;
		mindis = dis - BLOT_DIS;
		if (mindis < 0)
			mindis = 0;
		maxdis = dis + BLOT_DIS;
		if (maxdis > stride)
			maxdis = stride;
		p = hough + minang*stride + mindis;
		maxdis = (maxdis-mindis)*sizeof(uint32_t);
		for (y = maxang-minang; y > 0; y--)
		{
			memset(p, 0, maxdis);
			p += stride;
		}
	}
	num_edges = x;

	/* Find edges in terms of lines. */
	for (x = 0; x < num_edges; x++)
	{
		int ang = edge[x].ang;
		int dis = edge[x].dis;
		float thetarad = (ang-90)*M_PI/180;
		float p = (dis<<reduce) - maxlen;
		if (ang < 45 || ang > 135)
		{
			/* Mostly horizontal line */
			edge[x].x0 = 0;
			edge[x].x1 = w;
			edge[x].y0 = (p - edge[x].x0*cosf(thetarad))/sinf(thetarad);
			edge[x].y1 = (p - edge[x].x1*cosf(thetarad))/sinf(thetarad);
		}
		else
		{
			/* Mostly vertical line */
			edge[x].y0 = 0;
			edge[x].y1 = h;
			edge[x].x0 = (p - edge[x].y0*sinf(thetarad))/cosf(thetarad);
			edge[x].x1 = (p - edge[x].y1*sinf(thetarad))/cosf(thetarad);
		}
	}

	/* Find the points of intersection */
	num_points = 0;
	for (x = 0; x < num_edges-1; x++)
		for (y = x+1; y < num_edges; y++)
			num_points += intersect(&points[num_points],
						edge, x, y);

	printf("%d edges, %d points\n", num_edges, num_points);
	for (x = 0; x < num_points; x++)
		printf("%d %d (score %d, %d+%d)\n",
			points[x].x, points[x].y, points[x].score,
			points[x].e0, points[x].e1);

	/* Now, go looking for 'routes' A->B->C->D->A */
	{
		int i;
		route.w = src->w;
		route.h = src->h;
		route.best_score = 0;
		for (i = 0; i < num_points; i++)
		{
			route.edge[0] = points[i].e0;
			route.point[0] = i;
			route.edge[1] = points[i].e1;
			find_route(points, num_points, &route, 1);
		}

		if (route.best_score)
		{
			printf("Score: %d, Edges=%d->%d->%d->%d, Points=%d->%d->%d->%d\n",
				route.best_score,
				route.best_edge[0],
				route.best_edge[1],
				route.best_edge[2],
				route.best_edge[3],
				route.best_point[0],
				route.best_point[1],
				route.best_point[2],
				route.best_point[3]);
			printf("(%d,%d)->(%d,%d)->(%d,%d)->(%d,%d)\n",
				points[route.best_point[0]].x,
				points[route.best_point[0]].y,
				points[route.best_point[1]].x,
				points[route.best_point[1]].y,
				points[route.best_point[2]].x,
				points[route.best_point[2]].y,
				points[route.best_point[3]].x,
				points[route.best_point[3]].y);
		}
	}

	/* Mark up the src (again, for debugging) */
	{
		fz_device *dev = fz_new_draw_device(ctx, fz_identity, src);
		fz_stroke_state *stroke = fz_new_stroke_state(ctx);
		float col = 1;
		fz_color_params params = { FZ_RI_PERCEPTUAL };
		for (x = 0; x < num_edges; x++)
		{
			char text[64];
			fz_path *path = fz_new_path(ctx);
			fz_moveto(ctx, path, edge[x].x0, edge[x].y0);
			fz_lineto(ctx, path, edge[x].x1, edge[x].y1);
			fz_stroke_path(ctx, dev, path, stroke, fz_identity, fz_device_gray(ctx), &col, 1, params);
			fz_drop_path(ctx, path);
			//printf("%d %d -> %d %d\n", edge[x].x0, edge[x].y0, edge[x].x1, edge[x].y1);
			sprintf(text, "line%d.png", x);
			fz_save_pixmap_as_png(ctx, src, text);
		}

		stroke->linewidth *= 4;
		if (route.best_score)
		{
			fz_path *path = fz_new_path(ctx);
			fz_moveto(ctx, path, points[route.best_point[0]].x, points[route.best_point[0]].y);
			fz_lineto(ctx, path, points[route.best_point[1]].x, points[route.best_point[1]].y);
			fz_lineto(ctx, path, points[route.best_point[2]].x, points[route.best_point[2]].y);
			fz_lineto(ctx, path, points[route.best_point[3]].x, points[route.best_point[3]].y);
			fz_closepath(ctx, path);
			fz_stroke_path(ctx, dev, path, stroke, fz_identity, fz_device_gray(ctx), &col, 1, params);
			fz_drop_path(ctx, path);
		}

		fz_drop_stroke_state(ctx, stroke);
		fz_close_device(ctx, dev);
		fz_drop_device(ctx, dev);
	}

	fz_free(ctx, hough);

	return dst;
}

void
fz_detect_document(fz_context *ctx, fz_point *points, const fz_pixmap *src)
{
	fz_color_params p = {FZ_RI_PERCEPTUAL };
	fz_pixmap *grey = NULL;
	fz_pixmap *r = NULL;
	fz_pixmap *g = NULL;
	fz_pixmap *b = NULL;
	fz_pixmap *hough = NULL;

	fz_var(grey);
	fz_var(r);
	fz_var(g);
	fz_var(b);

	fz_try(ctx)
	{
		if (src->n == 1 && src->alpha == 0)
		{
			grey = fz_clone_pixmap(ctx, src);
		}
		else
		{
			grey = fz_convert_pixmap(ctx, src,
				fz_default_gray(ctx, NULL), NULL, NULL, p, 0);
		}
		gauss5x5(ctx, grey);
		histeq(grey);
		grad(ctx, grey);

		if (src->n == 3 && src->alpha == 0)
		{
			r = fz_new_pixmap(ctx, NULL, src->w, src->h, NULL, 0);
			g = fz_new_pixmap(ctx, NULL, src->w, src->h, NULL, 0);
			b = fz_new_pixmap(ctx, NULL, src->w, src->h, NULL, 0);
			gauss5x5_3(ctx, r, src, 0);
			gauss5x5_3(ctx, g, src, 1);
			gauss5x5_3(ctx, b, src, 2);
			histeq(r);
			histeq(g);
			histeq(b);
			grad(ctx, r);
			grad(ctx, g);
			grad(ctx, b);
			combine_grad(grey, r, g, b);
			fz_drop_pixmap(ctx, r);
			fz_drop_pixmap(ctx, g);
			fz_drop_pixmap(ctx, b);
			r = NULL;
			g = NULL;
			b = NULL;
		}
		fz_save_pixmap_as_png(ctx, grey, "grey.png");
		nonmax(ctx, grey);
		hysteresis(ctx, grey);
		//clean(ctx, grey);
		hough = make_hough(ctx, grey);

		fz_save_pixmap_as_png(ctx, hough, "hough.png");
		fz_save_pixmap_as_png(ctx, grey, "out.png");
	}
	fz_always(ctx)
	{
		fz_drop_pixmap(ctx, r);
		fz_drop_pixmap(ctx, g);
		fz_drop_pixmap(ctx, b);
		fz_drop_pixmap(ctx, grey);
		fz_drop_pixmap(ctx, hough);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}
