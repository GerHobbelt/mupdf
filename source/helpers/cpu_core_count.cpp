// https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine

#include "mupdf/fitz.h"
#include "mupdf/helpers/cpu.h"

#include <thread>
#include <math.h>

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

extern "C" int fz_set_cpu_core_count(float perunage_or_real_number_of_cores, int min_count, int max_count)
{
	// may return 0 when not able to detect
	int count = max(2, std::thread::hardware_concurrency());

	if (min_count < 2)
		min_count = 2;

	if (max_count < count)
		max_count = count;
	else if (max_count < min_count)
		max_count = min_count;

	int i = lroundf(count * perunage_or_real_number_of_cores);

	if (i > max_count)
		i = max_count;
	else if (i < min_count)
		i = min_count;

	processor_count = i;

	return processor_count;
}
