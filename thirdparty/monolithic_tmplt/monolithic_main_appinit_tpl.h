
#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include "shellscalingapi.h"
#endif

#if 0 // set to 1 when you need to debug the init code further below.
#define DEBUG_MONOLITH_INIT_CODE(...)    fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_MONOLITH_INIT_CODE(...)    ((void)0)
#endif

#if defined(_MSC_VER)
#define MONOLITH_NO_INLINE			__declspec(noinline)
#else
#define MONOLITH_NO_INLINE			__attribute__((noinline))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#define MONOLITH_GLOBAL_SCOPE     ::
#else
#define MONOLITH_GLOBAL_SCOPE     
#endif

// APIs for use in the generic startup code when you're writing your own main monolithic exec wrapper:

static MONOLITH_NO_INLINE int setup_exe_for_monitor_dpi_etc(void)
{
#ifdef _WIN32
#pragma comment(lib,"shcore.lib")

		int r = 0;
		MONOLITH_GLOBAL_SCOPE SetLastError(S_OK);
		PROCESS_DPI_AWARENESS dpi = (PROCESS_DPI_AWARENESS)(-1);
		HRESULT rv = MONOLITH_GLOBAL_SCOPE GetProcessDpiAwareness(NULL, &dpi);
		DWORD err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("GetProcessDpiAwareness --> %zu, %zu, %zu\n", (size_t)rv, (size_t)dpi, (size_t)err);
		r += (rv == S_OK);
		r += (dpi >= 0);
		HMONITOR mon = NULL;
		const POINT zero = {0, 0};
		mon = MONOLITH_GLOBAL_SCOPE MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
		UINT x = 1000000;
		UINT y = 1000000;
		rv = MONOLITH_GLOBAL_SCOPE GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &x, &y);
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("GetDpiForMonitor --> %zu, %zu\n", (size_t)rv, (size_t)err);
		r += (rv == S_OK);

		// are we running in a console or similar way, where this application actually *has* a window?
		HWND h = GetShellWindow();
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("GetShellWindow --> %zu, %zu\n", (size_t)h, (size_t)err);
		r += (h != NULL);

		HWND w = MONOLITH_GLOBAL_SCOPE GetConsoleWindow();
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("GetConsoleWindow --> %zu, %zu\n", (size_t)w, (size_t)err);
		r += (w != NULL);

		INT d = MONOLITH_GLOBAL_SCOPE GetDpiForWindow(NULL);
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("GetDpiForWindow --> %zu, %zu\n", (size_t)d, (size_t)err);
		r += (d >= 0);
		DEVICE_SCALE_FACTOR scale = (DEVICE_SCALE_FACTOR)(-1);
		rv = MONOLITH_GLOBAL_SCOPE GetScaleFactorForMonitor(mon, &scale);
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("GetScaleFactorForMonitor --> %zu, %zu\n", (size_t)rv, (size_t)err);
		r += (rv == S_OK);

		if (MONOLITH_GLOBAL_SCOPE IsWindow(w)) {
			d = MONOLITH_GLOBAL_SCOPE GetDpiForWindow(w);
			err = MONOLITH_GLOBAL_SCOPE GetLastError();
			DEBUG_MONOLITH_INIT_CODE("GetDpiForWindow --> %zu, %zu\n", (size_t)d, (size_t)err);
			r += (d > 0);
			MONOLITH_GLOBAL_SCOPE SetLastError(S_OK);
			mon = MONOLITH_GLOBAL_SCOPE MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST);
			err = MONOLITH_GLOBAL_SCOPE GetLastError();
			DEBUG_MONOLITH_INIT_CODE("MonitorFromWindow --> %zu, %zu\n", (size_t)mon, (size_t)err);
			rv = MONOLITH_GLOBAL_SCOPE GetScaleFactorForMonitor(mon, &scale);
			err = MONOLITH_GLOBAL_SCOPE GetLastError();
			DEBUG_MONOLITH_INIT_CODE("GetScaleFactorForMonitor --> %zu, %zu\n", (size_t)rv, (size_t)err);
			r += (rv == S_OK);
		}
		else {
			// we're not running in a console/shell, so there's no ConsoleWindow available!
			// We need to compensate for that.
			r += 2;
		}

		MONOLITH_GLOBAL_SCOPE SetLastError(S_OK);
		rv = MONOLITH_GLOBAL_SCOPE SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("SetProcessDpiAwareness --> %zu, %zu\n", (size_t)rv, (size_t)err);
		r += (rv == S_OK);
		rv = MONOLITH_GLOBAL_SCOPE SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("SetProcessDpiAwareness --> %zu, %zu\n", (size_t)rv, (size_t)err);
		r += (rv == S_OK);

		DEBUG_MONOLITH_INIT_CODE("setup_exe_for_monitor_dpi_etc --> %d\n", r);
		return r >= 9 ? 0 : 1 + r;   // 0 is success; at least one of the SetProcessDpiAwareness() calls must have passed successfully then.
