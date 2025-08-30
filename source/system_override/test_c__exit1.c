
#include <stdlib.h>
#include <stdio.h>
#include <process.h>

// Mandatory or MSVC will optimize this entire endeavour to Kingdom Come and we won't have our debugger breakpoints at start and end...
#pragma optimize("", off)

static __declspec(noinline) void foo(void) {
	(void)0;
}

int main(void) {
	fprintf(stderr, "testing abort...\n");
	foo();
	_Exit(0);
}
