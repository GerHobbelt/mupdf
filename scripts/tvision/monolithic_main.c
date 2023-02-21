
#define BUILD_MONOLITHIC 1
#include "tvision/monolithic_examples.h"

#define USAGE_NAME   "tvision"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()

{ "mmenu", {.f = tvision_mmenu_main } },
{ "palette", { .f = tvision_palette_test_main } },
{ "tvdemo", { .fa = tvision_tvdemo_main } },
{ "tvdir", { .fa = tvision_tvdir_main } },
{ "tvedit", { .fa = tvision_tvedit_main } },
{ "genphone", { .f = tvision_genphone_main } },
{ "genparts", { .f = tvision_genparts_main } },
{ "tvforms", { .f = tvision_tvforms_main } },
{ "tvhc", { .fa = tvision_tvhc_main } },
{ "hello", { .f = tvision_hello_main } },
{ "geninc", { .f = tvision_geninc_main } },

MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
