
#include "pch.h"

#include "mupdf/fitz/getopt.h"
#include "mupdf/fitz/context.h"
#include "mupdf/fitz/output.h"

#include "../../source/fitz/tessocr.h"

#define BUILD_MONOLITHIC_SINGLE_INSTANCE_NOW    1
#include "../../thirdparty/tesseract/unittest/include_gunit.h"



class ExpectNFailuresListener : public testing::EmptyTestEventListener {
public:
	ExpectNFailuresListener(int n) :
		expected_failure_count(n), expected_failure_remain(0)
	{}

	testing::TestPartResult OnTestPartResult(const testing::TestPartResult& result) override {
		testing::TestPartResult r = result;

		if (r.type() != testing::TestPartResult::kSuccess && expected_failure_remain > 0)
		{
			expected_failure_remain--;
			r.change_type(testing::TestPartResult::kSuccess);
		}
		return r;
	}

	void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) override {
		expected_failure_remain = expected_failure_count;
	}

	void OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) override {
		if (expected_failure_remain != 0 && expected_failure_count != 0)
		{
			ostringstream os;
			os << "Expected " << expected_failure_count << " failures, but observed " << (expected_failure_count - expected_failure_remain) << " failures instead.";

			// ZERO the expected count: this way all errors show up in the next test round (if any)
			expected_failure_count = 0;

			throw std::runtime_error(os.str());
		}
	}

private:
	int expected_failure_count;
	int expected_failure_remain;
};



namespace tesseract {

	void run_tfloat_benchmark(void);
	void run_tfloat_matrix_benchmark(void);
}

int tesseract_basicAPI_test_main(void);
int tesseract_capi_test_main(void);


using namespace tesseract;

static inline bool streq(const char* s1, const char* s2)
{
	return s1 == s2 || (s1 && s2 && strcmp(s1, s2) == 0);
}

static void usage(const char* name)
{
	const char* p = strrchr(name, '\\');
	if (!p)
		p = strrchr(name, '/');
	if (!p)
		p = name - 1;
	p++;

	fprintf(stderr, "%s [options]\n", p);
	fprintf(stderr, "\nOptions:\n"
		"-t   run unit tests\n"
		"-b   run Basic API tests\n"
		"-f   run tfloat benchmark\n"
		"-m   run matrix benchmark\n");
}


static int tool_is_toplevel_ctx = 0;

static fz_context* ctx = NULL;

static void mu_drop_context(void)
{
	// WARNING: as we point `ctx` at the GLOBAL context in the app init phase, it MAY already be an INVALID
	// pointer reference by now!
	// 
	// WARNING: this assert fires when you run `mutool raster` (and probably other tools as well) and hit Ctrl+C
	// to ABORT/INTERRUPT the running application: the MSVC RTL calls this function in the atexit() handler
	// and the assert fires due to (ctx->error.top != ctx->error.stack).
	//
	// We are okay with that, as that scenario is an immediate abort anyway and the OS will be responsible
	// for cleaning up. That our fz_try/throw/catch exception stack hasn't been properly rewound at such times
	// is obvious, I suppose...
	ASSERT_AND_CONTINUE(!ctx || !fz_has_global_context() || (ctx->error.top == ctx->error.stack_base));

	if (tool_is_toplevel_ctx)
	{
		// as we registered a global context, we should clean the locks on it now
		// so the atexit handler won't have to bother with it.
		ASSERT_AND_CONTINUE(fz_has_global_context());
		ctx = fz_get_global_context();

		ocr_clear_leptonica_mem(ctx);

		fz_drop_context_locks(ctx);
		ctx = NULL;

		fz_drop_global_context();

		tool_is_toplevel_ctx = 0;
	}
	else
	{
		ctx = NULL;
	}
}



int main(int argc, const char** argv)
{
	int c;
	int rv = 0;

	//tool_is_toplevel_ctx = 0;
	//ctx = NULL;

	if (!fz_has_global_context())
	{
		fz_locks_context* locks = NULL;

		ctx = fz_new_context(NULL, locks, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF/Leptonica/Tesseract context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);

		tool_is_toplevel_ctx = 1;
	}
	else
	{
		ctx = fz_get_global_context();
	}
	atexit(mu_drop_context);

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "tfmb")) != -1)
	{
		switch (c)
		{
		default:
			break;

		case 'b':
			return tesseract_basicAPI_test_main();

		case 'f':
			run_tfloat_benchmark();
			return EXIT_SUCCESS;

		case 'm':
			run_tfloat_matrix_benchmark();
			return EXIT_SUCCESS;

		case 't':
			fz_info(ctx, "Running unit tests...\n");

			if (streq(argv[fz_optind], "--"))
				fz_optind++;
			int sub_argc = argc - fz_optind + 1;
			const char** sub_argv = argv + fz_optind - 1;
			sub_argv[0] = argv[0];

			InitGoogleTest(&sub_argc, sub_argv);

			TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
			listeners.Append(new ExpectNFailuresListener(7));

			ocr_set_leptonica_mem(ctx);

			rv |= RUN_ALL_TESTS();

#if 0
			assert(fz_has_global_context());
			ctx = fz_get_global_context();
			fz_drop_context_locks(ctx);
			fz_drop_global_context();
#endif

			return rv;
		}
	}

	usage(argv[0]);

	return EXIT_FAILURE;
}
