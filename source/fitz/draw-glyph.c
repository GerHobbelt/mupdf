// Copyright (C) 2004-2022 Artifex Software, Inc.
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
#include <math.h>

#define MAX_GLYPH_SIZE 256
#define MAX_CACHE_SIZE (1024*1024)

#define GLYPH_HASH_LEN 509

typedef struct
{
	fz_font *font;
	int a, b;
	int c, d;
	unsigned short gid;
	unsigned char e, f;
	int aa;
} fz_glyph_key;

typedef struct fz_glyph_cache_entry
{
	fz_glyph_key key;
	unsigned hash;
	struct fz_glyph_cache_entry *lru_prev;
	struct fz_glyph_cache_entry *lru_next;
	struct fz_glyph_cache_entry *bucket_next;
	struct fz_glyph_cache_entry *bucket_prev;
	fz_glyph *val;
} fz_glyph_cache_entry;

struct fz_glyph_cache
{
	int refs;
	size_t total;
#ifndef NDEBUG
	int num_evictions;
	ptrdiff_t evicted;
#endif
	fz_glyph_cache_entry *entry[GLYPH_HASH_LEN];
	fz_glyph_cache_entry *lru_head;
	fz_glyph_cache_entry *lru_tail;
};

static size_t
fz_glyph_size(fz_context *ctx, fz_glyph *glyph)
{
	if (glyph == NULL)
		return 0;
	return sizeof(fz_glyph) + glyph->size + fz_pixmap_size(ctx, glyph->pixmap);
}

void
fz_new_glyph_cache_context(fz_context *ctx)
{
	fz_glyph_cache *cache;

	cache = fz_malloc_struct(ctx, fz_glyph_cache);
	cache->total = 0;
	cache->refs = 1;

	ctx->glyph_cache = cache;
}

static void
drop_glyph_cache_entry(fz_context *ctx, fz_glyph_cache_entry *entry)
{
	fz_glyph_cache *cache = ctx->glyph_cache;

	if (entry->lru_next)
		entry->lru_next->lru_prev = entry->lru_prev;
	else
		cache->lru_tail = entry->lru_prev;
	if (entry->lru_prev)
		entry->lru_prev->lru_next = entry->lru_next;
	else
		cache->lru_head = entry->lru_next;
	cache->total -= fz_glyph_size(ctx, entry->val);
	if (entry->bucket_next)
		entry->bucket_next->bucket_prev = entry->bucket_prev;
	if (entry->bucket_prev)
		entry->bucket_prev->bucket_next = entry->bucket_next;
	else
		cache->entry[entry->hash] = entry->bucket_next;
	fz_drop_font(ctx, entry->key.font);
	fz_drop_glyph(ctx, entry->val);
	fz_free(ctx, entry);
}

/* The glyph cache lock is always held when this function is called. */
static void
do_purge(fz_context *ctx)
{
	fz_glyph_cache *cache = ctx->glyph_cache;
	int i;

	for (i = 0; i < GLYPH_HASH_LEN; i++)
	{
		while (cache->entry[i])
			drop_glyph_cache_entry(ctx, cache->entry[i]);
	}

	cache->total = 0;
}

void
fz_purge_glyph_cache(fz_context *ctx)
{
	fz_lock(ctx, FZ_LOCK_GLYPHCACHE);
	do_purge(ctx);
	fz_unlock(ctx, FZ_LOCK_GLYPHCACHE);
}

void
fz_drop_glyph_cache_context(fz_context *ctx)
{
	if (!ctx || !ctx->glyph_cache)
		return;

	fz_lock(ctx, FZ_LOCK_GLYPHCACHE);
	ctx->glyph_cache->refs--;
	if (ctx->glyph_cache->refs == 0)
	{
		do_purge(ctx);
		fz_free(ctx, ctx->glyph_cache);
		ctx->glyph_cache = NULL;
	}
	fz_unlock(ctx, FZ_LOCK_GLYPHCACHE);
}

fz_glyph_cache *
fz_keep_glyph_cache(fz_context *ctx)
{
	fz_lock(ctx, FZ_LOCK_GLYPHCACHE);
	ctx->glyph_cache->refs++;
	fz_unlock(ctx, FZ_LOCK_GLYPHCACHE);
	return ctx->glyph_cache;
}

