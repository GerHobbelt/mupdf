
#include "system_override_internal.h"

using namespace system_override;



// these two are defined at the very bottom of this source file as for them to work we must undo some override stuff first,and all bets are off after we've done *that*!

//
// structure of the override:
// 
// app: calls exit() (overridden) --> invoke_exit() [*] --> invoke_original_exit() --> exit() (actual system RTL call)
// app: calls abort() (overridden) --> invoke_exit() [*] --> invoke_original_exit() --> exit() (actual system RTL call)
// app: runs main() and *returns* --> SystemOverrideClass object destructor is invoked [*] 
//
// call chain points marked with [*] are the places where you are supposed to "hook into" the exit phase of the application,
// i.e. break into the debugger, etc.
//

static __declspec(noreturn) void invoke_original_abort(void);
static __declspec(noreturn) void invoke_original_exit(int code);

static void BreakIntoDebugger(void);   

// The idea behind this seemingly convoluted way to set the invoke-debugger flag to true or false
// is that this is relatively to *patch* in the final compiled binary in both debug and release builds
// so that debugging facilities can be triggered ad hoc when trouble ensues somewhere.
//
// For the 'easily patched' feature to work, both strings should have the same length: you only want to
// flip a single byte in the hex editor when you want to switch this behaviour.
#define DO_INVOKE_DEBUGGER_MARKER_STRING             "marker:do.invoke.debugger"
#if !defined(NDEBUG)
#define ACTIVE_INVOKE_DEBUGGER_MARKER_STRING         DO_INVOKE_DEBUGGER_MARKER_STRING
#else
#define ACTIVE_INVOKE_DEBUGGER_MARKER_STRING         "marker:No.invoke.debugger"
#endif

static const char *flag_do_invoke_debugger = ACTIVE_INVOKE_DEBUGGER_MARKER_STRING;

namespace system_override {

	/* static */ bool SystemOverrideClass::always_kick_in = false;

	static SystemOverrideClass __override_singular_instance {};

	SystemOverrideClass *SystemOverrideClass::override = &__override_singular_instance;

	SystemOverrideClass::SystemOverrideClass(bool _always_kick_in) {
		always_kick_in = _always_kick_in;
		override = this;
		Starting();
	}

	SystemOverrideClass::~SystemOverrideClass() {
		Ending();
		override = nullptr;
	}

	void SystemOverrideClass::Starting() {
		KickInTheDoor();
		invoked++;
	}
	void SystemOverrideClass::Ending() {
		invoked--;
		KickInTheDoor(invoked >= 0);
	}

	void SystemOverrideClass::KickInTheDoor(bool should_do) {
		if (should_do || always_kick_in) {
			SystemOverrideClass *self = override;

			if (0 == strcmp(flag_do_invoke_debugger, DO_INVOKE_DEBUGGER_MARKER_STRING)) {
				BreakIntoDebugger();
			}
		}
	}

	void SystemOverrideClass::Tickle(void) {
		if (!override) {
			override = &__override_singular_instance;
		}
	}

}

static int __init_system_override(void) {
	system_override::SystemOverrideClass::Tickle();
	return 0;
}
#if 0
static void __init_system_override_ctor(void) __attribute__ ((constructor)) {
	__init_system_override();
}
#endif
static int __override_singular_instance_rv = __init_system_override();

#if 0
#pragma startup __init_system_override 1
#pragma exit    __init_system_override 1
#endif


static __declspec(noreturn) void invoke_abort(void)
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original_abort();
}

static __declspec(noreturn) void invoke_exit(int code)
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original_exit(code);
}



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
#pragma warning(push)
#pragma warning(disable: 4273)

_ACRTIMP __declspec(noreturn) void __cdecl abort(void);;
_ACRTIMP __declspec(noreturn) void __cdecl exit(_In_ int _Code);

// qiqqa_abort_application

extern "C" _ACRTEXP __declspec(noreturn) void __cdecl abort(void)
{
	invoke_abort();
}

namespace std {
	_ACRTEXP __declspec(noreturn) void abort()
	{
		invoke_abort();
	}
}


// qiqqa_exit_application

extern "C" _ACRTEXP __declspec(noreturn) void __cdecl exit(int _Code)
{
	invoke_exit(_Code);
}


namespace std {
	_ACRTEXP __declspec(noreturn) void exit(int _Code)
	{
		invoke_exit(_Code);
	}
}






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



#if defined(_MSC_VER) && 0

void BreakIntoDebugger(void)
{
#if defined(_CrtDbgBreak)
	_CrtDbgBreak();
#endif
	__debugbreak();
}

#elif defined(__MINGW32__) || defined(_WIN32) || defined(WIN64)

extern "C" __declspec(dllimport) void __stdcall DebugBreak();

