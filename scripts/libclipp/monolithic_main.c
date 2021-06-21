
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
#include "examples/monolithic_examples.h"


typedef int tool_f();
typedef int tool_fa(int argc, const char **argv);

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

	{ "?", { .f = usage } },
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


static int parse(const char* source, int argc, const char **argv)
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

