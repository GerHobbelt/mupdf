//
// part of Demo 
//

#include <stdio.h>
#include <stdlib.h>

#include "monolithic_examples.h"


int mbdemo1_main(int argc, const char** argv)
{
	printf("Running demo1.\n\n");
	for (int i = 0; i < argc; i++)
	{
		printf("  ARGV[%u] = \"%s\"\n", i, argv[i]);
}
	printf("\n\nExit code: 0 (SUCCESS)\n\n\n");
	return 0;
}

int mbdemo2_main(void)
{
	printf("Running demo2.\n\n");
	printf("Exit code: 42\n\n\n");
	return 42;
}

