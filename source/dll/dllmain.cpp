// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "mupdf/helpers/system-header-files.h"

#include "zlib-ng.h"


#if defined(_WIN32)

#ifdef __cplusplus
extern "C" {
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
		zng_lib_init();
		break;

    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */

