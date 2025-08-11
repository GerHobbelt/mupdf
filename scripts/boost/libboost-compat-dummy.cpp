
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/compat/bind_back.hpp>
#include <boost/compat/bind_front.hpp>
#include <boost/compat/function_ref.hpp>
#include <boost/compat/integer_sequence.hpp>
#include <boost/compat/invoke.hpp>
#include <boost/compat/latch.hpp>
#include <boost/compat/mem_fn.hpp>
#include <boost/compat/move_only_function.hpp>
#include <boost/compat/shared_lock.hpp>
#include <boost/compat/to_array.hpp>
#include <boost/compat/type_traits.hpp>

