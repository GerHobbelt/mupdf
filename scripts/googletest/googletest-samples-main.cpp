//
// main() for monolithic build of the googletest::mock tests
//

#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

#include "googletest/include/monolithic_examples.h"

using namespace std;

using ::testing::InitGoogleTest;
using ::testing::TestEventListeners;
using ::testing::TestWithParam;
using ::testing::UnitTest;
using ::testing::Test;
using ::testing::Values;


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

	InitGoogleTest(&argc, argv);

	TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
	listeners.Append(new ExpectNFailuresListener(1));

	rv |= gtest_sample11_main(argc, argv);
	rv |= gtest_sample10_main(argc, argv);
	rv |= gtest_sample9_main(argc, argv);

	rv |= gtest_main(argc, argv);


	// This is an example of using the UnitTest reflection API to inspect test
	// results. Here we discount failures from the tests we expected to fail.
	{
		using namespace testing;

		int unexpectedly_failed_tests = 0;
		int total_failed_tests = 0;
		int total_run_tests = 0;
		int total_tests = 0;
		UnitTest& unit_test = *UnitTest::GetInstance();

		for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
			const testing::TestSuite& test_suite = *unit_test.GetTestSuite(i);
			for (int j = 0; j < test_suite.total_test_count(); ++j) {
				const TestInfo& test_info = *test_suite.GetTestInfo(j);
				total_tests++;
				if (test_info.should_run())
					total_run_tests++;

				// Counts failed tests that were not meant to fail (those without
				// 'Fails' in the name).
				if (test_info.result()->Failed()) {
					total_failed_tests++;
					if (strcmp(test_info.name(), "Fails") != 0) {
						unexpectedly_failed_tests++;
					}
				}
			}
		}

		// Test that were meant to fail should not affect the test program outcome.
		if (unexpectedly_failed_tests == 0) 
			rv = 0;

		char msgbuf[500];
		snprintf(msgbuf, sizeof(msgbuf), 
			"\n\nSummary:\n"
			"  Tests #: ................. %6d\n"
			"  Tests run #: ............. %6d\n"
			"  Failed #: ................ %6d\n"
			"  %sUNEXPECTED Fail #: ....... %6d@D\n\n\n",
			total_tests,
			total_run_tests,
			total_failed_tests,
			(rv == 0 ? "@G" : "@R"),
			unexpectedly_failed_tests);
		PrintColorEncoded(msgbuf);
	}

	return rv;
}
