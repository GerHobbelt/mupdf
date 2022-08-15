// Copyright (C) 2004-2022 Artifex Software, Inc.
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

#undef assert
#undef ASSERT

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NO_ASSERTIONS)

#define assert(expression) (void)(                                          \
            (!!(expression)) ||                                             \
            fz_report_failed_assertion(#expression, __FILE__, __LINE__)		\
        )

#define ASSERT(expression) assert(expression)

#define ASSERT_AND_CONTINUE(expression) (void)(												\
            (!!(expression)) ||																\
            fz_report_failed_assertion_and_continue(#expression, __FILE__, __LINE__)		\
        )


int fz_report_failed_assertion(const char *expression, const char *srcfile, int srcline);
int fz_report_failed_assertion_and_continue(const char *expression, const char *srcfile, int srcline);

#else

#pragma message("You are compiling a binary with assertions removed. Be aware that this MAY only be a good thing for high quality, previously tested, production binaries that must produce the highest possible throughput. Cave canem!")

#define assert(expression)				((void)0)

#define ASSERT(expression)				((void)0)

#define ASSERT_AND_CONTINUE(expression) ((void)0)

#endif

#ifdef __cplusplus
}
#endif
