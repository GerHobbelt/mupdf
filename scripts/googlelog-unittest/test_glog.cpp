#include "pch.h"

#define BUILD_MONOLITHIC  1
#include "../../thirdparty/owemdjee/glog/src/testing.h"

#define BUILD_MONOLITHIC_SINGLE_INSTANCE_NOW   1
#include "../../thirdparty/owemdjee/glog/src/googletest.h"



static void init_test_argv(int& argc, const char**& argv)
{
	static const char* test_argv[] = {
		"unit-test",
		NULL
	};
	argv = test_argv;
	argc = int(sizeof(test_argv) / sizeof(test_argv[0])) - 1;
}


TEST(LibGoogleLog, logging_stripped1) {
	int lcl_argc;
	const char** lcl_argv;
	init_test_argv(lcl_argc, lcl_argv);
	EXPECT_EQ(glog_logging_striptest0_main(lcl_argc, lcl_argv), EXIT_SUCCESS);
}

TEST(LibGoogleLog, logging_stripped2) {
	int lcl_argc;
	const char** lcl_argv;
	init_test_argv(lcl_argc, lcl_argv);
	EXPECT_EQ(glog_logging_striptest2_main(lcl_argc, lcl_argv), EXIT_SUCCESS);
}

TEST(LibGoogleLog, logging_stripped10) {
	int lcl_argc;
	const char** lcl_argv;
	init_test_argv(lcl_argc, lcl_argv);
	EXPECT_EQ(glog_logging_striptest10_main(lcl_argc, lcl_argv), EXIT_SUCCESS);
}

TEST(LibGoogleLog, signalhandler) {
	int lcl_argc;
	const char** lcl_argv;
	init_test_argv(lcl_argc, lcl_argv);
	EXPECT_EQ(glog_signalhandler_unittest_main(lcl_argc, lcl_argv), EXIT_SUCCESS);
}

TEST(LibGoogleLog, stacktrace) {
	int lcl_argc;
	const char** lcl_argv;
	init_test_argv(lcl_argc, lcl_argv);
	EXPECT_EQ(glog_stacktrace_unittest_main(lcl_argc, lcl_argv), EXIT_SUCCESS);
}

TEST(LibGoogleLog, stl_logging) {
	int lcl_argc;
	const char** lcl_argv;
	init_test_argv(lcl_argc, lcl_argv);
	EXPECT_EQ(glog_stl_logging_unittest_main(lcl_argc, lcl_argv), EXIT_SUCCESS);
}


int run_all_tests_glog(void)
{
	int rv = 0;

	int lcl_argc;
	const char** lcl_argv;
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_demangle_unittest_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_logging_custom_prefix_unittest_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_logging_unittest_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_mock_log_test_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_symbolize_unittest_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_utilities_unittest_main(lcl_argc, lcl_argv);

	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_logging_striptest0_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_logging_striptest2_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_logging_striptest10_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_signalhandler_unittest_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_stacktrace_unittest_main(lcl_argc, lcl_argv);
	init_test_argv(lcl_argc, lcl_argv);
	rv |= glog_stl_logging_unittest_main(lcl_argc, lcl_argv);

	return rv;
}

