#include <toml.hpp>

#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE toml11
#endif

//#define BOOST_TEST_SOURCE
//#define BOOST_TEST_INCLUDED

#include <boost/test/unit_test_parameters.hpp>

#include "../../thirdparty/owemdjee/toml11/tests/unit_test.hpp"

#ifndef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#error "In monolithic build mode, we expect UNITTEST_FRAMEWORK_LIBRARY_EXIST to be defined for the entire toml test suite."
#endif

#include <boost/test/included/unit_test.hpp>

//  See http://www.boost.org/libs/test for the library home page.

BOOST_AUTO_TEST_CASE(dummy_test)
{
	BOOST_TEST(true);
}

using namespace boost::unit_test;

static
#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
bool my_init_unit_test()
#else
test_suite* my_init_unit_test(int arg_argc, char* arg_argv[])
#endif
{
	log_level logLevel = runtime_config::get<log_level>(runtime_config::btrt_log_level);
	results_reporter::get_stream() << "Current log level: " << static_cast<int>(logLevel) << std::endl;

	auto *suite = &framework::master_test_suite();

	int argc = suite->argc;
	char **argv = suite->argv;

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
	return true;
#else
	return nullptr;  // any suite we return will be added to framework::master_test_suite()
#endif
}

// initializing the master test suite name from the user defined macros
// this function should be seen exactly once.
#ifdef BOOST_TEST_MODULE
static const boost::unit_test::framework::impl::master_test_suite_name_setter mtsetter(BOOST_TEST_STRINGIZE(BOOST_TEST_MODULE).trim("\""));
#endif


extern "C"
int BOOST_TEST_CALL_DECL
main(int argc, const char** argv)
{
	return ::boost::unit_test::unit_test_main(&my_init_unit_test, argc, const_cast<char **>(argv));
}

