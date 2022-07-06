
#define BUILD_MONOLITHIC 1
#include "programs/monolithic_examples.h"

#define USAGE_NAME   "zstd"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()
	{ "zstd_dict_compress", {.fa = zstd_dict_compress_main } },
	{ "zstd_dict_decompress", {.fa = zstd_dict_decompress_main } },
	{ "zstd_multi_simple_compress", {.fa = zstd_multi_simple_compress_main } },
	{ "zstd_multi_stream_compress", {.fa = zstd_multi_stream_compress_main } },
	{ "zstd_simple_compress", {.fa = zstd_simple_compress_main } },
	{ "zstd_simple_decompress", {.fa = zstd_simple_decompress_main } },
	{ "zstd_stream_compress", {.fa = zstd_stream_compress_main } },
	{ "zstd_stream_compress_threadpool", {.fa = zstd_stream_compress_threadpool_main } },
	{ "zstd_stream_decompress", {.fa = zstd_stream_decompress_main } },
	{ "zstd_stream_mem_usage", {.fa = zstd_stream_mem_usage_main } },
	{ "zstd", {.fa = zstd_main } },
#if 0
	{ "zstd_fitblk_example", {.fa = zstd_fitblk_example_main } },
	{ "zstd_mini_gzip", {.fa = zstd_mini_gzip_main } },
	{ "zstd_zwrapbench", {.fa = zstd_zwrapbench_main } },
#endif

	// These require the zstd_zlib_wrapper code to be included in the build:
	{ "zlib_example", {.fa = zstd_zlib_example_main } },
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
