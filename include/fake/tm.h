
#pragma once

#include <stdint.h>

#define ENTER(title, ...)   /**/

typedef uint64_t tm_uint64;

extern tm_uint64 tm_mask;

unsigned long long tmGetAccumulationStart(tm_uint64 tm_mask);

#define LEAVE()  /**/

void SetupTM(const char *conf);
void CloseTM(void);

tm_uint64 tmGetAccumulationStart(tm_uint64 tm_mask );
void tmEmitAccumulationZone( int a, int b, tm_uint64 * start, int c, int clock, const char *description);