#if (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0400)) || (defined(_WIN32_WINDOWS) && (_WIN32_WINDOWS > 0x0400))

extern "C" __declspec(dllimport)  int __stdcall IsDebuggerPresent();

void BreakIntoDebugger(void)
{
	if (IsDebuggerPresent())
	{
#if defined(_CrtDbgBreak) && 01
		_CrtDbgBreak();
#else
		DebugBreak();
#endif
	}
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
















// trap.h - written and placed in public domain by Jeffrey Walton.

/// \file trap.h
/// \brief Debugging and diagnostic assertions
/// \details <tt>CRYPTOPP_ASSERT</tt> is the library's debugging and diagnostic
///  assertion. <tt>CRYPTOPP_ASSERT</tt> is enabled by <tt>CRYPTOPP_DEBUG</tt>,
///  <tt>DEBUG</tt> or <tt>_DEBUG</tt>.
/// \details <tt>CRYPTOPP_ASSERT</tt> raises a <tt>SIGTRAP</tt> (Unix) or calls
///  <tt>DebugBreak()</tt> (Windows).
/// \details <tt>CRYPTOPP_ASSERT</tt> is only in effect when the user requests a
///  debug configuration. <tt>NDEBUG</tt> (or failure to define it) does not
///  affect <tt>CRYPTOPP_ASSERT</tt>.
/// \since Crypto++ 5.6.5
/// \sa DebugTrapHandler, <A
///  HREF="http://github.com/weidai11/cryptopp/issues/277">Issue 277</A>,
///  <A HREF="http://seclists.org/oss-sec/2016/q3/520">CVE-2016-7420</A>

#ifndef CRYPTOPP_TRAP_H
#define CRYPTOPP_TRAP_H

#include "config.h"

#if defined(CRYPTOPP_DEBUG)
#  include <iostream>
#  include <sstream>
#  if defined(UNIX_SIGNALS_AVAILABLE)
#    include "ossig.h"
#  elif defined(CRYPTOPP_WIN32_AVAILABLE) && !defined(__CYGWIN__)
extern "C" __declspec(dllimport) void __stdcall DebugBreak();
extern "C" __declspec(dllimport)  int __stdcall IsDebuggerPresent();
#  endif
#endif // CRYPTOPP_DEBUG

// ************** run-time assertion ***************

#if defined(CRYPTOPP_DOXYGEN_PROCESSING)
/// \brief Debugging and diagnostic assertion
/// \details <tt>CRYPTOPP_ASSERT</tt> is the library's debugging and diagnostic
///  assertion. <tt>CRYPTOPP_ASSERT</tt> is enabled by the preprocessor macros
///  <tt>CRYPTOPP_DEBUG</tt>, <tt>DEBUG</tt> or <tt>_DEBUG</tt>.
/// \details <tt>CRYPTOPP_ASSERT</tt> raises a <tt>SIGTRAP</tt> (Unix) or calls
///  <tt>DebugBreak()</tt> (Windows). <tt>CRYPTOPP_ASSERT</tt> is only in effect
///  when the user explicitly requests a debug configuration.
/// \details If you want to ensure <tt>CRYPTOPP_ASSERT</tt> is inert, then <em>do
///  not</em> define <tt>CRYPTOPP_DEBUG</tt>, <tt>DEBUG</tt> or <tt>_DEBUG</tt>.
///  Avoiding the defines means <tt>CRYPTOPP_ASSERT</tt> is preprocessed into an
///  empty string.
/// \details The traditional Posix define <tt>NDEBUG</tt> has no effect on
///  <tt>CRYPTOPP_DEBUG</tt>, <tt>CRYPTOPP_ASSERT</tt> or DebugTrapHandler.
/// \details An example of using CRYPTOPP_ASSERT and DebugTrapHandler is shown
///  below. The library's test program, <tt>cryptest.exe</tt> (from test.cpp),
///  exercises the structure:
///  <pre>
///   \#if defined(CRYPTOPP_DEBUG) && defined(UNIX_SIGNALS_AVAILABLE)
///   static const DebugTrapHandler g_dummyHandler;
///   \#endif
///
///   int main(int argc, const char** argv)
///   {
///      CRYPTOPP_ASSERT(argv != nullptr);
///      ...
///   }
///  </pre>
/// \since Crypto++ 5.6.5
/// \sa DebugTrapHandler, SignalHandler, <A
///  HREF="http://github.com/weidai11/cryptopp/issues/277">Issue 277</A>,
///  <A HREF="http://seclists.org/oss-sec/2016/q3/520">CVE-2016-7420</A>
#  define CRYPTOPP_ASSERT(exp) { ... }
#endif

#if defined(CRYPTOPP_DEBUG)
# if defined(UNIX_SIGNALS_AVAILABLE) || defined(__CYGWIN__)
#  define CRYPTOPP_ASSERT(exp) {                                  \
    if (!(exp)) {                                                 \
      std::ostringstream oss;                                     \
      oss << "Assertion failed: " << __FILE__ << "("              \
          << __LINE__ << "): " << __func__                        \
          << std::endl;                                           \
      std::cout << std::flush;                                    \
      std::cerr << oss.str();                                     \
      raise(SIGTRAP);                                             \
    }                                                             \
  }
