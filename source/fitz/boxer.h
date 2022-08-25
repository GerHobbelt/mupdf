#ifndef BOXER_H
#define BOXER_H

#include "mupdf/fitz/geometry.h"    // fz_rect

typedef struct boxer_s boxer_t;

boxer_t *boxer_create(const fz_rect *rect);
void boxer_feed(boxer_t *boxer, const fz_rect *rect);
void boxer_sort(boxer_t *boxer);
int boxer_results(boxer_t *boxer, fz_rect **list);
fz_rect boxer_margins(const boxer_t *boxer);
boxer_t *boxer_subset(const boxer_t *boxer, const fz_rect *rect);
int boxer_subdivide(const boxer_t *boxer, boxer_t **boxer1, boxer_t **boxer2);
void boxer_destroy(boxer_t *boxer);

#endif
