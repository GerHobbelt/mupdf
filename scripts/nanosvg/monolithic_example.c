
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "nanosvg"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
{ "example1", { .f = nanosvg_example1_main } },
{ "example2", { .f = nanosvg_example1_main } },
MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
