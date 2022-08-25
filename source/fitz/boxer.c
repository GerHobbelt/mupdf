#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "boxer.h"

typedef struct {
	int len;
	int max;
	fz_rect list[1];
} rectlist_t;

struct boxer_s {
	fz_rect mediabox;
	rectlist_t *list;
};

static rectlist_t *
create_rectlist(int max)
{
	rectlist_t *list = malloc(sizeof(rectlist_t) + sizeof(fz_rect)*(max-1));

	if (list == NULL)
		return NULL;
	list->len = 0;
	list->max = max;

	return list;
}

/* Push box onto rectlist, unless it is completely enclosed by
 * another box, or completely encloses others (in which case they
 * are replaced by it). */
static void
rectlist_push(rectlist_t *list, const fz_rect *box)
{
	int i;
	float box_area = (box->x1 - box->x0)*(box->y1 - box->y0);

	for (i = 0; i < list->len; i++)
	{
		fz_rect *r = &list->list[i];
		float r_area = (r->x1 - r->x0)*(r->y1 - r->y0);
		float r_fudge = 4, box_fudge = 4;

#if 0
		if (r_area > 10*box_area) {
			/* r is much bigger than box. Allow box to be subsumed in r if box *almost* fits.*/
			r_fudge = 16;
		} else if (10*r_area < box_area) {
			/* box is much bigger than r. Allow r to be subsumed in box if r *almost* fits. */
			box_fudge = 16;
		}
#endif
		if (r->x0 - r_fudge <= box->x0  && r->x1 + r_fudge >= box->x1 && r->y0 - r_fudge <= box->y0 && r->y1 + r_fudge >= box->y1)
			return; /* box is enclosed! Nothing to do. */
		if (r->x0 >= box->x0 - box_fudge && r->x1 <= box->x1 + box_fudge && r->y0 >= box->y0 - box_fudge && r->y1 <= box->y1 + box_fudge ) {
			/* box encloses r. Ditch r. */
			if (i < --list->len) {
				*r = list->list[list->len];
				i--;
			}
		}
	}

	list->list[list->len++] = *box;
}

static boxer_t *
boxer_create_length(const fz_rect *mediabox, int len)
{
	boxer_t *boxer = malloc(sizeof(*boxer));

	memcpy(&boxer->mediabox, mediabox, sizeof(*mediabox));
	boxer->list = create_rectlist(len);

	return boxer;
}

boxer_t *
boxer_create(const fz_rect *mediabox)
{
	boxer_t *boxer = boxer_create_length(mediabox, 1);

	rectlist_push(boxer->list, mediabox);

	return boxer;
}

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)

static void
push_if_intersect_suitable(rectlist_t *dst, const fz_rect *a, const fz_rect *b)
{
	fz_rect c;

	/* Intersect a and b. */
	c.x0 = MAX(a->x0, b->x0);
	c.y0 = MAX(a->y0, b->y0);
	c.x1 = MIN(a->x1, b->x1);
	c.y1 = MIN(a->y1, b->y1);
	/* If no intersection, nothing to push. */
	if (c.x0 >= c.x1 || c.y0 >= c.y1)
		return;

	/* If the intersect is too narrow or too tall, ignore it.
	 * We don't care about inter character spaces, for example. */
#define THRESHOLD 4
	if (c.x0 + THRESHOLD >= c.x1 || c.y0+THRESHOLD >= c.y1)
		return;

	rectlist_push(dst, &c);
}

static void
boxlist_feed_intersect(rectlist_t *dst, const rectlist_t *src, const fz_rect *box)
{
	int i;

	for (i = 0; i < src->len; i++)
		push_if_intersect_suitable(dst, &src->list[i], box);
}

void boxer_feed(boxer_t *boxer, const fz_rect *bbox)
{
	fz_rect box;
	/* When we feed a box into a the boxer, we can never make
	 * the list more than 4 times as long. */
	rectlist_t *newlist = create_rectlist(boxer->list->len * 4);

	/* Left (0,0) (x0,H) */
	box.x0 = boxer->mediabox.x0;
	box.y0 = boxer->mediabox.y0;
	box.x1 = bbox->x0;
	box.y1 = boxer->mediabox.y1;
	boxlist_feed_intersect(newlist, boxer->list, &box);

	/* Right (x1,0) (W,H) */
	box.x0 = bbox->x1;
	box.y0 = boxer->mediabox.y0;
	box.x1 = boxer->mediabox.x1;
	box.y1 = boxer->mediabox.y1;
	boxlist_feed_intersect(newlist, boxer->list, &box);

	/* Bottom (0,0) (W,y0) */
	box.x0 = boxer->mediabox.x0;
	box.y0 = boxer->mediabox.y0;
	box.x1 = boxer->mediabox.x1;
	box.y1 = bbox->y0;
	boxlist_feed_intersect(newlist, boxer->list, &box);

	/* Top (0,y1) (W,H) */
	box.x0 = boxer->mediabox.x0;
	box.y0 = bbox->y1;
	box.x1 = boxer->mediabox.x1;
	box.y1 = boxer->mediabox.y1;
	boxlist_feed_intersect(newlist, boxer->list, &box);

	free(boxer->list);
	boxer->list = newlist;
}

