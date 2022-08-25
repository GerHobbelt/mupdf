//
// Part 1 of the template code: core definitions
//

#pragma once

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

#define MONOLITHIC_CMD_TABLE_START()	\
static struct cmd_info commands[] = 	\
{

#define MONOLITHIC_CMD_TABLE_END()	\
    { "?", {.f = usage } },			\
    { "h", {.f = usage } },			\
    { "help", {.f = usage } },		\
    { "-?", {.f = usage } },		\
    { "-h", {.f = usage } },		\
    { "--help", {.f = usage } },	\
}


