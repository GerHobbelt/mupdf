
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/signals2.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/signals2/deconstruct.hpp>
#include <boost/signals2/deconstruct_ptr.hpp>
#include <boost/signals2/dummy_mutex.hpp>
#include <boost/signals2/expired_slot.hpp>
#include <boost/signals2/last_value.hpp>
#include <boost/signals2/mutex.hpp>
#include <boost/signals2/optional_last_value.hpp>
#include <boost/signals2/postconstructible.hpp>
#include <boost/signals2/predestructible.hpp>
#include <boost/signals2/preprocessed_signal.hpp>
#include <boost/signals2/preprocessed_slot.hpp>
#include <boost/signals2/shared_connection_block.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/signal_base.hpp>
#include <boost/signals2/signal_type.hpp>
#include <boost/signals2/slot.hpp>
#include <boost/signals2/slot_base.hpp>
#include <boost/signals2/trackable.hpp>
#include <boost/signals2/variadic_signal.hpp>
#include <boost/signals2/variadic_slot.hpp>

