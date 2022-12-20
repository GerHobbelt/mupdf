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

typedef struct fz_context fz_context;

#ifndef CTX
#define CTX   ctx
#endif

// --------------------------------------------------------------------------------------------------------------------------------------

#ifndef FZ_WARN_ABOUT_ASSERT_COLLISION_WITH_STD_RTL
#define assert(expression) (void)(							                        \
            (!!(expression)) ||						                                \
            fz_report_failed_assertion(CTX, #expression, __FILE__, __LINE__)		\
        )
#else
// make sure this expands to a compiler error:
#define assert(expression)       							                        \
  ;WARNING:: %% "do not use assert() but ASSERT() et al to prevent potential mishaps due to header files' load order picking the undesirable system/compiler assert.h implementation after all" %% ;
#endif

#define ASSERT(expression) (void)(							                        \
            (!!(expression)) ||						                                \
            fz_report_failed_assertion(CTX, #expression, __FILE__, __LINE__)		\
        )

#define ASSERT_AND_CONTINUE(expression) (void)(													\
            (!!(expression)) ||																	\
            fz_report_failed_assertion_and_continue(CTX, #expression, __FILE__, __LINE__)		\
        )

#define VERIFY(expression)              ASSERT(expression)
#define VERIFY_AND_CONTINUE(expression) ASSERT_AND_CONTINUE(expression)

#define VERIFY_AND_CONTINUE_EQ(expr1, expr2) 												\
            fz_check_and_report_failed_assertion_and_continue(CTX, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, TRUE)		
#define ASSERT_AND_CONTINUE_EQ(expr1, expr2) 												\
            fz_check_and_report_failed_assertion_and_continue(CTX, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, TRUE)

#if !defined(GTEST_DONT_DEFINE_ASSERT_EQ)

#define VERIFY_EQ(expr1, expr2) 															\
            fz_check_and_report_failed_assertion_and_continue(CTX, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, FALSE)		
#define ASSERT_EQ(expr1, expr2) 															\
            fz_check_and_report_failed_assertion_and_continue(CTX, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, FALSE)

#endif

// --------------------------------------------------------------------------------------------------------------------------------------

#define ASSERT0(expression) (void)(							                        \
            (!!(expression)) ||						                                \
            fz_report_failed_assertion(NULL, #expression, __FILE__, __LINE__)		\
        )

#define ASSERT_AND_CONTINUE0(expression) (void)(												\
            (!!(expression)) ||																	\
            fz_report_failed_assertion_and_continue(NULL, #expression, __FILE__, __LINE__)		\
        )

#define VERIFY0(expression)              ASSERT0(expression)
#define VERIFY_AND_CONTINUE0(expression) ASSERT_AND_CONTINUE0(expression)

#define VERIFY_AND_CONTINUE_EQ0(expr1, expr2) 												\
            fz_check_and_report_failed_assertion_and_continue(NULL, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, TRUE)		
#define ASSERT_AND_CONTINUE_EQ0(expr1, expr2) 												\
            fz_check_and_report_failed_assertion_and_continue(NULL, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, TRUE)

#define VERIFY_EQ0(expr1, expr2) 															\
            fz_check_and_report_failed_assertion_and_continue(NULL, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, FALSE)		
#define ASSERT_EQ0(expr1, expr2) 															\
            fz_check_and_report_failed_assertion_and_continue(NULL, expr1, expr2, #expr1, #expr2, __FILE__, __LINE__, FALSE)

// --------------------------------------------------------------------------------------------------------------------------------------

int fz_report_failed_assertion(fz_context *ctx, const char *expression, const char *srcfile, int srcline);
int fz_report_failed_assertion_and_continue(fz_context *ctx, const char *expression, const char *srcfile, int srcline);
void fz_check_and_report_failed_assertion_and_continue(fz_context *ctx, int expr1, int expr2, const char *expr1_str, const char *expr2_str, const char *srcfile, int srcline, int contin);

#else

#pragma message("You are compiling a binary with assertions removed. Be aware that this MAY only be a good thing for high quality, previously tested, production binaries that must produce the highest possible throughput. Cave canem!")

#ifndef FZ_WARN_ABOUT_ASSERT_COLLISION_WITH_STD_RTL
#define assert(expression)				((void)0)
#else
	// make sure this expands to a compiler error:
#define assert(expression)       							                        \
  ;WARNING:: %% "do not use assert() but ASSERT() et al to prevent potential mishaps due to header files' load order picking the undesirable system/compiler assert.h implementation after all" %% ;
#endif

#define ASSERT(expression)				((void)0)
#define ASSERT_AND_CONTINUE(expression) ((void)0)

#define VERIFY(expression)              expression
#define VERIFY_AND_CONTINUE(expression) expression

#define VERIFY_AND_CONTINUE_EQ(expr1, expr2) 												\
            (void)((expr1) == (expr2))
#define ASSERT_AND_CONTINUE_EQ(expr1, expr2) 												\
			((void)0)

#define VERIFY_EQ(expr1, expr2) 															\
            (void)((expr1) == (expr2))
#define ASSERT_EQ(expr1, expr2) 															\
			((void)0)
// --------------------------------------------------------------------------------------------------------------------------------------

#define ASSERT0(expression)					((void)0)
#define ASSERT_AND_CONTINUE0(expression)	((void)0)

#define VERIFY0(expression)					expression
#define VERIFY_AND_CONTINUE0(expression)	expression

#define VERIFY_AND_CONTINUE_EQ0(expr1, expr2) 												\
            (void)((expr1) == (expr2))
#define ASSERT_AND_CONTINUE_EQ0(expr1, expr2) 												\
            ((void)0)

#define VERIFY_EQ0(expr1, expr2) 															\
            (void)((expr1) == (expr2))
#define ASSERT_EQ0(expr1, expr2) 															\
            ((void)0)

#endif

#ifdef __cplusplus
}
#endif
