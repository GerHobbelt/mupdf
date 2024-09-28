
#include "system_override_internal.h"

#if defined(_WIN32)
#include <windows.h>
#endif
#include <intrin.h>		// __debugbreak()
#include <stdint.h>
#include <assert.h>
#include <cassert>


// Mandatory or MSVC will optimize this entire endeavour to Kingdom Come and we won't have our debugger breakpoints at start and end...
#pragma optimize("", off)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#if defined(__MINGW32__) || defined(_WIN32) || defined(WIN64)

extern "C" __declspec(dllimport) void __stdcall DebugBreak();

#if (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0400)) || (defined(_WIN32_WINDOWS) && (_WIN32_WINDOWS > 0x0400))

extern "C" __declspec(dllimport)  int __stdcall IsDebuggerPresent();

void BreakIntoDebugger(void)
{
	if (IsDebuggerPresent())
	{
// MSVC only has a reasonable (= active) define for _CrtDbgBreak in debug build mode...
#if defined(_CrtDbgBreak) && defined(_DEBUG)
		_CrtDbgBreak();
#else
		DebugBreak();
#endif
	}
}

#elif defined(_MSC_VER)

void BreakIntoDebugger(void)
{
// MSVC only has a reasonable (= active) define for _CrtDbgBreak in debug build mode...
#if defined(_CrtDbgBreak) && defined(_DEBUG)
	_CrtDbgBreak();
#endif
	__debugbreak();
}

#else

void BreakIntoDebugger(void)
{
	DebugBreak();
}

#endif   // _WIN32_WINNT || _WIN32_WINDOWS

#elif defined(__has_builtin) && __has_builtin(__builtin_debugtrap)

void BreakIntoDebugger(void)
{
	__builtin_debugtrap();
}

#elif defined(__has_builtin) && __has_builtin(__debugbreak)

void BreakIntoDebugger(void)
{
	__debugbreak();
}

#elif defined(__GNUC__) && (defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) || defined(__i386))
// If we can use inline assembler, do it because this allows us to break
// directly at the location of the failing check instead of breaking inside
// raise() called from it, i.e. one stack frame below.

void BreakIntoDebugger(void)
{
	asm volatile ("int $3") /* NOLINT */;
}

#else // Fall back to the generic ways.

#include <signal.h>

#if defined(SIGTRAP) 

void BreakIntoDebugger(void)
{
	raise(SIGTRAP);
}

#else

void BreakIntoDebugger(void)
{
	static int s = 0;
	while (s == 0) {
		sleep(1);
	}
}

#endif

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
