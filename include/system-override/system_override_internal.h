
#pragma once

#ifndef __FZ_SYSTEM_OVERRIDE_H__
#define __FZ_SYSTEM_OVERRIDE_H__

#if defined(_WIN32)
#define _CRT_FUNCTIONS_REQUIRED  1
#define _CRT_INTERNAL_NONSTDC_NAMES  1
#include <crtdbg.h>
#include <windows.h>
#endif
#include <intrin.h>		// __debugbreak()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cassert>


namespace system_override {

	class SystemOverrideClass {
	public:
		SystemOverrideClass(bool _always_kick_in = false);
		~SystemOverrideClass();

	protected:
		void Starting();
		void Ending();

	public:
		static void KickInTheDoor(bool should_do = true);

		static void Tickle(void);
		static SystemOverrideClass *override;

	protected:
		int volatile invoked{0};
		static bool always_kick_in;
	};

}


// void BreakIntoDebugger(void);   // call system_override::SystemOverrideClass::KickInTheDoor() instead.

#endif