#else
		return 0;
#endif
}

static MONOLITH_NO_INLINE int setup_exe_for_utf8_console_locale(void)
{
#ifdef _WIN32
		/*
		 * Select UTF-8 locale.  This will change the way how C runtime
		 * functions such as fopen() and mkdir() handle character strings.
		 * For more information, please see:
		 * https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-160#utf-8-support
		 */
		char *srv = setlocale(LC_ALL, "LC_CTYPE=.utf8");
		DEBUG_MONOLITH_INIT_CODE("setlocale --> %s\n", srv);
		int r = (srv != NULL && strstr(srv, "utf8"));
		/* Select UTF-8 locale */
		//setlocale(LC_ALL, ".utf8");
		UINT ocp; // = GetConsoleOutputCP();
		UINT ccp; // = GetConsoleCP();
		MONOLITH_GLOBAL_SCOPE SetLastError(S_OK);
		HWND w = MONOLITH_GLOBAL_SCOPE GetConsoleWindow();
		DWORD err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("GetConsoleWindow --> %zu, %zu\n", (size_t)w, (size_t)err);
		r += (w != NULL);

		// when we're not running in a console window, the next couple of Win32 API calls are expected to FAIL:
		// we need to compensate for that?
		if (! MONOLITH_GLOBAL_SCOPE IsWindow(w)) {
			r += 1;
		}

		int rv = MONOLITH_GLOBAL_SCOPE SetConsoleCP(CP_UTF8);
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("SetConsoleCP --> %zu, %zu\n", (size_t)rv, (size_t)err);
		r += (rv != 0);
		rv = MONOLITH_GLOBAL_SCOPE SetConsoleOutputCP(CP_UTF8);
		err = MONOLITH_GLOBAL_SCOPE GetLastError();
		DEBUG_MONOLITH_INIT_CODE("SetConsoleOutputCP --> %zu, %zu\n", (size_t)rv, (size_t)err);
		r += (rv != 0);
		ocp = MONOLITH_GLOBAL_SCOPE GetConsoleOutputCP();
		DEBUG_MONOLITH_INIT_CODE("GetConsoleOutputCP --> %zu\n", (size_t)ocp);
		r += (ocp == CP_UTF8);
		ccp = MONOLITH_GLOBAL_SCOPE GetConsoleCP();
		DEBUG_MONOLITH_INIT_CODE("GetConsoleCP --> %zu\n", (size_t)ccp);
		r += (ccp == CP_UTF8);
		DEBUG_MONOLITH_INIT_CODE("setup_exe_for_utf8_console_locale --> %d\n", r);
		return r - 6;   // return value 0 is success, anything else is failure to set up the UTF8 locale & console
#else
		char *srv = setlocale(LC_ALL, "LC_CTYPE=.utf8");
		int r = (srv != NULL);
		/* Select UTF-8 locale */
		//setlocale(LC_ALL, ".utf8");
		return r - 1;   // return value 0 is success, anything else is failure to set up the UTF8 locale & console
#endif
}

#undef MONOLITH_GLOBAL_SCOPE

#ifdef __cplusplus
}
#endif
