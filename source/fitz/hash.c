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

#include <string.h>
#include "mupdf/assertions.h"

/*
	Simple hashtable with open addressing linear probe.
	Unlike text book examples, removing entries works
	correctly in this implementation, so it won't start
	exhibiting bad behaviour if entries are inserted
	and removed frequently.
*/

typedef struct
{
	unsigned char key[FZ_HASH_TABLE_KEY_LENGTH];
	void *val;
} fz_hash_entry;

struct fz_hash_table
{
	int keylen;
	int size; // number of slots available
	int load; // number of slots filled
	int lock; /* -1 or the lock used to protect this hash table */
	fz_hash_table_drop_fn *drop_val;
	fz_hash_entry *ents;
};

static unsigned hash(const unsigned char *s, int len)
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

fz_hash_table *
fz_new_hash_table(fz_context *ctx, int initialsize, int keylen, int lock, fz_hash_table_drop_fn *drop_val)
{
	fz_hash_table *table;

	if (initialsize < 16)
		fz_throw(ctx, FZ_ERROR_GENERIC, "initial hash table size (%d) too small", initialsize);

	if (keylen > FZ_HASH_TABLE_KEY_LENGTH)
		fz_throw(ctx, FZ_ERROR_ARGUMENT, "hash table key length too large");

	table = fz_malloc_struct(ctx, fz_hash_table);
	table->keylen = keylen;
	table->size = initialsize;
	table->load = 0;
	table->lock = lock;
	table->drop_val = drop_val;
	fz_try(ctx)
	{
		table->ents = Memento_label(fz_malloc_array(ctx, table->size, fz_hash_entry), "hash_entries");
		memset(table->ents, 0, sizeof(fz_hash_entry) * table->size);
	}
	fz_catch(ctx)
	{
		fz_free(ctx, table);
		fz_rethrow(ctx);
	}

	return table;
}

void
fz_drop_hash_table(fz_context *ctx, fz_hash_table *table)
{
	if (!table)
		return;

	if (table->drop_val)
	{
		int i, n = table->size;
		for (i = 0; i < n; ++i)
		{
			void *v = table->ents[i].val;
			if (v)
				table->drop_val(ctx, v);
		}
	}

	fz_free(ctx, table->ents);
	fz_free(ctx, table);
}

static void *
do_hash_insert(fz_context *ctx, fz_hash_table *table, const void *key, void *val)
{
	fz_hash_entry *ents;
	unsigned size;
	unsigned pos;

	ents = table->ents;
	size = table->size;
	pos = hash(key, table->keylen) % size;

	if (table->lock >= 0)
		fz_assert_lock_held(ctx, table->lock);

	while (1)
	{
		if (!ents[pos].val)
		{
			memcpy(ents[pos].key, key, table->keylen);
			ents[pos].val = val;
			table->load ++;
			return NULL;
		}

		if (memcmp(key, ents[pos].key, table->keylen) == 0)
		{
			/* This is legal, but should rarely happen. */
			return ents[pos].val;
		}

		pos = (pos + 1) % size;
	}
}

/* Entered with the lock taken, held throughout and at exit, UNLESS the lock
 * is the alloc lock in which case it may be momentarily dropped.
 *
 * Returns 1 when lock was dropped (and thus atomicity of the operation broken),
 * 0 when the atomicity was upheld.
 *
 * Note that the hash table can only grow: any request to shrink the table
 * is rejected. (You can *drop* a hash table, of course, but not through this function.)
 */
static int
fz_resize_hash(fz_context *ctx, fz_hash_table *table, int newsize)
{
	fz_hash_entry *oldents = table->ents;
	fz_hash_entry *newents;
	int oldsize = table->size;
	int oldload = table->load;
	int i;
	int break_atomicity = (table->lock == FZ_LOCK_ALLOC);

	// as size increases with a factor of 2 always, this condition will never fail as we'll have a theoretical worst-case fill ratio of <= 50% vs. newsize:
	ASSERT(newsize > oldload * 10 / 8);

	if (break_atomicity)
		fz_unlock(ctx, table->lock);
	newents = Memento_label(fz_malloc_no_throw(ctx, newsize * sizeof (fz_hash_entry)), "hash_entries");
	if (break_atomicity)
		fz_lock(ctx, table->lock);
	if (table->lock >= 0)
	{
		if (table->size >= newsize)
		{
			/* Someone else fixed it before we could lock! */
			if (break_atomicity)
				fz_unlock(ctx, table->lock);
			fz_free(ctx, newents);
			if (break_atomicity)
				fz_lock(ctx, table->lock);
			return break_atomicity;
		}
	}
	if (newents == NULL)
		fz_throw(ctx, FZ_ERROR_SYSTEM, "hash table resize failed; out of memory (%d entries)", newsize);
	table->ents = newents;
	memset(table->ents, 0, sizeof(fz_hash_entry) * newsize);
	table->size = newsize;
	table->load = 0;

	for (i = 0; i < oldsize; i++)
	{
		if (oldents[i].val)
		{
			do_hash_insert(ctx, table, oldents[i].key, oldents[i].val);
		}
	}

	if (break_atomicity)
		fz_unlock(ctx, table->lock);
	fz_free(ctx, oldents);
	if (break_atomicity)
		fz_lock(ctx, table->lock);

	return break_atomicity;
}