# elif CRYPTOPP_DEBUG && defined(CRYPTOPP_WIN32_AVAILABLE)
#  define CRYPTOPP_ASSERT(exp) {                                  \
    if (!(exp)) {                                                 \
      std::ostringstream oss;                                     \
      oss << "Assertion failed: " << __FILE__ << "("              \
          << __LINE__ << "): " << __FUNCTION__                    \
          << std::endl;                                           \
      std::cout << std::flush;                                    \
      std::cerr << oss.str();                                     \
      if (IsDebuggerPresent()) {DebugBreak();}                    \
    }                                                             \
  }
# endif // Unix or Windows
#endif  // CRYPTOPP_DEBUG

// Remove CRYPTOPP_ASSERT in non-debug builds.
#ifndef CRYPTOPP_ASSERT
#  define CRYPTOPP_ASSERT(exp) (void)0
#endif

NAMESPACE_BEGIN(CryptoPP)

// ************** SIGTRAP handler ***************

#if (CRYPTOPP_DEBUG && defined(UNIX_SIGNALS_AVAILABLE)) || defined(CRYPTOPP_DOXYGEN_PROCESSING)
/// \brief Default SIGTRAP handler
/// \details DebugTrapHandler() can be used by a program to install an empty
///  SIGTRAP handler. If present, the handler ensures there is a signal
///  handler in place for <tt>SIGTRAP</tt> raised by
///  <tt>CRYPTOPP_ASSERT</tt>. If <tt>CRYPTOPP_ASSERT</tt> raises
///  <tt>SIGTRAP</tt> <em>without</em> a handler, then one of two things can
///  occur. First, the OS might allow the program to continue. Second, the OS
///  might terminate the program. OS X allows the program to continue, while
///  some Linuxes terminate the program.
/// \details If DebugTrapHandler detects another handler in place, then it will
///  not install a handler. This ensures a debugger can gain control of the
///  <tt>SIGTRAP</tt> signal without contention. It also allows multiple
///  DebugTrapHandler to be created without contentious or unusual behavior.
///  Though multiple DebugTrapHandler can be created, a program should only
///  create one, if needed.
/// \details A DebugTrapHandler is subject to C++ static initialization
///  [dis]order. If you need to install a handler and it must be installed
///  early, then reference the code associated with
///  <tt>CRYPTOPP_INIT_PRIORITY</tt> in cryptlib.cpp and cpu.cpp.
/// \details If you want to ensure <tt>CRYPTOPP_ASSERT</tt> is inert, then
///  <em>do not</em> define <tt>CRYPTOPP_DEBUG</tt>, <tt>DEBUG</tt> or
///  <tt>_DEBUG</tt>. Avoiding the defines means <tt>CRYPTOPP_ASSERT</tt>
///  is processed into <tt>((void)0)</tt>.
/// \details The traditional Posix define <tt>NDEBUG</tt> has no effect on
///  <tt>CRYPTOPP_DEBUG</tt>, <tt>CRYPTOPP_ASSERT</tt> or DebugTrapHandler.
/// \details An example of using \ref CRYPTOPP_ASSERT "CRYPTOPP_ASSERT" and
///  DebugTrapHandler is shown below. The library's test program,
///  <tt>cryptest.exe</tt> (from test.cpp), exercises the structure:
///  <pre>
///   \#if defined(CRYPTOPP_DEBUG) && defined(UNIX_SIGNALS_AVAILABLE)
///   const DebugTrapHandler g_dummyHandler;
///   \#endif
///
///   int main(int argc, const char** argv)
///   {
///      CRYPTOPP_ASSERT(argv != nullptr);
///      ...
///   }
///  </pre>
/// \since Crypto++ 5.6.5
/// \sa \ref CRYPTOPP_ASSERT "CRYPTOPP_ASSERT", SignalHandler, <A
///  HREF="http://github.com/weidai11/cryptopp/issues/277">Issue 277</A>,
///  <A HREF="http://seclists.org/oss-sec/2016/q3/520">CVE-2016-7420</A>

#if defined(CRYPTOPP_DOXYGEN_PROCESSING)
class DebugTrapHandler: public SignalHandler<SIGTRAP, false> { };
#else
typedef SignalHandler<SIGTRAP, false> DebugTrapHandler;
#endif

#endif  // Linux, Unix and Documentation

NAMESPACE_END

#endif // CRYPTOPP_TRAP_H

































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


