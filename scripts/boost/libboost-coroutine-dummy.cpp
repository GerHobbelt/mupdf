
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/coroutine/all.hpp>
#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/coroutine.hpp>
#include <boost/coroutine/exceptions.hpp>
#include <boost/coroutine/flags.hpp>
#include <boost/coroutine/protected_stack_allocator.hpp>
#include <boost/coroutine/segmented_stack_allocator.hpp>
#include <boost/coroutine/stack_allocator.hpp>
#include <boost/coroutine/stack_context.hpp>
#include <boost/coroutine/stack_traits.hpp>
#include <boost/coroutine/standard_stack_allocator.hpp>
#include <boost/coroutine/symmetric_coroutine.hpp>

