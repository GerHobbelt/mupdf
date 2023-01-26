
#include "../../thirdparty/owemdjee/crow/include/crow/monolithic_examples.h"

#define USAGE_NAME   "mucrow"

#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
	{ "basic", { .f = crow_example_basic_main } },
	{ "catch_all", { .f = crow_example_catch_all_main }},
	{ "chat", { .f = crow_example_chat_main } },
	{ "compress", { .f = crow_example_compression_main } },
	{ "json_map", { .f = crow_example_json_map_main } },
	{ "static", { .f = crow_example_static_file_main } },
	{ "vs", { .f = crow_example_vs_main } },
	{ "with_all", { .f = crow_example_with_all_main } },
	{ "hello", { .f = crow_hello_world_main } },
	{ "ssl", { .f = crow_example_ssl_main } },
	{ "ws", { .f = crow_example_ws_main } },
	{ "mustache", { .f = crow_mustache_main } },
	{ "blueprint", { .f = crow_example_blueprint_main } },
	{ "middleware", { .f = crow_example_middleware_main } },
	{ "test_multi_file", { .f = crow_test_multi_file_main } },
    { "test_external_definition", {.f = crow_test_external_definition_main } },
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"

