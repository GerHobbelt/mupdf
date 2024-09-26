
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


#pragma warning(pop)

#endif



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








#if 0


void my_signal_handler(int signum) {
	::signal(signum, SIG_DFL);

	// Outputs nothing or trash on majority of platforms
	boost::stacktrace::safe_dump_to("./backtrace.dump");

	::raise(SIGABRT);
}
//]

void setup_handlers() {
	//[getting_started_setup_signel_handlers
	::signal(SIGSEGV, &my_signal_handler);
	::signal(SIGABRT, &my_signal_handler);
	//]
}


//[getting_started_terminate_handlers
#include <cstdlib>       // std::abort
#include <exception>     // std::set_terminate
#include <iostream>      // std::cerr

#include <boost/stacktrace.hpp>

void my_terminate_handler() {
	try {
		std::cerr << boost::stacktrace::stacktrace();
	} catch (...) {}
	std::abort();
}
//]

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










// *****************************************************************************
// * This file is part of the FreeFileSync project. It is distributed under    *
// * GNU General Public License: https://www.gnu.org/licenses/gpl-3.0          *
// * Copyright (C) Zenju (zenju AT freefilesync DOT org) - All Rights Reserved *
// *****************************************************************************

#ifndef SYS_ERROR_H_3284791347018951324534
#define SYS_ERROR_H_3284791347018951324534

#include "scope_guard.h" //
#include "i18n.h"        //not used by this header, but the "rest of the world" needs it!
#include "zstring.h"     //
#include "extra_log.h"   //

#include <glib.h>
#include <cerrno>


namespace zen
{
	//evaluate GetLastError()/errno and assemble specific error message
	using ErrorCode = int;

	ErrorCode getLastError();

	std::wstring formatSystemError(const std::string& functionName, const std::wstring& errorCode, const std::wstring& errorMsg);
	std::wstring formatSystemError(const std::string& functionName, ErrorCode ec);
	std::wstring formatGlibError(const std::string& functionName, GError* error);


	//A low-level exception class giving (non-translated) detail information only - same conceptional level like "GetLastError()"!
	class SysError
	{
	public:
		explicit SysError(const std::wstring& msg): msg_(msg) {}
		const std::wstring& toString() const { return msg_; }

	private:
		std::wstring msg_;
	};

#define DEFINE_NEW_SYS_ERROR(X) struct X : public zen::SysError { X(const std::wstring& msg) : SysError(msg) {} };



	//better leave it as a macro (see comment in file_error.h)
#define THROW_LAST_SYS_ERROR(functionName)                           \
    do { const ErrorCode ecInternal = getLastError(); throw zen::SysError(formatSystemError(functionName, ecInternal)); } while (false)


/* Example: ASSERT_SYSERROR(expr);

	Equivalent to:
		if (!expr)
			throw zen::SysError(L"Assertion failed: \"expr\"");            */
#define ASSERT_SYSERROR(expr) ASSERT_SYSERROR_IMPL(expr, #expr) //throw SysError



			//######################## implementation ########################
	inline
		ErrorCode getLastError()
	{
		return errno; //don't use "::" prefix, errno is a macro!
	}


	std::wstring getSystemErrorDescription(ErrorCode ec); //return empty string on error
	//intentional overload ambiguity to catch usage errors with HRESULT:
	std::wstring getSystemErrorDescription(long long) = delete;




