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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

#ifndef MUPDF_FITZ_HASH_H
#define MUPDF_FITZ_HASH_H

#include "mupdf/fitz/system.h"
#include "mupdf/fitz/context.h"
#include "mupdf/fitz/output.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FZ_HASH_TABLE_KEY_LENGTH 48

/**
	Generic hash-table with fixed-length keys.

	The keys and values are NOT reference counted by the hash table.
	Callers are responsible for taking care the reference counts are
	correct. Inserting a duplicate entry will NOT overwrite the old
	value, and will return the old value.

	The drop_val callback function is only used to release values
	when the hash table is destroyed.
*/

typedef struct fz_hash_table fz_hash_table;

/**
	Function type called when a hash table entry is dropped.

	Only used when the entire hash table is dropped.
*/
typedef void (fz_hash_table_drop_fn)(fz_context *ctx, void *val);

/**
	Create a new hash table.

	initialsize: The initial size of the hashtable. The hashtable
	may grow (double in size) if it starts to get crowded (80%
	full).

	keylen: byte length for each key.

	lock: -1 for no lock, otherwise the FZ_LOCK to use to protect
	this table.

	drop_val: Function to use to destroy values on table drop.
*/
fz_hash_table *fz_new_hash_table(fz_context *ctx, int initialsize, int keylen, int lock, fz_hash_table_drop_fn *drop_val);

/**
	Destroy the hash table.

	Values are dropped using the drop function.
*/
void fz_drop_hash_table(fz_context *ctx, fz_hash_table *table);

/**
	Announce and ensure the table has sufficient storage for `count`
	items more.

	This function is useful when you will be inserting items in large
	batches, thus reducing the number of hash table resize operations
	and consequent rehashing (re-inserting) of items already stored
	in the hash file before this call.

	Returns 1 if this call broke atomicity (assuming you use a lock)
	when all of these conditions are TRUE:

	1. you created the hash table with the lock/mutex set to FZ_STORE_ALLOC.
	2. fz_hash_ensure_space() has decided (based on internal heuristics)
	   to resize (grow) the hash table.

	Return 0 when atomicity was NOT broken by this call, i.e. when the
	complete set of conditions above did not apply to this call.
*/
int fz_hash_ensure_space(fz_context* ctx, fz_hash_table* table, int count);

/**
	Search for a matching hash within the table, and return the
	associated value.
*/
void *fz_hash_find(fz_context *ctx, fz_hash_table *table, const void *key);

/**
	Insert a new key/value pair into the hash table.

	If an existing entry with the same key is found, no change is
	made to the hash table, and a pointer to the existing value is
	returned.

	If no existing entry with the same key is found, ownership of
	val passes in, key is copied, and NULL is returned.

	WARNING: when the hash table lock is set to use the FZ_STORE_ALLOC
	mutex, then this operation is *potentially NON-ATOMIC*: the lock
	will be temporarily dropped (and re-taken) when the hash table must
	grow to accomodate the new item. The grow decision is heuristics-based
	(80% fill ratio at the moment) hence you MUST assume it can happen
	with any `fz_hash_insert()` call.
*/
void *fz_hash_insert(fz_context *ctx, fz_hash_table *table, const void *key, void *val);

/**
	Remove the entry for a given key.

	The value is NOT freed, so the caller is expected to take care
	of this.
*/
void fz_hash_remove(fz_context *ctx, fz_hash_table *table, const void *key);

/**
	Callback function called on each key/value pair in the hash
	table, when fz_hash_for_each is run.
*/
typedef void (fz_hash_table_for_each_fn)(fz_context *ctx, void *state, void *key, int keylen, void *val);

/**
	Iterate over the entries in a hash table.
*/
void fz_hash_for_each(fz_context *ctx, fz_hash_table *table, void *state, fz_hash_table_for_each_fn *callback);

/**
	Callback function called on each key/value pair in the hash
	table, when fz_hash_filter is run to remove entries where the
	callback returns true.
*/
typedef int (fz_hash_table_filter_fn)(fz_context *ctx, void *state, void *key, int keylen, void *val);

/**
	Iterate over the entries in a hash table, removing all the ones where callback returns true.
	Does NOT free the value of the entry, so the caller is expected to take care of this.
*/
void fz_hash_filter(fz_context *ctx, fz_hash_table *table, void *state, fz_hash_table_filter_fn *callback);

#ifdef __cplusplus
}
#endif

#endif
