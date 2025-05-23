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
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

#include "mupdf/fitz.h"

#include <math.h>
#include <limits.h>
#include <float.h>


#ifdef __cplusplus
extern "C" {
#endif

FZ_DATA const fz_quad fz_infinite_quad = { { -INFINITY, INFINITY }, { INFINITY, INFINITY }, { -INFINITY, -INFINITY }, { INFINITY, -INFINITY } };
// hotfix for MSVC2022 which keeps barfing on the fz_invalid_quad constant definition with: error C2099 : initializer is not a constant
// --> compile as C++ in separate source file.    !@#$%   :-(
//
// See MSWindows/MSVC's processthreadapi.h for the definition of the offending inline function: __ucrt_int_to_float() which is used in the system macro defining the `NAN` value.
FZ_DATA const fz_quad fz_invalid_quad = { { NAN, NAN }, { NAN, NAN }, { NAN, NAN }, { NAN, NAN } };


#ifdef __cplusplus
}
#endif

