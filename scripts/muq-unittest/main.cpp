
#include "pch.h"



// better to use the test-local (testsuite class based) approach instead, as demonstrated in googlelog/src/exception_handling_test.cc:
// that approach is far less error-prone and should work well with googletest features such as running test **subsets** from
// the commandline, etc., without causing "weird shit" to happen; the code below
// would exhibit those problems under such (and other slightly out of the ordinary) circumstances and is therefor ill-advised
// to be copied!

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



int main(int argc, const char** argv)
{
	int rv = 0;

	fprintf(stderr, "Running main() from %s\n", __FILE__);
	InitGoogleTest(&argc, argv);

	TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
	listeners.Append(new ExpectNFailuresListener(2));

	rv |= RUN_ALL_TESTS();

	return rv;
}
