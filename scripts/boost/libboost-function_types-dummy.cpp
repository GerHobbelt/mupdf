
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/function_types/components.hpp>
#include <boost/function_types/config/cc_names.hpp>
#include <boost/function_types/config/compiler.hpp>
#include <boost/function_types/config/config.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/function_pointer.hpp>
#include <boost/function_types/function_reference.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/is_callable_builtin.hpp>
#include <boost/function_types/is_function.hpp>
#include <boost/function_types/is_function_pointer.hpp>
#include <boost/function_types/is_function_reference.hpp>
#include <boost/function_types/is_member_function_pointer.hpp>
#include <boost/function_types/is_member_object_pointer.hpp>
#include <boost/function_types/is_member_pointer.hpp>
#include <boost/function_types/is_nonmember_callable_builtin.hpp>
#include <boost/function_types/member_function_pointer.hpp>
#include <boost/function_types/member_object_pointer.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/property_tags.hpp>
#include <boost/function_types/result_type.hpp>

