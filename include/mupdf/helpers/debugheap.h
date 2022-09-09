// Copyright (C) 2004-2021 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

#ifndef __FZ_HELPER_DEBUGHEAP_H__
#define __FZ_HELPER_DEBUGHEAP_H__

#include "mupdf/helpers/system-header-files.h"


#if defined(_DEBUG) || !defined(NDEBUG)

#ifdef __cplusplus
extern "C" {
#endif

int fzPushHeapDbgPurpose(const char* s, int l);
int fzPopHeapDbgPurpose(int related_dummy, int l);

#ifdef __cplusplus
}
#endif

#define FZ_HEAPDBG_TRACKER_SECTION_START_MARKER(prefix)												\
static int prefix ## HEAPDBG_SECTION_START = fzPushHeapDbgPurpose(__FILE__, __LINE__);

#define FZ_HEAPDBG_TRACKER_SECTION_END_MARKER(prefix)												\
static int prefix ## HEAPDBG_SECTION_END = fzPopHeapDbgPurpose(prefix ## HEAPDBG_SECTION_START, __LINE__);

#else

#define FZ_HEAPDBG_TRACKER_SECTION_START_MARKER(prefix)  /**/
#define FZ_HEAPDBG_TRACKER_SECTION_END_MARKER(prefix)    /**/

#endif

#endif
