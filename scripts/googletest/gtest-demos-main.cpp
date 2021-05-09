//
// main() for monolithic build of the googletest::mock tests
//

int gtest_main(int argc, char** argv);
int gtest_break_on_failure_test_main(int argc, char** argv);
int gtest_catch_exceptions_test_main(int argc, char** argv);
int gtest_color_test_main(int argc, char** argv);
int gtest_death_test_main(int argc, char** argv);
int gtest_env_var_test_main(int argc, char** argv);
int gtest_failfast_test_main(int argc, char** argv);
int gtest_filter_test_main(int argc, char** argv);
int gtest_global_env_test_main(int argc, char** argv);
int gtest_list_test_main(int argc, char** argv);
int gtest_listener_test_main(int argc, char** argv);
int gtest_output_test_main(int argc, char** argv);
int gtest_param_inv_name1_test_main(int argc, char** argv);
int gtest_param_inv_name2_test_main(int argc, char** argv);
int gtest_param_test_main(int argc, char** argv);
int gtest_shuffle_test_main(int argc, char** argv);
int gtest_throw_on_fail_test_main(int argc, char** argv);
int gtest_uninitialized_test_main(int argc, char** argv);
int gtest_api_test_main(int argc, char** argv);
int gtest_assert_ex_test_main(int argc, char** argv);
int gtest_env_test_main(int argc, char** argv);
int gtest_list_output_test_main(int argc, char** argv);
int gtest_no_test_main(int argc, char** argv);
int gtest_premature_exit_test_main(int argc, char** argv);
int gtest_repeat_test_main(int argc, char** argv);
int gtest_skip_env_test_main(int argc, char** argv);
int gtest_stress_test_main(int argc, char** argv);
int gtest_stress_test_main(int argc, char** argv);
int gtest_macro_stack_footprint_test_main(int argc, char** argv);
int gtest_throw_on_fail_ex_test_main(int argc, char** argv);
int gtest_xml_output_test_main(int argc, char** argv);

int main(int argc, char** argv)
{
	int rv = 0;
	rv |= gtest_break_on_failure_test_main(argc, argv);
	rv |= gtest_catch_exceptions_test_main(argc, argv);
	rv |= gtest_color_test_main(argc, argv);
	rv |= gtest_death_test_main(argc, argv);
	rv |= gtest_env_var_test_main(argc, argv);
	rv |= gtest_failfast_test_main(argc, argv);
	rv |= gtest_filter_test_main(argc, argv);
	rv |= gtest_global_env_test_main(argc, argv);
	rv |= gtest_list_test_main(argc, argv);
	rv |= gtest_listener_test_main(argc, argv);
	rv |= gtest_output_test_main(argc, argv);
	rv |= gtest_param_inv_name1_test_main(argc, argv);
	rv |= gtest_param_inv_name2_test_main(argc, argv);
	rv |= gtest_param_test_main(argc, argv);
	rv |= gtest_shuffle_test_main(argc, argv);
	rv |= gtest_throw_on_fail_test_main(argc, argv);
	rv |= gtest_uninitialized_test_main(argc, argv);
	rv |= gtest_api_test_main(argc, argv);
	rv |= gtest_assert_ex_test_main(argc, argv);
	rv |= gtest_env_test_main(argc, argv);
	rv |= gtest_list_output_test_main(argc, argv);
	rv |= gtest_no_test_main(argc, argv);
	rv |= gtest_premature_exit_test_main(argc, argv);
	rv |= gtest_repeat_test_main(argc, argv);
	rv |= gtest_skip_env_test_main(argc, argv);
	rv |= gtest_stress_test_main(argc, argv);
	rv |= gtest_stress_test_main(argc, argv);
	rv |= gtest_macro_stack_footprint_test_main(argc, argv);
	rv |= gtest_throw_on_fail_ex_test_main(argc, argv);
	rv |= gtest_xml_output_test_main(argc, argv);

	rv |= gtest_main(argc, argv);
	return rv;
}
