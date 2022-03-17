#ifndef BOXER_H
#define BOXER_H

typedef struct boxer_s boxer_t;

typedef struct {
	float x0;
	float y0;
	float x1;
	float y1;
} boxer_rect_t;

boxer_t *boxer_create(boxer_rect_t *rect);
void boxer_feed(boxer_t *boxer, boxer_rect_t *rect);
void boxer_sort(boxer_t *boxer);
int boxer_results(boxer_t *boxer, boxer_rect_t **list);
boxer_rect_t boxer_margins(boxer_t *boxer);
boxer_t *boxer_subset(boxer_t *boxer, boxer_rect_t rect);
int boxer_subdivide(boxer_t *boxer, boxer_t **boxer1, boxer_t **boxer2);
void boxer_destroy(boxer_t *boxer);

#endif
