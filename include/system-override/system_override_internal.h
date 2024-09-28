
#pragma once

#ifndef __FZ_SYSTEM_OVERRIDE_H__
#define __FZ_SYSTEM_OVERRIDE_H__

#if defined(_WIN32)
#define _CRT_FUNCTIONS_REQUIRED  1
#define _CRT_INTERNAL_NONSTDC_NAMES  1
#include <crtdbg.h>
#include <windows.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif 

__declspec(noreturn) void invoke_original_abort(void);      // WARNING @ user-land code: internal use only; DO NOT USE.
__declspec(noreturn) void invoke_original_exit(int code);   // WARNING @ user-land code: internal use only; DO NOT USE.

void SystemOverride_KickInTheDoor(void);

__declspec(noreturn) void invoke_abort(void);
__declspec(noreturn) void invoke_exit(int code);

void BreakIntoDebugger(void);   // WARNING @ user-land code: call system_override::SystemOverrideClass::KickInTheDoor() instead, or its C equivalent SystemOverride_KickInTheDoor().

#ifdef __cplusplus
}
#endif 

#endif
