#include "pch.h"

#define BUILD_MONOLITHIC 1
#include "../../thirdparty/owemdjee/clipp/test/monolithic_tests.h"


TEST(LibClipp, actions) {
  EXPECT_EQ(clipp_actions_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, alternative_groups) {
  EXPECT_EQ(clipp_alternative_groups_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, alternative_options) {
  EXPECT_EQ(clipp_alternative_options_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, alternative_required) {
  EXPECT_EQ(clipp_alternative_required_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_01) {
  EXPECT_EQ(clipp_blocking_test01_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_02) {
  EXPECT_EQ(clipp_blocking_test02_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_03) {
  EXPECT_EQ(clipp_blocking_test03_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_04) {
  EXPECT_EQ(clipp_blocking_test04_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_05) {
  EXPECT_EQ(clipp_blocking_test05_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_06) {
  EXPECT_EQ(clipp_blocking_test06_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_07) {
  EXPECT_EQ(clipp_blocking_test07_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_08) {
  EXPECT_EQ(clipp_blocking_test08_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_09) {
  EXPECT_EQ(clipp_blocking_test09_main(), EXIT_SUCCESS);
}

TEST(LibClipp, blocking_10) {
  EXPECT_EQ(clipp_blocking_test10_main(), EXIT_SUCCESS);
}

TEST(LibClipp, documentation) {
  EXPECT_EQ(clipp_documentation_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, empty_args) {
  EXPECT_EQ(clipp_empty_args_main(), EXIT_SUCCESS);
}

TEST(LibClipp, flag_param_factories) {
  EXPECT_EQ(clipp_flag_param_factories_test_main(0, NULL), EXIT_SUCCESS);
}

TEST(LibClipp, joined_flags_test1) {
  EXPECT_EQ(clipp_joined_flags_test1_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_flags_test2) {
  EXPECT_EQ(clipp_joined_flags_test2_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_flags_test3) {
  EXPECT_EQ(clipp_joined_flags_test3_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_flags_test4) {
  EXPECT_EQ(clipp_joined_flags_test4_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_flags_test5) {
  EXPECT_EQ(clipp_joined_flags_test5_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_flags_test6) {
  EXPECT_EQ(clipp_joined_flags_test6_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_params_test1) {
  EXPECT_EQ(clipp_joined_params_test1_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_params_test2) {
  EXPECT_EQ(clipp_joined_params_test2_main(), EXIT_SUCCESS);
}

TEST(LibClipp, joined_sequence) {
  EXPECT_EQ(clipp_joined_sequence_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, mixed_params) {
  EXPECT_EQ(clipp_mixed_params_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, nesting) {
  EXPECT_EQ(clipp_nesting_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, options) {
  EXPECT_EQ(clipp_options_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, prefix_free) {
  EXPECT_EQ(clipp_prefix_free_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, prefix) {
  EXPECT_EQ(clipp_prefix_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, repeatability) {
  EXPECT_EQ(clipp_repeatability_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, repeatable_alternatives) {
  EXPECT_EQ(clipp_repeatable_alternatives_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, required_params_test1) {
  EXPECT_EQ(clipp_required_params_test1_main(), EXIT_SUCCESS);
}

TEST(LibClipp, required_params_test2) {
  EXPECT_EQ(clipp_required_params_test2_main(), EXIT_SUCCESS);
}

TEST(LibClipp, usage_lines) {
  EXPECT_EQ(clipp_usage_lines_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, values_conversion) {
  EXPECT_EQ(clipp_values_conversion_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, values_filter) {
  EXPECT_EQ(clipp_values_filter_test_main(), EXIT_SUCCESS);
}

TEST(LibClipp, values_sequencing) {
  EXPECT_EQ(clipp_values_sequencing_test_main(), EXIT_SUCCESS);
}

