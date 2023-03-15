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

/* We put the font object files in the 'AdditionalDependencies' list,
 * but we need at least one C file to link the library.
 * Since we need different object files for 32 and 64 bit builds,
 * we can't just include them in the file list.
 */

// quick hack: use this area for debugging Why The EFF the resource compile phase in MSVC2022 sometimes (rarely!) spits out an error about VS_FF_DEBUG being undefined...

#if defined(_MSC_VER)

//#define RC_INVOKED   //<-- SHOULD've been set by the resource compiler itself.

//#define WIN32_LEAN_AND_MEAN
#define APSTUDIO_READONLY_SYMBOLS
#define APSTUDIO_HIDDEN_SYMBOLS
#include <windows.h>
#include <winver.h>      //<-- is included by winresrc.h: the "winresrc.h" header file cannot deal with this extra include and b0rks.
//#include <verrsrc.h>   //<-- doubly-defines the stuff in winver.h: those stupid header files don't have reload protection.
//#include <winres.h>    //<-- don't even TRY to use this one as you'll get severe collisions with winver.h et al.!
//#include <winresrc.h>  //<-- don't even TRY to use this one as you'll get severe collisions with winver.h et al.!

#include "afxres.h"      // -- for MFC stuff; no collisions or b0rks with the (uncommented!) includes above. --> C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\***\atlmfc\include\afxres.h


// ... and now in the repeat, testing the header include protections of those darned system files:

//#define WIN32_LEAN_AND_MEAN
#define APSTUDIO_READONLY_SYMBOLS
#define APSTUDIO_HIDDEN_SYMBOLS
#include <windows.h>
#include <winver.h>


#ifdef _DEBUG
int rc_fileflags_dummy = VS_FF_DEBUG;
#endif

#endif   // end of MSVC/RC hack area


int libresources_dummy;
