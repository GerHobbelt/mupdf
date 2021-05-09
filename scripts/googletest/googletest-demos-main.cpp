//
// main() for monolithic build of the googletest::mock tests
//

int gtest_main(int argc, char** argv);

int main(int argc, char** argv)
{
	int rv = 0;
	rv |= gtest_main(argc, argv);
	return rv;
}
