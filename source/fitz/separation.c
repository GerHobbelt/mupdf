#include "mupdf/fitz.h"

enum
{
	FZ_SEPARATION_DISABLED_RENDER = 3
};


struct fz_separations_s
{
	int refs;
	int num_separations;
	int controllable;
	uint32_t state[(2*FZ_MAX_SEPARATIONS + 31) / 32];
	fz_colorspace *cs[FZ_MAX_SEPARATIONS];
	uint8_t cs_pos[FZ_MAX_SEPARATIONS];
	uint32_t rgba[FZ_MAX_SEPARATIONS];
	uint32_t cmyk[FZ_MAX_SEPARATIONS];
	char *name[FZ_MAX_SEPARATIONS];
};

fz_separations *fz_new_separations(fz_context *ctx, int controllable)
{
	fz_separations *sep;

	sep = fz_malloc_struct(ctx, fz_separations);
	sep->refs = 1;
	sep->controllable = controllable;

	return sep;
}

fz_separations *fz_keep_separations(fz_context *ctx, fz_separations *sep)
{
	return fz_keep_imp(ctx, sep, &sep->refs);
}

void fz_drop_separations(fz_context *ctx, fz_separations *sep)
{
	if (fz_drop_imp(ctx, sep, &sep->refs))
	{
		int i;
		for (i = 0; i < sep->num_separations; i++)
		{
			fz_free(ctx, sep->name[i]);
			fz_drop_colorspace(ctx, sep->cs[i]);
		}
		fz_free(ctx, sep);
	}
}

void fz_add_separation(fz_context *ctx, fz_separations *sep, const char *name, fz_colorspace *cs, int colorant)
{
	int n;

	if (!sep)
		fz_throw(ctx, FZ_ERROR_GENERIC, "can't add to non-existent separations");

	n = sep->num_separations;
	if (n == FZ_MAX_SEPARATIONS)
		fz_throw(ctx, FZ_ERROR_GENERIC, "too many separations");

	sep->name[n] = fz_strdup(ctx, name);
	sep->cs[n] = fz_keep_colorspace(ctx, cs);
	sep->cs_pos[n] = colorant;

	sep->num_separations++;
}

void fz_add_separation_equivalents(fz_context *ctx, fz_separations *sep, uint32_t rgba, uint32_t cmyk, const char *name)
{
	int n;

	if (!sep)
		fz_throw(ctx, FZ_ERROR_GENERIC, "can't add to non-existent separations");

	n = sep->num_separations;
	if (n == FZ_MAX_SEPARATIONS)
		fz_throw(ctx, FZ_ERROR_GENERIC, "too many separations");

	sep->name[n] = fz_strdup(ctx, name);
	sep->rgba[n] = rgba;
	sep->cmyk[n] = cmyk;

	sep->num_separations++;
}

int fz_separations_controllable(fz_context *ctx, const fz_separations *sep)
{
	return (!sep || sep->controllable);
}

void fz_set_separation_behavior(fz_context *ctx, fz_separations *sep, int separation, fz_separation_behavior beh)
{
	int shift;
	fz_separation_behavior old;

	if (!sep || separation < 0 || separation >= sep->num_separations)
		fz_throw(ctx, FZ_ERROR_GENERIC, "can't control non-existent separation");

	if (beh == FZ_SEPARATION_DISABLED && !sep->controllable)
		beh = FZ_SEPARATION_DISABLED_RENDER;

	shift = ((2*separation) & 31);
	separation >>= 4;

	old = (sep->state[separation]>>shift) & 3;

	if (old == (fz_separation_behavior)FZ_SEPARATION_DISABLED_RENDER)
		old = FZ_SEPARATION_DISABLED;

	/* If no change, great */
	if (old == beh)
		return;

	sep->state[separation] = (sep->state[separation] & ~(3<<shift)) | (beh<<shift);

	/* FIXME: Could only empty images from the store, or maybe only
	 * images that depend on separations. */
	fz_empty_store(ctx);
}

static inline fz_separation_behavior
sep_state(const fz_separations *sep, int i)
{
	return (fz_separation_behavior)((sep->state[i>>5]>>((2*i) & 31)) & 3);
}

