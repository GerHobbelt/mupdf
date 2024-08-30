
//implement your copy of `#include "monolithic_main_defs.h"`:
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "demo"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()

{"boost_container", {.fa = toml11_boost_container_example_main}},
{"boost_multiprecision", { .f = toml11_boost_multiprecision_example_main }},
{"parse_array", { .f = toml11_parse_array_example_main }},
{"parse_array_of_tables", { .f = toml11_parse_array_of_tables_example_main }},
{"parse_key", { .f = toml11_parse_key_example_main }},
{"parse_spec", { .f = toml11_parse_spec_example_main }},
{"reflect", { .f = toml11_reflect_example_main }},
{"u8string", { .f = toml11_u8string_example_main }},
{"canonicalize", {.fa = toml11_canonicalize_example_main}},

{"expand", {.fa = toml11_expand_tool_main}},

MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
