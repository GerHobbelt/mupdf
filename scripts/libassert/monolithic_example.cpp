
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "assert_demo"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()

{ "add_subdirectory_integration_test", { .fa = assert_add_subdirectory_integration_test_main } },
{ "basic_demo_test", { .fa = assert_basic_demo_test_main } },
{ "basic_test", { .fa = assert_basic_test_main } },
{ "constexpr_contexts", { .fa = assert_constexpr_contexts_main } },
{ "demo", { .fa = assert_demo_main } },
{ "disambiguation", { .fa = assert_disambiguation_main } },
{ "fetchcontent_integration_test", { .fa = assert_fetchcontent_integration_test_main } },
{ "findpackage_integration_test", { .fa = assert_findpackage_integration_test_main } },
{ "integration", { .fa = assert_integration_main } },
{ "literals", { .fa = assert_literals_main } },
{ "stringify_test", { .fa = assert_stringify_test_main } },
{ "test_public_utilities", { .fa = assert_test_public_utilities_main } },
{ "test_type_prettier", { .fa = assert_test_type_prettier_main } },
{ "tokens_and_highlighting", { .fa = assert_tokens_and_highlighting_main } },
{ "type_handling", { .fa = assert_type_handling_main } },

MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