fz_separation_behavior fz_separation_current_behavior_internal(fz_context *ctx, const fz_separations *sep, int separation)
{
	if (!sep || separation < 0 || separation >= sep->num_separations)
		fz_throw(ctx, FZ_ERROR_GENERIC, "can't disable non-existent separation");

	return sep_state(sep, separation);
}

fz_separation_behavior fz_separation_current_behavior(fz_context *ctx, const fz_separations *sep, int separation)
{
	int beh = fz_separation_current_behavior_internal(ctx, sep, separation);

	if (beh == FZ_SEPARATION_DISABLED_RENDER)
		return FZ_SEPARATION_DISABLED;
	return beh;
}

int fz_separations_all_composite(fz_context *ctx, const fz_separations *sep)
{
	int i;

	if (!sep)
		return 1;

	for (i = 0; i < (FZ_MAX_SEPARATIONS + 31) / 32; i++)
		if (sep_state(sep, i) != FZ_SEPARATION_COMPOSITE)
			return 0;

	return 1;
}

const char *fz_separation_name(fz_context *ctx, const fz_separations *sep, int separation)
{
	if (!sep || separation < 0 || separation >= sep->num_separations)
		fz_throw(ctx, FZ_ERROR_GENERIC, "can't access non-existent separation");

	return sep->name[separation];
}

int fz_count_separations(fz_context *ctx, const fz_separations *sep)
{
	if (!sep)
		return 0;
	return sep->num_separations;
}

int fz_count_active_separations(fz_context *ctx, const fz_separations *sep)
{
	int i, n, c;

	if (!sep)
		return 0;
	n = sep->num_separations;
	c = 0;
	for (i = 0; i < n; i++)
		if (sep_state(sep, i) == FZ_SEPARATION_SPOT)
			c++;
	return c;
}

fz_separations *fz_clone_separations_for_overprint(fz_context *ctx, fz_separations *sep)
{
	int i, j, n, c;
	fz_separations *clone;

	if (!sep)
		return NULL;

	n = sep->num_separations;
	c = 0;
	for (i = 0; i < n; i++)
	{
		fz_separation_behavior state = sep_state(sep, i);
		if (state == FZ_SEPARATION_COMPOSITE)
			c++;
	}

	/* If no composites, then we are fine to render direct. */
	if (c == 0)
		return NULL;

	/* We need to clone us a separation structure, with all
	 * the composite separations marked as enabled. */
	clone = fz_malloc_struct(ctx, fz_separations);

	fz_try(ctx)
	{
		clone->refs = 1;
		clone->controllable = 0;
		for (i = 0; i < n; i++)
		{
			fz_separation_behavior beh = sep_state(sep, i);
			if (beh == FZ_SEPARATION_DISABLED)
				continue;
			j = clone->num_separations++;
			if (beh == FZ_SEPARATION_COMPOSITE)
				beh = FZ_SEPARATION_SPOT;
			fz_set_separation_behavior(ctx, clone, j, beh);
			clone->name[j] = sep->name[i] ? fz_strdup(ctx, sep->name[i]) : NULL;
			clone->cs[j] = fz_keep_colorspace(ctx, sep->cs[i]);
		}
	}
	fz_catch(ctx)
	{
		fz_drop_separations(ctx, clone);
		fz_rethrow(ctx);
	}

	return clone;
}

fz_pixmap *
fz_clone_pixmap_area_with_different_seps(fz_context *ctx, fz_pixmap *src, const fz_irect *bbox, fz_colorspace *dcs, fz_separations *dseps, const fz_color_params *color_params, fz_colorspace *prf, fz_default_colorspaces *default_cs)
{
	fz_irect local_bbox;
	fz_pixmap *dst;
	fz_colorspace *oi = fz_default_output_intent(ctx, default_cs);

	if (fz_colorspace_n(ctx, dcs) == fz_colorspace_n(ctx, oi))
		dcs = oi;

	if (bbox == NULL)
	{
		local_bbox.x0 = src->x;
		local_bbox.y0 = src->y;
		local_bbox.x1 = src->x + src->w;
		local_bbox.y1 = src->y + src->h;
		bbox = &local_bbox;
	}

	dst = fz_new_pixmap_with_bbox(ctx, dcs, bbox, dseps, src->alpha);
	if (src->flags & FZ_PIXMAP_FLAG_INTERPOLATE)
		dst->flags |= FZ_PIXMAP_FLAG_INTERPOLATE;
	else
		dst->flags &= ~FZ_PIXMAP_FLAG_INTERPOLATE;

	return fz_copy_pixmap_area_converting_seps(ctx, dst, src, color_params, prf, default_cs);
}

