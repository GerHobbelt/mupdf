
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

static void updateHeapDbgPurpose(long requestNumber)
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

static _CrtMemState heapDbgSnapshotState = { 0 };
static bool has_snapshot = false;

static char *leak = NULL;

extern "C" void fzMarkHeapDbgApplicationStart(void)
{
	if (leak)
		free(leak);
	leak = strdup("MARKER_4_MEMDEBUG");            // marked leakage.

	_CrtMemCheckpoint(&heapDbgSnapshotState);
	has_snapshot = true;
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
	//const int desired_flags = (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	const int desired_flags = (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
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




#if 0

_ACRTIMP void __cdecl _CrtMemCheckpoint(
	_Out_ _CrtMemState* _State
);

_ACRTIMP int __cdecl _CrtMemDifference(
	_Out_ _CrtMemState*       _State,
	_In_  _CrtMemState const* _OldState,
	_In_  _CrtMemState const* _NewState
);

_ACRTIMP void __cdecl _CrtMemDumpAllObjectsSince(
	_In_opt_ _CrtMemState const* _State
);

_ACRTIMP void __cdecl _CrtMemDumpStatistics(
	_In_ _CrtMemState const* _State
);

_Check_return_
_ACRTIMP int __cdecl _CrtReportBlockType(
	_In_opt_ void const* _Block
);







#if _MSC_VER && _DEBUG
// _CrtMemCheckpoint() and _CrtMemDifference are non-empty only in _DEBUG
_CrtMemState stateBefore, stateAfter, diffState;
_CrtMemCheckpoint(&stateBefore);
#endif
for (int i=0; i<100; i++) {
	tbb::task_arena arena(1, 1); arena.initialize(); // right approach?
	// tbb::task_scheduler_init sch(1);
	tbb::task_scheduler_handle sch = tbb::task_scheduler_handle::get();
	for (int k=0; k<10; k++) {
		// tbb::empty_task *t = new( tbb::task::allocate_root() ) tbb::empty_task();
		// tbb::task::enqueue(*t);
		arena.enqueue([&] {});
	}
	bool ok = tbb::finalize(sch, std::nothrow);
	ASSERT(ok, nullptr);
		}
#if _MSC_VER && _DEBUG
_CrtMemCheckpoint(&stateAfter);
int ret = _CrtMemDifference(&diffState, &stateBefore, &stateAfter);
ASSERT(!ret, "It must be no memory leaks at this point.");
#endif
if (utils::GetMemoryUsage() <= memBefore)
break;
	}
}






/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "tensorflow/lite/profiling/memory_info.h"

#ifdef __linux__
#include <malloc.h>
#include <sys/resource.h>
#include <sys/time.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <malloc/malloc.h>
#endif

namespace tflite {
	namespace profiling {
		namespace memory {

			const size_t MemoryUsage::kValueNotSet = 0;

			bool MemoryUsage::IsSupported() {
#if defined(__linux__) || defined(__APPLE__)
				return true;
#endif
				return false;
			}

			MemoryUsage GetMemoryUsage() {
				MemoryUsage result;
#ifdef __linux__
				rusage res;
				if (getrusage(RUSAGE_SELF, &res) == 0) {
					result.mem_footprint_kb = res.ru_maxrss;
				}
#if defined(__NO_MALLINFO__)
				result.total_allocated_bytes = -1;
				result.in_use_allocated_bytes = -1;
#elif defined(__GLIBC__) && __GLIBC_MINOR__ >= 33
				const auto mem = mallinfo2();
				result.total_allocated_bytes = mem.arena;
				result.in_use_allocated_bytes = mem.uordblks;
#else
				const auto mem = mallinfo();
				result.total_allocated_bytes = mem.arena;
				result.in_use_allocated_bytes = mem.uordblks;
#endif  // defined(__NO_MALLINFO__)
#elif defined(__APPLE__)
				struct task_vm_info vm_info;
				mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
				auto status = task_info(mach_task_self(), TASK_VM_INFO,
										reinterpret_cast<task_info_t>(&vm_info), &count);
				if (status == KERN_SUCCESS) {
					result.mem_footprint_kb =
						static_cast<int64_t>(vm_info.phys_footprint / 1024.0);
				}
				struct mstats stats = mstats();
				result.total_allocated_bytes = stats.bytes_total;
				result.in_use_allocated_bytes = stats.bytes_used;
#endif  // __linux__
				return result;
			}

			void MemoryUsage::AllStatsToStream(std::ostream* stream) const {
				*stream << "max resident set size/physical footprint = "
					<< mem_footprint_kb / 1024.0 << " MB, total malloc-ed size = "
					<< total_allocated_bytes / 1024.0 / 1024.0
					<< " MB, in-use allocated/mmapped size = "
					<< in_use_allocated_bytes / 1024.0 / 1024.0 << " MB";
			}

		}  // namespace memory
	}  // namespace profiling
}  // namespace tflite








#endif

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
