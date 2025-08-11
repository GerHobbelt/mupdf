
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/binding.hpp>
#include <boost/type_erasure/binding_of.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/call.hpp>
#include <boost/type_erasure/callable.hpp>
#include <boost/type_erasure/check_match.hpp>
#include <boost/type_erasure/concept_interface.hpp>
#include <boost/type_erasure/concept_of.hpp>
#include <boost/type_erasure/config.hpp>
#include <boost/type_erasure/constructible.hpp>
#include <boost/type_erasure/deduced.hpp>
#include <boost/type_erasure/derived.hpp>
#include <boost/type_erasure/dynamic_any_cast.hpp>
#include <boost/type_erasure/dynamic_binding.hpp>
#include <boost/type_erasure/exception.hpp>
#include <boost/type_erasure/free.hpp>
#include <boost/type_erasure/is_empty.hpp>
#include <boost/type_erasure/is_placeholder.hpp>
#include <boost/type_erasure/is_subconcept.hpp>
#include <boost/type_erasure/iterator.hpp>
#include <boost/type_erasure/member.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/param.hpp>
#include <boost/type_erasure/placeholder.hpp>
#include <boost/type_erasure/placeholder_of.hpp>
#include <boost/type_erasure/rebind_any.hpp>
#include <boost/type_erasure/register_binding.hpp>
#include <boost/type_erasure/relaxed.hpp>
#include <boost/type_erasure/require_match.hpp>
#include <boost/type_erasure/same_type.hpp>
#include <boost/type_erasure/static_binding.hpp>
#include <boost/type_erasure/tuple.hpp>
#include <boost/type_erasure/typeid_of.hpp>

