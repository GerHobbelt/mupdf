
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/statechart/asynchronous_state_machine.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/deep_history.hpp>
#include <boost/statechart/deferral.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/event_base.hpp>
#include <boost/statechart/event_processor.hpp>
#include <boost/statechart/exception_translator.hpp>
#include <boost/statechart/fifo_scheduler.hpp>
#include <boost/statechart/fifo_worker.hpp>
#include <boost/statechart/history.hpp>
#include <boost/statechart/in_state_reaction.hpp>
#include <boost/statechart/null_exception_translator.hpp>
#include <boost/statechart/processor_container.hpp>
#include <boost/statechart/result.hpp>
#include <boost/statechart/shallow_history.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/termination.hpp>
#include <boost/statechart/transition.hpp>

