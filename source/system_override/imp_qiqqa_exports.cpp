
#include "system_override_internal.h"

#if defined(_WIN32)
#include <windows.h>
#endif
#include <intrin.h>		// __debugbreak()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cassert>
#include <atomic>


// Mandatory or MSVC will optimize this entire endeavour to Kingdom Come and we won't have our debugger breakpoints at start and end...
#pragma optimize("", off)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#if defined(_MSC_VER)

#ifndef _CRTEXP
#define _VCRT_DEFINED_CRTIMP
#if defined CRTDLL && defined _CRTBLD
#define _CRTEXP __declspec(dllexport)
#else
#ifdef _DLL
#define _CRTEXP __declspec(dllexport)
#else
#define _CRTEXP
#endif
#endif
#endif

#ifndef _ACRTEXP
#if defined _CRTIMP && !defined _VCRT_DEFINED_CRTIMP
#define _ACRTEXP _CRTEXP
#elif !defined _CORECRT_BUILD && defined _DLL
#define _ACRTEXP __declspec(dllexport)
#else
#define _ACRTEXP
#endif
#endif


#ifndef abort
#error "Expected abort to be redefined to point at our override function(s) below..."
#endif
#ifndef exit
#error "Expected exit to be redefined to point at our override function(s) below..."
#endif


// warning C4273: 'exit': inconsistent dll linkage
#pragma warning(disable: 4273)

_ACRTIMP __declspec(noreturn) void __cdecl abort(void);
_ACRTIMP __declspec(noreturn) void __cdecl exit(_In_ int _Code);




extern "C" __declspec(noreturn) void __cdecl __imp_qiqqa_abort_application(void)
{
	invoke_abort();
}

namespace std {
	__declspec(noreturn) void __imp_qiqqa_abort_application()
	{
		invoke_abort();
	}
}


// qiqqa_exit_application

extern "C" __declspec(noreturn) void __cdecl __imp_qiqqa_exit_application(int _Code)
{
	invoke_exit(_Code);
}


namespace std {
	__declspec(noreturn) void __imp_qiqqa_exit_application(int _Code)
	{
		invoke_exit(_Code);
	}
}











// only when building in 32-bit mode do we need these as well, as the name mangling in object files is just a tad different then! *sigh*

#if defined(_WIN32) && !defined(_WIN64)

// Warning: adding `_ACRTEXP` here will crash/fault the MSVC linker with a very obscure error message:
//
//  2>LINK : the 32-bit linker (C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.43.34808\bin\HostX86\x86\link.exe) failed to do memory mapped file I/O
//           on `Z:\lib\tooling\qiqqa\MuPDF\platform\win32\bin\Release-Unicode-32bit-x86\msvc_sln_cleaner.exp' and is going to restart linking with a 64-bit linker for better throughput
//  2>LINK : restarting link with 64-bit linker `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.43.34808\bin\HostX64\x86\link.exe'
//  2>       Creating library Z:\lib\tooling\qiqqa\MuPDF\platform\win32\bin\Release-Unicode-32bit-x86\msvc_sln_cleaner.lib and
//           object Z:\lib\tooling\qiqqa\MuPDF\platform\win32\bin\Release-Unicode-32bit-x86\msvc_sln_cleaner.exp
//  2>Z:\lib\tooling\qiqqa\MuPDF\platform\win32\bin\Release-Unicode-32bit-x86\msvc_sln_cleaner.exp : fatal error LNK1106: invalid file or disk full: cannot seek to 0x713F054E

extern "C" /* _ACRTEXP */ __declspec(noreturn) void __cdecl _imp__qiqqa_abort_application(void)
{
	invoke_abort();
}

extern "C" /* _ACRTEXP */ __declspec(noreturn) void __cdecl _imp__qiqqa_exit_application(int _Code)
{
	invoke_exit(_Code);
}

#endif   // defined(_WIN32) && !defined(_WIN64)


#endif


