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

#ifndef MUPDF_FITZ_STORY_H
#define MUPDF_FITZ_STORY_H

#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"
#include "mupdf/fitz/buffer.h"
#include "mupdf/fitz/device.h"
#include "mupdf/fitz/writer.h"
#include "mupdf/fitz/xml.h"
#include "mupdf/fitz/archive.h"

#if FZ_ENABLE_HTML

#ifdef __cplusplus
extern "C" {
#endif

/*
	This header file provides an API for laying out and placing styled
	text on a page, or pages.

	First a text story is created from some styled HTML.

	Next, this story can be laid out into a given rectangle (possibly
	retrying several times with updated rectangles as required).

	Next, the laid out story can be drawn to a given device.

	In the case where the text story cannot be fitted into the given
	areas all at once, these two steps can be repeated multiple
	times until the text story is completely consumed.

	Finally, the text story can be dropped in the usual fashion.
*/


typedef struct fz_story fz_story;

/**
	Create a text story using styled html.

	Passing a NULL buffer will be treated as an empty document.
	Passing a NULL user_css will be treated as an empty CSS string.
	A non-NULL dir will allow images etc to be loaded. The
	story keeps its own reference, so the caller can drop its
	reference after this call.
*/
fz_story *fz_new_story(fz_context *ctx, fz_buffer *buf, const char *user_css, float em, fz_archive *dir);

/**
	Retrieve the warnings given from parsing this story.

	If there are warnings, this will be returned as a NULL terminated
	C string. If there are no warnings, this will return NULL.

	These warnings will not be complete until AFTER any DOM manipulations
	have been completed.

	This function does not need to be called, but once it has been
	the DOM is no longer accessible, and any fz_xml pointer
	retrieved from fz_story_docment is no longer valid.
*/
const char *fz_story_warnings(fz_context *ctx, fz_story *story);

/**
	Equivalent to fz_place_story_flags with flags being 0.
*/
int fz_place_story(fz_context *ctx, fz_story *story, fz_rect where, fz_rect *filled);

/**
	Place (or continue placing) a story into the supplied rectangle
	'where', updating 'filled' with the actual area that was used.
	Returns zero (FZ_PLACE_STORY_RETURN_ALL_FITTED) if all the
	content fitted, non-zero if there is more to fit.

	If the FZ_PLACE_STORY_FLAG_NO_OVERFLOW flag is set, then a
	return code of FZ_PLACE_STORY_RETURN_OVERFLOW_WIDTH will be
	returned when the next item (word) to be placed would not fit
	in a rectangle of that given width.

	Note, that filled may not be returned as a strict subset of
	where, due to padding/margins at the bottom of pages, and
	non-wrapping content extending to the right.

	Subsequent calls will attempt to place the same section of story
	again and again, until the placed story is drawn using fz_draw_story,
	whereupon subsequent calls to fz_place_story will attempt to place
	the unused remainder of the story.

	After this function is called, the DOM is no longer accessible,
	and any fz_xml pointer retrieved from fz_story_document is no
	longer valid.

	flags: Additional flags controlling layout. Pass 0 if none
	required.
*/
int fz_place_story_flags(fz_context *ctx, fz_story *story, fz_rect where, fz_rect *filled, int flags);

enum
{
	/* Avoid the usual HTML behaviour of overflowing the box horizontally
	 * in some circumstances. We now abort the place in such cases and
	 * return with */
	FZ_PLACE_STORY_FLAG_NO_OVERFLOW = 1,

	/* Specific return codes from fz_place_story_flags. Also
	 * "non-zero" for 'more to fit'. */
	FZ_PLACE_STORY_RETURN_ALL_FITTED = 0,
	FZ_PLACE_STORY_RETURN_OVERFLOW_WIDTH = 2
};

/*
	Draw the placed story to the given device.

	This moves the point at which subsequent calls to fz_place_story
	will restart placing to the end of what has just been output.
*/
void fz_draw_story(fz_context *ctx, fz_story *story, fz_device *dev, fz_matrix ctm);

/**
	Reset the position within the story at which the next layout call
	will continue to the start of the story.
*/
void fz_reset_story(fz_context *ctx, fz_story *story);

/**
	Drop the html story.
*/
void fz_drop_story(fz_context *ctx, fz_story *story);

/**
	Get a borrowed reference to the DOM document pointer for this
	story. Do not destroy this reference, it will be destroyed
	when the story is laid out.

	This only makes sense before the first placement of the story
	or retrieval of the warnings. Once either of those things happen
	the DOM representation is destroyed.
*/
fz_xml *fz_story_document(fz_context *ctx, fz_story *story);


typedef struct
{
	/* The overall depth of this element in the box structure.
	 * This can be used to compare the relative depths of different
	 * elements, but shouldn't be relied upon not to change between
	 * different versions of MuPDF. */
	int depth;

	/* The heading level of this element. 0 if not a header, or 1-6 for h1-h6. */
	int heading;

	/* The id for this element. */
	const char *id;

	/* The href for this element. */
	const char *href;

	/* The rectangle for this element. */
	fz_rect rect;

	/* The immediate text for this element. */
	const char *text;

	/* This indicates whether this opens and/or closes this element.
	 *
	 * As we traverse the tree we do a depth first search. In order for
	 * the caller of fz_story_positions to know whether a given element
	 * is inside another element, we therefore announce 'start' and 'stop'
	 * for each element. For instance, with:
	 *
	 *   <div id="part1">
	 *    <h1>Chapter 1</h1>...
	 *    <h1>Chapter 2</h1>...
	 *    ...
	 *   </div>
	 *   <div id="part2">
	 *    <h1>Chapter 10</h1>...
	 *    <h1>Chapter 11</h1>...
	 *    ...
	 *   </div>
	 *
	 * We would announce:
	 *   + id='part1' (open)
	 *   + header=1 "Chapter 1" (open/close)
	 *   + header=1 "Chapter 2" (open/close)
	 *   ...
	 *   + id='part1' (close)
	 *   + id='part2' (open)
	 *   + header=1 "Chapter 10" (open/close)
	 *   + header=1 "Chapter 11" (open/close)
	 *   ...
	 *   + id='part2' (close)
	 *
	 * If bit 0 is set, then this 'opens' the element.
	 * If bit 1 is set, then this 'closes' the element.
	 */
	int open_close;

	/* A count of the number of rectangles that the layout code has split the
	 * story into so far. After the first layout, this will be 1. If a
	 * layout is repeated, this number is not incremented. */
	int rectangle_num;
} fz_story_element_position;

typedef void (fz_story_position_callback)(fz_context *ctx, void *arg, const fz_story_element_position *);

/**
	Enumerate the positions for key blocks in the story.

	This will cause the supplied function to be called with details of each
	element in the story that is either a header, or has an id.
*/
void fz_story_positions(fz_context *ctx, fz_story *story, fz_story_position_callback *cb, void *arg);


/* A fz_story_element_position plus page number information. */
typedef struct
{
	fz_story_element_position element;
	int page_num;
} fz_story_tocwrite_item;

/* A set of fz_story_tocwrite_item items, for passing to a
fz_story_tocwrite_contentfn(). */
typedef struct
{
   fz_story_tocwrite_item *items;
   int num;
} fz_story_tocwrite_items;

/**
Callback type for fz_story_tocwrite().

Should set *rect to rect number <num>. If this is on a new page, should also
set <mediabox> and return 1, otherwise return 0.

ref:
	As passed to fz_story_tocwrite().
num:
	The rect number. Will typically increment by one each time, being reset to
	zero when fz_story_tocwrite() starts a new iteration.
filled:
	From earlier internal call to fz_place_story().
rect:
	Out param.
ctm:
	Out param, defaults to fz_identity.
mediabox:
	Out param, only used if we return 1.
*/
typedef int (*fz_story_tocwrite_rectfn)(fz_context *ctx, void *ref, int num, fz_rect filled, fz_rect *rect, fz_matrix *ctm, fz_rect *mediabox);

/**
Callback type for fz_story_tocwrite().

Should populate the supplied buffer with html content for use with
fz_new_story(). This may include extra content derived from the
fz_story_tocwrite_items *items arg, for example a table of contents.

ref:
	As passed to fz_story_tocwrite().
items:
	Information from previous iteration.
buffer:
	Where to write the new content. Will be initially empty.
*/
typedef void (*fz_story_tocwrite_contentfn)(fz_context *ctx, void *ref, fz_story_tocwrite_items *items, fz_buffer *buffer);

/**
Does iterative layout of html content to a fz_document_writer. For example
this allows one to add a table of contents section while ensuring that page
numbers are patched up until stable.

Repeatedly creates new story from (contentfn(), user_css, em) and lays it out
into rects returned by rectfn(), writing to a dummy writer, while gathering
fz_story_element_position information to be passed to the next call of
contentfn().

When the html from contentfn() becomes unchanged, we do a final iteration which
uses <writer>.

writer:
	Where to write in the final iteration.
rectfn:
	Should return information about the rect to be used in the next internal
	call to fz_place_story().
contentfn:
	Should return html content for use with fz_new_story(), possibly
	including extra content such as a table-of-contents.
user_css:
	Used in internal calls to fz_new_story().
em:
	Used in internal calls to fz_new_story().
ref:
	Passed to rectfn() and contentfn().

Passes <ref> to rectfn() and contentfn().
*/
void fz_story_tocwrite(fz_context *ctx, fz_document_writer *writer, fz_story_tocwrite_rectfn rectfn, fz_story_tocwrite_contentfn contentfn, const char *user_css, float em, void *ref);

#ifdef __cplusplus
}
#endif

#endif

#endif
