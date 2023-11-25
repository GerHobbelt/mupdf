
#define MONOLITHIC_SUBCLUSTER_MAIN  glfw_monolithic_subcluster_main

#include "monolithic_subcluster_main_defs.h"


int main(int argc, const char** argv)
{
	int rv = glfw_monolithic_subcluster_main_init();
	return glfw_monolithic_subcluster_main(argc, argv);
}
