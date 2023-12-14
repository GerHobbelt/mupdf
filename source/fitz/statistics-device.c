//
// Inspired by the list-device, this device collects statistics about the various
// types of elements (primitives) used to define/draw a page.
//
// This device is used in the `mutool metadump` tool to report which PDF primitives
// are used in the PDF (and how often). This then can be used to pick PDFs from
// test sets to test for the proper behaviour of specific primitives.
//

#include "mupdf/fitz.h"

#include "mupdf/assertions.h"
#include <string.h>
#include <time.h>

#if FZ_ENABLE_RENDER_CORE 

typedef struct
{
	fz_device super;

	fz_device* passthrough;

	fz_gathered_statistics stats;

	int stack_depth;
	int clip_depth;
	int mask_depth;
	int group_depth;
	int tile_depth;
	int layer_depth;

	//fz_colorspace *colorspace;
} fz_stats_device;

#define COUNT_AND_MAXIMIZE(name)				\
{												\
	int d = writer->name##_depth;				\
	if (writer->stats.name##_depth < d)			\
		writer->stats.name##_depth = d;			\
	writer->stats.name##_count++;				\
}

#define MAXIMIZE(dst, src)						\
	if ((dst) < (src))							\
		dst = (src)	

#define MINIMIZE(dst, src)						\
	if ((dst) > (src))							\
		dst = (src)

#define COUNT_AND_MIN_AND_MAX(name, value)				\
	MINIMIZE(writer->stats.name##_smallest, value);		\
	MAXIMIZE(writer->stats.name##_largest, value);		\
	writer->stats.name++


static inline void
fz_account_colorspace(fz_context* ctx, fz_stats_device* writer, fz_colorspace* colorspace)
{
	if (colorspace == fz_device_gray(ctx))
	{
		writer->stats.colorspaces.grey++;
	}
	else if (colorspace == fz_device_rgb(ctx))
	{
		writer->stats.colorspaces.rgb++;
	}
	else if (colorspace == fz_device_cmyk(ctx))
	{
		writer->stats.colorspaces.cmyk++;
	}
	else
	{
		writer->stats.colorspaces.indexed++;
		MINIMIZE(writer->stats.colorspaces.indexed_smallest, writer->stats.colorspaces.indexed);
		MAXIMIZE(writer->stats.colorspaces.indexed_largest, writer->stats.colorspaces.indexed);
	}
}

static inline void
fz_account_color_parameters(fz_context* ctx, fz_stats_device* writer, fz_color_params color_params)
{
	writer->stats.color_params_aggregate.ri |= color_params.ri;
	writer->stats.color_params_aggregate.bp |= color_params.bp;
	writer->stats.color_params_aggregate.op |= color_params.op;
	writer->stats.color_params_aggregate.opm |= color_params.opm;
}

static inline void
fz_account_alpha(fz_context* ctx, fz_stats_device* writer, float alpha)
{
	if (alpha > 1.0f)
		writer->stats.outofbounds_alpha++;
	else if (alpha < 0.0f)
		writer->stats.outofbounds_alpha++;
	else if (alpha == 1.0f)
		writer->stats.opaque_alpha++;
	else if (alpha == 0.0f)
		writer->stats.transparent_alpha++;
	else
	{
		COUNT_AND_MIN_AND_MAX(non_opaque_alpha, alpha);
	}
}

static inline void
fz_account_bounds(fz_context* ctx, fz_stats_device* writer, fz_rect rect)
{
	if (!fz_is_empty_rect(rect) && !fz_is_infinite_rect(rect))
	{
		writer->stats.bounds_smallest = fz_intersect_rect(writer->stats.bounds_smallest, rect);
		writer->stats.bounds_largest = fz_union_rect(writer->stats.bounds_largest, rect);
	}
}

static void
fz_stats_fill_path(fz_context* ctx, fz_device* dev, const fz_path* path, int even_odd, fz_matrix ctm,
	fz_colorspace* colorspace, const float* color, float alpha, fz_color_params color_params)
{
	fz_rect rect = fz_bound_path(ctx, path, NULL, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->stats.fill_path++;

	fz_account_colorspace(ctx, writer, colorspace);
	fz_account_alpha(ctx, writer, alpha);
	fz_account_color_parameters(ctx, writer, color_params);

	if (writer->passthrough)
		fz_fill_path(ctx, writer->passthrough, path, even_odd, ctm, colorspace, color, alpha, color_params);
}

static void
fz_stats_stroke_path(fz_context* ctx, fz_device* dev, const fz_path* path, const fz_stroke_state* stroke,
	fz_matrix ctm, fz_colorspace* colorspace, const float* color, float alpha, fz_color_params color_params)
{
	fz_rect rect = fz_bound_path(ctx, path, stroke, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	writer->stats.stroke_path++;

	fz_account_bounds(ctx, writer, rect);

	fz_account_colorspace(ctx, writer, colorspace);
	fz_account_alpha(ctx, writer, alpha);
	fz_account_color_parameters(ctx, writer, color_params);

	if (writer->passthrough)
		fz_stroke_path(ctx, writer->passthrough, path, stroke, ctm, colorspace, color, alpha, color_params);
}

static void
fz_stats_clip_path(fz_context* ctx, fz_device* dev, const fz_path* path, int even_odd, fz_matrix ctm, fz_rect scissor)
{
	fz_rect rect = fz_bound_path(ctx, path, NULL, ctm);
	rect = fz_intersect_rect(rect, scissor);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->clip_depth++;
	writer->stack_depth++;
	MAXIMIZE(writer->stats.stack_depth, writer->stack_depth);
	COUNT_AND_MAXIMIZE(clip);

	writer->stats.clip_path++;

	if (writer->passthrough)
		fz_clip_path(ctx, writer->passthrough, path, even_odd, ctm, scissor);
}

static void
fz_stats_clip_stroke_path(fz_context* ctx, fz_device* dev, const fz_path* path, const fz_stroke_state* stroke, fz_matrix ctm, fz_rect scissor)
{
	fz_rect rect = fz_bound_path(ctx, path, stroke, ctm);
	rect = fz_intersect_rect(rect, scissor);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->clip_depth++;
	writer->stack_depth++;
	MAXIMIZE(writer->stats.stack_depth, writer->stack_depth);
	COUNT_AND_MAXIMIZE(clip);

	writer->stats.clip_stroke_path++;

	if (writer->passthrough)
		fz_clip_stroke_path(ctx, writer->passthrough, path, stroke, ctm, scissor);
}

static void
fz_stats_fill_text(fz_context* ctx, fz_device* dev, const fz_text* text, fz_matrix ctm,
	fz_colorspace* colorspace, const float* color, float alpha, fz_color_params color_params)
{
	fz_rect rect = fz_bound_text(ctx, text, NULL, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->stats.fill_text++;

	fz_account_colorspace(ctx, writer, colorspace);
	fz_account_alpha(ctx, writer, alpha);
	fz_account_color_parameters(ctx, writer, color_params);

	if (writer->passthrough)
		fz_fill_text(ctx, writer->passthrough, text, ctm, colorspace, color, alpha, color_params);
}

static void
fz_stats_stroke_text(fz_context* ctx, fz_device* dev, const fz_text* text, const fz_stroke_state* stroke, fz_matrix ctm,
	fz_colorspace* colorspace, const float* color, float alpha, fz_color_params color_params)
{
	fz_rect rect = fz_bound_text(ctx, text, stroke, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->stats.stroke_text++;

	fz_account_colorspace(ctx, writer, colorspace);
	fz_account_alpha(ctx, writer, alpha);
	fz_account_color_parameters(ctx, writer, color_params);

	if (writer->passthrough)
		fz_stroke_text(ctx, writer->passthrough, text, stroke, ctm, colorspace, color, alpha, color_params);
}

static void
fz_stats_clip_text(fz_context* ctx, fz_device* dev, const fz_text* text, fz_matrix ctm, fz_rect scissor)
{
	fz_rect rect = fz_bound_text(ctx, text, NULL, ctm);
	rect = fz_intersect_rect(rect, scissor);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->clip_depth++;
	writer->stack_depth++;
	MAXIMIZE(writer->stats.stack_depth, writer->stack_depth);
	COUNT_AND_MAXIMIZE(clip);

	writer->stats.clip_text++;

	if (writer->passthrough)
		fz_clip_text(ctx, writer->passthrough, text, ctm, scissor);
}

static void
fz_stats_clip_stroke_text(fz_context* ctx, fz_device* dev, const fz_text* text, const fz_stroke_state* stroke, fz_matrix ctm, fz_rect scissor)
{
	fz_rect rect = fz_bound_text(ctx, text, stroke, ctm);
	rect = fz_intersect_rect(rect, scissor);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->clip_depth++;
	writer->stack_depth++;
	MAXIMIZE(writer->stats.stack_depth, writer->stack_depth);
	COUNT_AND_MAXIMIZE(clip);

	writer->stats.clip_stroke_text++;

	if (writer->passthrough)
		fz_clip_stroke_text(ctx, writer->passthrough, text, stroke, ctm, scissor);
}

static void
fz_stats_ignore_text(fz_context* ctx, fz_device* dev, const fz_text* text, fz_matrix ctm)
{
	fz_rect rect = fz_bound_text(ctx, text, NULL, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->stats.ignore_text++;

	if (writer->passthrough)
		fz_ignore_text(ctx, writer->passthrough, text, ctm);
}

static void
fz_stats_pop_clip(fz_context* ctx, fz_device* dev)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	writer->clip_depth--;
	writer->stack_depth--;

	if (writer->passthrough)
		fz_pop_clip(ctx, writer->passthrough);
}

static void
fz_stats_fill_shade(fz_context* ctx, fz_device* dev, fz_shade* shade, fz_matrix ctm, float alpha, fz_color_params color_params)
{
	fz_rect rect = fz_bound_shade(ctx, shade, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->stats.fill_shade++;

	fz_account_alpha(ctx, writer, alpha);
	fz_account_color_parameters(ctx, writer, color_params);

	if (writer->passthrough)
		fz_fill_shade(ctx, writer->passthrough, shade, ctm, alpha, color_params);
}

static void
fz_stats_fill_image(fz_context* ctx, fz_device* dev, fz_image* image, fz_matrix ctm, float alpha, fz_color_params color_params)
{
	fz_rect rect = fz_transform_rect(fz_unit_rect, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->stats.fill_image++;

	fz_account_alpha(ctx, writer, alpha);
	fz_account_color_parameters(ctx, writer, color_params);

	if (writer->passthrough)
		fz_fill_image(ctx, writer->passthrough, image, ctm, alpha, color_params);
}

static void
fz_stats_fill_image_mask(fz_context* ctx, fz_device* dev, fz_image* image, fz_matrix ctm,
	fz_colorspace* colorspace, const float* color, float alpha, fz_color_params color_params)
{
	fz_rect rect = fz_transform_rect(fz_unit_rect, ctm);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->stats.fill_image_mask++;

	fz_account_colorspace(ctx, writer, colorspace);
	fz_account_alpha(ctx, writer, alpha);
	fz_account_color_parameters(ctx, writer, color_params);

	if (writer->passthrough)
		fz_fill_image_mask(ctx, writer->passthrough, image, ctm, colorspace, color, alpha, color_params);
}

static void
fz_stats_clip_image_mask(fz_context* ctx, fz_device* dev, fz_image* image, fz_matrix ctm, fz_rect scissor)
{
	fz_rect rect = fz_transform_rect(fz_unit_rect, ctm);
	rect = fz_intersect_rect(rect, scissor);

	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->clip_depth++;
	writer->stack_depth++;
	COUNT_AND_MAXIMIZE(clip);

	writer->stats.clip_image_mask++;

	if (writer->passthrough)
		fz_clip_image_mask(ctx, writer->passthrough, image, ctm, scissor);
}

static void
fz_stats_begin_mask(fz_context* ctx, fz_device* dev, fz_rect rect, int luminosity, fz_colorspace* colorspace, const float* color, fz_color_params color_params)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, rect);

	writer->mask_depth++;
	COUNT_AND_MAXIMIZE(mask);

	writer->stack_depth++;
	MAXIMIZE(writer->stats.stack_depth, writer->stack_depth);

	fz_account_colorspace(ctx, writer, colorspace);
	fz_account_color_parameters(ctx, writer, color_params);
	COUNT_AND_MIN_AND_MAX(luminosity, luminosity);

	if (writer->passthrough)
		fz_begin_mask(ctx, writer->passthrough, rect, luminosity, colorspace, color, color_params);
}

static void
fz_stats_end_mask(fz_context* ctx, fz_device* dev, fz_function* fn)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	writer->mask_depth--;

	// oddly enough the list device INCREASED the stack depth for this one?!?!
	//writer->stack_depth++;

	writer->stack_depth--;

	if (writer->passthrough)
		fz_end_mask(ctx, writer->passthrough);
}

static void
fz_stats_begin_group(fz_context* ctx, fz_device* dev, fz_rect rect, fz_colorspace* colorspace, int isolated, int knockout, int blendmode, float alpha)
{
	// strip off the extra bits, so we get a pure blendmode number:
	int extras = blendmode & (FZ_BLEND_ISOLATED | FZ_BLEND_KNOCKOUT);
	unsigned int pure_mode = blendmode & ~(FZ_BLEND_ISOLATED | FZ_BLEND_KNOCKOUT);
	if (pure_mode >= 30)
		pure_mode = 30;
	unsigned int mode_bit = 1U << pure_mode;

	if (isolated || (extras & FZ_BLEND_ISOLATED))
		isolated = 1;
	if (knockout || (extras & FZ_BLEND_KNOCKOUT))
		knockout = 1;

	fz_stats_device* writer = (fz_stats_device*)dev;

	if (knockout)
		writer->stats.group_knockout++;
	if (isolated)
		writer->stats.group_isolated++;
	writer->stats.blendmodes_as_bits |= mode_bit;

	fz_account_bounds(ctx, writer, rect);

	writer->group_depth++;
	COUNT_AND_MAXIMIZE(group);

	fz_account_colorspace(ctx, writer, colorspace);
	fz_account_alpha(ctx, writer, alpha);

	if (writer->passthrough)
		fz_begin_group(ctx, writer->passthrough, rect, colorspace, isolated, knockout, blendmode, alpha);
}

static void
fz_stats_end_group(fz_context* ctx, fz_device* dev)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	writer->group_depth--;

	if (writer->passthrough)
		fz_end_group(ctx, writer->passthrough);
}

static int
fz_stats_begin_tile(fz_context* ctx, fz_device* dev, fz_rect area, fz_rect pattern_bbox_view, float xstep, float ystep, fz_matrix ctm, int id)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_bounds(ctx, writer, area);

	writer->tile_depth++;
	COUNT_AND_MAXIMIZE(tile);

	if (writer->passthrough)
		return fz_begin_tile_id(ctx, writer->passthrough, area, pattern_bbox_view, xstep, ystep, ctm, id);
	else
		return 0;
}

static void
fz_stats_end_tile(fz_context* ctx, fz_device* dev)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	writer->tile_depth--;

	if (writer->passthrough)
		fz_end_tile(ctx, writer->passthrough);
}

static void
fz_stats_render_flags(fz_context* ctx, fz_device* dev, int set, int clear)
{
	int flags;

	/* Pack the options down */
	if (set == FZ_DEVFLAG_GRIDFIT_AS_TILED && clear == 0)
		flags = 1;
	else if (set == 0 && clear == FZ_DEVFLAG_GRIDFIT_AS_TILED)
		flags = 0;
	else
	{
		assert(!"Unsupported flags combination");
		return;
	}

	fz_stats_device* writer = (fz_stats_device*)dev;

	if (writer->passthrough)
		fz_render_flags(ctx, writer->passthrough, set, clear);
}

static void
fz_stats_set_default_colorspaces(fz_context* ctx, fz_device* dev, fz_default_colorspaces* default_cs)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	fz_account_colorspace(ctx, writer, default_cs->gray);
	fz_account_colorspace(ctx, writer, default_cs->rgb);
	fz_account_colorspace(ctx, writer, default_cs->cmyk);
	fz_account_colorspace(ctx, writer, default_cs->oi);

	writer->stats.default_colorspaces++;

	if (writer->passthrough)
		fz_set_default_colorspaces(ctx, writer->passthrough, default_cs);
}

static void
fz_stats_begin_layer(fz_context* ctx, fz_device* dev, const char* layer_name)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	writer->layer_depth++;
	COUNT_AND_MAXIMIZE(layer);

	const int max_layers = sizeof(writer->stats.layer_names) / sizeof(writer->stats.layer_names[0]);
	for (int i = 0; i < max_layers; i++)
	{
		const char* nm = writer->stats.layer_names[i];
		int match = (nm && !strcmp(nm, layer_name));
		if (match)
			break;
		if (!nm)
		{
			// first free slot in the array: store the new layer name here.
			writer->stats.layer_names[i] = fz_strdup(ctx, layer_name);
			break;
		}
	}

	if (writer->passthrough)
		fz_begin_layer(ctx, writer->passthrough, layer_name);
}

static void
fz_stats_end_layer(fz_context* ctx, fz_device* dev)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	writer->layer_depth--;

	if (writer->passthrough)
		fz_end_layer(ctx, writer->passthrough);
}

