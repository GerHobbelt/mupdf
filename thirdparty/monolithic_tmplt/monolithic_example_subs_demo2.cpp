//
// part of Demo 
//

#include <stdio.h>
#include <stdlib.h>

#include "monolithic_examples.h"


#if defined(BUILD_MONOLITHIC)
#define main   mbdemo2_main
#endif

int main(void)
{
	printf("Running demo2.\n\n");
	printf("Exit code: 42\n\n\n");
	return 42;
}

