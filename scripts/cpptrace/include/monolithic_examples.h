
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

	int cpptrace_asi_test_main(void);
	int cpptrace_benchmark_main(int argc, const char** argv);
	int cpptrace_ctrace_demo_main(void);
	int cpptrace_demo_main(void);
	int cpptrace_fci_test_main(void);
	int cpptrace_fpi_test_main(void);
	int cpptrace_integration_main(void);
	int cpptrace_signal_demo_main(void);
	int cpptrace_signal_tracer_main(void);
	int cpptrace_tests_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
