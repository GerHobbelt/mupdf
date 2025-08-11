
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/outcome.hpp>
#include <boost/outcome/bad_access.hpp>
#include <boost/outcome/basic_outcome.hpp>
#include <boost/outcome/basic_result.hpp>
#include <boost/outcome/boost_outcome.hpp>
#include <boost/outcome/boost_result.hpp>
#include <boost/outcome/config.hpp>
#include <boost/outcome/convert.hpp>
#include <boost/outcome/coroutine_support.hpp>
#include <boost/outcome/iostream_support.hpp>
#include <boost/outcome/iostream_support_result.hpp>
#include <boost/outcome/outcome.hpp>
#include <boost/outcome/outcome_gdb.h>
#include <boost/outcome/policy/all_narrow.hpp>
#include <boost/outcome/policy/base.hpp>
#include <boost/outcome/policy/fail_to_compile_observers.hpp>
#include <boost/outcome/policy/outcome_error_code_throw_as_system_error.hpp>
#include <boost/outcome/policy/outcome_exception_ptr_rethrow.hpp>
#include <boost/outcome/policy/result_error_code_throw_as_system_error.hpp>
#include <boost/outcome/policy/result_exception_ptr_rethrow.hpp>
#include <boost/outcome/policy/terminate.hpp>
#include <boost/outcome/policy/throw_bad_result_access.hpp>
#include <boost/outcome/result.hpp>
#include <boost/outcome/std_outcome.hpp>
#include <boost/outcome/std_result.hpp>
#include <boost/outcome/success_failure.hpp>
#include <boost/outcome/trait.hpp>
#include <boost/outcome/try.hpp>
#include <boost/outcome/utils.hpp>

