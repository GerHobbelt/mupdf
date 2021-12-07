
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

	extern int subproc_sleep_main(int argc, const char** argv);
	extern int subproc_printenv_main(int argc, const char** argv);
	extern int subproc_examples_main(int argc, const char** argv);
	extern int subproc_echo_main(int argc, const char** argv);
	extern int subproc_cat_child_main(int argc, const char** argv);
#if 0
	extern int subproc_basic_test_main(int argc, const char** argv);
#endif

#ifdef __cplusplus
}
#endif

#endif
