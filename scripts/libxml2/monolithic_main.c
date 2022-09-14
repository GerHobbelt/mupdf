
#include "zlib-ng.h"

#define BUILD_MONOLITHIC 1
#include "libxml/monolithic_examples.h"

#define USAGE_NAME   "xml"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()
	{ "xml_api", {.fa = xml_testapi_main } },
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
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
