
#include "system_override_internal.h"


// Mandatory or MSVC will optimize this entire endeavour to Kingdom Come and we won't have our debugger breakpoints at start and end...
#pragma optimize("", off)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
extern int system_override_has_been_initialized;

static int init = system_override_has_been_initialized;

