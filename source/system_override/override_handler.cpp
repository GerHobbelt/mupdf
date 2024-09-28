
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


namespace system_override {

	class SystemOverrideClass {
	public:
		SystemOverrideClass(bool _always_kick_in = false);
		~SystemOverrideClass();

	protected:
		void Starting();
		void Ending();

	public:
		static void KickInTheDoor(bool should_do = true);

		static void Tickle(void);
		static SystemOverrideClass *override;

	protected:
		std::atomic_int invoked{0};
		static bool always_kick_in;
	};

}

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

#if 0

#ifdef __cplusplus
extern "C" {
#endif 

__declspec(noreturn) void invoke_original_abort(void);
__declspec(noreturn) void invoke_original_exit(int code);

void BreakIntoDebugger(void);   

#ifdef __cplusplus
}
#endif 

#endif

// The idea behind this seemingly convoluted way to set the invoke-debugger flag to true or false
// is that this is relatively to *patch* in the final compiled binary in both debug and release builds
// so that debugging facilities can be triggered ad hoc when trouble ensues somewhere.
//
// For the 'easily patched' feature to work, both strings should have the same length: you only want to
// flip a single byte in the hex editor when you want to switch this behaviour.
#define DO_INVOKE_DEBUGGER_MARKER_STRING             "marker:do.invoke.debugger"
#if !defined(NDEBUG) || 1  // always on
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


#ifdef __cplusplus
extern "C" {
#endif 

__declspec(noreturn) void invoke_abort(void)
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original_abort();
}

__declspec(noreturn) void invoke_exit(int code)
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original_exit(code);
}

void SystemOverride_KickInTheDoor(void)
{
	SystemOverrideClass::KickInTheDoor();
}

#ifdef __cplusplus
}
#endif 


