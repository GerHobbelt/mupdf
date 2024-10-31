//
// part of Demo 
//

//implement your copy of `#include "monolithic_main_defs.h"`:
#include "./monolithic_examples.h"
#include "../../thirdparty/owemdjee/gumbo-libxml/gumbo_libxml.h"
#include "../../thirdparty/owemdjee/gumbo-query/example/monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "gumbo-parser-examples"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
{ "benchmark", { .fa = gumbo_benchmark_main } },
{ "clean_text", { .fa = gumbo_clean_text_main } },
{ "eval_xpath", {.fa = gumbo_eval_xpath_main } },
{ "find_links", {.fa = gumbo_find_links_main } },
{ "get_title", {.fa = gumbo_get_title_main } },
{ "positions_of_class", {.fa = gumbo_positions_of_class_main } },
{ "prettyprint", {.fa = gumbo_prettyprint_main } },
{ "print", {.fa = gumbo_print_main } },
{ "serialize", {.fa = gumbo_serialize_main } },

{ "libxml_example", {.fa = gumbo_libxml_example_main } },

{ "query_example", {.f = gumboquery_example_main } },
{ "query_test", {.f = gumboquery_test_main } },
MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
