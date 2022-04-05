
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

#define BUILD_MONOLITHIC 1
#include "programs/monolithic_examples.h"


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
	fprintf(stderr, "Usage: zstd_demo [command]\n");
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

