
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/bimap.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/container_adaptor/associative_container_adaptor.hpp>
#include <boost/bimap/container_adaptor/container_adaptor.hpp>
#include <boost/bimap/container_adaptor/list_adaptor.hpp>
#include <boost/bimap/container_adaptor/list_map_adaptor.hpp>
#include <boost/bimap/container_adaptor/map_adaptor.hpp>
#include <boost/bimap/container_adaptor/multimap_adaptor.hpp>
#include <boost/bimap/container_adaptor/multiset_adaptor.hpp>
#include <boost/bimap/container_adaptor/ordered_associative_container_adaptor.hpp>
#include <boost/bimap/container_adaptor/sequence_container_adaptor.hpp>
#include <boost/bimap/container_adaptor/set_adaptor.hpp>
#include <boost/bimap/container_adaptor/unordered_associative_container_adaptor.hpp>
#include <boost/bimap/container_adaptor/unordered_map_adaptor.hpp>
#include <boost/bimap/container_adaptor/unordered_multimap_adaptor.hpp>
#include <boost/bimap/container_adaptor/unordered_multiset_adaptor.hpp>
#include <boost/bimap/container_adaptor/unordered_set_adaptor.hpp>
#include <boost/bimap/container_adaptor/vector_adaptor.hpp>
#include <boost/bimap/container_adaptor/vector_map_adaptor.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/property_map/set_support.hpp>
#include <boost/bimap/property_map/unordered_set_support.hpp>
#include <boost/bimap/relation/member_at.hpp>
#include <boost/bimap/relation/mutant_relation.hpp>
#include <boost/bimap/relation/pair_layout.hpp>
#include <boost/bimap/relation/structured_pair.hpp>
#include <boost/bimap/relation/symmetrical_base.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/tags/tagged.hpp>
#include <boost/bimap/unconstrained_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/views/list_map_view.hpp>
#include <boost/bimap/views/list_set_view.hpp>
#include <boost/bimap/views/map_view.hpp>
#include <boost/bimap/views/multimap_view.hpp>
#include <boost/bimap/views/multiset_view.hpp>
#include <boost/bimap/views/set_view.hpp>
#include <boost/bimap/views/unconstrained_map_view.hpp>
#include <boost/bimap/views/unconstrained_set_view.hpp>
#include <boost/bimap/views/unordered_map_view.hpp>
#include <boost/bimap/views/unordered_multimap_view.hpp>
#include <boost/bimap/views/unordered_multiset_view.hpp>
#include <boost/bimap/views/unordered_set_view.hpp>
#include <boost/bimap/views/vector_map_view.hpp>
#include <boost/bimap/views/vector_set_view.hpp>