fz_pixmap *
fz_copy_pixmap_area_converting_seps(fz_context *ctx, fz_pixmap *dst, fz_pixmap *src, const fz_color_params *color_params, fz_colorspace *prf, fz_default_colorspaces *default_cs)
{
	int dw = dst->w;
	int dh = dst->h;
	fz_separations *sseps = src->seps;
	fz_separations *dseps = dst->seps;
	int sseps_n = sseps ? sseps->num_separations : 0;
	int dseps_n = dseps ? dseps->num_separations : 0;
	int sstride = src->stride;
	int dstride = dst->stride;
	int sn = src->n;
	int dn = dst->n;
	int sa = src->alpha;
	int da = dst->alpha;
	int ss = src->s;
	int ds = dst->s;
	int sc = sn - ss - sa;
	int dc = dn - ds - da;
	const unsigned char *sdata = src->samples + sstride * (dst->y - src->y) + (dst->x - src->x) * sn;
	unsigned char *ddata = dst->samples;
	signed char map[FZ_MAX_COLORS];
	int x, y, i, j, k;
	unsigned char mapped[FZ_MAX_COLORS];
	int unmapped = sseps_n;
	int device_n = 0;

	assert(da == sa);
	assert(ss == fz_count_active_separations(ctx, sseps));
	assert(ds == fz_count_active_separations(ctx, dseps));

	dstride -= dn * dw;
	sstride -= sn * dw;

	/* Process colorants first */
	if (dst->colorspace == src->colorspace)
	{
		/* Simple copy */
		unsigned char *dd = ddata;
		const unsigned char *sd = sdata;
		for (y = dh; y > 0; y--)
		{
			for (x = dw; x > 0; x--)
			{
				for (i = 0; i < dn; i++)
					dd[i] = sd[i];
				dd += dn;
				sd += sn;
			}
			dd += dstride;
			sd += sstride;
		}
	}
	else
	{
		device_n = fz_colorspace_is_device_n(ctx, src->colorspace);
		if (device_n)
			fz_clear_pixmap(ctx, dst);
		else
		{
			fz_pixmap_converter *pc = fz_lookup_pixmap_converter(ctx, dst->colorspace, src->colorspace);

			pc(ctx, dst, src, prf, default_cs, NULL, 0);
		}
	}

	/* Make a map of what spots go where */
	for (i = 0; i < sseps_n; i++)
		mapped[i] = 0;

	for (i = 0, k = 0; i < dseps_n; i++)
	{
		const char *name;
		int state = sep_state(dseps, i);

		if (state != FZ_SEPARATION_SPOT)
			continue;
		name = dseps->name[i];
		if (name == NULL)
			continue;
		map[k] = -1;
		for (j = 0; j < sseps_n; j++)
		{
			const char *sname;
			if (mapped[j])
				continue;
			if (sep_state(sseps, j) != FZ_SEPARATION_SPOT)
				continue;
			sname = sseps->name[j];
			if (sname && !strcmp(name, sname))
			{
				map[k] = j;
				unmapped--;
				mapped[j] = 1;
				break;
			}
		}
		k++;
	}
	if (sa)
		map[k] = sseps_n;

	/* Now we need to make d[i] = map[i] < 0 : 0 ? s[map[i]] */

	ds += da;
	if (ds)
	{
		unsigned char *dd = ddata + dc;
		const unsigned char *sd = sdata + sc;
		for (y = dh; y > 0; y--)
		{
			for (x = dw; x > 0; x--)
			{
				for (i = 0; i < ds; i++)
					dd[i] = map[i] < 0 ? 0 : sd[map[i]];
				dd += dn;
				sd += sn;
			}
			dd += dstride;
			sd += sstride;
		}
	}

	if (unmapped)
	{
		/* Still need to handle mapping 'lost' spots down to process colors */
		for (i = 0; i < sseps_n; i++)
		{
			float convert[FZ_MAX_COLORS];

			if (mapped[i])
				continue;
			/* Src spot i is not mapped. We need to convert that down. */
			fz_separation_equivalent(ctx, sseps, i, color_params, dst->colorspace, prf, convert);

			if (fz_colorspace_is_subtractive(ctx, dst->colorspace))
			{
				if (fz_colorspace_is_subtractive(ctx, src->colorspace))
				{
					unsigned char *dd = ddata;
					const unsigned char *sd = sdata + sc;
					for (y = dh; y > 0; y--)
					{
						for (x = dw; x > 0; x--)
						{
							unsigned char v = sd[i];
							if (v != 0)
								for (k = 0; k < dc; k++)
									dd[k] = fz_clampi(dd[k] + v * convert[k], 0, 255);
							dd += dn;
							sd += sn;
						}
						dd += dstride;
						sd += sstride;
					}
				}
				else
				{
					unsigned char *dd = ddata;
					const unsigned char *sd = sdata + sc;
					for (y = dh; y > 0; y--)
					{
						for (x = dw; x > 0; x--)
						{
							unsigned char v = 0xff - sd[i];
							if (v != 0)
								for (k = 0; k < dc; k++)
									dd[k] = fz_clampi(dd[k] + v * convert[k], 0, 255);
							dd += dn;
							sd += sn;
						}
						dd += dstride;
						sd += sstride;
					}
				}
			}
			else
			{
				if (fz_colorspace_is_subtractive(ctx, src->colorspace))
				{
					unsigned char *dd = ddata;
					const unsigned char *sd = sdata + sc;
					for (y = dh; y > 0; y--)
					{
						for (x = dw; x > 0; x--)
						{
							unsigned char v = sd[i];
							if (v != 0)
								for (k = 0; k < dc; k++)
									dd[k] = fz_clampi(dd[k] - v * (1-convert[k]), 0, 255);
							dd += dn;
							sd += sn;
						}
						dd += dstride;
						sd += sstride;
					}
				}
				else
				{
					unsigned char *dd = ddata;
					const unsigned char *sd = sdata + sc;
					for (y = dh; y > 0; y--)
					{
						for (x = dw; x > 0; x--)
						{
							unsigned char v = 0xff - sd[i];
							if (v != 0)
								for (k = 0; k < dc; k++)
									dd[k] = fz_clampi(dd[k] - v * (1-convert[k]), 0, 255);
							dd += dn;
							sd += sn;
						}
						dd += dstride;
						sd += sstride;
					}
				}
			}
		}
	}

	if (device_n)
	{
		/* Map the colorants down */
		int n = fz_colorspace_n(ctx, src->colorspace);

		fz_color_converter cc;
		fz_find_color_converter(ctx, &cc, prf, dst->colorspace, src->colorspace, color_params);

		fz_try(ctx)
		{
			unmapped = 0;
			for (i = 0; i < n; i++)
			{
				const char *name = fz_colorspace_colorant(ctx, src->colorspace, i);

				map[i] = 0;

				if (name)
				{
					if (!strcmp(name, "None"))
						continue;
					if (!strcmp(name, "All"))
					{
						int n1 = dn - da;
						unsigned char *dd = ddata;
						const unsigned char *sd = sdata + i;

						for (y = dh; y > 0; y--)
						{
							for (x = dw; x > 0; x--)
							{
								unsigned char v = *sd;
								sd += sn;
								for (k = 0; k < n1; k++)
									dd[k] = v;
								dd += dn;
							}
							dd += dstride;
							sd += sstride;
						}
						continue;
					}
					for (j = 0; j < dc; j++)
					{
						const char *dname = fz_colorspace_colorant(ctx, dst->colorspace, j);
						if (dname && !strcmp(name, dname))
							goto map_device_n_spot;
					}
					for (j = 0; j < dseps_n; j++)
					{
						const char *dname = dseps->name[j];
						if (dname && !strcmp(name, dname))
						{
							j += dc;
							goto map_device_n_spot;
						}
					}
				}
				if (0)
				{
					unsigned char *dd;
					const unsigned char *sd;
	map_device_n_spot:
					/* Directly map a devicen colorant to a
					 * component (either process or spot)
					 * in the destination. */
					dd = ddata + j;
					sd = sdata + i;

					for (y = dh; y > 0; y--)
					{
						for (x = dw; x > 0; x--)
						{
							*dd = *sd;
							dd += dn;
							sd += sn;
						}
						dd += dstride;
						sd += sstride;
					}
				}
				else
				{
					unmapped = 1;
					map[i] = 1;
				}
			}
			if (unmapped)
			{
/* The standard spot mapping algorithm assumes that it's reasonable
 * to treat the components of deviceN spaces as being orthogonal,
 * and to add them together at the end. This avoids a color lookup
 * per pixel. The alternative mapping algorithm looks up each
 * pixel at a time, and is hence slower. */
#define ALTERNATIVE_SPOT_MAP
#ifndef ALTERNATIVE_SPOT_MAP
				for (i = 0; i < n; i++)
				{
					/* Src component i is not mapped. We need to convert that down. */
					unsigned char *dd = ddata;
					const unsigned char *sd = sdata;
					float convert[FZ_MAX_COLORS];
					float colors[FZ_MAX_COLORS];

					if (map[i] == 0)
						continue;

					memset(colors, 0, sizeof(float) * n);
					colors[i] = 1;
					cc.convert(ctx, &cc, convert, colors);

					if (fz_colorspace_is_subtractive(ctx, dst->colorspace))
					{
						for (y = dh; y > 0; y--)
						{
							for (x = dw; x > 0; x--)
							{
								unsigned char v = sd[i];
								if (v != 0)
								{
									for (j = 0; j < dc; j++)
										dd[j] = fz_clampi(dd[j] + v * convert[j], 0, 255);
								}
								dd += dn;
								sd += sn;
							}
							dd += dstride;
							sd += sstride;
						}
					}
					else
					{
						for (y = dh; y > 0; y--)
						{
							for (x = dw; x > 0; x--)
							{
								unsigned char v = sd[i];
								if (v != 0)
								{
									for (j = 0; j < dc; j++)
										dd[j] = fz_clampi(dd[j] - v * (1-convert[j]), 0, 255);
								}
								dd += dn;
								sd += sn;
							}
							dd += dstride;
							sd += sstride;
						}
					}
				}
#else
/* If space is subtractive then treat spots like Adobe does in Photoshop.
 * Which is to just use an equivalent CMYK value.  If we are in an additive
 * color space we will need to convert on a pixel-by-pixel basis.
 */
				float convert[FZ_MAX_COLORS];
				float colors[FZ_MAX_COLORS];

				if (fz_colorspace_is_subtractive(ctx, dst->colorspace))
				{
					for (i = 0; i < n; i++)
					{
						unsigned char *dd = ddata;
						const unsigned char *sd = sdata;

						if (map[i] == 0)
							continue;

						memset(colors, 0, sizeof(float) * n);
						colors[i] = 1;
						cc.convert(ctx, &cc, convert, colors);

						for (y = dh; y > 0; y--)
						{
							for (x = dw; x > 0; x--)
							{
								unsigned char v = sd[i];
								if (v != 0)
									for (j = 0; j < dc; j++)
										dd[j] = fz_clampi(dd[j] + v * convert[j], 0, 255);
								dd += dn;
								sd += sn;
							}
							dd += dstride;
							sd += sstride;
						}
					}
				}
				else
				{
					unsigned char *dd = ddata;
					const unsigned char *sd = sdata;
					for (y = dh; y > 0; y--)
					{
						for (x = dw; x > 0; x--)
						{
							for (j = 0; j < n; j++)
								colors[j] = map[j] ? sd[j] / 255.0f : 0;
							cc.convert(ctx, &cc, convert, colors);

							for (j = 0; j < dc; j++)
								dd[j] = fz_clampi(255 * convert[j], 0, 255);
							dd += dn;
							sd += sn;
						}
						dd += dstride;
						sd += sstride;
					}
				}
#endif
			}
		}
		fz_always(ctx)
			fz_drop_color_converter(ctx, &cc);
		fz_catch(ctx)
			fz_rethrow(ctx);
	}

	return dst;
}

