
#include "pch.h"

#if defined(_MSC_VER)

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#endif

#include <stdlib.h>
#include <crtdbg.h>



#pragma init_seg(compiler)

static const unsigned char* fz_crtdbg_purpose_stack[100] = { 0 };
static const unsigned char* fz_crtdbg_purpose_lu_table[100000] = { 0 };

static _CRT_ALLOC_HOOK old_alloc_hook = NULL;

// _CRT_ALLOC_HOOK
static int __CRTDECL fz_alloc_hook_f(int allocType, void* userData, size_t size, int blockType, long requestNumber, const unsigned char* filename, int lineNumber)
{
	switch (allocType)
	{
	case _HOOK_ALLOC:
		break;
	case _HOOK_REALLOC:
		break;
	case _HOOK_FREE:
		break;
	}
	int rv = (old_alloc_hook != NULL ? old_alloc_hook(allocType, userData, size, blockType, requestNumber, filename, lineNumber) : TRUE);
	return rv;
}

static int __CRTDECL fz_debug_report_f(int reportType, char* message, int* returnValue)
{
	int rv = FALSE;

	switch (reportType)
	{
	case _CRT_ASSERT:
		break;

	case _CRT_WARN:
		break;

	case _CRT_ERROR:
		break;

	default:
		break;
	}

	if (returnValue)
		*returnValue = 0;

	return rv;
}
static int __CRTDECL fz_debug_report_wf(int reportType, wchar_t* message, int* returnValue)
{
	int rv = FALSE;

	switch (reportType)
	{
	case _CRT_ASSERT:
		break;

	case _CRT_WARN:
		break;

	case _CRT_ERROR:
		break;

	default:
		break;
	}

	if (returnValue)
		*returnValue = 0;

	return rv;
}

static int EnableMemLeakChecking(void)
{
	// check for memory leaks on program exit (another useful flag
	// is _CRTDBG_DELAY_FREE_MEM_DF which doesn't free deallocated
	// memory which may be used to simulate low-memory condition)

	//_CrtSetBreakAlloc(744);  /* Break at memalloc{744}, or 'watch' _crtBreakAlloc */
	const int desired_flags = (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	flags |= desired_flags;
	_CrtSetDbgFlag(flags);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

	old_alloc_hook = _CrtSetAllocHook(fz_alloc_hook_f);

	_CrtSetReportHookW2(_CRT_RPTHOOK_INSTALL, fz_debug_report_wf);
	_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, fz_debug_report_f);

	return 1;
}

static int fz_memleak_checking_is_set_up = EnableMemLeakChecking();

#endif // __VISUALC__