static void default_sub_pix_quantizer(float size, int *x, int *y)
{
	/* First, in the direction of movement (i.e. normally X). We
	 * never need more than 4 subpixel positions for glyphs -
	 * arguably even that is too much. Suppress this as we get
	 * larger, because it makes less impact. */
	if (size >= 48)
		*x = 1;
	else if (size >= 24)
		*x = 2;
	else
		*x = 4;

	/* Then in the 'downward' direction (normally Y). */
	if (size >= 8)
		*y = 1;
	else if (size >= 4)
		*y = 2;
	else
		*y = 4;
}

float
fz_subpixel_adjust(fz_context *ctx, fz_matrix *ctm, fz_matrix *subpix_ctm, unsigned char *qe, unsigned char *qf)
{
	float size = fz_matrix_expansion(*ctm);
	int q, hq, vq, qmin;
	float pix_e, pix_f, pix_se, pix_sf, r, hr, vr, rmin;
	fz_aa_sub_pix_quantizer *quant;

	quant = ctx->aa.sub_pix_quantizer;
	if (quant == NULL)
		default_sub_pix_quantizer(size, &q, &qmin);
	else
		quant(size, &q, &qmin);

	/* Quantise the subpixel positions. */
	/* q = 0 => round, no subpixel */
	/* q = 1 => r = 1/2 */
	/* q = 2 => r = 1/4 */
	/* q = 3 => r = 1/6 */
	/* q = 4 => r = 1/8 */
	if (q > 0)
		r = 0.5f / q;
	else {
		q = 0;
		r = 0.5f;
	}
	if (qmin > 0)
		rmin = 0.5f / qmin;
	else {
		qmin = 0;
		rmin = 0.5f;
	}

	/* Suppress subpixel antialiasing in y axis if we have a horizontal
	 * matrix, and in x axis if we have a vertical matrix, unless we're
	 * really small. */
	hq = vq = q;
	hr = vr = r;
	if (ctm->a == 0 && ctm->d == 0) {
		hq = qmin;
		hr = rmin;
	}
	if (ctm->b == 0 && ctm->c == 0) {
		vq = qmin;
		vr = rmin;
	}

	/* Split translation into pixel and subpixel parts */
	subpix_ctm->a = ctm->a;
	subpix_ctm->b = ctm->b;
	subpix_ctm->c = ctm->c;
	subpix_ctm->d = ctm->d;
	pix_se = ctm->e + hr;
	pix_e = floorf(pix_se);
	pix_se -= pix_e;
	pix_sf = ctm->f + vr;
	pix_f = floorf(pix_sf);
	pix_sf -= pix_f;

	/* Quantise the subpixel part */
	if (hq) {
		subpix_ctm->e = floorf(pix_se * hq) / hq;
		if (qe)
			*qe = (int)(256 * pix_se);
	}
	else {
		subpix_ctm->e = 0;
		if (qe)
			*qe = 0;
	}
	if (vq) {
		subpix_ctm->f = floorf(pix_sf * vq) / vq;
		if (qf)
			*qf = (int)(256 * pix_sf);
	}
	else {
		subpix_ctm->f = 0;
		if (qf)
			*qf = 0;
	}

	/* Reassemble the complete translation */
	ctm->e = subpix_ctm->e + pix_e;
	ctm->f = subpix_ctm->f + pix_f;

	return size;
}

fz_glyph *
fz_render_stroked_glyph(fz_context *ctx, fz_font *font, int gid, fz_matrix *trm, fz_matrix ctm, fz_colorspace *model, const fz_stroke_state *stroke, const fz_irect *scissor, int aa)
{
	if (fz_font_ft_face(ctx, font))
	{
		fz_matrix subpix_trm;

		if (stroke->dash_len > 0)
			return NULL;
		(void)fz_subpixel_adjust(ctx, trm, &subpix_trm, NULL, NULL);
		return fz_render_ft_stroked_glyph(ctx, font, gid, subpix_trm, ctm, stroke, aa);
	}
	return fz_render_glyph(ctx, font, gid, trm, model, scissor, 1, aa);
}

