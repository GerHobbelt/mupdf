
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "assert_demo"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()

{ "add_subdirectory_integration_test", { .f = assert_add_subdirectory_integration_test_main } },
{ "basic_demo_test", { .f = assert_basic_demo_test_main } },
{ "basic_test", { .f = assert_basic_test_main } },
{ "constexpr_contexts", { .f = assert_constexpr_contexts_main } },
{ "demo", { .f = assert_demo_main } },
{ "disambiguation", { .f = assert_disambiguation_main } },
{ "fetchcontent_integration_test", { .f = assert_fetchcontent_integration_test_main } },
{ "findpackage_integration_test", { .f = assert_findpackage_integration_test_main } },
{ "integration", { .f = assert_integration_main } },
{ "literals", { .f = assert_literals_main } },
{ "test_public_utilities", { .f = assert_test_public_utilities_main } },
{ "test_type_prettier", { .f = assert_test_type_prettier_main } },
{ "tokens_and_highlighting", { .f = assert_tokens_and_highlighting_main } },
{ "type_handling", { .f = assert_type_handling_main } },

{"basic_primitive_demo_test", { .f = assert_basic_primitive_demo_test_main }},
{"basic_primitive_test", { .f = assert_basic_primitive_test_main }},
{"primitive_demo", { .f = assert_primitive_demo_main }},

MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
