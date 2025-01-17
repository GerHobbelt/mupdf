
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

	int assert_add_subdirectory_integration_test_main(int argc, const char** argv);
	int assert_basic_demo_test_main(int argc, const char** argv);
	int assert_basic_test_main(int argc, const char** argv);
	int assert_constexpr_contexts_main(int argc, const char** argv);
	int assert_demo_main(int argc, const char** argv);
	int assert_disambiguation_main(int argc, const char** argv);
	int assert_fetchcontent_integration_test_main(int argc, const char** argv);
	int assert_findpackage_integration_test_main(int argc, const char** argv);
	int assert_integration_main(int argc, const char** argv);
	int assert_literals_main(int argc, const char** argv);
	int assert_stringify_test_main(int argc, const char** argv);
	int assert_test_public_utilities_main(int argc, const char** argv);
	int assert_test_type_prettier_main(int argc, const char** argv);
	int assert_tokens_and_highlighting_main(int argc, const char** argv);
	int assert_type_handling_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
