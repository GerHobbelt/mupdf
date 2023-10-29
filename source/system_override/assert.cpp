
#include <intrin.h>		// __debugbreak()
#if defined(_WIN32)
#include <crtdbg.h>
#include <windows.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cassert>


#if defined(_MSC_VER) && 0

static void BreakIntoDebugger(void)
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

static void BreakIntoDebugger(void)
{
	if (IsDebuggerPresent())
	{
#if defined(_CrtDbgBreak)
		_CrtDbgBreak();
#endif
		DebugBreak();
	}
}

#else

static void BreakIntoDebugger(void)
{
	DebugBreak();
}

#endif   // _WIN32_WINNT || _WIN32_WINDOWS

#elif defined(__has_builtin) && __has_builtin(__builtin_debugtrap)

static void BreakIntoDebugger(void)
{
	__builtin_debugtrap();
}

#elif defined(__has_builtin) && __has_builtin(__debugbreak)

static void BreakIntoDebugger(void)
{
	__debugbreak();
}

#elif defined(__GNUC__) && (defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) || defined(__i386))
// If we can use inline assembler, do it because this allows us to break
// directly at the location of the failing check instead of breaking inside
// raise() called from it, i.e. one stack frame below.

static void BreakIntoDebugger(void)
{
	asm volatile ("int $3") /* NOLINT */ ;
}

#else // Fall back to the generic ways.

#include <signal.h>

#if defined(SIGTRAP) 

static void BreakIntoDebugger(void)
{
	raise(SIGTRAP);
}

#else

static void BreakIntoDebugger(void)
{
	static int s = 0;
	while (s == 0) {
		sleep(1);
	}
}

#endif

#endif


#if 0


static void invoke_abort(void)
{
	BreakIntoDebugger();
	exit(999);
}



static void invoke_exit(int code)
{
	BreakIntoDebugger();
	exit(999);
}

#endif

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


#if 0


// warning C4273: 'exit': inconsistent dll linkage
#pragma warning(push)
#pragma warning(disable: 4273)

/**
* Replacement for the C standard `abort` that returns to the `setjmp` call for
* recoverable errors.
*/
extern "C" _ACRTEXP __declspec(noreturn) void __cdecl abort(void)
{
#if 0
	if (can_longjmp)
	{
		longjmp(jmp, 1);
	}
#endif
	invoke_abort();
}


namespace std {
	_ACRTEXP __declspec(noreturn) void abort()
	{
#if 0
		if (can_longjmp)
		{
			longjmp(jmp, 1);
		}
#endif
		invoke_abort();
	}
}


extern "C" _ACRTEXP __declspec(noreturn) void __cdecl exit(int _Code)
{
#if 0
	if (can_longjmp)
	{
		longjmp(jmp, 1);
	}
#endif
	invoke_exit(_Code);
}


extern "C" _ACRTEXP __declspec(noreturn) void __cdecl _exit(int _Code)
{
#if 0
	if (can_longjmp)
	{
		longjmp(jmp, 1);
	}
#endif
	invoke_exit(_Code);
}


namespace std {
	_ACRTEXP __declspec(noreturn) void exit(int _Code)
	{
#if 0
		if (can_longjmp)
		{
			longjmp(jmp, 1);
		}
#endif
		invoke_exit(_Code);
	}
}

#pragma warning(pop)


#endif


#if defined(BUILD_FOR_USE_WITH_GOOGLETEST)

#include <gtest/gtest.h>

extern "C" void __cdecl fz_sysassert(const char * _Message, const char * _File, const char * _Function, unsigned int _Line)
{
	// GTEST_FAIL_AT(file, line)             
	// travels through 
	//     GTEST_MESSAGE_AT_()
	// -->
	//     AssertHelper class constructor
	// -->
	//     UnitTest::AddTestPartResult()
	// which shall invoke    
	//     gtest_break_into_debugger()
	// and either of
	//     gtest_throw_failure_exception()
	//     gtest_exit_application()
	//
	GTEST_FAIL_AT(_File, _Line) << "Assertion: " << _Message << "  in " << _Function << "(), by way of fz_sysassert()/assert()";
}

extern "C" void __cdecl fz_sysassert_and_continue(const char * _Message, const char * _File, const char * _Function, unsigned int _Line)
{
	ADD_FAILURE_AT(_File, _Line) << "Assertion: " << _Message << "  in " << _Function << "(), by way of fz_sysassert()/assert()";
}

#else

// vanilla assert failure report function

extern "C" void __cdecl fz_sysassert(const char * _Message, const char * _File, const char * _Function, unsigned int _Line)
{
	fprintf(stderr, "Assertion failed: %s in %s() (%s:%d)\n", _Message, _Function, _File, _Line);
	BreakIntoDebugger();
	exit(666);
}

