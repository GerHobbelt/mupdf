
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


#endif


