// Copyright (C) 2004-2021 Artifex Software, Inc.
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

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "timeval.h"

#include "plf_nanotimer_c_api.h"


/* Enable FITZ_DEBUG_LOCKING_TIMES below if you want to check the times
 * for which locks are held too. */
#ifdef FITZ_DEBUG_LOCKING
#define FITZ_DEBUG_LOCKING_TIMES
#endif

/*
 * The malloc family of functions will always try scavenging when they run out of memory.
 * They will only fail when scavenging cannot free up memory from caches in the fz_context.
 * All the functions will throw an exception when no memory can be allocated,
 * except the _no_throw family which instead silently returns NULL.
 */

static void *
do_scavenging_malloc(fz_context *ctx, size_t size   FZDBG_DECL_ARGS)
{
	void *p;
	int phase = 0;

	if (ctx == NULL)
		return NULL;

	fz_lock(ctx, FZ_LOCK_ALLOC);
	do {
		p = ctx->alloc.malloc_(ctx->alloc.user, size   FZDBG_PASS);
		if (p != NULL)
		{
			fz_unlock(ctx, FZ_LOCK_ALLOC);
			return p;
		}
	} while (fz_store_scavenge(ctx, size, &phase));
	fz_unlock(ctx, FZ_LOCK_ALLOC);

	return NULL;
}

static void *
do_scavenging_realloc(fz_context *ctx, void *p, size_t size   FZDBG_DECL_ARGS)
{
	void *q;
	int phase = 0;

	if (ctx == NULL)
		return NULL;

	fz_lock(ctx, FZ_LOCK_ALLOC);
	do {
		q = ctx->alloc.realloc_(ctx->alloc.user, p, size   FZDBG_PASS);
		if (q != NULL)
		{
			fz_unlock(ctx, FZ_LOCK_ALLOC);
			return q;
		}
	} while (fz_store_scavenge(ctx, size, &phase));
	fz_unlock(ctx, FZ_LOCK_ALLOC);

	return NULL;
}

#undef fz_malloc
void *
fz_malloc(fz_context *ctx, size_t size   FZDBG_DECL_ARGS)
{
	void *p;
	if (size == 0)
		return NULL;
	p = do_scavenging_malloc(ctx, size   FZDBG_PASS);
	if (!p)
		fz_throw(ctx, FZ_ERROR_MEMORY, "malloc of %zu bytes failed", size);
	return p;
}

#undef fz_malloc_no_throw
void *
fz_malloc_no_throw(fz_context *ctx, size_t size   FZDBG_DECL_ARGS)
{
	if (size == 0)
		return NULL;
	return do_scavenging_malloc(ctx, size   FZDBG_PASS);
}

#undef fz_calloc
void *
fz_calloc(fz_context *ctx, size_t count, size_t size   FZDBG_DECL_ARGS)
{
	void *p;
	if (count == 0 || size == 0)
		return NULL;
	if (count > SIZE_MAX / size)
		fz_throw(ctx, FZ_ERROR_MEMORY, "calloc (%zu x %zu bytes) failed (size_t overflow)", count, size);
	p = do_scavenging_malloc(ctx, count * size   FZDBG_PASS);
	if (!p)
		fz_throw(ctx, FZ_ERROR_MEMORY, "calloc (%zu x %zu bytes) failed", count, size);
	memset(p, 0, count*size);
	return p;
}

#undef fz_calloc_no_throw
void *
fz_calloc_no_throw(fz_context *ctx, size_t count, size_t size   FZDBG_DECL_ARGS)
{
	void *p;
	if (count == 0 || size == 0)
		return NULL;
	if (count > SIZE_MAX / size)
		return NULL;
	p = do_scavenging_malloc(ctx, count * size   FZDBG_PASS);
	if (p)
		memset(p, 0, count * size);
	return p;
}

#undef fz_realloc
void *
fz_realloc(fz_context *ctx, void *p, size_t size   FZDBG_DECL_ARGS)
{
	if (size == 0)
	{
		fz_free(ctx, p);
		return NULL;
	}
	p = do_scavenging_realloc(ctx, p, size   FZDBG_PASS);
	if (!p)
		fz_throw(ctx, FZ_ERROR_MEMORY, "realloc (%zu bytes) failed", size);
	return p;
}

#undef fz_realloc_no_throw
void *
fz_realloc_no_throw(fz_context *ctx, void *p, size_t size   FZDBG_DECL_ARGS)
{
	if (size == 0)
	{
		fz_free(ctx, p);
		return NULL;
	}
	return do_scavenging_realloc(ctx, p, size   FZDBG_PASS);
}

