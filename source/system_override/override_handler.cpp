
#include "system_override_internal.h"

#if defined(_WIN32)
#include <windows.h>
#endif
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <atomic>

#include <detours.h>


#ifdef abort
#error "Expected abort to be the system call, not the OLD override hack..."
#endif
#ifdef exit
#error "Expected exit to be the system call, not the OLD override hack..."
#endif



// Mandatory or MSVC will optimize this entire endeavour to Kingdom Come and we won't have our debugger breakpoints at start and end...
#pragma optimize("", off)


// While *from a C++ Standard's perspective*, we *are* (risking und)ab)using undefined behaviour as the code in this source file
// obviously suffers from the Static Initialization Order Fiasco (see: XXX),
// PLEASE DO NOTE that we are doing some **very compiler specific** stuff here.
//
// The Actual Reality is that MSVC2022 at least initializes the globals in this source file **in order of appearance**, which is
// *why* we have those `system_abort_ref` (et al) global variables placed **above** the global/singleton instance of
// `override_singular_instance`, PLUS we made sure all references to each of these all occur only further down the lane within
// this source file ONLY -- tests with MSVC2022 have shown that you CANNOT predict ANYTHING beyond the single source file
// (with optimizations turned OFF) and then you DO suffer badly from Static Initialization Order Fiasco, indeed.
//
// However, for our trick to succeed, we don't need any other globals in other sources (object files) to be prepped before ours,
// so we are good to go, just as-is.
//
// Which solves the problem of having a need for several global variables initialized before first use in here (particularly
// the singleton constructor invoking the `void override_the_abort_and_exit_system_calls(void)` function: that requirement
// is the reason why this source code file looks oddly segmented as first we declare a SystemOverrideClass class (segment 1),
// then switch tracks and get ourselves a couple of pointers to the system abort() and exit() calls, *plus* we *define* the
// Detours-based secret sauce in `void override_the_abort_and_exit_system_calls(void)`, so that is ready as well
// **before** we go and actually define a global SystemOverrideClass class instance (singleton) plus the constructor and
// destructor codes needed to go with the SystemOverrideClass class declaration near he top of the source file.
//
// So far, so good.
// Now that we have established an acceptable init order and Detours helps us "wrapping" those MSVCRT/MSCRTD DLL-based
// system calls, all that's left is us becoming part of the **actual** linked executable!
// That turned into a real pickle at first as the linker was smart enough to realize that it could resolve all dependencies
// very nicely, thank you very much, without *ever* needing to this object file, that's sitting inside a `libsystem_override`
// STATIC LIBRARY.
// 
// Oops! Nothing happens! Ay!ay!ay!
//
// Turns out (we tried a few things, but nothing else delivered) you MUST have a dependency *into* this object file,
// before the linker will ever consider including all this stuff.
// To ensure the entire works in here is properly resolved via linker dependency chain the obvious thing to reference
// is the bit near the very end of this source file: if we reference that bit, we MIGHT get lucky and have all of the stuff
// above already initialized once we access that last global near the bottom of this source file; meanwhile we must ensure
// inside this source file that that last global is a real root of a dependency chain that will pull all the stuff above it
// into the executable: that's why we have some rediculous singleton class instance references sprinkled through the
// code that's invoked (dependency chain! yay!) from the bottom global's initializing function call.
//
// Of course we DO NOT want the compiler & linker to be as smart as they can be and find out that some of this
// dependent stuff really goes unused (bogus fetch/access, cast to `(void)`) and can safely be discarded: that would really
// throw a spanner in our works here, so we instruct the compiler to be as dumb as a plank!
//
// Testing shows that the least source-code-impactful approach towards ensuring this override code is included by
// **everybody** and their granny, is to have each EXE project "compile" an extra "otherwise quite dummy" source file
// which references our static library enclosed stuff here: the object file's reference will be seen as a MANDATORY
// dependency to resolve by the linker (nope, haven't yet tested with Whole Program Optimization; I fear that's gonna
// be a real scorcher!)
// 
// Anyway, our goal is to get this stuff included in every EXE. The initialization order is not that relevant as we only
// bother about wrapping/hooking exit() and abort() calls once they happen from inside main(), so we have some leeway there.
//
// We DO however aim to be **late** when it comes to *Destruction O'clock* once the application is well and truly in its
// exit/termination phase: *that* is why we applied that `#pragam init_seg` just below: at init time, we MUST be later than
// the compiler-internal stuff, so init_seg(compiler) won't do, so we go with the next one available: init_seg(lib).
// Which *implies* that the SystemOverrideClass class global singleton instance **should** be destructed with the last dregs
// of the application, meaning we **should** have our abort() and exit() wrappers executing very nicely *long before*
// the scaffolding surrounding that bit, including the hook/pointer globals halfway down this source file, are still way
// of from getting destructed.
// All that wriggling with the init order is done so we MAY have the debug_break(), which is part of the SystemOverrideClass class
// destructor code sequence, execute near the *very end of the application execution phase*: one last glance for us
// to look at stuff through the lense of the debugger du jour. Yay!
//
// 
// NOTE: the `#pragma optimize(OFF)` above is useful when you want to place extra breakpoint in this source code: without
// that line, the class destructor and quite a few other bits get inlined or otherwise smartly rewritten, so the MSVC debugger
// won't be able to give you many spots in here where hitting [F9] key to inject a breakpoint will actually fly.
// Without the pragma, the code still builds and does its job properly, but will just be so much harder to debug if you
// any WTFs left to investigate later on. Meanwhile this piece of code isn't all that big or costly either, so we can quite
// safely instruct the compiler to play dumb and remain like that until the very last line of code in here.
//



