
#include <stdlib.h>
#include <stdio.h>
#include <process.h>

int main(void) {
	fprintf(stderr, "testing abort...\n");
	abort();
}
