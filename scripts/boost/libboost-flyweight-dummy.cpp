
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/flyweight.hpp>
#include <boost/flyweight/assoc_container_factory.hpp>
#include <boost/flyweight/assoc_container_factory_fwd.hpp>
#include <boost/flyweight/concurrent_factory.hpp>
#include <boost/flyweight/concurrent_factory_fwd.hpp>
#include <boost/flyweight/factory_tag.hpp>
#include <boost/flyweight/flyweight.hpp>
#include <boost/flyweight/flyweight_fwd.hpp>
#include <boost/flyweight/hashed_factory.hpp>
#include <boost/flyweight/hashed_factory_fwd.hpp>
#include <boost/flyweight/holder_tag.hpp>
#include <boost/flyweight/intermodule_holder.hpp>
#include <boost/flyweight/intermodule_holder_fwd.hpp>
#include <boost/flyweight/key_value.hpp>
#include <boost/flyweight/key_value_fwd.hpp>
#include <boost/flyweight/locking_tag.hpp>
#include <boost/flyweight/no_locking.hpp>
#include <boost/flyweight/no_locking_fwd.hpp>
#include <boost/flyweight/no_tracking.hpp>
#include <boost/flyweight/no_tracking_fwd.hpp>
#include <boost/flyweight/refcounted.hpp>
#include <boost/flyweight/refcounted_fwd.hpp>
#include <boost/flyweight/serialize.hpp>
#include <boost/flyweight/set_factory.hpp>
#include <boost/flyweight/set_factory_fwd.hpp>
#include <boost/flyweight/simple_locking.hpp>
#include <boost/flyweight/simple_locking_fwd.hpp>
#include <boost/flyweight/static_holder.hpp>
#include <boost/flyweight/static_holder_fwd.hpp>
#include <boost/flyweight/tag.hpp>
#include <boost/flyweight/tracking_tag.hpp>

