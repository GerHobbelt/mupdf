
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/enable_shared_from_this.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/make_unique.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/pointer_to_other.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/allocate_local_shared_array.hpp>
#include <boost/smart_ptr/allocate_shared_array.hpp>
#include <boost/smart_ptr/allocate_unique.hpp>
#include <boost/smart_ptr/atomic_shared_ptr.hpp>
#include <boost/smart_ptr/bad_weak_ptr.hpp>
#include <boost/smart_ptr/enable_shared_from.hpp>
#include <boost/smart_ptr/enable_shared_from_raw.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>
#include <boost/smart_ptr/make_local_shared_array.hpp>
#include <boost/smart_ptr/make_local_shared_object.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/smart_ptr/make_shared_array.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/make_unique.hpp>
#include <boost/smart_ptr/owner_equal_to.hpp>
#include <boost/smart_ptr/owner_hash.hpp>
#include <boost/smart_ptr/owner_less.hpp>
#include <boost/smart_ptr/scoped_array.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/smart_ptr/shared_array.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <boost/weak_ptr.hpp>

