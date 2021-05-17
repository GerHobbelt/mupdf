
#include "pch.h"

GTEST_API_ int main(int argc, const char** argv) {
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
