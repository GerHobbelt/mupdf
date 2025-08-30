
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <signal.h>
#include <assert.h>

// Mandatory or MSVC will optimize this entire endeavour to Kingdom Come and we won't have our debugger breakpoints at start and end...
#pragma optimize("", off)

static void signal_handler(int signal)
{
	if (signal == SIGABRT)
		fprintf(stderr, "SIGABRT received\n");
	else
		fprintf(stderr, "Unexpected signal %d received\n", signal);
	exit(EXIT_FAILURE);
}

static __declspec(noinline) void foo(void) {
	(void)0;
}

int main(void) {
	_crt_signal_t previous_handler = signal(SIGABRT, signal_handler);
	if (previous_handler == SIG_ERR)
	{
		fprintf(stderr, "Setup failed\n");
		return EXIT_FAILURE;
	}

	fprintf(stderr, "testing abort...\n");
	foo();

	raise(SIGABRT); // Raise SIGABRT
	//abort(); // <-- this one only raises SIGABRT in POSIX boxes, so we do it explicitly in the line above.
	assert(0 && "Should never get here!");
	return 666;
	abort();
}
