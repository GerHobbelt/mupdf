
#include "pch.h"

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


using namespace tesseract;

int main(int argc, const char** argv)
{
	int rv = 0;

	fprintf(stderr, "Running main() from %s\n", __FILE__);
	InitGoogleTest(&argc, argv);

	TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
	listeners.Append(new ExpectNFailuresListener(7));

	if (1)
		run_tfloat_matrix_benchmark();
	else
		run_tfloat_benchmark();
	exit(7);

	rv |= RUN_ALL_TESTS();

	return rv;
}