
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/ptr_container/clone_allocator.hpp>
#include <boost/ptr_container/exception.hpp>
#include <boost/ptr_container/indirect_fun.hpp>
#include <boost/ptr_container/nullable.hpp>
#include <boost/ptr_container/ptr_array.hpp>
#include <boost/ptr_container/ptr_circular_buffer.hpp>
#include <boost/ptr_container/ptr_container.hpp>
#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/ptr_container/ptr_inserter.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_map_adapter.hpp>
#include <boost/ptr_container/ptr_sequence_adapter.hpp>
#include <boost/ptr_container/ptr_set.hpp>
#include <boost/ptr_container/ptr_set_adapter.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/ptr_container/ptr_unordered_set.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/serialize_ptr_array.hpp>
#include <boost/ptr_container/serialize_ptr_circular_buffer.hpp>
#include <boost/ptr_container/serialize_ptr_container.hpp>
#include <boost/ptr_container/serialize_ptr_deque.hpp>
#include <boost/ptr_container/serialize_ptr_list.hpp>
#include <boost/ptr_container/serialize_ptr_map.hpp>
#include <boost/ptr_container/serialize_ptr_set.hpp>
#include <boost/ptr_container/serialize_ptr_unordered_map.hpp>
#include <boost/ptr_container/serialize_ptr_unordered_set.hpp>
#include <boost/ptr_container/serialize_ptr_vector.hpp>

