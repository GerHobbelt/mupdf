
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "cpptrace_demo"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
{ "asi_test", { .f = cpptrace_asi_test_main } },
{ "benchmark", { .fa = cpptrace_benchmark_main } },
{ "ctrace_demo", { .f = cpptrace_ctrace_demo_main } },
{ "demo", { .f = cpptrace_demo_main } },
{ "fci_test", { .f = cpptrace_fci_test_main } },
{ "fpi_test", { .f = cpptrace_fpi_test_main } },
{ "integration", { .f = cpptrace_integration_main } },
#if !defined(_WIN32)
{ "signal_demo", { .f = cpptrace_signal_demo_main } },
#endif
{ "signal_tracer", { .f = cpptrace_signal_tracer_main } },
{ "tests", { .fa = cpptrace_tests_main } },
MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
