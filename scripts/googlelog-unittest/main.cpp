
#include "pch.h"

// better to use the test-local (testsuite class based) approach instead, as demonstrated in googlelog/src/exception_handling_test.cc:
// that approach is far less error-prone and should work well with googletest features such as running test **subsets** from
// the commandline, etc., without causing "weird shit" to happen; the #if 0-ed code below
// would exhibit those problems under such (and other slightly out of the ordinary) circumstances and is therefor ill-advised
// to be copied!

#if 0

class ExpectNFailuresListener: public testing::EmptyTestEventListener {
public:
	ExpectNFailuresListener(int n, int skips = 0):
		expected_failure_count(n), expected_failure_remain(0),
		expected_skip_count(skips), expected_skip_remain(0)
	{
	}

	testing::TestPartResult OnTestPartResult(const testing::TestPartResult& result) override {
		testing::TestPartResult r = result;

		if (r.type() != testing::TestPartResult::kSuccess && r.type() != testing::TestPartResult::kSkip)
		{
			expected_failure_remain--;
#if 0
			if (expected_failure_remain >= 0)
				r.change_type(testing::TestPartResult::kSuccess);
#endif
		}
		if (r.type() == testing::TestPartResult::kSkip)
		{
			expected_skip_remain--;
#if 0
			if (expected_skip_remain >= 0)
				r.change_type(testing::TestPartResult::kSuccess);
#endif
		}
		return r;
	}

	void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) override {
		expected_failure_remain = expected_failure_count;
		expected_skip_remain = expected_skip_count;
	}

	void OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) override {
		if (expected_failure_remain != 0 && expected_failure_count != 0)
		{
			std::ostringstream os;
			os << "Expected " << expected_failure_count << " failures, but observed " << (expected_failure_count - expected_failure_remain) << " failures instead.";

			// ZERO the expected count: this way all errors show up in the next test round (if any)
			expected_failure_count = 0;

			//throw std::runtime_error(os.str());
		}
		if (expected_skip_remain != 0 && expected_skip_count != 0)
		{
			std::ostringstream os;
			os << "Expected " << expected_skip_count << " skipped tests, but observed " << (expected_skip_count - expected_skip_remain) << " skips instead.";

			// ZERO the expected count: this way all errors show up in the next test round (if any)
			expected_skip_count = 0;

			//throw std::runtime_error(os.str());
		}
	}

private:
	int expected_failure_count;
	int expected_failure_remain;
	int expected_skip_count;
	int expected_skip_remain;
};

#endif


GTEST_API_ int main(int argc, const char** argv) {
	fprintf(stderr, "Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);

	int rv = 0;

    std::set_terminate([]() {
	  fprintf(stderr, "terminate_handler: uncaught_exceptions: %d\n", std::uncaught_exceptions());

	  std::abort();
    });

#if 0
	testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new ExpectNFailuresListener(3));
#endif	

#if 0
	rv |= run_all_tests_glog();  // call this one to ensure all google-log tests are included in this monolithic test executable.
#else
	rv |= RUN_ALL_TESTS();
#endif

	return rv;
}
