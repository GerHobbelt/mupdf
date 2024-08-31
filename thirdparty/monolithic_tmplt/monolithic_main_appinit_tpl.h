
#pragma once

#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include "shellscalingapi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// APIs for use in the generic startup code when you're writing your own main monolithic exec wrapper:

static int setup_exe_for_monitor_dpi_etc(void)
{
#ifdef _WIN32
#pragma comment(lib,"shcore.lib")

		int r = 0;
		PROCESS_DPI_AWARENESS dpi = (PROCESS_DPI_AWARENESS)(-1);
		HRESULT rv = GetProcessDpiAwareness(NULL, &dpi);
		r += (rv == S_OK);
		r += (dpi >= 0);
		HMONITOR mon = NULL;
		const POINT zero = {0, 0};
		mon = MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
		UINT x = 1000000;
		UINT y = 1000000;
		rv = GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &x, &y);
		r += (rv == S_OK);
		UINT d = GetDpiForWindow(NULL);
		r += (d >= 0);
		DEVICE_SCALE_FACTOR scale = (DEVICE_SCALE_FACTOR)(-1);
		rv = GetScaleFactorForMonitor(mon, &scale);
		r += (rv == S_OK);

		HWND w = GetConsoleWindow();
		d = GetDpiForWindow(w);
		r += (d > 0);
		mon = MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST);
		rv = GetScaleFactorForMonitor(mon, &scale);
		r += (rv == S_OK);

#ifdef __cplusplus
		rv = ::SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
		r += (rv == S_OK);
		rv = ::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		r += (rv == S_OK);
#else
		rv = SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
		r += (rv == S_OK);
		rv = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		r += (rv == S_OK);
#endif

		return r >= 8 ? 0 : 1 + r;   // 0 is success; at least one of the SetProcessDpiAwareness() calls must have passed successfully then.
#else
		return 0;
#endif
}

static int setup_exe_for_utf8_console_locale(void)
{
#ifdef _WIN32
		/*
		 * Select UTF-8 locale.  This will change the way how C runtime
		 * functions such as fopen() and mkdir() handle character strings.
		 * For more information, please see:
		 * https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-160#utf-8-support
		 */
		char *srv = setlocale(LC_ALL, "LC_CTYPE=.utf8");
		int r = (srv != NULL && strstr(srv, "utf8"));
		/* Select UTF-8 locale */
		//setlocale(LC_ALL, ".utf8");
		UINT ocp; // = GetConsoleOutputCP();
		UINT ccp; // = GetConsoleCP();
		int rv = SetConsoleCP(CP_UTF8);
		r += (rv != 0);
		rv = SetConsoleOutputCP(CP_UTF8);
		r += (rv != 0);
		ocp = GetConsoleOutputCP();
		r += (ocp == CP_UTF8);
		ccp = GetConsoleCP();
		r += (ccp == CP_UTF8);
		return r - 5;   // return value 0 is success, anything else is failure to set up the UTF8 locale & console
#else
		char *srv = setlocale(LC_ALL, "LC_CTYPE=.utf8");
		int r = (srv != NULL);
		/* Select UTF-8 locale */
		//setlocale(LC_ALL, ".utf8");
		return r - 1;   // return value 0 is success, anything else is failure to set up the UTF8 locale & console
#endif
}

#ifdef __cplusplus
}
#endif
