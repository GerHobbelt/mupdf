
#define BUILD_MONOLITHIC 1
#include "examples/monolithic_examples.h"

#define USAGE_NAME   "clipp"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()
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
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
