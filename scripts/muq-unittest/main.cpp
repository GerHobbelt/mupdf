
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

	rv |= RUN_ALL_TESTS();

	return rv;
}
