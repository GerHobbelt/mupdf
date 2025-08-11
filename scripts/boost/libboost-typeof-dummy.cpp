
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/typeof/constant.hpp>
#include <boost/typeof/decltype.hpp>
#include <boost/typeof/incr_registration_group.hpp>
#include <boost/typeof/msvc/typeof_impl.hpp>
#include <boost/typeof/std/bitset.hpp>
#include <boost/typeof/std/complex.hpp>
#include <boost/typeof/std/deque.hpp>
#include <boost/typeof/std/fstream.hpp>
#include <boost/typeof/std/functional.hpp>
#include <boost/typeof/std/iostream.hpp>
#include <boost/typeof/std/istream.hpp>
#include <boost/typeof/std/iterator.hpp>
#include <boost/typeof/std/list.hpp>
#include <boost/typeof/std/locale.hpp>
#include <boost/typeof/std/map.hpp>
#include <boost/typeof/std/memory.hpp>
#include <boost/typeof/std/ostream.hpp>
#include <boost/typeof/std/queue.hpp>
#include <boost/typeof/std/set.hpp>
#include <boost/typeof/std/sstream.hpp>
#include <boost/typeof/std/stack.hpp>
#include <boost/typeof/std/streambuf.hpp>
#include <boost/typeof/std/string.hpp>
#include <boost/typeof/std/utility.hpp>
#include <boost/typeof/std/valarray.hpp>
#include <boost/typeof/std/vector.hpp>
#include <boost/typeof/typeof.hpp>

