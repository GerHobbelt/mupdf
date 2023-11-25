// OpenCV CPU baseline features
//
// See also: https://stackoverflow.com/questions/28185844/do-all-64-bit-intel-architectures-support-ssse3-sse4-1-sse4-2-instructions
//
// in 2015AD this is the state of affairs:
// A x64 native (AMD64 or Intel 64) processor is only mandated to support SSE and SSE2.
//
// [...] the support for SSE3 for PC gamers per the Valve survey is now 100%. 
// SSSE3, SSE4.1, and SSE4.2 are all in the 97-98% range. 
// AVX is around 92%--the current generation gaming consoles from Sony & Microsoft 
// support up through AVX. The primary value of AVX is that you can use the /arch:AVX 
// switch which allows all SSE code-generation to use the 3-operand VEX prefix which 
// makes register scheduling more efficient.
// AVX2 is approaching 75% which is really good, but still potentially a blocker 
// for a game to rely on without a fallback path.


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
#endif

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

#if defined ( __SSE4_1__ ) || \
	defined ( __SSE4_2__ ) || \
	defined ( __AVX__ ) || \
	defined (__AVX2__) 
#ifndef __SSE4_1__
#define __SSE4_1__ 1
#endif
#define CV_CPU_COMPILE_SSE4_1 1
#define CV_CPU_BASELINE_COMPILE_SSE4_1 1
#endif

#if defined ( __SSE4_2__ ) || \
	defined ( __AVX__ ) || \
	defined (__AVX2__) 
#ifndef __SSE4_2__
#define __SSE4_2__ 1
#endif
#define CV_CPU_COMPILE_SSE4_2 1
#define CV_CPU_BASELINE_COMPILE_SSE4_2 1
#endif

#if defined ( __AVX__ ) || \
	defined (__AVX2__) 
#ifndef __AVX__
#define __AVX__ 1
#endif
#define CV_CPU_COMPILE_AVX 1
#define CV_CPU_BASELINE_COMPILE_AVX 1
#endif

#if defined (__AVX2__) 
#define CV_CPU_COMPILE_AVX2 1
#define CV_CPU_BASELINE_COMPILE_AVX2 1
#endif

// Special: wheh AVX512 is enabled as default for the entire compilate, 
// we assume this is a build for the very latest hardware: 
// we assume support for the hole caboodle then!

#if defined (__AVX512F__) 
#define  CV_CPU_COMPILE_POPCNT 1
#define  CV_CPU_COMPILE_FP16 1
#define  CV_CPU_COMPILE_AVX_512F 1
#define  CV_CPU_COMPILE_AVX512_COMMON 1
#define  CV_CPU_COMPILE_AVX512_KNL 1
#define  CV_CPU_COMPILE_AVX512_KNM 1
#define  CV_CPU_COMPILE_AVX512_SKX 1
#define  CV_CPU_COMPILE_AVX512_CNL 1
#define  CV_CPU_COMPILE_AVX512_CLX 1
#define  CV_CPU_COMPILE_AVX512_ICL 1
#endif





// TODO: do this detection at run-time; this is a crap solution for that reporting issue (see getCPUFeaturesLine() for its single use).
#define CV_CPU_BASELINE_FEATURES 0 \
    /* , CV_CPU_SSE \ */
    /* ... */

#define CV_CPU_DISPATCH_FEATURES 0 \
    /* , CV_CPU_SSE4_1 \ */
    /* ... */
