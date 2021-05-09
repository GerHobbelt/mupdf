//
// main() for monolithic build of the googletest::mock tests
//

int gtest_main(int argc, const char** argv);

int gtest_api_test_main(int argc, const char** argv);
int gtest_assert_ex_test_main(int argc, const char** argv);
int gtest_env_test_main(int argc, const char** argv);
int gtest_list_output_test_main(int argc, const char** argv);
int gtest_no_test_main(int argc, const char** argv);
int gtest_premature_exit_test_main(int argc, const char** argv);
int gtest_repeat_test_main(int argc, const char** argv);
int gtest_skip_env_test_main(int argc, const char** argv);
int gtest_stress_test_main(int argc, const char** argv);
int gtest_macro_stack_footprint_test_main(int argc, const char** argv);
int gtest_throw_on_fail_ex_test_main(int argc, const char** argv);
int gtest_xml_output_test_main(int argc, const char** argv);

int main(int argc, const char** argv)
{
	int rv = 0;
	rv |= gtest_api_test_main(argc, argv);
	rv |= gtest_assert_ex_test_main(argc, argv);
	rv |= gtest_env_test_main(argc, argv);
	rv |= gtest_list_output_test_main(argc, argv);
	rv |= gtest_no_test_main(argc, argv);
	rv |= gtest_premature_exit_test_main(argc, argv);
	rv |= gtest_repeat_test_main(argc, argv);
	rv |= gtest_skip_env_test_main(argc, argv);
	rv |= gtest_stress_test_main(argc, argv);
	rv |= gtest_macro_stack_footprint_test_main(argc, argv);
	rv |= gtest_throw_on_fail_ex_test_main(argc, argv);
	rv |= gtest_xml_output_test_main(argc, argv);

	rv |= gtest_main(argc, argv);
	return rv;
}
