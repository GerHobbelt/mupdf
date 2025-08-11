
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/parameter.hpp>
#include <boost/parameter/are_tagged_arguments.hpp>
#include <boost/parameter/binding.hpp>
#include <boost/parameter/compose.hpp>
#include <boost/parameter/config.hpp>
#include <boost/parameter/deduced.hpp>
#include <boost/parameter/is_argument_pack.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/keyword_fwd.hpp>
#include <boost/parameter/macros.hpp>
#include <boost/parameter/match.hpp>
#include <boost/parameter/name.hpp>
#include <boost/parameter/nested_keyword.hpp>
#include <boost/parameter/optional.hpp>
#include <boost/parameter/parameters.hpp>
#include <boost/parameter/preprocessor.hpp>
#include <boost/parameter/preprocessor_no_spec.hpp>
#include <boost/parameter/required.hpp>
#include <boost/parameter/template_keyword.hpp>
#include <boost/parameter/value_type.hpp>

