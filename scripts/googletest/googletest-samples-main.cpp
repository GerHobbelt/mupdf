//
// main() for monolithic build of the googletest::mock tests
//

int gtest_sample10_main(int argc, const char** argv);
int gtest_sample9_main(int argc, const char** argv);

int gtest_main(int argc, const char** argv);

int main(int argc, const char** argv)
{
	int rv = 0;
	rv |= gtest_sample10_main(argc, argv);
	rv |= gtest_sample9_main(argc, argv);
	
	rv |= gtest_main(argc, argv);
	return rv;
}
