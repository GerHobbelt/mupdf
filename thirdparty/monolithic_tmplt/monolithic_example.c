//
// part of Demo 
//

//#include "monolithic_main_defs.h"

#include "monolithic_examples.h"

#define USAGE_NAME   "demo"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()
{ "demo1", { .fa = mbdemo1_main } },
{ "demo2", { .f = mbdemo2_main } },
MONOLITHIC_CMD_TABLE_END();


#include "monolithic_main_tpl.h"
