#ifndef __MUPDF_HELPERS_CPU_H__
#define __MUPDF_HELPERS_CPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mupdf/fitz/version.h"
#include "mupdf/fitz/config.h"
#include "mupdf/fitz/system.h"

	int fz_get_cpu_core_count(void);

#ifdef __cplusplus
}
#endif

#endif

