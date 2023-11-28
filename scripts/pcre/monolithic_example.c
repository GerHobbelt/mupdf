
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "pcre2"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()

{ "jit_test", { .f = pcre2_jit_test_main } },
{ "ucp_test", { .fa = pcre2_ucp_test_main } },
{ "utf8", { .fa = pcre2_utf8_main } },
{ "demo", { .fa = pcre2_demo_main } },
{ "grep", { .fa = pcre2_grep_main } },
{ "posix_test", { .fa = pcre2posix_test_main } },
{ "test8", { .fa = pcre2_test8_main } },
{ "test16", { .fa = pcre2_test16_main } },
{ "test32", { .fa = pcre2_test32_main } },
{ "dftables", { .fa = pcre2_dftables_main } },

MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
