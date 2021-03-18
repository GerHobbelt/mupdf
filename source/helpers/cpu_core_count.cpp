// https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine

#include "mupdf/fitz.h"
#include "mupdf/helpers/cpu.h"

#include <thread>

static int processor_count = 0;

extern "C" int fz_get_cpu_core_count(void)
{
	if (!processor_count)
	{
		// may return 0 when not able to detect
		processor_count = std::thread::hardware_concurrency();
	}
	return processor_count;
}
