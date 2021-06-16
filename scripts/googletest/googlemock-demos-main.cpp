//
// main() for monolithic build of the googletest::mock tests
//

#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

using namespace std;

using ::testing::InitGoogleTest;
using ::testing::TestEventListeners;
using ::testing::TestWithParam;
using ::testing::UnitTest;
using ::testing::Test;
using ::testing::Values;


int gmock_output_test_main(int argc, const char** argv);
int gmock_test_main(int argc, const char** argv);
int gmock_spec_builders_test_main(int argc, const char** argv);
int gmock_stress_test_main(int argc, const char** argv);

int gtest_main(int argc, const char** argv);


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

	rv |= gmock_output_test_main(argc, argv);
	rv |= gmock_test_main(argc, argv);
	rv |= gmock_spec_builders_test_main(argc, argv);
	rv |= gmock_stress_test_main(argc, argv);

	rv |= gtest_main(argc, argv);
	return rv;
}