	namespace impl
	{
		inline bool validateBool(bool  b) { return b; }
		inline bool validateBool(void* b) { return b; }
		bool validateBool(int) = delete; //catch unintended bool conversions, e.g. HRESULT
	}
#define ASSERT_SYSERROR_IMPL(expr, exprStr) \
    { if (!zen::impl::validateBool(expr))        \
            throw zen::SysError(L"Assertion failed: \"" L ## exprStr L"\""); }
}

#endif //SYS_ERROR_H_3284791347018951324534

































//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Exit and Abort
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Argument values for exit()
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#if _CRT_FUNCTIONS_REQUIRED
_ACRTIMP __declspec(noreturn) void __cdecl exit(_In_ int _Code);
_ACRTIMP __declspec(noreturn) void __cdecl _exit(_In_ int _Code);
_ACRTIMP __declspec(noreturn) void __cdecl _Exit(_In_ int _Code);
_ACRTIMP __declspec(noreturn) void __cdecl quick_exit(_In_ int _Code);
_ACRTIMP __declspec(noreturn) void __cdecl abort(void);
#endif // _CRT_FUNCTIONS_REQUIRED

// Argument values for _set_abort_behavior().
#define _WRITE_ABORT_MSG  0x1 // debug only, has no effect in release
#define _CALL_REPORTFAULT 0x2

_ACRTIMP unsigned int __cdecl _set_abort_behavior(
	_In_ unsigned int _Flags,
	_In_ unsigned int _Mask
);



#ifndef _CRT_ONEXIT_T_DEFINED
#define _CRT_ONEXIT_T_DEFINED

typedef int (__CRTDECL* _onexit_t)(void);
#ifdef _M_CEE
typedef int (__clrcall* _onexit_m_t)(void);
#endif
#endif

#if defined(_CRT_INTERNAL_NONSTDC_NAMES) && _CRT_INTERNAL_NONSTDC_NAMES
// Non-ANSI name for compatibility
#define onexit_t _onexit_t
#endif



#ifdef _M_CEE
#pragma warning (push)
#pragma warning (disable: 4985) // Attributes not present on previous declaration

_Check_return_ int __clrcall _atexit_m_appdomain(_In_opt_ void (__clrcall* _Function)(void));

_onexit_m_t __clrcall _onexit_m_appdomain(_onexit_m_t _Function);

#ifdef _M_CEE_MIXED
#ifdef __cplusplus
[System::Security::SecurityCritical]
#endif
_Check_return_ int __clrcall _atexit_m(_In_opt_ void (__clrcall* _Function)(void));

_onexit_m_t __clrcall _onexit_m(_onexit_m_t _Function);
#else
#ifdef __cplusplus
[System::Security::SecurityCritical]
#endif
_Check_return_ inline int __clrcall _atexit_m(_In_opt_ void (__clrcall* _Function)(void))
{
	return _atexit_m_appdomain(_Function);
}

inline _onexit_m_t __clrcall _onexit_m(_onexit_t _Function)
{
	return _onexit_m_appdomain(_Function);
}
#endif
#pragma warning (pop)
#endif



#ifdef _M_CEE_PURE
// In pure mode, atexit is the same as atexit_m_appdomain
extern "C++"
{

#ifdef __cplusplus
	[System::Security::SecurityCritical]
#endif
		inline  int __clrcall atexit(void (__clrcall* _Function)(void))
	{
		return _atexit_m_appdomain(_Function);
	}

	inline _onexit_t __clrcall _onexit(_onexit_t _Function)
	{
		return _onexit_m_appdomain(_Function);
	}

} // extern "C++"
#else
int       __cdecl atexit(void (__cdecl*)(void));
_onexit_t __cdecl _onexit(_In_opt_ _onexit_t _Func);
#endif

int __cdecl at_quick_exit(void (__cdecl*)(void));



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Global State (errno, global handlers, etc.)
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef _M_CEE_PURE
// a purecall handler procedure. Never returns normally
typedef void (__cdecl* _purecall_handler)(void);

// Invalid parameter handler function pointer type
typedef void (__cdecl* _invalid_parameter_handler)(
	wchar_t const*,
	wchar_t const*,
	wchar_t const*,
	unsigned int,
	uintptr_t
	);

// Establishes a purecall handler
_VCRTIMP _purecall_handler __cdecl _set_purecall_handler(
	_In_opt_ _purecall_handler _Handler
);

_VCRTIMP _purecall_handler __cdecl _get_purecall_handler(void);

// Establishes an invalid parameter handler
_ACRTIMP _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(
	_In_opt_ _invalid_parameter_handler _Handler
);

_ACRTIMP _invalid_parameter_handler __cdecl _get_invalid_parameter_handler(void);

_ACRTIMP _invalid_parameter_handler __cdecl _set_thread_local_invalid_parameter_handler(
	_In_opt_ _invalid_parameter_handler _Handler
);

_ACRTIMP _invalid_parameter_handler __cdecl _get_thread_local_invalid_parameter_handler(void);
#endif


#if defined __cplusplus && defined _M_CEE_PURE
extern "C++"
{
	typedef void (__clrcall* _purecall_handler)(void);
	typedef _purecall_handler _purecall_handler_m;

	_MRTIMP _purecall_handler __cdecl _set_purecall_handler(
		_In_opt_ _purecall_handler _Handler
	);
} // extern "C++"
#endif



  // Argument values for _set_error_mode().
#define _OUT_TO_DEFAULT 0
#define _OUT_TO_STDERR  1
#define _OUT_TO_MSGBOX  2
#define _REPORT_ERRMODE 3

_Check_return_opt_ _ACRTIMP int __cdecl _set_error_mode(_In_ int _Mode);



#if _CRT_FUNCTIONS_REQUIRED
_ACRTIMP int* __cdecl _errno(void);
#define errno (*_errno())

_ACRTIMP errno_t __cdecl _set_errno(_In_ int _Value);
_ACRTIMP errno_t __cdecl _get_errno(_Out_ int* _Value);

_ACRTIMP unsigned long* __cdecl __doserrno(void);
#define _doserrno (*__doserrno())

_ACRTIMP errno_t __cdecl _set_doserrno(_In_ unsigned long _Value);
_ACRTIMP errno_t __cdecl _get_doserrno(_Out_ unsigned long * _Value);

// This is non-const for backwards compatibility; do not modify it.
_ACRTIMP _CRT_INSECURE_DEPRECATE(strerror) char** __cdecl __sys_errlist(void);
#define _sys_errlist (__sys_errlist())

_ACRTIMP _CRT_INSECURE_DEPRECATE(strerror) int * __cdecl __sys_nerr(void);
#define _sys_nerr (*__sys_nerr())

_ACRTIMP void __cdecl perror(_In_opt_z_ char const* _ErrMsg);
#endif // _CRT_FUNCTIONS_REQUIRED



// These point to the executable module name.
_CRT_INSECURE_DEPRECATE_GLOBALS(_get_pgmptr ) _ACRTIMP char**    __cdecl __p__pgmptr (void);
_CRT_INSECURE_DEPRECATE_GLOBALS(_get_wpgmptr) _ACRTIMP wchar_t** __cdecl __p__wpgmptr(void);
_CRT_INSECURE_DEPRECATE_GLOBALS(_get_fmode  ) _ACRTIMP int*      __cdecl __p__fmode  (void);

#ifdef _CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY
_CRT_INSECURE_DEPRECATE_GLOBALS(_get_pgmptr ) extern char*    _pgmptr;
_CRT_INSECURE_DEPRECATE_GLOBALS(_get_wpgmptr) extern wchar_t* _wpgmptr;
#ifndef _CORECRT_BUILD
_CRT_INSECURE_DEPRECATE_GLOBALS(_get_fmode  ) extern int      _fmode;
#endif
#else
#define _pgmptr  (*__p__pgmptr ())
#define _wpgmptr (*__p__wpgmptr())
#define _fmode   (*__p__fmode  ())
#endif

_Success_(return == 0)
_ACRTIMP errno_t __cdecl _get_pgmptr (_Outptr_result_z_ char**    _Value);

_Success_(return == 0)
_ACRTIMP errno_t __cdecl _get_wpgmptr(_Outptr_result_z_ wchar_t** _Value);

_ACRTIMP errno_t __cdecl _set_fmode  (_In_              int       _Mode );

_ACRTIMP errno_t __cdecl _get_fmode  (_Out_             int*      _PMode);




#endif






// and finally a few functions which will be able to call the original runtime library functions we did override...


#if defined(_MSC_VER)


static __declspec(noreturn) void invoke_original_abort(void);
static __declspec(noreturn) void invoke_original_exit(int code);


// now blow away our overrides; as we do that we must re-introduce the original system prototypes, so we better make sure we agree with the current compiler here!

#undef abort
#undef exit

extern "C" _ACRTIMP __declspec(noreturn) void __cdecl abort(void);
extern "C" _ACRTIMP __declspec(noreturn) void __cdecl exit(int _Code);
namespace std {
	_ACRTIMP __declspec(noreturn) void abort();
	_ACRTIMP __declspec(noreturn) void exit(int _Code);
}



// warning C4273: 'exit': inconsistent dll linkage
#pragma warning(push)
#pragma warning(disable: 4273)

/**
* Replacement for the C standard `abort` that returns to the `setjmp` call for
* recoverable errors.
*/
static __declspec(noreturn) void invoke_original_abort(void)
{
	abort();
}

static __declspec(noreturn) void invoke_original_exit(int code)
{
	exit(code);
}


#pragma warning(pop)


#endif


