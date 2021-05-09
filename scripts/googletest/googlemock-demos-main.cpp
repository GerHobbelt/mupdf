//
// main() for monolithic build of the googletest::mock tests
//

int gmock_output_test_main(int argc, const char** argv);
int gmock_test_main(int argc, const char** argv);
int gmock_spec_builders_test_main(int argc, const char** argv);
int gmock_spec_builders_test_main(int argc, const char** argv);
int gmock_stress_test_main(int argc, const char** argv);

int gtest_main(int argc, const char** argv);


int main(int argc, const char** argv)
{
	int rv = 0;
	rv |= gmock_output_test_main(argc, argv);
	rv |= gmock_test_main(argc, argv);
	rv |= gmock_spec_builders_test_main(argc, argv);
	rv |= gmock_spec_builders_test_main(argc, argv);
	rv |= gmock_stress_test_main(argc, argv);

	rv |= gtest_main(argc, argv);
	return rv;
}
