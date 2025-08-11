
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/assign.hpp>
#include <boost/assign/assignment_exception.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/ptr_list_inserter.hpp>
#include <boost/assign/ptr_list_of.hpp>
#include <boost/assign/ptr_map_inserter.hpp>
#include <boost/assign/std.hpp>
#include <boost/assign/std/deque.hpp>
#include <boost/assign/std/list.hpp>
#include <boost/assign/std/map.hpp>
#include <boost/assign/std/queue.hpp>
#include <boost/assign/std/set.hpp>
#include <boost/assign/std/slist.hpp>
#include <boost/assign/std/stack.hpp>
#include <boost/assign/std/vector.hpp>

