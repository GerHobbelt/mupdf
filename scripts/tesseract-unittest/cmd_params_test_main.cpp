
#define BUILD_MONOLITHIC_SINGLE_INSTANCE_NOW    1
#include "../../thirdparty/tesseract/unittest/include_gunit.h"

#include <sstream>
#include <iostream>
#include <ostream>

#include "gtest/gtest.h"


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
			std::ostringstream os;
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
		"-t   run unit tests\n");
}


int main(int argc, const char** argv)
{
	int rv = 0;

	testing::InitGoogleTest(&argc, argv);

	testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new ExpectNFailuresListener(7));

	rv |= RUN_ALL_TESTS();

	return rv;
}
