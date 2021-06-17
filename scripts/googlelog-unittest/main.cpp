
#include "pch.h"

GTEST_API_ int main(int argc, const char** argv) {
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);

	int rv = 0;

	rv |= run_all_tests_glog();  // call this one to ensure all google-log tests are included in this monolithic test executable.

	return rv;
}
