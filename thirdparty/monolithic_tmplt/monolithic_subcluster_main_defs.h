//
// Defines the subcluster init + exec function prototypes for use in a larger monolith.
// 

#if defined(MONOLITHIC_SUBCLUSTER_MAIN)

#ifdef __cplusplus
extern "C" {
#endif

int MONOLITHIC_SUBCLUSTER_MAIN(int argc, const char** argv);

// MSVC preproc: unfolding + function name generate only works as you'ld expect when we have this precise nested preproc call structure:
#define MONOLITHIC_SUBCLUSTER_MAIN_INIT_FEXPAND2(a)       a##_init
#define MONOLITHIC_SUBCLUSTER_MAIN_INIT_FEXPAND(a)        MONOLITHIC_SUBCLUSTER_MAIN_INIT_FEXPAND2(a)
#define MONOLITHIC_SUBCLUSTER_MAIN_INIT                   MONOLITHIC_SUBCLUSTER_MAIN_INIT_FEXPAND(MONOLITHIC_SUBCLUSTER_MAIN)

int MONOLITHIC_SUBCLUSTER_MAIN_INIT(void);

#ifdef __cplusplus
}
#endif

#endif
