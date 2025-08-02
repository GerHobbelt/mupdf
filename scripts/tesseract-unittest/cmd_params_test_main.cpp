
#define BUILD_MONOLITHIC_SINGLE_INSTANCE_NOW    1
#include "../../thirdparty/tesseract/unittest/include_gunit.h"

#include <sstream>
#include <iostream>
#include <ostream>

#include "gtest/gtest.h"



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