void fz_convert_separation_colors(fz_context *ctx, const fz_color_params *color_params, const fz_colorspace *dst_cs, const fz_separations *dst_seps, float *dst_color, const fz_colorspace *src_cs, const float *src_color)
{
	int i, j, n, dc, ds, dn, pred;
	float remainders[FZ_MAX_COLORS];
	int remaining = 0;

	assert(dst_cs && dst_seps && src_cs && dst_color && src_color);
	assert(fz_colorspace_is_device_n(ctx, src_cs));

	dc = fz_colorspace_n(ctx, dst_cs);
	ds = dst_seps->num_separations;
	dn = dc + ds;

	i = 0;
	if (!fz_colorspace_is_subtractive(ctx, dst_cs))
		for (; i < dc; i++)
			dst_color[i] = 1;
	for (; i < dn; i++)
		dst_color[i] = 0;

	n = fz_colorspace_n(ctx, src_cs);
	pred = 0;
	for (i = 0; i < n; i++)
	{
		const char *name = fz_colorspace_colorant(ctx, src_cs, i);

		if (i == 0 && !strcmp(name, "All"))
		{
			/* This is only supposed to happen in separation spaces, not DeviceN */
			if (n != 1)
				fz_warn(ctx, "All found in DeviceN space");
			for (i = 0; i < dn; i++)
				dst_color[i] = src_color[0];
			break;
		}
		if (!strcmp(name, "None"))
			continue;

		/* The most common case is that the colorant we match is the
		 * one after the one we matched before, so optimise for that. */
		for (j = pred; j < ds; j++)
		{
			const char *dname = dst_seps->name[j];
			if (!strcmp(name, dname))
				goto found_sep;
		}
		for (j = 0; j < pred; j++)
		{
			const char *dname = dst_seps->name[j];
			if (!strcmp(name, dname))
				goto found_sep;
		}
		for (j = 0; j < dc; j++)
		{
			const char *dname = fz_colorspace_colorant(ctx, dst_cs, j);
			if (!strcmp(name, dname))
				goto found_process;
		}
		if (0) {
found_sep:
			dst_color[j+dc] = src_color[i];
			pred = j+1;
		}
		else if (0)
		{
found_process:
			dst_color[j] += src_color[i];
		}
		else
		{
			if (remaining == 0)
			{
				memset(remainders, 0, sizeof(float) * n);
				remaining = 1;
			}
			remainders[i] = src_color[i];
		}
	}

	if (remaining)
	{
		/* There were some spots that didn't copy over */
		float converted[FZ_MAX_COLORS];
		fz_convert_color(ctx, color_params, NULL, dst_cs, converted, src_cs, remainders);

		for (i = 0; i < dc; i++)
			dst_color[i] += converted[i];
	}
}

