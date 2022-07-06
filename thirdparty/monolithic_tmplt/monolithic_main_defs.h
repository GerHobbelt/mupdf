//
// Example: core content of each `monolithic_examples.h` header file, which carries the declarations of the various `main()` functions we want to bundle in a monolithic build.
//

#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

// extern int XYZ_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
