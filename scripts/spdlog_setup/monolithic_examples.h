
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

int mbdemo1_main(int argc, const char** argv);
int spdlog_async_bench_main(int argc, const char** argv);
int spdlog_bench_main(int argc, const char** argv);
int spdlog_formatter_bench_main(int argc, const char** argv);
int spdlog_latency_bench_main(int argc, const char** argv);
int spdlog_example_main(int argc, const char** argv);
int spdlog_example2_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
