
#include <stdlib.h>

#define BUILD_MONOLITHIC 1
#include "../../thirdparty/owemdjee/clipp/test/monolithic_tests.h"


#if defined(BUILD_MONOLITHIC)
#define main      clipp_run_all_tests_main
#endif

int main(void)
{
	int rv = 0;
	
	rv |= clipp_actions_test_main();
	rv |= clipp_alternative_groups_test_main();
	rv |= clipp_alternative_options_test_main();
	rv |= clipp_alternative_required_test_main();
	rv |= clipp_blocking_test01_main();
	rv |= clipp_blocking_test02_main();
	rv |= clipp_blocking_test03_main();
	rv |= clipp_blocking_test04_main();
	rv |= clipp_blocking_test05_main();
	rv |= clipp_blocking_test06_main();
	rv |= clipp_blocking_test07_main();
	rv |= clipp_blocking_test08_main();
	rv |= clipp_blocking_test09_main();
	rv |= clipp_blocking_test10_main();
	rv |= clipp_documentation_test_main();
	rv |= clipp_empty_args_main();
	rv |= clipp_flag_param_factories_test_main(0, NULL);
	rv |= clipp_joined_flags_test1_main();
	rv |= clipp_joined_flags_test2_main();
	rv |= clipp_joined_flags_test3_main();
	rv |= clipp_joined_flags_test4_main();
	rv |= clipp_joined_flags_test5_main();
	rv |= clipp_joined_flags_test6_main();
	rv |= clipp_joined_params_test1_main();
	rv |= clipp_joined_params_test2_main();
	rv |= clipp_joined_sequence_test_main();
	rv |= clipp_mixed_params_test_main();
	rv |= clipp_nesting_test_main();
	rv |= clipp_options_test_main();
	rv |= clipp_prefix_free_test_main();
	rv |= clipp_prefix_test_main();
	rv |= clipp_repeatability_test_main();
	rv |= clipp_repeatable_alternatives_test_main();
	rv |= clipp_required_params_test1_main();
	rv |= clipp_required_params_test2_main();
	rv |= clipp_usage_lines_test_main();
	rv |= clipp_values_conversion_test_main();
	rv |= clipp_values_filter_test_main();
	rv |= clipp_values_sequencing_test_main();
	return rv;
}

