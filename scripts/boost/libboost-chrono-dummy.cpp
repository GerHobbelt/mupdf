
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/chrono.hpp>
#include <boost/chrono/ceil.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/chrono/chrono_io.hpp>
#include <boost/chrono/clock_string.hpp>
#include <boost/chrono/config.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/chrono/floor.hpp>
#include <boost/chrono/include.hpp>
#include <boost/chrono/io/duration_get.hpp>
#include <boost/chrono/io/duration_io.hpp>
#include <boost/chrono/io/duration_put.hpp>
#include <boost/chrono/io/duration_style.hpp>
#include <boost/chrono/io/duration_units.hpp>
#include <boost/chrono/io/ios_base_state.hpp>
#include <boost/chrono/io/time_point_get.hpp>
#include <boost/chrono/io/time_point_io.hpp>
#include <boost/chrono/io/time_point_put.hpp>
#include <boost/chrono/io/time_point_units.hpp>
#include <boost/chrono/io/timezone.hpp>
#include <boost/chrono/io/utility/ios_base_state_ptr.hpp>
#include <boost/chrono/io/utility/manip_base.hpp>
#include <boost/chrono/io/utility/to_string.hpp>
#include <boost/chrono/io_v1/chrono_io.hpp>
#include <boost/chrono/process_cpu_clocks.hpp>
#include <boost/chrono/round.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/thread_clock.hpp>
#include <boost/chrono/time_point.hpp>
#include <boost/chrono/typeof/boost/chrono/chrono.hpp>
#include <boost/chrono/typeof/boost/ratio.hpp>

