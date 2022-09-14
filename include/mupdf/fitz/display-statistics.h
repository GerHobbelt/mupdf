#ifndef MUPDF_FITZ_DISPLAY_STATS_H
#define MUPDF_FITZ_DISPLAY_STATS_H

#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"
#include "mupdf/fitz/geometry.h"
#include "mupdf/fitz/device.h"

#if FZ_ENABLE_RENDER_CORE 

#ifdef __cplusplus
extern "C" {
#endif

/**
	Display statistics device -- count the occurrence of the various drawing primitives and their aspects.
*/

typedef struct
{
	unsigned int transparent_alpha;
	unsigned int opaque_alpha;
	unsigned int outofbounds_alpha;				// # of alpha values which were *outside* the range [0..1]
	unsigned int non_opaque_alpha;
	float non_opaque_alpha_smallest;
	float non_opaque_alpha_largest;

	unsigned int luminosity;
	float luminosity_smallest;
	float luminosity_largest;

	fz_color_params color_params_aggregate;

	unsigned int blendmodes_as_bits;

	fz_rect bounds_smallest;
	fz_rect bounds_largest;

	const char* layer_names[100];

	unsigned int fill_path;
	unsigned int stroke_path;
	unsigned int clip_path;
	unsigned int clip_stroke_path;
	unsigned int fill_text;
	unsigned int stroke_text;
	unsigned int clip_text;
	unsigned int clip_stroke_text;
	unsigned int ignore_text;
	unsigned int fill_shade;
	unsigned int fill_image;
	unsigned int fill_image_mask;
	unsigned int clip_image_mask;
	unsigned int clip_count;     // lump sum of the clip_xyz counts above
	unsigned int clip_depth;
	unsigned int mask_count;
	unsigned int mask_depth;
	unsigned int group_count;
	unsigned int group_depth;
	unsigned int group_knockout;
	unsigned int group_isolated;
	unsigned int tile_count;
	unsigned int tile_depth;
	struct
	{
		unsigned int grey;
		unsigned int rgb;
		unsigned int cmyk;
		unsigned int indexed;
		unsigned int indexed_smallest;
		unsigned int indexed_largest;
	} colorspaces;
	unsigned int default_colorspaces;
	unsigned int layer_count;
	unsigned int layer_depth;

	unsigned int stack_depth;
} fz_gathered_statistics;

/**
	Create a statistics gathering device.

	When you specify a pointer to an existing statistics structure, that one will be
	copied internally and used as the starting point.

	To obtain the up-to-date statistics after a run, call fz_get_device_statistics() API.
*/
fz_device *fz_new_stats_device(fz_context *ctx, const fz_gathered_statistics *stats, fz_device *passthrough);

/**
	Reset the statistics.
*/
void fz_clear_statistics(fz_context* ctx, fz_gathered_statistics* stats);

/**
	Obtain the gathered statistics.
*/
void fz_extract_device_statistics(fz_context* ctx, fz_device* dev, fz_gathered_statistics* dst);

#ifdef __cplusplus
}
#endif

#endif

#endif
