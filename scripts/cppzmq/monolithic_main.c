
#define BUILD_MONOLITHIC 1
#include "examples/monolithic_examples.h"

#define USAGE_NAME   "cppzmq"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()
	{ "clipp_actions", {.fa = clipp_actions_main } },
	{ "clipp_sanity", {.f = clipp_sanity_main } },
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