static unsigned do_hash(unsigned char *s, int len)
{
	unsigned val = 0;
	int i;
	for (i = 0; i < len; i++)
	{
		val += s[i];
		val += (val << 10);
		val ^= (val >> 6);
	}
	val += (val << 3);
	val ^= (val >> 11);
	val += (val << 15);
	return val;
}

static inline void
move_to_front(fz_glyph_cache *cache, fz_glyph_cache_entry *entry)
{
	if (entry->lru_prev == NULL)
		return; /* At front already */

	/* Unlink */
	entry->lru_prev->lru_next = entry->lru_next;
	if (entry->lru_next)
		entry->lru_next->lru_prev = entry->lru_prev;
	else
		cache->lru_tail = entry->lru_prev;
	/* Relink */
	entry->lru_next = cache->lru_head;
	if (entry->lru_next)
		entry->lru_next->lru_prev = entry;
	cache->lru_head = entry;
	entry->lru_prev = NULL;
}

fz_glyph *
fz_render_glyph(fz_context *ctx, fz_font *font, int gid, fz_matrix *ctm, fz_colorspace *model, const fz_irect *scissor, int alpha, int aa)
{
	fz_glyph_cache *cache;
	fz_glyph_key key;
	fz_matrix subpix_ctm;
	fz_irect subpix_scissor;
	float size;
	fz_glyph *val;
	int do_cache, locked, caching;
	fz_glyph_cache_entry *entry;
	unsigned hash;
	int is_ft_font = !!fz_font_ft_face(ctx, font);

	fz_var(locked);
	fz_var(caching);
	fz_var(val);

	memset(&key, 0, sizeof key);
	size = fz_subpixel_adjust(ctx, ctm, &subpix_ctm, &key.e, &key.f);
	if (size <= MAX_GLYPH_SIZE)
	{
		scissor = &fz_infinite_irect;
		do_cache = 1;
	}
	else
	{
		if (is_ft_font)
			return NULL;
		subpix_scissor.x0 = scissor->x0 - floorf(ctm->e);
		subpix_scissor.y0 = scissor->y0 - floorf(ctm->f);
		subpix_scissor.x1 = scissor->x1 - floorf(ctm->e);
		subpix_scissor.y1 = scissor->y1 - floorf(ctm->f);
		scissor = &subpix_scissor;
		do_cache = 0;
	}

	cache = ctx->glyph_cache;

	key.font = font;
	key.gid = gid;
	key.a = subpix_ctm.a * 65536;
	key.b = subpix_ctm.b * 65536;
	key.c = subpix_ctm.c * 65536;
	key.d = subpix_ctm.d * 65536;
	key.aa = aa;

	hash = do_hash((unsigned char *)&key, sizeof(key)) % GLYPH_HASH_LEN;
	fz_lock(ctx, FZ_LOCK_GLYPHCACHE);
	entry = cache->entry[hash];
	while (entry)
	{
		if (memcmp(&entry->key, &key, sizeof(key)) == 0)
		{
			move_to_front(cache, entry);
			val = fz_keep_glyph(ctx, entry->val);
			fz_unlock(ctx, FZ_LOCK_GLYPHCACHE);
			return val;
		}
		entry = entry->bucket_next;
	}

	locked = 1;
	caching = 0;
	val = NULL;

	fz_try(ctx)
	{
		if (is_ft_font)
		{
			val = fz_render_ft_glyph(ctx, font, gid, subpix_ctm, aa);
		}
		else if (fz_font_t3_procs(ctx, font))
		{
			/* We drop the glyphcache here, and execute the t3
			 * glyph code. The danger here is that some other
			 * thread will come along, and want the same glyph
			 * too. If it does, we may both end up rendering
			 * pixmaps. We cope with this later on, by ensuring
			 * that only one gets inserted into the cache. If
			 * we insert ours to find one already there, we
			 * abandon ours, and use the one there already.
			 */
			fz_unlock(ctx, FZ_LOCK_GLYPHCACHE);
			locked = 0;
			val = fz_render_t3_glyph(ctx, font, gid, subpix_ctm, model, scissor, aa);
			fz_lock(ctx, FZ_LOCK_GLYPHCACHE);
			locked = 1; //-V519
		}
		else
		{
			fz_warn(ctx, "assert: uninitialized font structure");
		}
		if (val && do_cache)
		{
			if (val->w < MAX_GLYPH_SIZE && val->h < MAX_GLYPH_SIZE)
			{
				/* If we throw an exception whilst caching,
				 * just ignore the exception and carry on. */
				caching = 1;
				if (!is_ft_font)
				{
					/* We had to unlock. Someone else might
					 * have rendered in the meantime */
					entry = cache->entry[hash];
					while (entry)
					{
						if (memcmp(&entry->key, &key, sizeof(key)) == 0)
						{
							fz_drop_glyph(ctx, val);
							move_to_front(cache, entry);
							val = fz_keep_glyph(ctx, entry->val);
							goto unlock_and_return_val;
						}
						entry = entry->bucket_next;
					}
				}

				entry = fz_malloc_struct(ctx, fz_glyph_cache_entry);
				entry->key = key;
				entry->hash = hash;
				entry->bucket_next = cache->entry[hash];
				if (entry->bucket_next)
					entry->bucket_next->bucket_prev = entry;
				cache->entry[hash] = entry;
				entry->val = fz_keep_glyph(ctx, val);
				fz_keep_font(ctx, key.font);

				entry->lru_next = cache->lru_head;
				if (entry->lru_next)
					entry->lru_next->lru_prev = entry;
				else
					cache->lru_tail = entry;
				cache->lru_head = entry;

				cache->total += fz_glyph_size(ctx, val);
				while (cache->total > MAX_CACHE_SIZE)
				{
#ifndef NDEBUG
					cache->num_evictions++;
					cache->evicted += fz_glyph_size(ctx, cache->lru_tail->val);
#endif
					drop_glyph_cache_entry(ctx, cache->lru_tail);
				}
			}
		}
unlock_and_return_val:
		{
		}
	}
	fz_always(ctx)
	{
		if (locked) //-V547
			fz_unlock(ctx, FZ_LOCK_GLYPHCACHE);
	}
	fz_catch(ctx)
	{
		if (caching)
			fz_warn(ctx, "cannot encache glyph; continuing");
		else
			fz_rethrow(ctx);
	}

	return val; //-V1020
}

