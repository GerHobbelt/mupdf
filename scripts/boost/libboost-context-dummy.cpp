
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/context/continuation.hpp>
#include <boost/context/continuation_fcontext.hpp>
#include <boost/context/continuation_ucontext.hpp>
#include <boost/context/continuation_winfib.hpp>
#include <boost/context/fiber.hpp>
#include <boost/context/fiber_fcontext.hpp>
#include <boost/context/fiber_ucontext.hpp>
#include <boost/context/fiber_winfib.hpp>
#include <boost/context/fixedsize_stack.hpp>
#include <boost/context/flags.hpp>
#include <boost/context/pooled_fixedsize_stack.hpp>
#include <boost/context/preallocated.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>
#include <boost/context/segmented_stack.hpp>
#include <boost/context/stack_context.hpp>
#include <boost/context/stack_traits.hpp>