static int
compare_areas(const void *a_, const void *b_)
{
	const fz_rect *a = (const fz_rect *)a_;
	const fz_rect *b = (const fz_rect *)b_;
	float area_a = (a->x1-a->x0) * (a->y1-a->y0);
	float area_b = (b->x1-b->x0) * (b->y1-b->y0);

	if (area_a < area_b)
		return 1;
	else if (area_a > area_b)
		return -1;
	else
		return 0;
}

void boxer_sort(boxer_t *boxer)
{
	qsort(boxer->list->list, boxer->list->len, sizeof(boxer->list->list[0]), compare_areas);
}

int boxer_results(boxer_t *boxer, fz_rect **list)
{
	*list = boxer->list->list;
	return boxer->list->len;
}

void boxer_destroy(boxer_t *boxer)
{
	if (boxer)
		free(boxer->list);
	free(boxer);
}

fz_rect boxer_margins(const boxer_t *boxer)
{
	rectlist_t *list = boxer->list;
	int i;
	fz_rect margins = boxer->mediabox;

	for (i = 0; i < list->len; i++)
	{
		fz_rect *r = &list->list[i];
		if (r->x0 <= margins.x0 && r->y0 <= margins.y0 && r->y1 >= margins.y1) {
			margins.x0 = r->x1; /* Left Margin */
		} else if (r->x1 >= margins.x1 && r->y0 <= margins.y0 && r->y1 >= margins.y1) {
			margins.x1 = r->x0; /* Right Margin */
		} else if (r->x0 <= margins.x0 && r->x1 >= margins.x1 && r->y0 <= margins.y0) {
			margins.y0 = r->y1; /* Top Margin */
		} else if (r->x0 <= margins.x0 && r->x1 >= margins.x1 && r->y1 >= margins.y1) {
			margins.y1 = r->y0; /* Bottom Margin */
		}
	}

	return margins;
}

boxer_t *boxer_subset(const boxer_t *boxer, const fz_rect *rect)
{
	boxer_t *new_boxer = boxer_create_length(rect, boxer->list->len);
	int i;

	for (i = 0; i < boxer->list->len; i++) {
		fz_rect r = boxer->list->list[i];

		r.x0 = MAX(r.x0, rect->x0);
		r.y0 = MAX(r.y0, rect->y0);
		r.x1 = MIN(r.x1, rect->x1);
		r.y1 = MIN(r.y1, rect->y1);
		if (r.x0 >= r.x1 || r.y0 >= r.y1)
			continue;
		rectlist_push(new_boxer->list, &r);
	}

	return new_boxer;
}

int boxer_subdivide(const boxer_t *boxer, boxer_t **boxer1, boxer_t **boxer2)
{
	rectlist_t *list = boxer->list;
	int num_h = 0, num_v = 0;
	float max_h = 0, max_v = 0;
	fz_rect best_h, best_v;
	int i;

	*boxer1 = NULL;
	*boxer2 = NULL;

	for (i = 0; i < list->len; i++) {
		fz_rect r = boxer->list->list[i];

		if (r.x0 <= boxer->mediabox.x0 && r.x1 >= boxer->mediabox.x1) {
			/* Horizontal split */
			float size = r.y1 - r.y0;
			if (size > max_h) {
				max_h = size;
				best_h = r;
			}
			num_h++;
		}
		if (r.y0 <= boxer->mediabox.y0 && r.y1 >= boxer->mediabox.y1) {
			/* Vertical split */
			float size = r.x1 - r.x0;
			if (size > max_v) {
				max_v = size;
				best_v = r;
			}
			num_v++;
		}
	}
	printf("num_h=%d num_v=%d\n", num_h, num_v);
	printf("max_h=%g max_v=%g\n", max_h, max_v);

	if (max_h > max_v) {
		fz_rect r;
		/* Split horizontally. */
		r = boxer->mediabox;
		r.y0 = best_h.y1;
		*boxer1 = boxer_subset(boxer, &r);
		r = boxer->mediabox;
		r.y1 = best_h.y0;
		*boxer2 = boxer_subset(boxer, &r);
		return 1;
	} else if (max_v > 0) {
		fz_rect r;
		/* Split vertically. */
		r = boxer->mediabox;
		r.x0 = best_v.x1;
		*boxer1 = boxer_subset(boxer, &r);
		r = boxer->mediabox;
		r.x1 = best_v.x0;
		*boxer2 = boxer_subset(boxer, &r);
		return 1;
	}

	return 0;
}
