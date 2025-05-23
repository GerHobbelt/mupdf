/*
 * Type and type-support definitions
 *
 * Copyright (C) 2011-2024, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#if !defined( _LIBEVT_TYPES_H )
#define _LIBEVT_TYPES_H

#include "common.h"

#if defined( HAVE_LIMITS_H ) || defined( WINAPI )
#include <limits.h>
#endif

/* Make sure to include inttypes.h or stdint.h before
 * PRI definitions are defined
 */
#include <libevt/types.h>

#if defined( WINAPI ) && ( defined( _UNICODE ) || defined( UNICODE ) )

#define HAVE_WIDE_SYSTEM_CHARACTER 1

/* The system character type is wide
 * A system string contains either UTF-16 or UTF-32
 */
typedef wchar_t system_character_t;
typedef wint_t system_integer_t;

#define PRIc_SYSTEM "lc"
#define PRIs_SYSTEM "ls"

#else

/* The system character type is narrow
 * A system string contains either UTF-8 or extended ASCII with a codepage
 */
typedef char system_character_t;
typedef int system_integer_t;

#define PRIc_SYSTEM "c"
#define PRIs_SYSTEM "s"

#endif /* defined( WINAPI ) && ( defined( _UNICODE ) || defined( UNICODE ) ) */

/* Fallback for systems without PRI definitions
 * Do not define when pyconfig.h has been included via python.h
 */
#if !defined( HAVE_PYCONFIG_H )

#if !defined( PRId8 )
#define PRId8 "d"
#endif

#if !defined( PRId16 )
#define PRId16 "d"
#endif

#if !defined( PRId32 )
#if defined( WINAPI )
#define PRId32 "I32d"

#else
#define PRId32 "d"

#endif
#endif /* !defined( PRId32 ) */

#if !defined( PRId64 )
#if defined( WINAPI )
#define PRId64 "I64d"

#elif __WORDSIZE == 64
#define PRId64 "ld"

#else
#define PRId64 "lld"

#endif
#endif /* !defined( PRId64 ) */

#if !defined( PRIi8 )
#define PRIi8 "i"
#endif

#if !defined( PRIi16 )
#define PRIi16 "i"
#endif

#if !defined( PRIi32 )
#if defined( WINAPI )
#define PRIi32 "I32i"

#else
#define PRIi32 "i"

#endif
#endif /* !defined( PRIi32 ) */

#if !defined( PRIi64 )
#if defined( WINAPI )
#define PRIi64 "I64i"

#elif __WORDSIZE == 64
#define PRIi64 "li"

#else
#define PRIi64 "lli"

#endif
#endif /* !defined( PRIi64 ) */

#if !defined( PRIu8 )
#define PRIu8 "u"
#endif

#if !defined( PRIu16 )
#define PRIu16 "u"
#endif

#if !defined( PRIu32 )
#if defined( WINAPI )
#define PRIu32 "I32u"

#else
#define PRIu32 "u"

#endif
#endif /* !defined( PRIu32 ) */

#if !defined( PRIu64 )
#if defined( WINAPI )
#define PRIu64 "I64u"

#elif __WORDSIZE == 64
#define PRIu64 "lu"

#else
#define PRIu64 "llu"

#endif
#endif /* !defined( PRIu64 ) */

#if !defined( PRIo8 )
#define PRIo8 "o"
#endif

#if !defined( PRIo16 )
#define PRIo16 "o"
#endif

#if !defined( PRIo32 )
#if defined( WINAPI )
#define PRIo32 "I32o"

#else
#define PRIo32 "o"

#endif
#endif /* !defined( PRIo32 ) */

#if !defined( PRIo64 )
#if defined( WINAPI )
#define PRIo64 "I64o"

#elif __WORDSIZE == 64
#define PRIo64 "lo"

#else
#define PRIo64 "llo"

#endif
#endif /* !defined( PRIo64 ) */

#if !defined( PRIx8 )
#define PRIx8 "x"
#endif

#if !defined( PRIx16 )
#define PRIx16 "x"
#endif

#if !defined( PRIx32 )
#if defined( WINAPI )
#define PRIx32 "I32x"

#else
#define PRIx32 "x"

#endif
#endif /* !defined( PRIx32 ) */

#if !defined( PRIx64 )
#if defined( WINAPI )
#define PRIx64 "I64x"

#elif __WORDSIZE == 64
#define PRIx64 "lx"

#else
#define PRIx64 "llx"

#endif
#endif /* !defined( PRIx64 ) */

#endif /* !defined( HAVE_PYCONFIG_H ) */

/* Fallback for systems without printf %jd definition
 */
#if defined( HAVE_PRINTF_JD )
#define PRIjd	"jd"
#define PRIji	"ji"
#define PRIju	"ju"
#define PRIjx	"jx"

