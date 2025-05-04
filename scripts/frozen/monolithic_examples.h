
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

int frozen_benchmark_main(int argc, const char** argv);
int frozen_enum_to_string_example_main(void);
int frozen_enum_to_string_hash_example_main(void);
int frozen_html_entities_map_example_main(int argc, const char** argv);
int frozen_pixel_art_example_main(void);
int frozen_static_assert_example_main(void);
int frozen_value_modification_example_main(void);

int frozen_no_exceptions_test_main(void);
int frozen_tests_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
