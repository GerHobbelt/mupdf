
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


#define main(cnt, arr)      xml_gio_bread_example_main(cnt, arr)        Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\example\gjobread.c       290     1
#define main(cnt, arr)      xml_nanoftp_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\nanoftp.c        2074    1
#define main(cnt, arr)      xml_nanoftp_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\nanoftp.c        2117    1
#define main(cnt, arr)      xml_nanohttp_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\nanohttp.c       1873    1
#define main(cnt, arr)      xml_nanohttp_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\nanohttp.c       1901    1
#define main(cnt, arr)      xml_runsuite_tests_main(cnt, arr)   Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\runsuite.c       1045    1
#define main(cnt, arr)      xml_runsuite_tests_main(cnt, arr)   Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\runsuite.c       1168    1
#define main(cnt, arr)      xml_runtest_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\runtest.c        4579    1
#define main(cnt, arr)      xml_runtest_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\runtest.c        4637    1
#define main(cnt, arr)      xml_runxmlconfig_main(cnt, arr)     Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\runxmlconf.c     561     1
#define main(cnt, arr)      xml_runxmlconfig_main(cnt, arr)     Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\runxmlconf.c     617     1
#define main(cnt, arr)      xml_schematron_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\schematron.c     1745    1
#define main(cnt, arr)      xml_testapi_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testapi.c        121     1
#define main(cnt, arr)      xml_testAutomata_main(cnt, arr)     Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testAutomata.c   218     1
#define main(cnt, arr)      xml_testAutomata_main(cnt, arr)     Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testAutomata.c   310     1
#define main(cnt, arr)      xml_testC14N_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testC14N.c       137     1
#define main(cnt, arr)      xml_testC14N_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testC14N.c       374     1
#define main(cnt, arr)      xml_testchar_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testchar.c       588     1
#define main(cnt, arr)      xml_testdict_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testdict.c       426     1
#define main(cnt, arr)      xml_testhtml_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testHTML.c       802     1
#define main(cnt, arr)      xml_testhtml_main(cnt, arr) Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testHTML.c       882     1
#define main(cnt, arr)      xml_testlimits_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testlimits.c     1605    1
#define main(cnt, arr)      xml_testmodule_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testModule.c     45      1
#define main(cnt, arr)      xml_testmodule_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testModule.c     85      1
#define main(cnt, arr)      xml_testOOM_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testOOM.c        320     1
#define main(cnt, arr)      xml_testreader_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testReader.c     106     1
#define main(cnt, arr)      xml_testreader_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testReader.c     148     1
#define main(cnt, arr)      xml_testrecurse_main(cnt, arr)      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testrecurse.c    938     1
#define main(cnt, arr)      xml_testregexp_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testRegexp.c     272     1
#define main(cnt, arr)      xml_testregexp_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testRegexp.c     405     1
#define main(cnt, arr)      xml_testrelax_main(cnt, arr)        Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testRelax.c      59      1
#define main(cnt, arr)      xml_testrelax_main(cnt, arr)        Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testRelax.c      191     1
#define main(cnt, arr)      xml_testSAX_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testSAX.c        1193    1
#define main(cnt, arr)      xml_testSAX_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testSAX.c        1263    1
#define main(cnt, arr)      xml_testschemas_main(cnt, arr)      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testSchemas.c    59      1
#define main(cnt, arr)      xml_testschemas_main(cnt, arr)      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testSchemas.c    182     1
#define main()      xml_testthreads_main()      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testThreads.c    111     1
#define main()      xml_testthreads_main()      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testThreads.c    166     1
#define main()      xml_testthreads_main()      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testThreads.c    227     1
#define main()      xml_testthreads_main()      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testThreads.c    285     1
#define main()      xml_testthreads_main()      Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testThreads.c    298     1
#define main(cnt, arr)      xml_testURI_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testURI.c        82      1
#define main(cnt, arr)      xml_testXPath_main(cnt, arr)        Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testXPath.c      145     1
#define main(cnt, arr)      xml_testXPath_main(cnt, arr)        Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\testXPath.c      232     1
#define main()      xml_trionan_main()  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\trionan.c        814     1
#define main(cnt, arr)      xml_xmlcatalog_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\xmlcatalog.c     332     1
#define main(cnt, arr)      xml_xmlcatalog_main(cnt, arr)       Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\xmlcatalog.c     627     1
#define main(cnt, arr)      xml_xmllint_main(cnt, arr)  Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\xmllint.c        3187    1
#define main(cnt, arr)      xml_test_xmlreader_main(cnt, arr)   Z:\lib\tooling\qiqqa\MuPDF\thirdparty\owemdjee\libxml2\xmlreader.c      5960    1


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

