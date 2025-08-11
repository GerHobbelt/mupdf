
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/safe_numerics/automatic.hpp>
#include <boost/safe_numerics/checked_default.hpp>
#include <boost/safe_numerics/checked_float.hpp>
#include <boost/safe_numerics/checked_integer.hpp>
#include <boost/safe_numerics/checked_result.hpp>
#include <boost/safe_numerics/checked_result_operations.hpp>
#include <boost/safe_numerics/concept/exception_policy.hpp>
#include <boost/safe_numerics/concept/integer.hpp>
#include <boost/safe_numerics/concept/numeric.hpp>
#include <boost/safe_numerics/concept/promotion_policy.hpp>
#include <boost/safe_numerics/concept/safe_numeric.hpp>
#include <boost/safe_numerics/cpp.hpp>
#include <boost/safe_numerics/exception.hpp>
#include <boost/safe_numerics/exception_policies.hpp>
#include <boost/safe_numerics/interval.hpp>
#include <boost/safe_numerics/native.hpp>
#include <boost/safe_numerics/range_value.hpp>
#include <boost/safe_numerics/safe_base.hpp>
#include <boost/safe_numerics/safe_base_operations.hpp>
#include <boost/safe_numerics/safe_common.hpp>
#include <boost/safe_numerics/safe_compare.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/safe_numerics/safe_integer_literal.hpp>
#include <boost/safe_numerics/safe_integer_range.hpp>
#include <boost/safe_numerics/utility.hpp>

