
#include "mupdf/fitz.h"
#include "mupdf/assertions.h"
#include "mupdf/helpers/debugheap.h"

#include <string.h>
#include <stdlib.h>

#if defined(_MSC_VER)

#include <crtdbg.h>

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#if defined(FZDBG_HAS_TRACING)

#pragma init_seg(compiler)

static struct memPurposeRange
{
	long start_reqnum;
	long end_reqnum;

	const char* source;
	unsigned int line : 15;
	unsigned int line2 : 15;
	unsigned int is_root_slot : 1;
} fz_crtdbg_purpose_lu_table[100000] = { {0} };
static int fz_crtdbg_purpose_stack[100] = { 0 };
static int fz_crtdbg_purpose_lu_table_top = 1;		// use slot [0] as a dummy filler for the entire application duration
static int fz_crtdbg_purpose_lu_stack_index = 0;

static _CRT_ALLOC_HOOK old_alloc_hook = NULL;

#define countof(e)   (sizeof(e) / sizeof((e)[0]))

extern "C" int fzPushHeapDbgPurpose(const char* s, int l)
{
	if (fz_crtdbg_purpose_lu_table_top >= countof(fz_crtdbg_purpose_lu_table))
		return 0;

	int idx = fz_crtdbg_purpose_lu_table_top++;
	struct memPurposeRange* info = &fz_crtdbg_purpose_lu_table[idx];
	info->source = s;
	info->line = l;
	info->line2 = l;
	info->is_root_slot = (fz_crtdbg_purpose_lu_stack_index == 0);

	// As soon we've pushed a 'root slot' above, do we know for certain that any
	// requestNumbers that are earlier than the start of this new root slot MUST
	// all be located in previous (root and sub) slots, as the requestNumber in
	// the MSVCRT debug heap is a continuous ever-incrementing number.
	// Thus marking our root slots in the trace dump will help us reduce
	// scan/search times when it comes time to report the heap leaks, etc.

	int sp = ++fz_crtdbg_purpose_lu_stack_index;
	fz_crtdbg_purpose_stack[sp] = idx;

	return 1;
}

extern "C" int fzPopHeapDbgPurpose(int related_dummy, int l)
{
	int sp = fz_crtdbg_purpose_lu_stack_index;
	int idx = fz_crtdbg_purpose_stack[sp];
	struct memPurposeRange* info = &fz_crtdbg_purpose_lu_table[idx];
	info->line2 = l;

	fz_crtdbg_purpose_lu_stack_index--;

	return related_dummy + 1;
}

void updateHeapDbgPurpose(long requestNumber)
{
	int sp = fz_crtdbg_purpose_lu_stack_index;
	int idx = fz_crtdbg_purpose_stack[sp];
	struct memPurposeRange* info = &fz_crtdbg_purpose_lu_table[idx];
	if (info->start_reqnum > requestNumber)
		return; // bad code/use!
	if (!info->start_reqnum)
		info->start_reqnum = requestNumber;
	// keep updating the end marker until we POP the slot:
	info->end_reqnum = requestNumber;
}

// _CRT_ALLOC_HOOK
static int __CRTDECL fz_alloc_hook_f(int allocType, void* userData, size_t size, int blockType, long requestNumber, const unsigned char* filename, int lineNumber)
{
	int sp = fz_crtdbg_purpose_lu_stack_index;
	if (sp <= 0 && !filename)
	{
		sp++;
	}

	switch (allocType)
	{
	case _HOOK_ALLOC:
		updateHeapDbgPurpose(requestNumber);
		break;

	case _HOOK_REALLOC:
		updateHeapDbgPurpose(requestNumber);
		break;

	case _HOOK_FREE:
		break;
	}
	int rv = (old_alloc_hook != NULL ? old_alloc_hook(allocType, userData, size, blockType, requestNumber, filename, lineNumber) : TRUE);
	return rv;
}

static int streq(const char* a, const char* b)
{
	return !strcmp(a, b);
}

static int __CRTDECL fz_debug_report_f(int reportType, char* message, int* returnValue)
{
	int rv = FALSE;

	switch (reportType)
	{
	case _CRT_ASSERT:
		break;

	case _CRT_WARN:
	{
		//_RPTN(_CRT_WARN, "{%ld} ", header->_request_number);
		size_t len = strlen(message);
		if (message[0] == '{' && streq(message + len - 2, "} "))
		{
			long reqnum = 0;
			if (1 == sscanf(message + 1, "%ld", &reqnum))
			{
				// find the purpose stack slot(s) which are relevant and pick the most precise match, i.e. the LAST match:
				struct memPurposeRange* match = NULL;

				// skip the dummy slot at [0]:
				for (int i = 1; i < fz_crtdbg_purpose_lu_table_top; i++)
				{
					struct memPurposeRange* info = &fz_crtdbg_purpose_lu_table[i];
					if (info->start_reqnum <= reqnum && info->end_reqnum >= reqnum)
					{
						match = info;
					}
					else if (info->start_reqnum > reqnum && info->is_root_slot)
					{
						// quit looking when we know from the stack info that there won't be any more viable slots
						// as all subsequent slots will certainly be further down the lane, i.e. will all carry
						// HIGHER requestNumber ranges.
						break;
					}
				}

				if (match)
				{
					// abuse the MSVCRT debug output string buffer:
					strcat(message, "--> ...");
#if defined(_DEBUG)
					_CrtDbgReport(reportType, NULL, 0, NULL, "%s\n", message);
#endif

					if (match->line != match->line2)
					{
						sprintf(message, "%s(%u): {%ld} (source lines: %u-%u) ", match->source, match->line, reqnum, match->line, match->line2);
					}
					else
					{
						sprintf(message, "%s(%u): {%ld} ", match->source, match->line, reqnum);
					}
				}
			}
		}
	}
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
	//const int desired_flags = (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//const int desired_flags = (_CRTDBG_ALLOC_MEM_DF);
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

void fz_TurnHeapLeakReportingAtProgramExitOn(void)
{
	const int desired_flags = _CRTDBG_LEAK_CHECK_DF;
	int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	flags |= desired_flags;
	_CrtSetDbgFlag(flags);
}

void *fz_TakeHeapSnapshot(void)
{
	_CrtMemState *data = (_CrtMemState *)calloc(1, sizeof(*data));
	_CrtMemCheckpoint(data);
	return data;
}

void fz_ReleaseHeapSnapshot(void *snapshot)
{
	free(snapshot);
}

void fz_ReportHeapLeakageAgainstSnapshot(void *snapshot_)
{
	_CrtMemState *snapshot = (_CrtMemState *)snapshot_;
	_CrtMemState snap_now;
	_CrtMemCheckpoint(&snap_now);
	_CrtMemState state_now;
	int rv = _CrtMemDifference(&state_now, snapshot, &snap_now);

	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%s", "\n\n### Heap memory allocation statistics: AT START:\n\n");

	_CrtMemDumpStatistics(snapshot);

	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%s", "\n\n### Heap memory allocation statistics: DELTA:\n\n");

	_CrtMemDumpStatistics(&state_now);

	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%s", "\n\n### Heap leakage report:\n\n");

	_CrtMemDumpAllObjectsSince(snapshot);
}

#else

// just satisfy the linker, using the build-mode agnostic .DEF file for the DLL container:

extern "C" void fzPushHeapDbgPurpose(void)
{
}

extern "C" void fzPopHeapDbgPurpose(void)
{
}

#endif

#endif // __VISUALC__
