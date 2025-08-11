
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/caller_context.hpp>
#include <boost/thread/completion_latch.hpp>
#include <boost/thread/concurrent_queues/deque_adaptor.hpp>
#include <boost/thread/concurrent_queues/deque_base.hpp>
#include <boost/thread/concurrent_queues/deque_views.hpp>
#include <boost/thread/concurrent_queues/queue_adaptor.hpp>
#include <boost/thread/concurrent_queues/queue_base.hpp>
#include <boost/thread/concurrent_queues/queue_op_status.hpp>
#include <boost/thread/concurrent_queues/queue_views.hpp>
#include <boost/thread/concurrent_queues/sync_bounded_queue.hpp>
#include <boost/thread/concurrent_queues/sync_deque.hpp>
#include <boost/thread/concurrent_queues/sync_priority_queue.hpp>
#include <boost/thread/concurrent_queues/sync_queue.hpp>
#include <boost/thread/concurrent_queues/sync_timed_queue.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/csbl/deque.hpp>
#include <boost/thread/csbl/devector.hpp>
#include <boost/thread/csbl/functional.hpp>
#include <boost/thread/csbl/list.hpp>
#include <boost/thread/csbl/memory.hpp>
#include <boost/thread/csbl/memory/allocator_arg.hpp>
#include <boost/thread/csbl/memory/allocator_traits.hpp>
#include <boost/thread/csbl/memory/config.hpp>
#include <boost/thread/csbl/memory/default_delete.hpp>
#include <boost/thread/csbl/memory/pointer_traits.hpp>
#include <boost/thread/csbl/memory/scoped_allocator.hpp>
#include <boost/thread/csbl/memory/shared_ptr.hpp>
#include <boost/thread/csbl/memory/unique_ptr.hpp>
#include <boost/thread/csbl/queue.hpp>
#include <boost/thread/csbl/tuple.hpp>
#include <boost/thread/csbl/vector.hpp>
#include <boost/thread/cv_status.hpp>
#include <boost/thread/exceptional_ptr.hpp>
#include <boost/thread/exceptions.hpp>
#include <boost/thread/executor.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>
#include <boost/thread/executors/executor.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <boost/thread/executors/generic_executor_ref.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/executors/loop_executor.hpp>
#include <boost/thread/executors/scheduled_thread_pool.hpp>
#include <boost/thread/executors/scheduler.hpp>
#include <boost/thread/executors/scheduling_adaptor.hpp>
#include <boost/thread/executors/serial_executor.hpp>
#include <boost/thread/executors/serial_executor_cont.hpp>
#include <boost/thread/executors/thread_executor.hpp>
#include <boost/thread/executors/work.hpp>
#include <boost/thread/externally_locked.hpp>
#include <boost/thread/externally_locked_stream.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/futures/future_error.hpp>
#include <boost/thread/futures/future_error_code.hpp>
#include <boost/thread/futures/future_status.hpp>
#include <boost/thread/futures/is_future_type.hpp>
#include <boost/thread/futures/launch.hpp>
#include <boost/thread/futures/wait_for_all.hpp>
#include <boost/thread/futures/wait_for_any.hpp>
#include <boost/thread/interruption.hpp>
#include <boost/thread/is_locked_by_this_thread.hpp>
#include <boost/thread/latch.hpp>
#include <boost/thread/lock_algorithms.hpp>
#include <boost/thread/lock_concepts.hpp>
#include <boost/thread/lock_factories.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lock_options.hpp>
#include <boost/thread/lock_traits.hpp>
#include <boost/thread/lock_types.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/lockable_concepts.hpp>
#include <boost/thread/lockable_traits.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/null_mutex.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/ostream_buffer.hpp>
#include <boost/thread/poly_lockable.hpp>
#include <boost/thread/poly_lockable_adapter.hpp>
#include <boost/thread/poly_shared_lockable.hpp>
#include <boost/thread/poly_shared_lockable_adapter.hpp>
#include <boost/thread/pthread/condition_variable.hpp>
#include <boost/thread/pthread/condition_variable_fwd.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <boost/thread/pthread/once.hpp>
#include <boost/thread/pthread/once_atomic.hpp>
#include <boost/thread/pthread/pthread_helpers.hpp>
#include <boost/thread/pthread/pthread_mutex_scoped_lock.hpp>
#include <boost/thread/pthread/recursive_mutex.hpp>
#include <boost/thread/pthread/shared_mutex.hpp>
#include <boost/thread/pthread/thread_data.hpp>
#include <boost/thread/pthread/thread_heap_alloc.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/reverse_lock.hpp>
#include <boost/thread/scoped_thread.hpp>
#include <boost/thread/shared_lock_guard.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/strict_lock.hpp>
#include <boost/thread/sync_bounded_queue.hpp>
#include <boost/thread/sync_queue.hpp>
#include <boost/thread/synchronized_value.hpp>
#include <boost/thread/testable_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_functors.hpp>
#include <boost/thread/thread_guard.hpp>
#include <boost/thread/thread_only.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/tss.hpp>
#include <boost/thread/user_scheduler.hpp>
#include <boost/thread/v2/shared_mutex.hpp>
#include <boost/thread/with_lock_guard.hpp>
#include <boost/thread/xtime.hpp>

