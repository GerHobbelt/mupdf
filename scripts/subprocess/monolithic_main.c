
#include "monolithic_examples.h"

#define USAGE_NAME   "subprocess"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()
	{ "sleep", { .fa = subproc_sleep_main } },
	{ "examples", { .fa = subproc_examples_main } },
	{ "echo", { .fa = subproc_echo_main } },
	{ "cat_child", { .fa = subproc_cat_child_main } },
#if 0
	{ "basic_test", { .fa = subproc_basic_test_main } },
#endif
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
