//
// assert.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the assert macro and related functionality.
//

// NOTE: no include guard

#if defined(_MSC_VER) && _MSC_VER >= 1929
# include <corecrt.h>
#endif


#ifndef _UCRT_DISABLE_CLANG_WARNINGS
#define _UCRT_DISABLE_CLANG_WARNINGS
#endif 

#ifndef _UCRT_RESTORE_CLANG_WARNINGS
#define _UCRT_RESTORE_CLANG_WARNINGS
#endif 


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Warning Suppression
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// C4412: function signature contains type '_locale_t';
//        C++ objects are unsafe to pass between pure code and mixed or native. (/Wall)
#ifndef _UCRT_DISABLED_WARNING_4412
#ifdef _M_CEE_PURE
#define _UCRT_DISABLED_WARNING_4412 4412
#else
#define _UCRT_DISABLED_WARNING_4412
#endif
#endif

// Use _UCRT_EXTRA_DISABLED_WARNINGS to add additional warning suppressions to UCRT headers.
#ifndef _UCRT_EXTRA_DISABLED_WARNINGS
#define _UCRT_EXTRA_DISABLED_WARNINGS
#endif

// C4324: structure was padded due to __declspec(align()) (/W4)
// C4514: unreferenced inline function has been removed (/Wall)
// C4574: 'MACRO' is defined to be '0': did you mean to use '#if MACRO'? (/Wall)
// C4668: '__cplusplus' is not defined as a preprocessor macro (/Wall)
// C4710: function not inlined (/Wall)
// C4793: 'function' is compiled as native code (/Wall and /W1 under /clr:pure)
// C4820: padding after data member (/Wall)
// C4995: name was marked #pragma deprecated
// C4996: __declspec(deprecated)
// C28719: Banned API, use a more robust and secure replacement.
// C28726: Banned or deprecated API, use a more robust and secure replacement.
// C28727: Banned API.
#ifndef _UCRT_DISABLED_WARNINGS
#define _UCRT_DISABLED_WARNINGS 4324 _UCRT_DISABLED_WARNING_4412 4514 4574 4710 4793 4820 4995 4996 28719 28726 28727 _UCRT_EXTRA_DISABLED_WARNINGS
#endif

#define _CRT_PACKING 8


// All C headers have a common prologue and epilogue, to enclose the header in
// an extern "C" declaration when the header is #included in a C++ translation
// unit and to push/pop the packing.
#if defined __cplusplus

#if defined(_MSC_VER) 

#define _CRT_BEGIN_C_HEADER										\
        __pragma(pack(push, _CRT_PACKING))						\
        extern "C" {

#define _CRT_END_C_HEADER										\
        }														\
        __pragma(pack(pop))

#else

#define _CRT_BEGIN_C_HEADER										\
        extern "C" {

#define _CRT_END_C_HEADER										\
        }														

#endif

#else    // __cplusplus

#if defined(_MSC_VER) 

#define _CRT_BEGIN_C_HEADER										\
        __pragma(pack(push, _CRT_PACKING))

#define _CRT_END_C_HEADER										\
        __pragma(pack(pop))

#else

#define _CRT_BEGIN_C_HEADER										

#define _CRT_END_C_HEADER										

#endif

#endif



#pragma warning(push)
#pragma warning(disable: _UCRT_DISABLED_WARNINGS)
_UCRT_DISABLE_CLANG_WARNINGS


#undef assert
#undef assert_and_continue


_CRT_BEGIN_C_HEADER

void __cdecl fz_sysassert(const char * _Message, const char * _File, const char * _Function, unsigned int _Line);
void __cdecl fz_sysassert_and_continue(const char * _Message, const char * _File, const char * _Function, unsigned int _Line);

_CRT_END_C_HEADER


#if defined __cplusplus && !defined __FZ_VALIDATE_BOOL_FOR_ASSERT_DEFINED__
#define                             __FZ_VALIDATE_BOOL_FOR_ASSERT_DEFINED__    1

#if !defined FZ_DO_NOT_VALIDATE_BOOL_FOR_ASSERT

namespace fz_assert_impl
{
	static constexpr inline /* explicit */ bool validateBool4Assert(bool b) { return b; }
	static constexpr inline bool validateBool4Assert(const void* b) { return !!b; }
	static constexpr inline /* explicit */ bool validateBool4Assert(int b) { return !!b; }
	static inline bool validateBool4Assert(unsigned int) = delete;      // catch unintended bool conversions, e.g. HRESULT
	static inline bool validateBool4Assert(float) = delete; 
	static inline bool validateBool4Assert(double) = delete; 
	static inline bool validateBool4Assert(long int) = delete; 
	static inline bool validateBool4Assert(unsigned long int) = delete; 
	static inline bool validateBool4Assert(long long int) = delete; 
	static inline bool validateBool4Assert(unsigned long long int) = delete; 
}

#ifndef NDEBUG

#if 0

#define assert(expression) (void)(                                                      \
            (!!::fz_assert_impl::validateBool4Assert(expression)) ||                    \
            (fz_sysassert(#expression, __FILE__, __FUNCTION__, __LINE__), 0)			\
        )

#endif

#define assert(expression) (void)(                                                      \
            (!!::fz_assert_impl::validateBool4Assert(expression)) ||                    \
            (fz_sysassert(#expression, __FILE__, __func__, __LINE__), 0)				\
        )

#define assert_and_continue(expression) (void)(                                         \
            (!!::fz_assert_impl::validateBool4Assert(expression)) ||                    \
            (fz_sysassert_and_continue(#expression, __FILE__, __func__, __LINE__), 0)	\
        )

#else 

#define assert(expression) (void)(0)

#define assert_and_continue(expression) (void)(expression)

#endif  // !NDEBUG

#endif   // FZ_DO_NOT_VALIDATE_BOOL_FOR_ASSERT

#endif   // __cplusplus && !__FZ_VALIDATE_BOOL_FOR_ASSERT_DEFINED__


#ifndef assert

_CRT_BEGIN_C_HEADER

#ifndef NDEBUG

#if 0

#define assert(expression) (void)(                                                      \
            (!!(expression)) ||                                                         \
            (fz_sysassert(#expression, __FILE__, __FUNCTION__, __LINE__), 0)			\
        )

#endif

#define assert(expression) (void)(                                                      \
            (!!(expression)) ||                                                         \
            (fz_sysassert(#expression, __FILE__, __func__, __LINE__), 0)				\
        )

#define assert_and_continue(expression) (void)(                                         \
            (!!(expression)) ||                                                         \
            (fz_sysassert_and_continue(#expression, __FILE__, __func__, __LINE__), 0)	\
        )

#else

#define assert(expression) (void)(0)

#define assert_and_continue(expression) (void)(expression)

#endif   // NDEBUG

_CRT_END_C_HEADER

#endif  // !assert


_UCRT_RESTORE_CLANG_WARNINGS
#pragma warning(pop) // _UCRT_DISABLED_WARNINGS