#elif SIZEOF_OFF_T == 8
#define PRIjd	PRId64
#define PRIji	PRIi64
#define PRIju	PRIu64
#define PRIjx	PRIx64

#else
#define PRIjd	PRId32
#define PRIji	PRIi32
#define PRIju	PRIu32
#define PRIjx	PRIx32

#endif /* defined( HAVE_PRINTF_JD ) */

/* Fallback for systems without printf %zd definition
 */
#if defined( HAVE_PRINTF_ZD )
#define PRIzd	"zd"
#define PRIzi	"zi"
#define PRIzu	"zu"
#define PRIzx	"zx"

#elif defined( _MSC_VER )
#define PRIzd	"Id"
#define PRIzi	"Ii"
#define PRIzu	"Iu"
#define PRIzx	"Ix"

#elif SIZEOF_SIZE_T == 8
#define PRIzd	PRId64
#define PRIzi	PRIi64
#define PRIzu	PRIu64
#define PRIzx	PRIx64

#else
#define PRIzd	PRId32
#define PRIzi	PRIi32
#define PRIzu	PRIu32
#define PRIzx	PRIx32

#endif /* defined( HAVE_PRINTF_ZD ) */

/* Fallback for systems without (U)INTx_MAX definitions
 */

/* The maximum signed 8-bit integer is 127 (0x7f)
 */
#if !defined( INT8_MAX )
#define INT8_MAX (0x7f)
#endif

/* The maximum unsigned 8-bit integer is 256 (0xff)
 */
#if !defined( UINT8_MAX )
#define UINT8_MAX (0xff)
#endif

/* The maximum signed 16-bit integer is 32767 (0x7ffff)
 */
#if !defined( INT16_MAX )
#define INT16_MAX (0x7fff)
#endif

/* The maximum unsigned 16-bit integer is 65535 (0xffff)
 */
#if !defined( UINT16_MAX )
#define UINT16_MAX (0xffff)
#endif

/* The maximum signed 32-bit integer is 2147483647 (0x7fffffff)
 */
#if !defined( INT32_MAX )
#define INT32_MAX (0x7fffffffL)
#endif

/* The maximum unsigned 32-bit integer is 4294967295 (0xffffffff)
 */
#if !defined( UINT32_MAX )
#define UINT32_MAX (0xffffffffUL)
#endif

/* The minimum signed 64-bit integer is -9223372036854775808 (0x8000000000000000)
 */
#if !defined( INT64_MIN )
#if defined( __BORLANDC__ ) && ( __BORLANDC__ < 0x0560 )
#define INT64_MIN (0x8000000000000000UL)
#else
#define INT64_MIN (0x8000000000000000ULL)
#endif
#endif /* !defined( INT64_MIN ) */

/* The maximum signed 64-bit integer is 9223372036854775807 (0x7fffffffffffffff)
 */
#if !defined( INT64_MAX )
#if defined( __BORLANDC__ ) && ( __BORLANDC__ < 0x0560 )
#define INT64_MAX (0x7fffffffffffffffUL)
#else
#define INT64_MAX (0x7fffffffffffffffULL)
#endif
#endif /* !defined( INT64_MAX ) */

/* The maximum unsigned 64-bit integer is 18446744073709551615 (0xffffffffffffffff)
 */
#if !defined( UINT64_MAX )
#if defined( __BORLANDC__ ) && ( __BORLANDC__ < 0x0560 )
#define UINT64_MAX (0xffffffffffffffffUL)
#else
#define UINT64_MAX (0xffffffffffffffffULL)
#endif
#endif /* !defined( UINT64_MAX ) */

/* The maximum signed integer
 */
#if !defined( INT_MAX )
#define INT_MAX INT32_MAX
#endif

/* The maximum unsigned integer
 */
#if !defined( UINT_MAX )
#define UINT_MAX UINT32_MAX
#endif

/* The maximum signed long integer
 */
#if !defined( LONG_MAX )
#define LONG_MAX INT32_MAX
#endif

/* The maximum unsigned long integer
 */
#if !defined( ULONG_MAX )
#define ULONG_MAX UINT32_MAX
#endif

/* The maximum signed size type is platform dependent
 */
#if !defined( SSIZE_MAX )

#if defined( WINAPI )

#if defined( _WIN64 )
#define SSIZE_MAX INT64_MAX
#else
#define SSIZE_MAX INT32_MAX
#endif

#else

#if __WORDSIZE == 64
#define SSIZE_MAX INT64_MAX
#else
#define SSIZE_MAX INT32_MAX
#endif

#endif /* WINAPI */

#endif /* SSIZE_MAX */

#endif /* !defined( _TYPES_H ) */

