
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

	int assert_add_subdirectory_integration_test_main(void);
	int assert_basic_demo_test_main(void);
	int assert_basic_test_main(void);
	int assert_constexpr_contexts_main(void);
	int assert_demo_main(void);
	int assert_disambiguation_main(void);
	int assert_fetchcontent_integration_test_main(void);
	int assert_findpackage_integration_test_main(void);
	int assert_integration_main(void);
	int assert_literals_main(void);
	int assert_stringify_test_main(void);
	int assert_test_public_utilities_main(void);
	int assert_test_type_prettier_main(void);
	int assert_tokens_and_highlighting_main(void);
	int assert_type_handling_main(void);

#ifdef __cplusplus
}
#endif

#endif
