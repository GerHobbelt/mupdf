
#include "mupdf/fitz.h"
#include "mupdf/assertions.h"
#include "mupdf/helpers/system-header-files.h"

#if defined(_WIN32)
#include <windows.h>
#endif


static void attempt_to_write_message_to_console_and_debug_channel(const char *prefix, const char *postfix, const char *expression, const char *srcfile, int srcline)
{
#if !defined(_WIN32)
	fprintf(stderr, "%sAssertion failed%s: %s --> %s::%d\n", prefix, postfix, expression, srcfile, srcline);
#else
	char buf[LONGLINE+300];
	snprintf(buf, sizeof(buf), "%sAssertion failed%s: %s --> %s::%d\n", prefix, postfix, expression, srcfile, srcline);
	buf[sizeof(buf) - 1] = 0;
	OutputDebugStringA(buf);

	HANDLE stdOut = GetStdHandle(STD_ERROR_HANDLE);
	if (stdOut != NULL && stdOut != INVALID_HANDLE_VALUE)
	{
		DWORD written = 0;
		WriteConsoleA(stdOut, buf, strlen(buf), &written, NULL);
	}
#endif
}

static int hits = 0;

// NOTE/WARNING: fz_report_failed_assertion() et al CANNOT use any of the fz_error/warn/info
// functions as that COULD potentially cause an infinite recursion as some assertions are
// located in those functions.
// Hence we use standard RTL fprintf and MSWindows OutputDebugString APIs directly instead.

int fz_report_failed_assertion(fz_context* ctx, const char *expression, const char *srcfile, int srcline)
{
#if !defined(HAS_NO_CTX_ANYWAY)
	if (!hits)
	{
		hits++;
		attempt_to_write_message_to_console_and_debug_channel("", "", expression, srcfile, srcline);

		// CAN we throw an exception and handle it that way? If the exception stack is filled and alive, we can /try/:
		if (!ctx)
			ctx = __fz_get_RAW_global_context();
		if (ctx && ctx->error.top > ctx->error.stack_base)
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "EXCEPTION: Assertion failed: %s --> %s::%d\n", expression, srcfile, srcline);
		}
		else
		{
			exit(77);
		}
	}
	else
	{
		attempt_to_write_message_to_console_and_debug_channel("", " in atexit() handler on the rebound", expression, srcfile, srcline);
	}
	abort();
#else
	attempt_to_write_message_to_console_and_debug_channel("", "", expression, srcfile, srcline);
	exit(77);
#endif
	return 0;
}

int fz_report_failed_assertion_and_continue(fz_context* ctx, const char *expression, const char *srcfile, int srcline)
{
	attempt_to_write_message_to_console_and_debug_channel("Soft ", "", expression, srcfile, srcline);
	return 0;
}

void fz_check_and_report_failed_assertion_and_continue(fz_context* ctx, int expr1, int expr2, const char* expr1_str, const char* expr2_str, const char* srcfile, int srcline, int contin)
{
	if (expr1 == expr2)
		return;

	char buf[LONGLINE];
	snprintf(buf, sizeof(buf), "%s == %s (because %d != %d)", expr1_str, expr2_str, expr1, expr2);
	buf[sizeof(buf) - 1] = 0;

	if (!contin)
	{
		fz_report_failed_assertion(ctx, buf, srcfile, srcline);
	}
	else
	{
		fz_report_failed_assertion_and_continue(ctx, buf, srcfile, srcline);
	}
}

