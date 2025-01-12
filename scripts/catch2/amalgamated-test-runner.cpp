
#include <stdio.h>
#include <stdlib.h>

extern "C"
int catch2_runner_main(int argc, const char** argv);

// Standard C/C++ main entry point
extern "C"
int main(int argc, const char** argv) {
	return catch2_runner_main(argc, argv);
}
