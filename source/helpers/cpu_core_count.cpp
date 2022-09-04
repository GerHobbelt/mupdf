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
		// std::thread::hardware_concurrency() may return 0 when not able to detect.
		//
		// use fz_set_cpu_core_count() instead.
		return fz_set_cpu_core_count(0.0f, 0, INT_MAX);
	}
	return processor_count;
}

extern "C" int fz_set_cpu_core_count(float perunage_or_real_number_of_cores, int min_count, int max_count)
{
	// may return 0 when not able to detect
	//
	// When this happens, *2* is assumed as we expect these failures to occur on older hardware, which usually has only 1 or 2 cores.
	int count = std::thread::hardware_concurrency();

	if (perunage_or_real_number_of_cores == 0.0f)  // no need for approx match
		perunage_or_real_number_of_cores = 1.0f;

	if (min_count < 2)
		min_count = 2;

	if (count < min_count)
		count = min_count;

	if (max_count < count)
		max_count = count;

	int i = lroundf(count * perunage_or_real_number_of_cores);

	if (i > max_count)
		i = max_count;
	else if (i < min_count)
		i = min_count;

	processor_count = i;

	return processor_count;
}

extern "C" int fz_parse_pool_threadcount_preference(const char *spec, int min_count, int max_count, int default_count)
{
	int N = fz_get_cpu_core_count();

	int dflt = N;
	if (default_count)
		dflt = default_count;

	if (!min_count)
		min_count = 1;

	if (!max_count)
		max_count = N * 32;

	if (max_count < min_count)
		max_count = min_count;

	int count = N;

	int v = dflt;

	if (spec)
	{
		// skip whitespace
		spec += strspn(spec, " \t\r\n");

		if (toupper(*spec) == 'N' || *spec == '*')
		{
			v = N;

			// clip input to sanity range:
			if (v < min_count)
				v = min_count;
			else if (v > max_count)
				v = max_count;

			spec++;
		}
		else
		{
			const char *p = spec;
			long lv = strtol(spec, (char **)&p, 0);

			// skip whitespace
			p += strspn(p, " \t\r\n");

			// when there's still an input parse tail, the parse via strtol() is considered FAILED,
			// UNLESS it's a '%' character and nothing else.
			//
			// when it did not consume any characters, we look for 'N' & '*'; anything else a FAIL.
			if (p > spec && *p == '%')
			{
				p++;

				lv = (N * lv) / 100;
			}
			else if (lv < 0)
			{
				lv = N - lv;
			}

			// clip input to sanity range:
			if (lv < min_count)
				v = min_count;
			else if (lv > max_count)
				v = max_count;
			else
				v = (int)lv;

			spec = p;
		}

		// skip whitespace
		spec += strspn(spec, " \t\r\n");


		// any tail? yes --> FAIL.
		if (*spec)
		{
			v = dflt;
		}
	}

	return v;
}