// fix warning C4073: initializers put in library initialization area
#pragma warning(disable: 4073)
#pragma init_seg(lib)


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

	protected:
		std::atomic_int invoked{0};
		std::atomic_int destroyed{0};
		volatile bool always_kick_in{false};
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







// and finally a few functions which will be able to call the original runtime library functions we did override...

#if defined(_MSC_VER)

__declspec(noreturn) void invoke_original_abort(void);
__declspec(noreturn) void invoke_original_exit(int code);
__declspec(noreturn) void invoke_original__exit(int code);
__declspec(noreturn) void invoke_original__Exit(int code);
__declspec(noreturn) void invoke_original_quick_exit(int code);
__declspec(noreturn) void invoke_original_terminate(void) throw();

typedef void __cdecl system_abort_ref_t(void);
typedef void __cdecl system_exit_ref_t(int _Code);
typedef void __cdecl system_terminate_ref_t(void) throw();


static system_abort_ref_t* system_abort_ref = abort;
static system_exit_ref_t* system_exit_ref = exit;
static system_exit_ref_t* system__exit_ref = _exit;
static system_exit_ref_t* system__Exit1_ref = _Exit;
static system_exit_ref_t* system_quick_exit_ref = quick_exit;

static system_terminate_ref_t* system_terminate_ref = terminate;

// namespace std::
typedef void system_std_abort_ref_t(void);
typedef void system_std_exit_ref_t(int _Code);

static system_std_abort_ref_t* system_std_abort_ref = std::abort;
static system_std_exit_ref_t* system_std_exit_ref = std::exit;
//static system_std_exit_ref_t* system_std__exit_ref = std::_exit;  <-- not available
static system_std_exit_ref_t* system_std__Exit1_ref = std::_Exit;
static system_std_exit_ref_t* system_std_quick_exit_ref = std::quick_exit;

static system_terminate_ref_t* system_std_terminate_ref = std::terminate;

/**
* Replacement for the C standard `abort` that returns to the `setjmp` call for
* recoverable errors.
*/
__declspec(noreturn) void invoke_original_abort(void)
{
	(*system_abort_ref)();
}

__declspec(noreturn) void invoke_original_exit(int code)
{
	(*system_exit_ref)(code);
}

__declspec(noreturn) void invoke_original__exit(int code)
{
	(*system__exit_ref)(code);
}

__declspec(noreturn) void invoke_original__Exit(int code)
{
	(*system__Exit1_ref)(code);
}

__declspec(noreturn) void invoke_original_quick_exit(int code)
{
	(*system_quick_exit_ref)(code);
}

