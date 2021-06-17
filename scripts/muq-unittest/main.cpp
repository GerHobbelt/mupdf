
#include "pch.h"

TEST(muqCoreBelief, ThrownCpluplusExceptionIsCaught) {
	throw std::exception("aborting");
}

TEST(muqCoreBelief, ThrownCpluplusExceptionPointerIsCaught) {
	throw new std::exception("aborting");
}

TEST(muqCoreBelief, ThrownCpluplusArbitraryExceptionIsCaught) {
	throw 42;
}

TEST(muqCoreBelief, ThrownCpluplusArbitraryExceptionPointerIsCaught) {
	throw NULL;
}

TEST(muqCoreBelief, MemoryAccessFailureCaughtBySEH) {
	volatile int* pInt = 0x00000000;
	*pInt = 20;
}

GTEST_API_ int main(int argc, const char** argv) {
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);

	int rv = 0;

	// HACK: don't execute the tests until we say they should. This prevents the monolithic build resulting in executing all the tests many times.
	::testing::internal::g_help_flag = true;

	rv |= run_all_tests_glog();  // call this one to ensure all google-log tests are included in this monolithic test executable.

	::testing::internal::g_help_flag = false;

	rv |= RUN_ALL_TESTS();

	return rv;
}
