
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

#include "zlib-ng.h"

#define BUILD_MONOLITHIC 1
#include "libxml/monolithic_examples.h"


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
	{ "xml_api", {.fa = xml_testapi_main } },
	{ "xml_Automata", {.fa = xml_testAutomata_main } },
	{ "xml_C14N", {.fa = xml_testC14N_main } },
	{ "xml_catalog", {.fa = xml_xmlcatalog_main } },
	{ "xml_char", {.fa = xml_testchar_main } },
	{ "xml_dict", {.fa = xml_testdict_main } },
	{ "xml_gio_bread", {.fa = xml_gio_bread_example_main } },
	{ "xml_html", {.fa = xml_testhtml_main } },
	{ "xml_limits", {.fa = xml_testlimits_main } },
	{ "xml_lint", {.fa = xml_xmllint_main } },
	{ "xml_module", {.fa = xml_testmodule_main } },
	{ "xml_nanoftp", {.fa = xml_nanoftp_main } },
	{ "xml_nanohttp", {.fa = xml_nanohttp_main } },
	{ "xml_OOM", {.fa = xml_testOOM_main } },
	{ "xml_reader", {.fa = xml_testreader_main } },
	{ "xml_recurse", {.fa = xml_testrecurse_main } },
	{ "xml_regexp", {.fa = xml_testregexp_main } },
	{ "xml_relax", {.fa = xml_testrelax_main } },
	{ "xml_runsuite", {.fa = xml_runsuite_tests_main } },
	{ "xml_runtest", {.fa = xml_runtest_main } },
	{ "xml_runxmlconfig", {.fa = xml_runxmlconfig_main } },
	{ "xml_SAX", {.fa = xml_testSAX_main } },
	{ "xml_schemas", {.fa = xml_testschemas_main } },
	{ "xml_schematron", {.fa = xml_schematron_main } },
	{ "xml_URI", {.fa = xml_testURI_main } },
#ifdef NOT_USED_YET
	{ "xml_xmlreader", {.fa = xml_test_xmlreader_main } },
#endif
	{ "xml_XPath", {.fa = xml_testXPath_main } },

	{ "xml_threads", {.f = xml_testthreads_main } },
	{ "xml_trionan", {.f = xml_trionan_main } },

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
	zng_lib_init();

	return parse(argv[1], argc - 1, argv + 1);
}