void *
fz_hash_find(fz_context *ctx, fz_hash_table *table, const void *key)
{
	fz_hash_entry *ents = table->ents;
	unsigned size = table->size;
	unsigned pos = hash(key, table->keylen) % size;

	if (table->lock >= 0)
		fz_assert_lock_held(ctx, table->lock);

	while (1)
	{
		if (!ents[pos].val)
			return NULL;

		if (memcmp(key, ents[pos].key, table->keylen) == 0)
			return ents[pos].val;

		pos = (pos + 1) % size;
	}
}

int
fz_hash_ensure_space(fz_context* ctx, fz_hash_table* table, int count)
{
	// for fun, a tiny optimization that a compiler cannot do: 
	// see the 'P.S. nitpickers' note in `fz_hash_insert()` below.
	//
	// the heuristic for triggering hash table growth action was:
	//   fill ratio (`= load + count`) > 80%
	//
	// div by 10 takes clockcycles; division by a power-of-2 is a fast
	// bitshift which the compiler will surely produce.
	//
	// 8/10 (80%) ~ 13/16 (81.25%)
	//
	// so we'll go with that AND make the `>` check a `>=` check to
	// 'compensate' for the extra 1.25%. ;-)
	//
	// Old:
	//   if (table->load + count > (table->size * 8) / 10)
	// New:
	if (table->load + count >= (table->size * 13) / 16)
	{
		int newsize = table->size * 2;

		while (table->load + count >= (newsize * 13) / 16)
			newsize *= 2;

		// drops lock temporarily.
		return fz_resize_hash(ctx, table, newsize);
	}
	return 0;
}

void *
fz_hash_insert(fz_context *ctx, fz_hash_table *table, const void *key, void *val)
{
	// `fz_hash_ensure_space()` MAY break atomicity (drop + re-acquire lock)
	// when it has determined the hash table needs to grow!
	//
	// To prevent a race condition at EXTREME LOADS where 
	// 1. a competing thread has done the resize work already while we were 
	//    in the dropped-lock zone in `fz_hash_ensure_space()`, which is
	//    signalled to us by `fz_hash_ensure_space()` returning 1, AND
	// 2. one or more competing threads have already used up all that freshly
	//    allocated space in the same time slot,
	// THEN we would face a situation where there would NBOT be sufficient 
	// space in the hash table to insert our item.
	//
	// HENCE we MUST ensure that the 'enough-space-available' check PLUS the
	// insert-item action itself are GUARANTEED ATOMIC.
	//
	// We therefor loop `fz_hash_ensure_space()` until it reports 0, i.e.
	// the active critical section has not been broken.
	//
	// Yes, this is an *almost* theoretical race as the chances of it occurring
	// are close to *infinitely small*, but I've seen stuff like that happen
	// like it was clockwork in complex, heavily loaded systems. Writing
	// thread-safe software is all about GUARANTEEING thread-safety, so we better
	// make sure! The usual run-time cost is utterly negligible (func-call + fast integer
	// calculus & comparison). 
	//
	// P.S. to throw nitpickers a bone, we've optimized the growth check calculus by getting
	// rid of the division by 10, which even on modern hardware is relatively costly.
	while(0 != fz_hash_ensure_space(ctx, table, 1))
		;
	return do_hash_insert(ctx, table, key, val);
}

static void
do_removal(fz_context *ctx, fz_hash_table *table, unsigned hole)
{
	fz_hash_entry *ents = table->ents;
	unsigned size = table->size;
	unsigned look, code;

	if (table->lock >= 0)
		fz_assert_lock_held(ctx, table->lock);

	ents[hole].val = NULL;

	look = hole + 1;
	if (look == size)
		look = 0;

	while (ents[look].val)
	{
		code = hash(ents[look].key, table->keylen) % size;
		if ((code <= hole && hole < look) ||
			(look < code && code <= hole) ||
			(hole < look && look < code))
		{
			ents[hole] = ents[look];
			ents[look].val = NULL;
			hole = look;
		}

		look++;
		if (look == size)
			look = 0;
	}

	table->load --;
}

void
fz_hash_remove(fz_context *ctx, fz_hash_table *table, const void *key)
{
	fz_hash_entry *ents = table->ents;
	unsigned size = table->size;
	unsigned pos = hash(key, table->keylen) % size;

	if (table->lock >= 0)
		fz_assert_lock_held(ctx, table->lock);

	while (1)
	{
		if (!ents[pos].val)
		{
			fz_warn(ctx, "assert: remove non-existent hash entry");
			return;
		}

		if (memcmp(key, ents[pos].key, table->keylen) == 0)
		{
			do_removal(ctx, table, pos);
			return;
		}

		pos++;
		if (pos == size)
			pos = 0;
	}
}

void
fz_hash_for_each(fz_context *ctx, fz_hash_table *table, void *state, fz_hash_table_for_each_fn *callback)
{
	int i;
	for (i = 0; i < table->size; ++i)
		if (table->ents[i].val)
			callback(ctx, state, table->ents[i].key, table->keylen, table->ents[i].val);
}

void
fz_hash_filter(fz_context *ctx, fz_hash_table *table, void *state, fz_hash_table_filter_fn *callback)
{
	int i;
restart:
	for (i = 0; i < table->size; ++i)
	{
		if (table->ents[i].val)
		{
			if (callback(ctx, state, table->ents[i].key, table->keylen, table->ents[i].val))
			{
				do_removal(ctx, table, i);
				goto restart; /* we may have moved some slots around, so just restart the scan */
			}
		}
	}
}
