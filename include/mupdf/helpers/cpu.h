#ifndef __MUPDF_HELPERS_CPU_H__
#define __MUPDF_HELPERS_CPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mupdf/fitz/version.h"
#include "mupdf/fitz/config.h"
#include "mupdf/fitz/system.h"

/**
  Return the number of logical cores in the CPU.

  Return 0 if this number cannot be determined.

  NOTE: this datapoint is determines once, at the time of the first call into this function.
  When you wish to adjust or re-set the core count returned by this function later on, you can use the `fz_set_cpu_core_count()` API,
  where you can specify a multiplying factor and lower+upper bounds on this number.
*/
int fz_get_cpu_core_count(void);

/**
  Set the number of logical cores in the CPU, multiplied by a factor (`perunage_or_real_number_of_cores`): 1.0f means 100%, i.e. take the real value.

  A lower and upper bound can be specified:
  - when the lower bound is 2 or less, 2 is assumed, i.e. the lower bound in the number of cores will always be 2 or greater.
  - when the upper bound is below the lower bound, it is assumed to equal the lower bound.

    Hence to quickly set a hardcoded, fixed, number of cores is to set the number in the lower and upper bound parameters,
    irrespective of the multiplier.

  Returns the number of cores set.
*/
int fz_set_cpu_core_count(float perunage_or_real_number_of_cores, int min_count, int max_count);

/**
    Parse the requested thread count (string value).

    You can specify the thread count in various ways:

    - as a direct POSITIVE integer number, e.g. "4"
    - as equal to the number of (virtual) cores available on your machine: "N" or "*"
    - as a *percentage* of the number of (virtual) cores available on your machine, e.g. "50%" or "200%" (the latter will then, obviously, allocate twice as many threads as you have cores)
    - as a NEGATIVE integer number, meaning "number of cores MINUS this number", e.g. "-1". This comes in handy when you want to keep one or more cores completely free and available for other work, e.g. GUI message pumping.

    When the input string value is NULL or does not match any of the above, the returned value is "number of available cores", i.e. identical to when you would have specified `"N"`.

    All parsed input values are sanity-checked and are clipped to a "sensible range": the accepted input is \[1 .. N*32\] (i.e.: "from 1 up to and including 32 times the number of available CPU cores")

    When the number of available CPU cores cannot be properly detected by the underlying code, *4* is assumed as most medium-grade consumer hardware these days is based on 2 cores times 2 ("hyperthreading") hence *4* virtual cores.

    You MAY override the default count ("N") -- used in case the parse failed to match any of the rules stated above -- by specifying an explicit default via a non-NULL `default_count` argument. This value is NOT sanity-checked and range-limited following the rules above, so you MAY pass in, f.e., a NEGATIVE number to signal a parse error, to be further dealt with by your userland code.

    P.S.: when you wish to provide a different number for `N`, we advise you to use the `fz_set_cpu_core_count()` API before this call.
*/
int fz_parse_pool_threadcount_preference(const char *spec, int min_count, int max_count, int default_count);

#ifdef __cplusplus
}
#endif

#endif