void fz_separation_equivalent(fz_context *ctx, const fz_separations *seps, int i, const fz_color_params *color_params, const fz_colorspace *dst_cs, const fz_colorspace *prf, float *convert)
{
	float colors[FZ_MAX_COLORS];

	if (!seps->cs[i])
	{
		switch (fz_colorspace_n(ctx, dst_cs))
		{
		case 3:
			convert[0] = (seps->rgba[i] & 0xff)/ 255.0f;
			convert[1] = ((seps->rgba[i]>>8) & 0xff)/ 255.0f;
			convert[2] = ((seps->rgba[i]>>16) & 0xff)/ 255.0f;
			convert[3] = ((seps->rgba[i]>>24) & 0xff)/ 255.0f;
			return;
		case 4:
			convert[0] = (seps->cmyk[i] & 0xff)/ 255.0f;
			convert[1] = ((seps->cmyk[i]>>8) & 0xff)/ 255.0f;
			convert[2] = ((seps->cmyk[i]>>16) & 0xff)/ 255.0f;
			convert[3] = ((seps->cmyk[i]>>24) & 0xff)/ 255.0f;
			return;
		default:
			fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot return equivalent in this colorspace");
		}
	}

	memset(colors, 0, sizeof(float) * fz_colorspace_n(ctx, seps->cs[i]));
	colors[seps->cs_pos[i]] = 1;
	fz_convert_color(ctx, color_params, prf, dst_cs, convert, seps->cs[i], colors);
}
