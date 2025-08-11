
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/callable_traits.hpp>
#include <boost/callable_traits/add_member_const.hpp>
#include <boost/callable_traits/add_member_cv.hpp>
#include <boost/callable_traits/add_member_lvalue_reference.hpp>
#include <boost/callable_traits/add_member_rvalue_reference.hpp>
#include <boost/callable_traits/add_member_volatile.hpp>
#include <boost/callable_traits/add_noexcept.hpp>
#include <boost/callable_traits/add_transaction_safe.hpp>
#include <boost/callable_traits/add_varargs.hpp>
#include <boost/callable_traits/apply_member_pointer.hpp>
#include <boost/callable_traits/apply_return.hpp>
#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/class_of.hpp>
#include <boost/callable_traits/function_type.hpp>
#include <boost/callable_traits/has_member_qualifiers.hpp>
#include <boost/callable_traits/has_varargs.hpp>
#include <boost/callable_traits/has_void_return.hpp>
#include <boost/callable_traits/is_const_member.hpp>
#include <boost/callable_traits/is_cv_member.hpp>
#include <boost/callable_traits/is_invocable.hpp>
#include <boost/callable_traits/is_lvalue_reference_member.hpp>
#include <boost/callable_traits/is_noexcept.hpp>
#include <boost/callable_traits/is_reference_member.hpp>
#include <boost/callable_traits/is_rvalue_reference_member.hpp>
#include <boost/callable_traits/is_transaction_safe.hpp>
#include <boost/callable_traits/is_volatile_member.hpp>
#include <boost/callable_traits/qualified_class_of.hpp>
#include <boost/callable_traits/remove_member_const.hpp>
#include <boost/callable_traits/remove_member_cv.hpp>
#include <boost/callable_traits/remove_member_reference.hpp>
#include <boost/callable_traits/remove_member_volatile.hpp>
#include <boost/callable_traits/remove_noexcept.hpp>
#include <boost/callable_traits/remove_transaction_safe.hpp>
#include <boost/callable_traits/remove_varargs.hpp>
#include <boost/callable_traits/return_type.hpp>

