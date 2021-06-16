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


int gtest_sample10_main(int argc, const char** argv);
int gtest_sample9_main(int argc, const char** argv);

int gtest_main(int argc, const char** argv);

class ExpectNFailuresListener : public testing::EmptyTestEventListener {
public:
	ExpectNFailuresListener(int n) :
		expected_failure_count(n), expected_failure_remain(0)
	{}

	void OnTestStart(const testing::TestInfo& test_info) override {
		test_suite_name = test_info.test_suite_name();
		test_name = test_info.name();
		type_param = test_info.type_param();
		value_param = test_info.value_param();
	}

	testing::TestPartResult OnTestPartResult(const testing::TestPartResult& result) override {
		//if (result.type() == testing::TestPartResult::kFatalFailure) {
		rv = result.type();
		file_name = result.file_name();
		line_number = result.line_number();
		summary = result.summary();
		message = result.message();

		testing::TestPartResult r = result;
		if (rv != testing::TestPartResult::kSuccess && expected_failure_remain > 0)
		{
			expected_failure_remain--;
			r.change_type(testing::TestPartResult::kSuccess);
		}
		return r;
	}

	void OnTestEnd(const testing::TestInfo& test_info) override {
		test_suite_name = NULL;
		test_name = NULL;
		type_param = NULL;
		value_param = NULL;
	}

#if 0
	void OnTestSuiteEnd(const testing::TestSuite& test_suite) override {
		test_suite_name = NULL;
	}

	void OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test) override {
		test_name = NULL;
	}

	void OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test) override {
		type_param = NULL;
	}
#endif

	void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) override {
		test_name = NULL;
		value_param = NULL;

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
		value_param = NULL;
	}

	void OnTestProgramStart(const testing::UnitTest& unit_test) override {
		test_suite_name = NULL;
	}

	void OnTestProgramEnd(const testing::UnitTest& unit_test) override {
		test_suite_name = NULL;
		test_name = NULL;
	}

private:
	int expected_failure_count;
	int expected_failure_remain;

	const char* test_suite_name;
	const char* test_name;
	const char* type_param;
	const char* value_param;

	const char* file_name;
	int line_number;
	const char* summary;
	const char* message;
	testing::TestPartResult::Type rv;
};

int main(int argc, const char** argv)
{
	int rv = 0;

	InitGoogleTest(&argc, argv);

	TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
	listeners.Append(new ExpectNFailuresListener(1));

	rv |= gtest_sample10_main(argc, argv);
	rv |= gtest_sample9_main(argc, argv);
	
	rv |= gtest_main(argc, argv);
	return rv;
}