void
fz_free(fz_context *ctx, const void *p)
{
	if (p)
	{
		assert(ctx != NULL);
		fz_lock(ctx, FZ_LOCK_ALLOC);
		ctx->alloc.free_(ctx->alloc.user, (void *)p);
		fz_unlock(ctx, FZ_LOCK_ALLOC);
	}
}

#undef fz_strdup
char *
fz_strdup(fz_context *ctx, const char *s   FZDBG_DECL_ARGS)
{
	size_t len = strlen(s) + 1;
	char *ns = (char *)fz_malloc(ctx, len   FZDBG_PASS);
	ASSERT(ns);
	if (ns)
	{
		memcpy(ns, s, len);
	}
	return ns;
}

static void *
fz_malloc_default(void *opaque, size_t size   FZDBG_DECL_ARGS)
{
#if defined(FZDBG_HAS_TRACING) && defined(_NORMAL_BLOCK)
	return _malloc_dbg(size, _NORMAL_BLOCK   FZDBG_PASS);
#else
	return malloc(size);
#endif
}

static void *
fz_realloc_default(void *opaque, void *old, size_t size   FZDBG_DECL_ARGS)
{
#if defined(FZDBG_HAS_TRACING) && defined(_NORMAL_BLOCK)
	return _realloc_dbg(old, size, _NORMAL_BLOCK   FZDBG_PASS);
#else
	return realloc(old, size);
#endif
}

static void
fz_free_default(void *opaque, void *ptr)
{
	free(ptr);
}

fz_alloc_context fz_alloc_default =
{
	NULL,
	fz_malloc_default,
	fz_realloc_default,
	fz_free_default
};

static void
fz_lock_default(void *user, int lock)
{
}

static void
fz_unlock_default(void *user, int lock)
{
}

fz_locks_context fz_locks_default =
{
	NULL,
	fz_lock_default,
	fz_unlock_default
};

#ifdef FITZ_DEBUG_LOCKING

enum
{
	FZ_LOCK_DEBUG_CONTEXT_MAX = 100
};

static fz_context *fz_lock_debug_contexts[FZ_LOCK_DEBUG_CONTEXT_MAX];
static int fz_locks_debug[FZ_LOCK_DEBUG_CONTEXT_MAX][FZ_LOCK_MAX];

#ifdef FITZ_DEBUG_LOCKING_TIMES

static double fz_lock_time[FZ_LOCK_DEBUG_CONTEXT_MAX][FZ_LOCK_MAX] = { { 0 } };
static nanotimer_data_t fz_lock_taken[FZ_LOCK_DEBUG_CONTEXT_MAX][FZ_LOCK_MAX] = { { { 0 } } };

static const char* fz_locks_debug_srcfile[FZ_LOCK_DEBUG_CONTEXT_MAX][FZ_LOCK_MAX] = { { 0 } };
static unsigned int fz_locks_debug_srcline[FZ_LOCK_DEBUG_CONTEXT_MAX][FZ_LOCK_MAX] = { { 0 } };

/* We implement our own millisecond clock, as clock() cannot be trusted
 * when threads are involved. */

void fz_dump_lock_times(fz_context* ctx, int total_program_time_ms)
{
	int i, j;

	for (j = 0; j < FZ_LOCK_MAX; j++)
	{
		double total = 0;
		for (i = 0; i < FZ_LOCK_DEBUG_CONTEXT_MAX; i++)
		{
			total += fz_lock_time[i][j];
		}
		fz_info(ctx, "Lock %d held for %g seconds (%g%%)\n", j, total / 1000.0, 100.0 * total / fz_max(1.0, total_program_time_ms));
	}
	fz_info(ctx, "Total program time %g seconds\n", total_program_time_ms / 1000.0);
}

#endif  // FITZ_DEBUG_LOCKING_TIMES

static int find_context(fz_context *ctx)
{
	int i;

	for (i = 0; i < FZ_LOCK_DEBUG_CONTEXT_MAX; i++)
	{
		if (fz_lock_debug_contexts[i] == ctx)
			return i;
		if (fz_lock_debug_contexts[i] == NULL)
		{
			int gottit = 0;
			/* We've not locked on this context before, so use
			 * this one for this new context. We might have other
			 * threads trying here too though so, so claim it
			 * atomically. No one has locked on this context
			 * before, so we are safe to take the ALLOC lock. */
			ctx->locks.lock(ctx->locks.user, FZ_LOCK_ALLOC);
			/* If it's still free, then claim it as ours,
			 * otherwise we'll keep hunting. */
			if (fz_lock_debug_contexts[i] == NULL)
			{
				gottit = 1;
				fz_lock_debug_contexts[i] = ctx;
			}
			ctx->locks.unlock(ctx->locks.user, FZ_LOCK_ALLOC);
			if (gottit)
				return i;
		}
	}
	return -1;
}

