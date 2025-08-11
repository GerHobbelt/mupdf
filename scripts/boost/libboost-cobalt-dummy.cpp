
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/cobalt.hpp>
#include <boost/cobalt/async_for.hpp>
#include <boost/cobalt/channel.hpp>
#include <boost/cobalt/composition.hpp>
#include <boost/cobalt/concepts.hpp>
#include <boost/cobalt/config.hpp>
#include <boost/cobalt/detached.hpp>
#include <boost/cobalt/error.hpp>
#include <boost/cobalt/gather.hpp>
#include <boost/cobalt/generator.hpp>
#include <boost/cobalt/io.hpp>
#include <boost/cobalt/io/acceptor.hpp>
#include <boost/cobalt/io/buffer.hpp>
#include <boost/cobalt/io/datagram_socket.hpp>
#include <boost/cobalt/io/endpoint.hpp>
#include <boost/cobalt/io/file.hpp>
#include <boost/cobalt/io/ops.hpp>
#include <boost/cobalt/io/pipe.hpp>
#include <boost/cobalt/io/random_access_device.hpp>
#include <boost/cobalt/io/random_access_file.hpp>
#include <boost/cobalt/io/read.hpp>
#include <boost/cobalt/io/resolver.hpp>
#include <boost/cobalt/io/seq_packet_socket.hpp>
#include <boost/cobalt/io/serial_port.hpp>
#include <boost/cobalt/io/signal_set.hpp>
#include <boost/cobalt/io/sleep.hpp>
#include <boost/cobalt/io/socket.hpp>
#include <boost/cobalt/io/ssl.hpp>
#include <boost/cobalt/io/steady_timer.hpp>
#include <boost/cobalt/io/stream.hpp>
#include <boost/cobalt/io/stream_file.hpp>
#include <boost/cobalt/io/stream_socket.hpp>
#include <boost/cobalt/io/system_timer.hpp>
#include <boost/cobalt/io/write.hpp>
#include <boost/cobalt/join.hpp>
#include <boost/cobalt/main.hpp>
#include <boost/cobalt/noop.hpp>
#include <boost/cobalt/op.hpp>
#include <boost/cobalt/promise.hpp>
#include <boost/cobalt/race.hpp>
#include <boost/cobalt/result.hpp>
#include <boost/cobalt/run.hpp>
#include <boost/cobalt/spawn.hpp>
#include <boost/cobalt/task.hpp>
#include <boost/cobalt/this_coro.hpp>
#include <boost/cobalt/this_thread.hpp>
#include <boost/cobalt/thread.hpp>
#include <boost/cobalt/unique_handle.hpp>
#include <boost/cobalt/wait_group.hpp>
#include <boost/cobalt/with.hpp>

