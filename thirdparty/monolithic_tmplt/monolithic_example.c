//
// part of Demo 
//

//implement your copy of `#include "monolithic_main_defs.h"`:
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "demo"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
{ "demo1", { .fa = mbdemo1_main } },
{ "demo2", { .f = mbdemo2_main } },
MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
