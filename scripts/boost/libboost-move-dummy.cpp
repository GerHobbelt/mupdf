
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/move/adl_move_swap.hpp>
#include <boost/move/algo/adaptive_merge.hpp>
#include <boost/move/algo/adaptive_sort.hpp>
#include <boost/move/algo/move.hpp>
#include <boost/move/algo/predicate.hpp>
#include <boost/move/algo/unique.hpp>
#include <boost/move/algorithm.hpp>
#include <boost/move/core.hpp>
#include <boost/move/default_delete.hpp>
#include <boost/move/iterator.hpp>
#include <boost/move/make_unique.hpp>
#include <boost/move/move.hpp>
#include <boost/move/traits.hpp>
#include <boost/move/unique_ptr.hpp>
#include <boost/move/utility.hpp>
#include <boost/move/utility_core.hpp>

