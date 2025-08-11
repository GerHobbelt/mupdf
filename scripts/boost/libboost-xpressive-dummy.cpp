
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/xpressive/basic_regex.hpp>
#include <boost/xpressive/match_results.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <boost/xpressive/regex_algorithms.hpp>
#include <boost/xpressive/regex_compiler.hpp>
#include <boost/xpressive/regex_constants.hpp>
#include <boost/xpressive/regex_error.hpp>
#include <boost/xpressive/regex_iterator.hpp>
#include <boost/xpressive/regex_primitives.hpp>
#include <boost/xpressive/regex_token_iterator.hpp>
#include <boost/xpressive/regex_traits.hpp>
#include <boost/xpressive/sub_match.hpp>
#include <boost/xpressive/traits/c_regex_traits.hpp>
#include <boost/xpressive/traits/cpp_regex_traits.hpp>
#include <boost/xpressive/traits/null_regex_traits.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/xpressive/xpressive_fwd.hpp>
#include <boost/xpressive/xpressive_static.hpp>
#include <boost/xpressive/xpressive_typeof.hpp>

