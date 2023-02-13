
#include "mupdf/fitz/context.h"

#define BUILD_MONOLITHIC 1
#include "../../thirdparty/owemdjee/replxx/examples/monolithic_examples.h"
#include "../../thirdparty/owemdjee/QuickJS/monolithic_examples.h"

#define USAGE_NAME   "qjscalc"

#include "monolithic_main_internal_defs.h"

int main(int argc, const char **argv)
{
	return qjs_main(argc, argv);
}