static void
fz_stats_drop_device(fz_context* ctx, fz_device* dev)
{
	fz_stats_device* writer = (fz_stats_device*)dev;

	const int max_layers = sizeof(writer->stats.layer_names) / sizeof(writer->stats.layer_names[0]);
	for (int i = 0; i < max_layers; i++)
	{
		const char* nm = writer->stats.layer_names[i];
		if (!nm)
			break;
		fz_free(ctx, nm);
		writer->stats.layer_names[i] = NULL;
	}

	if (writer->passthrough)
		fz_drop_device(ctx, writer->passthrough);
}

void
fz_clear_statistics(fz_context* ctx, fz_gathered_statistics* stats)
{
	memset(stats, 0, sizeof(*stats));
	stats->non_opaque_alpha_smallest = 1E38f;
	stats->luminosity_smallest = 1E38f;
	stats->colorspaces.indexed_smallest = INT_MAX;

	stats->bounds_smallest.x0 = FZ_MIN_INF_RECT;
	stats->bounds_smallest.x1 = FZ_MAX_INF_RECT;
	stats->bounds_smallest.y0 = FZ_MIN_INF_RECT;
	stats->bounds_smallest.y1 = FZ_MAX_INF_RECT;
}

fz_device *
fz_new_stats_device(fz_context* ctx, const fz_gathered_statistics* stats, fz_device *passthrough)
{
	fz_stats_device* dev;

	dev = fz_new_derived_device(ctx, fz_stats_device);

	dev->super.fill_path = fz_stats_fill_path;
	dev->super.stroke_path = fz_stats_stroke_path;
	dev->super.clip_path = fz_stats_clip_path;
	dev->super.clip_stroke_path = fz_stats_clip_stroke_path;

	dev->super.fill_text = fz_stats_fill_text;
	dev->super.stroke_text = fz_stats_stroke_text;
	dev->super.clip_text = fz_stats_clip_text;
	dev->super.clip_stroke_text = fz_stats_clip_stroke_text;
	dev->super.ignore_text = fz_stats_ignore_text;

	dev->super.fill_shade = fz_stats_fill_shade;
	dev->super.fill_image = fz_stats_fill_image;
	dev->super.fill_image_mask = fz_stats_fill_image_mask;
	dev->super.clip_image_mask = fz_stats_clip_image_mask;

	dev->super.pop_clip = fz_stats_pop_clip;

	dev->super.begin_mask = fz_stats_begin_mask;
	dev->super.end_mask = fz_stats_end_mask;
	dev->super.begin_group = fz_stats_begin_group;
	dev->super.end_group = fz_stats_end_group;

	dev->super.begin_tile = fz_stats_begin_tile;
	dev->super.end_tile = fz_stats_end_tile;

	dev->super.render_flags = fz_stats_render_flags;
	dev->super.set_default_colorspaces = fz_stats_set_default_colorspaces;

	dev->super.begin_layer = fz_stats_begin_layer;
	dev->super.end_layer = fz_stats_end_layer;

	dev->super.drop_device = fz_stats_drop_device;

	dev->passthrough = passthrough;

	if (stats)
	{
		dev->stats = *stats;

		// dup the layer names so dropping the original stats doesn't nuke them:
		const int max_layers = sizeof(dev->stats.layer_names) / sizeof(dev->stats.layer_names[0]);
		for (int i = 0; i < max_layers; i++)
		{
			const char* nm = dev->stats.layer_names[i];
			if (nm)
				nm = fz_strdup(ctx, nm);
			dev->stats.layer_names[i] = nm;
		}
	}
	else
	{
		fz_clear_statistics(ctx, &dev->stats);
	}
	dev->stack_depth = 0;
	dev->clip_depth = 0;
	dev->mask_depth = 0;
	dev->group_depth = 0;
	dev->tile_depth = 0;
	dev->layer_depth = 0;

	return &dev->super;
}

void
fz_extract_device_statistics(fz_context* ctx, fz_device* dev, fz_gathered_statistics* dst)
{
	fz_stats_device* writer = (fz_stats_device*)dev;
	const fz_gathered_statistics* stats = &writer->stats;

	// first clean the old layer names from the destination record:
	const int max_layers = sizeof(writer->stats.layer_names) / sizeof(writer->stats.layer_names[0]);
	for (int i = 0; i < max_layers; i++)
	{
		const char* nm = dst->layer_names[i];
		if (!nm)
			break;
		fz_free(ctx, nm);
	}

	// then copy our set:
	*dst = *stats;

	// copy the layer names into the destination record so dropping this device doesn't nuke them:
	for (int i = 0; i < max_layers; i++)
	{
		const char* nm = stats->layer_names[i];
		if (nm)
			nm = fz_strdup(ctx, nm);
		dst->layer_names[i] = nm;
	}
}

#endif