fz_pixmap *
fz_render_glyph_pixmap(fz_context *ctx, fz_font *font, int gid, fz_matrix *ctm, const fz_irect *scissor, int aa)
{
	fz_pixmap *val = NULL;
	fz_matrix subpix_ctm;
	float size = fz_subpixel_adjust(ctx, ctm, &subpix_ctm, NULL, NULL);
	int is_ft_font = !!fz_font_ft_face(ctx, font);

	if (size <= MAX_GLYPH_SIZE)
	{
		scissor = &fz_infinite_irect;
	}
	else
	{
		if (is_ft_font)
			return NULL;
	}

	if (is_ft_font)
	{
		val = fz_render_ft_glyph_pixmap(ctx, font, gid, subpix_ctm, aa);
	}
	else if (fz_font_t3_procs(ctx, font))
	{
		val = fz_render_t3_glyph_pixmap(ctx, font, gid, subpix_ctm, NULL, scissor, aa);
	}
	else
	{
		fz_warn(ctx, "assert: uninitialized font structure");
		val = NULL;
	}

	return val;
}

void
fz_dump_glyph_cache_stats(fz_context *ctx)
{
	fz_glyph_cache *cache = ctx->glyph_cache;
	// Only print these values when the values are non-zero: only print these when there's something interesting to report at least.
	if (cache->total
#ifndef NDEBUG
		|| cache->num_evictions || cache->evicted
#endif
		)
	{
		fz_info(ctx, "Glyph Cache Size: %zu\n", cache->total);
#ifndef NDEBUG
		fz_info(ctx, "Glyph Cache Evictions: %d (%zu bytes)\n", cache->num_evictions, cache->evicted);
#endif
	}
}

#endif