__declspec(noreturn) void invoke_original_terminate(void) throw()
{
	(*system_terminate_ref)();
}

//////////////////////////////////////////////////////////////////////////////

static __declspec(noreturn) void my_abort_hook(void)
{
	invoke_abort();
}

static __declspec(noreturn) void my_exit_hook(int code)
{
	invoke_exit(code);
}

static __declspec(noreturn) void my__exit_hook(int code)
{
	invoke__exit(code);
}

static __declspec(noreturn) void my__Exit1_hook(int code)
{
	invoke__Exit(code);
}

static __declspec(noreturn) void my_quick_exit_hook(int code)
{
	invoke_quick_exit(code);
}

static __declspec(noreturn) void my_terminate_hook(void) throw()
{
	invoke_terminate();
}



void override_the_abort_and_exit_system_calls(void)
{
	static bool system_overrides_done = false;

	if (!system_overrides_done)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)system_abort_ref, my_abort_hook);
		DetourAttach(&(PVOID&)system_std_abort_ref, my_abort_hook);
		DetourAttach(&(PVOID&)system_exit_ref, my_exit_hook);
		DetourAttach(&(PVOID&)system__exit_ref, my__exit_hook);
		DetourAttach(&(PVOID&)system__Exit1_ref, my__Exit1_hook);
		DetourAttach(&(PVOID&)system_quick_exit_ref, my_quick_exit_hook);
		DetourAttach(&(PVOID&)system_terminate_ref, my_terminate_hook);

		DetourAttach(&(PVOID&)system_std_exit_ref, my_exit_hook);
		//DetourAttach(&(PVOID&)system_std__exit_ref, my__exit_hook);
		DetourAttach(&(PVOID&)system_std__Exit1_ref, my__Exit1_hook);
		DetourAttach(&(PVOID&)system_std_quick_exit_ref, my_quick_exit_hook);
		DetourAttach(&(PVOID&)system_std_terminate_ref, my_terminate_hook);
		DetourTransactionCommit();

		system_overrides_done = true;
	}
}

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

	// Note the Static Initialization Order Fiasco: http://dietmar-kuehl.de/mirror/c++-faq/ctors.html#faq-10.14
	static SystemOverrideClass override_singular_instance{};

	SystemOverrideClass::SystemOverrideClass(bool _always_kick_in) {
		always_kick_in = _always_kick_in;

		override_the_abort_and_exit_system_calls();

		Starting();
	}

	SystemOverrideClass::~SystemOverrideClass() {
		Ending();
		destroyed++;
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
		SystemOverrideClass &self = override_singular_instance; // get_singleton_instance();

		if (should_do || self.always_kick_in) {
			if (0 == strcmp(flag_do_invoke_debugger, DO_INVOKE_DEBUGGER_MARKER_STRING)) {
				BreakIntoDebugger();
			}
		}
	}

	void SystemOverrideClass::Tickle(void) {
		SystemOverrideClass &self = override_singular_instance; // get_singleton_instance();

		(void)self;
	}

	// Cannot be `static` as it must be exported outside the namespace: it is used further below...
	extern "C"
	SystemOverrideClass &get_system_override_singleton_instance(void) {
		return override_singular_instance; // get_singleton_instance();
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

#if 0
#pragma startup __init_system_override 1
#pragma exit    __init_system_override 1
#endif


//////////////////////////////////////////////////////////////////////////////


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

__declspec(noreturn) void invoke__exit(int code)
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original__exit(code);
}

__declspec(noreturn) void invoke__Exit(int code)
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original__Exit(code);
}

__declspec(noreturn) void invoke_quick_exit(int code)
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original_quick_exit(code);
}

__declspec(noreturn) void invoke_terminate(void) throw()
{
	SystemOverrideClass::KickInTheDoor();
	invoke_original_terminate();
}

void SystemOverride_KickInTheDoor(void)
{
	SystemOverrideClass::KickInTheDoor();
}


void init_the_global_override(void) {
	SystemOverrideClass &self = get_system_override_singleton_instance();

	(void)self;
	__init_system_override();
}

// extern int system_override_has_been_initialized;
int system_override_has_been_initialized = __init_system_override();

#ifdef __cplusplus
}
#endif 