void
fz_assert_lock_held(fz_context *ctx, int lock)
{
	int idx;

	if (ctx->locks.lock != fz_lock_default)
		return;

	idx = find_context(ctx);
	if (idx < 0)
		return;

	if (fz_locks_debug[idx][lock] == 0)
		fz_error(ctx, "Lock %d not held when expected", lock);
}

void
fz_assert_lock_not_held(fz_context *ctx, int lock)
{
	int idx;

	if (ctx->locks.lock != fz_lock_default)
		return;

	idx = find_context(ctx);
	if (idx < 0)
		return;

	if (fz_locks_debug[idx][lock] != 0)
		fz_error(ctx, "Lock %d held when not expected", lock);
}

void fz_lock_debug_lock_start_timer_assist(fz_context* ctx, int lock)
{
	int idx;

#if 0
	if (ctx->locks.lock != fz_lock_default)
		return;
#endif

	idx = find_context(ctx);
	if (idx < 0)
		return;

#ifdef FITZ_DEBUG_LOCKING_TIMES
	nanotimer_start(&fz_lock_taken[idx][lock]);
#endif
}

void fz_lock_debug_attempt_lock(fz_context* ctx, int lock)
{
	int idx;

#if 0
	if (ctx->locks.lock != fz_lock_default)
		return;
#endif

	idx = find_context(ctx);
	if (idx < 0)
		return;

#if 0
	if (fz_locks_debug[idx][lock] != 0)
	{
		fz_error(ctx, "Attempt to take lock %d when held already!", lock);
	}
	for (int i = lock - 1; i >= 0; i--)
	{
		if (fz_locks_debug[idx][i] != 0)
		{
			fz_error(ctx, "Lock ordering violation: Attempt to take lock %d when %d held already!", lock, i);
		}
	}
#endif
}

void fz_lock_debug_lock_obtained(fz_context *ctx, int lock)
{
	int idx;

#if 0
	if (ctx->locks.lock != fz_lock_default)
		return;
#endif

	idx = find_context(ctx);
	if (idx < 0)
		return;

	if (fz_locks_debug[idx][lock] != 0)
	{
		fz_error(ctx, "Obtained lock %d when held already!", lock);
	}
#if 0
	for (int i = lock-1; i >= 0; i--)
	{
		if (fz_locks_debug[idx][i] != 0)
		{
			fz_error(ctx, "Lock ordering violation: obtained lock %d when %d held already!", lock, i);
		}
	}
#endif
	fz_locks_debug[idx][lock] = 1;
}

void fz_lock_debug_unlock(fz_context *ctx, int lock)
{
	int idx;

#if 0
	if (ctx->locks.lock != fz_lock_default)
		return;
#endif

	idx = find_context(ctx);
	if (idx < 0)
		return;

	if (fz_locks_debug[idx][lock] == 0)
	{
		fz_error(ctx, "Attempt to release lock %d when not held!", lock);
	}
	fz_locks_debug[idx][lock] = 0;
#ifdef FITZ_DEBUG_LOCKING_TIMES
	fz_lock_time[idx][lock] += nanotimer_get_elapsed_ms(&fz_lock_taken[idx][lock]);
#endif
}

#elif !(defined(FITZ_DEBUG_LOCKING) && defined(FITZ_DEBUG_LOCKING_TIMES)) && defined(fz_dump_lock_times)

// provide dummy for MuPDFLib.DLL anytime the real thing isn't available...

#undef fz_dump_lock_times
void fz_dump_lock_times(fz_context* ctx, int total_program_time_ms)
{
}

#else  // FITZ_DEBUG_LOCKING

void
(fz_assert_lock_held)(fz_context *ctx, int lock)
{
}

void
(fz_assert_lock_not_held)(fz_context *ctx, int lock)
{
}

void (fz_lock_debug_lock)(fz_context *ctx, int lock)
{
}

void (fz_lock_debug_unlock)(fz_context *ctx, int lock)
{
}

#endif  // FITZ_DEBUG_LOCKING

