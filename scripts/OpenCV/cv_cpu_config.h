// OpenCV CPU baseline features

#if defined ( __SSE__ ) || \
	defined ( __SSE2__ ) || defined ( __x86_64__ ) || defined(_M_X64) || \
	defined ( __SSE3__ ) || \
	defined ( __SSE4_1__ ) || \
	defined ( __SSE4_2__ ) || \
	defined ( __AVX__ ) || \
	defined (__AVX2__)
#ifndef __SSE__
#define __SSE__ 1
#endif
#define CV_CPU_COMPILE_SSE 1
#define CV_CPU_BASELINE_COMPILE_SSE 1
#endif

#if defined ( __SSE2__ ) || defined ( __x86_64__ ) || defined(_M_X64) || \
	defined ( __SSE3__ ) || \
	defined ( __SSE4_1__ ) || \
	defined ( __SSE4_2__ ) || \
	defined ( __AVX__ ) || \
	defined (__AVX2__)
#ifndef __SSE2__
#define __SSE2__ 1
#endif
#define CV_CPU_COMPILE_SSE2 1
#define CV_CPU_BASELINE_COMPILE_SSE2 1

#if defined ( __SSE3__ ) || \
	defined ( __SSE4_1__ ) || \
	defined ( __SSE4_2__ ) || \
	defined ( __AVX__ ) || \
	defined (__AVX2__)
#ifndef __SSE3__
#define __SSE3__ 1
#endif
#define CV_CPU_COMPILE_SSE3 1
#define CV_CPU_BASELINE_COMPILE_SSE3 1
#endif

#if defined(__SSE4_1__) || defined(_M_IX86) || defined(_M_X64)
#endif

#if defined(__AVX__) || defined(_M_IX86) || defined(_M_X64)
#endif

#define CV_CPU_BASELINE_FEATURES 0 \
    , CV_CPU_SSE \
    , CV_CPU_SSE2 \
    , CV_CPU_SSE3 \


// OpenCV supported CPU dispatched features

#define CV_CPU_DISPATCH_COMPILE_SSE4_1 1
#define CV_CPU_DISPATCH_COMPILE_SSE4_2 1
#define CV_CPU_DISPATCH_COMPILE_FP16 1
#define CV_CPU_DISPATCH_COMPILE_AVX 1
#define CV_CPU_DISPATCH_COMPILE_AVX2 1
#define CV_CPU_DISPATCH_COMPILE_AVX512_SKX 1


#define CV_CPU_DISPATCH_FEATURES 0 \
    , CV_CPU_SSE4_1 \
    , CV_CPU_SSE4_2 \
    , CV_CPU_FP16 \
    , CV_CPU_AVX \
    , CV_CPU_AVX2 \
    , CV_CPU_AVX512_SKX \

