
#include "mupdf/fitz/context.h"

#define BUILD_MONOLITHIC 1
#include "../../thirdparty/owemdjee/replxx/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS/monolithic_examples.h"
#include "../../thirdparty/mujs/monolithic_examples.h"

#define USAGE_NAME   "qjsrepl"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()

	{"cpp_api", {.fa = replxx_cpp_api_main}},
	{"c_api", {.fa = replxx_c_api_main}},

	{"qjscompress", {.fa = qjscompress_main}},
	{"qjs", {.fa = qjs_main}},
	{"qjsc", {.fa = qjsc_main}},
	{"qjs_unicode_gen", {.fa = qjs_unicode_gen_main}},
	{"qjs_test262", {.fa = qjs_test262_main}},

	{"mujs_example", {.fa = mujs_example_main}},
	{"mujs_prettyprint", {.fa = mujs_prettyprint_main}},

MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"



// ----------------------


extern "C" void fz_error(fz_context* ctx, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

