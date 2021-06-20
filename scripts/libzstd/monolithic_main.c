
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif
#include <errno.h>

#include "examples/monolithic_examples.h"


typedef int tool_f();
typedef int tool_fa(int argc, const char** argv);

static int usage();


static struct cmd_info
{
	const char* cmd;
	union
	{
		tool_f* f;
		tool_fa* fa;
	} f;
} commands[] = {

#define main(cnt, arr)      zstd_dict_compress_main(cnt, arr)   Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\dictionary_compression.c   71      1
#define main(cnt, arr)      zstd_dict_decompress_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\dictionary_decompression.c 79      1
#define main(cnt, arr)      zstd_multi_simple_compress_main(cnt, arr)   Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\multiple_simple_compression.c      84      1
#define main(cnt, arr)      zstd_multi_stream_compress_main(cnt, arr)   Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\multiple_streaming_compression.c   97      1
#define main(cnt, arr)      zstd_simple_compress_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\simple_compression.c       53      1
#define main(cnt, arr)      zstd_simple_decompress_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\simple_decompression.c     50      1
#define main(cnt, arr)      zstd_stream_compress_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\streaming_compression.c    107     1
#define main(cnt, arr)      zstd_stream_compress_threadpool_main(cnt, arr)      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\streaming_compression_thread_pool.c        129     1
#define main(cnt, arr)      zstd_stream_decompress_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\streaming_decompression.c  87      1
#define main(cnt, arr)      zstd_stream_mem_usage_main(cnt, arr)        Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\examples\streaming_memory_usage.c   50      1
#define main(cnt, arr)      zstd_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\programs\zstdcli.c  745     1
#define main(cnt, arr)      zlib_example_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\zlibWrapper\examples\example.c      1006    1
#define main(cnt, arr)      zlib_example_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\zlibWrapper\examples\example_original.c     1007    1
#define main(cnt, arr)      zstd_fitblk_example_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\zlibWrapper\examples\fitblk.c       141     1
#define main(cnt, arr)      zstd_mini_gzip_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\zlibWrapper\examples\minigzip.c     284     1
#define main(cnt, arr)      zstd_mini_gzip_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\zlibWrapper\examples\minigzip.c     391     1
#define main(cnt, arr)      zstd_zwrapbench_main(cnt, arr)      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\zstd\zlibWrapper\examples\zwrapbench.c   875     1
#define main(v)      zlib_adler32_test_main(v)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\adler32_test.c  360     1
#define main(v)      zlib_deflate_quick_bi_valid_test_main(v)   Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\deflate_quick_bi_valid.c        16      1
#define main(v)      zlib_deflate_quick_block_open_test_main(v) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\deflate_quick_block_open.c      16      1
#define main(cnt, arr)      zlib_example_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\example.c       1007    1
#define main(v)      zlib_hash_head_0_test_main(v)      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\hash_head_0.c   16      1
#define main(v)      zlib_infcover_test_main(v) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\infcover.c      675     1
#define main(cnt, arr)      zlib_mini_deflate_main(cnt, arr)    Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\minideflate.c   232     1
#define main(cnt, arr)      zlib_mini_gzip_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\minigzip.c      280     1
#define main(cnt, arr)      zlib_mini_gzip_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\minigzip.c      387     1
#define main(cnt, arr)      zlib_switchlevels_main(cnt, arr)    Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\test\switchlevels.c  124     1
#define main(cnt, arr)      zlib_mk_crc32_tables_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\tools\makecrct.c     170     1
#define main(v)      zlib_mk_fixed_table_main(v)        Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\tools\makefixed.c    88      1
#define main(v)      zlib_mk_trees_header_main(v)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\zlib\tools\maketrees.c    145     1

	{ "clipp_actions", {.fa = clipp_actions_main } },
	{ "clipp_align", {.fa = clipp_align_main } },
	{ "clipp_alternatives", {.fa = clipp_alternatives_main } },
	{ "clipp_annotate", {.fa = clipp_annotate_main } },
	{ "clipp_argv0", {.fa = clipp_argv0_main } },
	{ "clipp_commands", {.fa = clipp_commands_main } },
	{ "clipp_complex_nesting", {.fa = clipp_complex_nesting_main } },
	{ "clipp_convert", {.fa = clipp_convert_main } },
	{ "clipp_counter", {.fa = clipp_counter_main } },
	{ "clipp_doc_filter", {.fa = clipp_doc_filter_main } },
	{ "clipp_documentation", {.fa = clipp_documentation_main } },
	{ "clipp_finder", {.fa = clipp_finder_main } },
	{ "clipp_float_vector", {.fa = clipp_float_vector_main } },
	{ "clipp_groups", {.fa = clipp_groups_main } },
	{ "clipp_joinable_flags", {.fa = clipp_joinable_flags_main } },
	{ "clipp_model", {.fa = clipp_model_main } },
	{ "clipp_naval_fate", {.fa = clipp_naval_fate_main } },
	{ "clipp_nested_alternatives", {.fa = clipp_nested_alternatives_main } },
	{ "clipp_numbers", {.fa = clipp_numbers_main } },
	{ "clipp_options", {.fa = clipp_options_main } },
	{ "clipp_options_values", {.fa = clipp_options_values_main } },
	{ "clipp_parsing", {.fa = clipp_parsing_main } },
	{ "clipp_positional_values", {.fa = clipp_positional_values_main } },
	{ "clipp_repeatable", {.fa = clipp_repeatable_main } },
	{ "clipp_required_flags", {.fa = clipp_required_flags_main } },
	{ "clipp_send", {.fa = clipp_send_main } },
	{ "clipp_simplify", {.fa = clipp_simplify_main } },
	{ "clipp_switches", {.fa = clipp_switches_main } },
	{ "clipp_tagnames", {.fa = clipp_tagnames_main } },
	{ "clipp_text_formatting", {.fa = clipp_text_formatting_main } },
	{ "clipp_timing", {.fa = clipp_timing_main } },
	{ "clipp_transform", {.fa = clipp_transform_main } },

	{ "clipp_sanity", {.f = clipp_sanity_main } },

	{ "?", {.f = usage } },
	{ "h", {.f = usage } },
	{ "help", {.f = usage } },
	{ "-?", {.f = usage } },
	{ "-h", {.f = usage } },
	{ "--help", {.f = usage } },
};


static void help(void)
{
	fprintf(stderr, "Commands:\n");
	for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
	{
		struct cmd_info el = commands[i];
		fprintf(stderr, "  %s\n", el.cmd);
	}
}


static int usage()
{
	fprintf(stderr, "Usage: clipp_demo [command]\n");
	help();
	return 1;
}


static int parse(const char* source, int argc, const char** argv)
{
	if (source)
	{
		for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
		{
			struct cmd_info el = commands[i];
			size_t cmd_len = strlen(el.cmd);
			char sentinel = 0;

			if (strlen(source) >= cmd_len)
				sentinel = source[cmd_len];

			if (strncmp(source, el.cmd, cmd_len) == 0 && (sentinel == 0 || isspace(sentinel)))
			{
				return el.f.fa(argc, argv);
			}
		}

		fprintf(stderr, "Unknown command '%s'.\n\nUse 'h' or 'help' command to get a list of supported commands.\n", source);
		return 1;
	}
	else
	{
		return usage();
	}
}

int main(int argc, const char** argv)
{
	return parse(argv[1], argc - 1, argv + 1);
}