extern "C" void __cdecl fz_sysassert_and_continue(const char * _Message, const char * _File, const char * _Function, unsigned int _Line)
{
	fprintf(stderr, "Assertion failed: %s in %s() (%s:%d)\n", _Message, _Function, _File, _Line);
	BreakIntoDebugger();
}

#endif






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

BOOST_CONSTEXPR_OR_CONST std::size_t shared_memory_size = 4096 * 8;

//[getting_started_terminate_handlers_shmem
#include <boost/stacktrace.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

boost::interprocess::shared_memory_object g_shm; // inited at program start
boost::interprocess::mapped_region g_region;     // inited at program start


void my_signal_handler2(int signum) {
	::signal(signum, SIG_DFL);
	void** f = static_cast<void**>(g_region.get_address());
	*f = reinterpret_cast<void*>(1);                      // Setting flag that shared memory now constains stacktrace.
	boost::stacktrace::safe_dump_to(f + 1, g_region.get_size() - sizeof(void*));

	::raise(SIGABRT);
}
//]

#include <iostream>     // std::cerr
#include <fstream>     // std::ifstream
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>


inline void copy_and_run(const char* exec_name, char param, bool not_null) {
	std::cout << "Running with param " << param << std::endl;
	boost::filesystem::path command = exec_name;
	command = command.parent_path() / (command.stem().string() + param + command.extension().string());
	boost::filesystem::copy_file(exec_name, command, boost::filesystem::copy_option::overwrite_if_exists);

	boost::filesystem::path command_args = command;
	command_args += ' ';
	command_args += param;
	const int ret = std::system(command_args.string().c_str());

	std::cout << "End Running with param " << param << "; ret code is " << ret << std::endl;
	boost::system::error_code ignore;
	boost::filesystem::remove(command, ignore);
	if (not_null && !ret) {
		std::exit(97);
	} else if (!not_null && ret) {
		std::exit(ret);
	}
}

int run_0(const char* /*argv*/[]) {
	//[getting_started_setup_terminate_handlers
	std::set_terminate(&my_terminate_handler);
	//]
	foo(5);
	return 1;
}


int run_1(const char* /*argv*/[]) {
	setup_handlers();
	foo(5);
	return 11;
}

int run_2(const char* argv[]) {
	if (!boost::filesystem::exists("./backtrace.dump")) {
		if (std::string(argv[0]).find("noop") == std::string::npos) {
			return 21;
		}

		boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace::from_dump(std::cin);
		if (st) {
			return 22;
		}
		return 0;
	}

	//[getting_started_on_program_restart
	if (boost::filesystem::exists("./backtrace.dump")) {
		// there is a backtrace
		std::ifstream ifs("./backtrace.dump");

		boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace::from_dump(ifs);
		std::cout << "Previous run crashed:\n" << st << std::endl; /*<-*/

		if (!st) {
			return 23;
		} /*->*/

		  // cleaning up
		ifs.close();
		boost::filesystem::remove("./backtrace.dump");
	}
	//]

	return 0;
}


int run_3(const char* /*argv*/[]) {
	using namespace boost::interprocess;
	{
		shared_memory_object shm_obj(open_or_create, "shared_memory", read_write);
		shm_obj.swap(g_shm);
	}
	g_shm.truncate(shared_memory_size);

	{
		mapped_region m(g_shm, read_write, 0, shared_memory_size);
		m.swap(g_region);
	}
	void** f = static_cast<void**>(g_region.get_address());
	*f = 0;

	::signal(SIGSEGV, &my_signal_handler2);
	::signal(SIGABRT, &my_signal_handler2);
	foo(5);
	return 31;
}

int run_4(const char* argv[]) {
	using namespace boost::interprocess;
	{
		shared_memory_object shm_obj(open_only, "shared_memory", read_write);
		shm_obj.swap(g_shm);
	}

	{
		mapped_region m(g_shm, read_write, 0, shared_memory_size);
		m.swap(g_region);
	}

	//[getting_started_on_program_restart_shmem
	void** f = static_cast<void**>(g_region.get_address());
	if (*f) {                                                 // Checking if memory constains stacktrace.
		boost::stacktrace::stacktrace st 
			= boost::stacktrace::stacktrace::from_dump(f + 1, g_region.get_size() - sizeof(bool));

		std::cout << "Previous run crashed and left trace in shared memory:\n" << st << std::endl;
		*f = 0; /*<-*/
		shared_memory_object::remove("shared_memory");
		if (std::string(argv[0]).find("noop") == std::string::npos) {
			if (!st) {
				return 43;
			}
		} else {
			if (st) {
				return 44;
			}
		}
	} else {
		return 42; /*->*/
	}
	//]


	return 0;
}




















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

