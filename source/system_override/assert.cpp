
#include "system_override_internal.h"

using namespace system_override;




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
	SystemOverrideClass::KickInTheDoor();
	exit(666);
}

extern "C" void __cdecl fz_sysassert_and_continue(const char * _Message, const char * _File, const char * _Function, unsigned int _Line)
{
	fprintf(stderr, "Assertion failed: %s in %s() (%s:%d)\n", _Message, _Function, _File, _Line);
	SystemOverrideClass::KickInTheDoor();
}

#endif


