
#include <catch2/catch_session.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_config_wchar.hpp>
#include <catch2/internal/catch_leak_detector.hpp>
#include <catch2/internal/catch_platform.hpp>

#include <stdio.h>
#include <stdlib.h>

extern "C"
int catch2_runner_main(int argc, const char** argv);
extern "C"
int catch2_custom_main_example_main(int argc, const char** argv);


namespace Catch {
	CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
		CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
		static LeakDetector leakDetector;
	CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
}


// Standard C/C++ main entry point
extern "C"
int main(int argc, const char** argv) {

	// We want to force the linker not to discard the global variable
	// and its constructor, as it (optionally) registers leak detector
	(void)&Catch::leakDetector;

	return catch2_custom_main_example_main(argc, argv);
}

