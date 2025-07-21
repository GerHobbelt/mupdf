// -----------------------------------------------------------------------------------------------------------------------------------------
// WARNING: No include guard. This was done intentionally. GLibc and others 'leak' assert() macro re-definitions, which we aim to override!
// -----------------------------------------------------------------------------------------------------------------------------------------

// The assert() macro + override functionality is now made available via the (extended) libassert library.

#undef assert

#define LIBASSERT_OFFER_SYSTEM_ASSERT_REPLACEMENT    1
#include <libassert/assert.h>
