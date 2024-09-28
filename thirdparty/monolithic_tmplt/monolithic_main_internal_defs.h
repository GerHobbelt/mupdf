//
// Part 1 of the template code: core definitions
//

#pragma once

#ifdef __cplusplus
#include <string>
#include <exception>
#include <stdexcept>
#endif
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

#ifdef _WIN32
#include "windows.h"
#include "shellscalingapi.h"
#endif



#ifdef __cplusplus
extern "C" {
#endif

typedef int tool_f(void);
typedef int tool_fa(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
typedef int tool_f_cpp();
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int help(void);
static int quit(void);
static int usage(void);

struct cmd_info
{
	const char* cmd;
	union
	{
		tool_f* f;
		tool_fa* fa;
#ifdef __cplusplus
		tool_f_cpp* fpp;
#endif
	} f;
};

#define MONOLITHIC_CMD_TABLE_START()				\
static const struct cmd_info commands[] = 	\
{

#define MONOLITHIC_CMD_TABLE_END()	        \
}

#ifdef __cplusplus
}
#endif
