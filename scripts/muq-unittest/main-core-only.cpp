
#include "pch.h"

TEST(muqCoreBelief, ThrownCpluplusExceptionIsCaught) {
	throw std::exception("aborting");
}

TEST(muqCoreBelief, ThrownCpluplusExceptionPointerIsCaught) {
	throw new std::exception("aborting");
}

TEST(muqCoreBelief, ThrownCpluplusArbitraryExceptionIsCaught) {
	throw 42;
}

TEST(muqCoreBelief, ThrownCpluplusArbitraryExceptionPointerIsCaught) {
	throw NULL;
}

TEST(muqCoreBelief, MemoryAccessFailureCaughtBySEH) {
	volatile int* pInt = 0x00000000;
	*pInt = 20;
}




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
#if 0
	for (int i = 0; i < argc; i++)
		fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
#endif
	InitGoogleTest(&argc, argv);

	TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
	listeners.Append(new ExpectNFailuresListener(5));

	rv |= RUN_ALL_TESTS();

	return rv;
}
