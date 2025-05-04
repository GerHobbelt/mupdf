
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "frozen"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
{ "benchmark", { .fa = frozen_benchmark_main} },
{ "enum_to_string_example", {.f = frozen_enum_to_string_example_main} },
{ "enum_to_string_hash_example", {.f = frozen_enum_to_string_hash_example_main} },
{ "html_entities_map_example", {.fa = frozen_html_entities_map_example_main} },
{ "pixel_art_example", {.f = frozen_pixel_art_example_main} },
{ "static_assert_example", {.f = frozen_static_assert_example_main} },
{ "value_modification_example", {.f = frozen_value_modification_example_main} },
{ "tests", {.fa = frozen_tests_main} },
{ "no_exceptions_test", {.f = frozen_no_exceptions_test_main} },
MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
