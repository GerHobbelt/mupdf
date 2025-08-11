
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/property_map/parallel/basic_reduce.hpp>
#include <boost/property_map/parallel/caching_property_map.hpp>
#include <boost/property_map/parallel/distributed_property_map.hpp>
#include <boost/property_map/parallel/global_index_map.hpp>
#include <boost/property_map/parallel/local_property_map.hpp>
#include <boost/property_map/parallel/parallel_property_maps.hpp>
#include <boost/property_map/parallel/process_group.hpp>
#include <boost/property_map/parallel/simple_trigger.hpp>
#include <boost/property_map/parallel/unsafe_serialize.hpp>
#include <boost/property_map/parallel/vector_property_map.hpp>

