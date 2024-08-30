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
typedef int tool_f_cpp();

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
		tool_f_cpp* fpp;
	} f;
};

#define MONOLITHIC_CMD_TABLE_START()				\
static const struct cmd_info commands[] = 	\
{

#define MONOLITHIC_CMD_TABLE_END()	        \
}

// APIs for use in the generic startup code when you're writing your own main monolithic exec wrapper:

int setup_exe_for_monitor_dpi_etc(void);
int setup_exe_for_utf8_console_locale(void);

#ifdef __cplusplus
}
#endif
