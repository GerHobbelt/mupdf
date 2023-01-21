//
// part of Demo 
//

#pragma once

#if !defined(MONOLITHIC_TEMPLATE_DEMO)
#error "You are missing an include path pointing at your own monolithic_example.h as you are loading this template demo one instead!"
#endif

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

int mbdemo1_main(int argc, const char** argv);
int mbdemo2_main(void);

#ifdef __cplusplus
}
#endif

#endif
