//
// part of Demo 
//

#include <stdio.h>
#include <stdlib.h>

#include "monolithic_examples.h"


#if defined(BUILD_MONOLITHIC)
#define main   mbdemo1_main
#endif

int main(int argc, const char** argv)
{
	printf("Running demo1.\n\n");
	for (int i = 0; i < argc; i++)
	{
		printf("  ARGV[%u] = \"%s\"\n", i, argv[i]);
}
	printf("\n\nExit code: 0 (SUCCESS)\n\n\n");
	return 0;
}

