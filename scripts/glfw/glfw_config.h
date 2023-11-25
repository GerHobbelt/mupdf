
#pragma once

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define _GLFW_WIN32

// load windows.h early, with OEMRESOURCES set up, etc.; otherwise you'll "enjoy" plenty compiler/linker errors.
//#include "win32_platform.h"

// We don't need all the fancy stuff
#ifndef NOMINMAX
#define NOMINMAX
#endif

//#ifndef VC_EXTRALEAN
// #define VC_EXTRALEAN
//#endif

//#ifndef WIN32_LEAN_AND_MEAN
// #define WIN32_LEAN_AND_MEAN
//#endif

// This is a workaround for the fact that glfw3.h needs to export APIENTRY (for
// example to allow applications to correctly declare a GL_KHR_debug callback)
// but windows.h assumes no one will define APIENTRY before it does
#undef APIENTRY

// GLFW on Windows is Unicode only and does not work in MBCS mode
#ifndef UNICODE
#define UNICODE
#endif

// GLFW requires Windows XP or later
#if WINVER < 0x0501
#undef WINVER
#define WINVER 0x0501
#endif
#if _WIN32_WINNT < 0x0501
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

// GLFW uses DirectInput8 interfaces
#define DIRECTINPUT_VERSION 0x0800

// GLFW uses OEM cursor resources
#define OEMRESOURCE

#include <windows.h>

#else
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_EGL
#define GLFW_INCLUDE_NONE
#endif


/*
#define _GLFW_X11
#define GLFW_EXPOSE_NATIVE_X11

#if defined( CINDER_GL_ES_2 )
	#define _GLFW_GLX
	#define _GLFW_USE_GLESV2
	#define GLFW_EXPOSE_NATIVE_EGL
#else
	#define _GLFW_GLX
	#define _GLFW_USE_OPENGL
	#define GLFW_EXPOSE_NATIVE_GLX
#endif
*/
