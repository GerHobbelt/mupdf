
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/unordered/concurrent_flat_map_fwd.hpp>
#include <boost/unordered/concurrent_flat_set.hpp>
#include <boost/unordered/concurrent_flat_set_fwd.hpp>
#include <boost/unordered/concurrent_node_map.hpp>
#include <boost/unordered/concurrent_node_map_fwd.hpp>
#include <boost/unordered/concurrent_node_set.hpp>
#include <boost/unordered/concurrent_node_set_fwd.hpp>
#include <boost/unordered/hash_traits.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_map_fwd.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/unordered/unordered_flat_set_fwd.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <boost/unordered/unordered_map_fwd.hpp>
#include <boost/unordered/unordered_node_map.hpp>
#include <boost/unordered/unordered_node_map_fwd.hpp>
#include <boost/unordered/unordered_node_set.hpp>
#include <boost/unordered/unordered_node_set_fwd.hpp>
#include <boost/unordered/unordered_set.hpp>
#include <boost/unordered/unordered_set_fwd.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

