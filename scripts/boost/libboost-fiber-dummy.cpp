
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/fiber/algo/algorithm.hpp>
#include <boost/fiber/algo/round_robin.hpp>
#include <boost/fiber/algo/shared_work.hpp>
#include <boost/fiber/algo/work_stealing.hpp>
#include <boost/fiber/all.hpp>
#include <boost/fiber/barrier.hpp>
#include <boost/fiber/buffered_channel.hpp>
#include <boost/fiber/channel_op_status.hpp>
#include <boost/fiber/condition_variable.hpp>
#include <boost/fiber/context.hpp>
#include <boost/fiber/cuda/waitfor.hpp>
#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/fixedsize_stack.hpp>
#include <boost/fiber/fss.hpp>
#include <boost/fiber/future.hpp>
#include <boost/fiber/future/async.hpp>
#include <boost/fiber/future/future.hpp>
#include <boost/fiber/future/future_status.hpp>
#include <boost/fiber/future/packaged_task.hpp>
#include <boost/fiber/future/promise.hpp>
#include <boost/fiber/hip/waitfor.hpp>
#include <boost/fiber/mutex.hpp>
#include <boost/fiber/numa/algo/work_stealing.hpp>
#include <boost/fiber/numa/all.hpp>
#include <boost/fiber/numa/pin_thread.hpp>
#include <boost/fiber/numa/topology.hpp>
#include <boost/fiber/operations.hpp>
#include <boost/fiber/policy.hpp>
#include <boost/fiber/pooled_fixedsize_stack.hpp>
#include <boost/fiber/properties.hpp>
#include <boost/fiber/protected_fixedsize_stack.hpp>
#include <boost/fiber/recursive_mutex.hpp>
#include <boost/fiber/recursive_timed_mutex.hpp>
#include <boost/fiber/scheduler.hpp>
#include <boost/fiber/segmented_stack.hpp>
#include <boost/fiber/stack_allocator_wrapper.hpp>
#include <boost/fiber/timed_mutex.hpp>
#include <boost/fiber/type.hpp>
#include <boost/fiber/unbuffered_channel.hpp>
#include <boost/fiber/waker.hpp>

