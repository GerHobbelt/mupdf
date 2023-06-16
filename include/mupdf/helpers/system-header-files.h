//
// (c) Copyright 2022, Ger Hobbelt
// 
// The purpose of this header file is to include all obnoxious system header files
// (I'm looking at you, Microsoft!) which *blargh* a zillion useless warnings at
// maximum/high compiler warning levels, obscuring the compiler warnings we can
// actually DO something about!
//
// windows.h is the biggest DoS attacker in this regard, but we'll need at least
// winsock2.h to go with that one to prevent bad stuff happening down the road
// in bigger applications: the dread winsock.h vs. winsock2.h compile-time collisions.
//
// We've centralized all this cruft (and the work-arounds to make them shut up
// without losing our ability to **sensibly** use the highest compiler warning
// level settings) in this here header file, which is intended to be used by anybody
// who's interested in achieving this goal (of system files not obscuring and
// carpet-bombing your compiler diagnostics output).
//

#pragma once

#ifndef MUPDF_HELPER_SYSTEM_HEADER_FILES_H
#define MUPDF_HELPER_SYSTEM_HEADER_FILES_H

#pragma warning(disable: 4464)  // warning C4464: relative include path contains '..'
#pragma warning(disable: 4820)  // warning C4820: '<unnamed-tag>': '8' bytes padding added after data member 'top', ...
#pragma warning(disable: 5045)  // warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#pragma warning(disable: 4061)  // warning C4061: enumerator 'PDF_ANNOT_WATERMARK' in switch of enum 'pdf_annot_type' is not explicitly handled by a case label
                                // ^^^ the latter makes itself bloody useless as the switch has a `default` section which suffices nicely, while this noise makes missing a switch without a default hard/impossible.
#pragma warning(disable: 4514)  // warning C4514: 'fz_is_pow2': unreferenced inline function has been removed (compiling source file ../../source/dll/dllmain.cpp)
#pragma warning(disable: 4242)  // warning C4242: '=': conversion from 'int' to 'unsigned char', possible loss of data, etc.etc.
#pragma warning(disable: 4365)  // warning C4365: 'return': conversion from 'int' to 'unsigned int', signed/unsigned mismatch
#pragma warning(disable: 5220)  // warning C5220: '<unnamed-tag>::abort': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial (compiling source file ../../source/dll/pch.cpp)
#pragma warning(disable: 4611)  // warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
// 4514;4820;5045;4464;4180;4244;4018;4267;5105;4100;4127;4206;
#pragma warning(disable: 4100)  // warning C4100: 'dn': unreferenced formal parameter
#pragma warning(disable: 4127)  // warning C4127: conditional expression is constant
// warning C4133: 'function': incompatible types - from 'void (__cdecl *)(fz_context *,fz_output *,const void *,size_t)' to 'fz_output_write_fn (__cdecl *)'
#pragma warning(disable: 4267)  // warning C4267: 'function': conversion from 'size_t' to 'unsigned long', possible loss of data
#pragma warning(disable: 4324)  // warning C4324: '<unnamed-tag>': structure was padded due to alignment specifier
#pragma warning(disable: 4018)  // warning C4018: '<': signed/unsigned mismatch
#pragma warning(disable: 4388)  // warning C4388: '>=': signed/unsigned mismatch

#pragma warning(disable: 5219)  // warning C5219: implicit conversion from 'int' to 'float', possible loss of data

#pragma warning(push)

#pragma warning(disable: 4820)  // warning C4820: '<unnamed-tag>': '8' bytes padding added after data member 'top', ...


// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
// 
// We're doing Unicode builds

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
// according to MS docs, _WIN32 is the onee that should always be defined; making sure it is:
#ifndef _WIN32
#define _WIN32
#endif
#endif


#define _WIN32_WINNT_WIN10_TH2                     0x0A01
#define _WIN32_WINNT_WIN10_RS1                     0x0A02
#define _WIN32_WINNT_WIN10_RS2                     0x0A03
#define _WIN32_WINNT_WIN10_RS3                     0x0A04
#define _WIN32_WINNT_WIN10_RS4                     0x0A05
#define _WIN32_WINNT_WIN10_RS5                     0x0A06
#define _WIN32_WINNT_WIN10_19H1                    0x0A07
#define _WIN32_WINNT_WIN10_VB                      0x0A08
#define _WIN32_WINNT_WIN10_MN                      0x0A09
#define _WIN32_WINNT_WIN10_FE                      0x0A0A
#define _WIN32_WINNT_WIN10_CO                      0x0A0B


#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(_MSC_VER)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif

#ifndef _CRTDBG_MAP_ALLOC_NEW
#define _CRTDBG_MAP_ALLOC_NEW
#endif

#include <crtdbg.h>

#if defined(_DEBUG) && defined(_CRTDBG_REPORT_FLAG)

#ifndef NEW_CBDBG // new operator: debug clientblock:
#define NEW_CBDBG new (_CLIENT_BLOCK, __FILE__, __LINE__)
#undef new
#define new NEW_CBDBG
#endif
#endif
#endif

/*
example:

#if defined _DEBUG && defined _CRTDBG_MAP_ALLOC
    #pragma push_macro("_strdup")
    #undef _strdup
#endif

_Check_return_
_ACRTIMP _CRTALLOCATOR char* __cdecl _strdup(
    _In_opt_z_ char const* _Source
    );

#if defined _DEBUG && defined _CRTDBG_MAP_ALLOC
    #pragma pop_macro("_strdup")
#endif

*/

#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

// https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
// fix for missing WINNT defines:
#include <sdkddkver.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	_WIN32_WINNT_WIN10
#endif

// MSWin fix: make sure we include winsock2.h *before* windows.h implicitly includes the antique winsock.h and causes all kinds of weird errors at compile time:
#include <winsock2.h>

#include <winerror.h>
#include <windows.h>
#include <intrin.h>
#include <tchar.h>      // used for ANSI v.s. UNICODE abstraction
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <wingdi.h>
#include <commdlg.h>
#include <shellapi.h>
#include <winbase.h>
#include <winapifamily.h>
#include <time.h>
#include <sys/stat.h>
#else
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#    include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
#ifdef TIME_WITH_SYS_TIME
#    include <sys/time.h>
#    include <time.h>
#elif defined(HAVE_SYS_TIME_H)
#    include <sys/time.h>
#else
#    include <time.h>
#endif
#ifdef HAVE_DIRENT_H
#    include <dirent.h>
#endif

#endif

#include <string.h>
#include <limits.h>
#include <stdlib.h>

// override assert.h:
#include "mupdf/assertions.h"

#if defined(_MSC_VER) && !defined(stat)
#define stat _stat
#endif

#undef min
#undef max

#pragma warning(pop)

#endif
