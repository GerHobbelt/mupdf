
#include "monolithic_examples.h"

#define USAGE_NAME   "spdlog"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()

{ "async_bench", { .fa = spdlog_async_bench_main} },
{ "bench", { .fa = spdlog_bench_main } },
{ "formatter_bench", { .fa = spdlog_formatter_bench_main } },
{ "latency_bench", { .fa = spdlog_latency_bench_main } },
{ "example", { .fa = spdlog_example_main } },
{ "example2", { .fa = spdlog_example2_main } },

MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"

