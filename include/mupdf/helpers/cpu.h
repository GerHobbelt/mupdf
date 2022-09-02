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

#ifdef __cplusplus
}
#endif

#endif

