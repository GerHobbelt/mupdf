
#include "pch.h"

TEST(TestFzAssumptions, TestRectMinMaxInf) {
  /*
    [...]
    We choose FZ_{MIN,MAX}_INF_RECT to be the largest 32bit signed
		integer values that survive roundtripping to floats.
	 */
	EXPECT_NE(FZ_MIN_INF_RECT, 0);
	EXPECT_NE(FZ_MAX_INF_RECT, 0);

	volatile int v1 = FZ_MIN_INF_RECT;
	volatile float f1 = (float)v1;
	v1 = (int)f1;

	volatile int v2 = FZ_MAX_INF_RECT;
	volatile float f2 = (float)v2;
	v2 = (int)f2;

	EXPECT_LT(v1, 0);
	EXPECT_GT(v2, 0);
	// both EQ(Equal) checks: these values MUST survive the roundtrip back to type:int
	EXPECT_EQ(FZ_MIN_INF_RECT, v1);
	EXPECT_EQ(FZ_MAX_INF_RECT, v2);

	f1 -= 1.0;
	f2 += 1.0;

	v1 = (int)f1;
	v2 = (int)f2;

	EXPECT_LT(v1, 0);
	EXPECT_GT(v2, 0);
	// both NE(NotEqual) checks: these values SHOULD NOT survive the roundtrip back to type:int
	EXPECT_NE(FZ_MIN_INF_RECT - 1LL, v1);
	EXPECT_NE(FZ_MAX_INF_RECT + 1LL, v2);
}
