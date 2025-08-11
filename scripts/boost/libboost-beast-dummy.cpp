
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/beast.hpp>
#include <boost/beast/_experimental/http/icy_stream.hpp>
#include <boost/beast/_experimental/unit_test/amount.hpp>
#include <boost/beast/_experimental/unit_test/dstream.hpp>
#include <boost/beast/_experimental/unit_test/global_suites.hpp>
#include <boost/beast/_experimental/unit_test/match.hpp>
#include <boost/beast/_experimental/unit_test/recorder.hpp>
#include <boost/beast/_experimental/unit_test/reporter.hpp>
#include <boost/beast/_experimental/unit_test/results.hpp>
#include <boost/beast/_experimental/unit_test/runner.hpp>
#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <boost/beast/_experimental/unit_test/suite_info.hpp>
#include <boost/beast/_experimental/unit_test/suite_list.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/async_base.hpp>
#include <boost/beast/core/basic_stream.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffer_ref.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/buffered_read_stream.hpp>
#include <boost/beast/core/buffers_adaptor.hpp>
#include <boost/beast/core/buffers_cat.hpp>
#include <boost/beast/core/buffers_generator.hpp>
#include <boost/beast/core/buffers_prefix.hpp>
#include <boost/beast/core/buffers_range.hpp>
#include <boost/beast/core/buffers_suffix.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/file.hpp>
#include <boost/beast/core/file_base.hpp>
#include <boost/beast/core/file_posix.hpp>
#include <boost/beast/core/file_stdio.hpp>
#include <boost/beast/core/file_win32.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/flat_static_buffer.hpp>
#include <boost/beast/core/flat_stream.hpp>
#include <boost/beast/core/make_printable.hpp>
#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/core/ostream.hpp>
#include <boost/beast/core/rate_policy.hpp>
#include <boost/beast/core/read_size.hpp>
#include <boost/beast/core/role.hpp>
#include <boost/beast/core/saved_handler.hpp>
#include <boost/beast/core/span.hpp>
#include <boost/beast/core/static_buffer.hpp>
#include <boost/beast/core/static_string.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/beast/core/string_param.hpp>
#include <boost/beast/core/string_type.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/basic_dynamic_body.hpp>
#include <boost/beast/http/basic_dynamic_body_fwd.hpp>
#include <boost/beast/http/basic_file_body.hpp>
#include <boost/beast/http/basic_file_body_fwd.hpp>
#include <boost/beast/http/basic_parser.hpp>
#include <boost/beast/http/buffer_body.hpp>
#include <boost/beast/http/buffer_body_fwd.hpp>
#include <boost/beast/http/chunk_encode.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/dynamic_body_fwd.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/empty_body_fwd.hpp>
#include <boost/beast/http/error.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/fields_fwd.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/file_body_fwd.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/message_generator_fwd.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/parser_fwd.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/rfc7230.hpp>
#include <boost/beast/http/serializer.hpp>
#include <boost/beast/http/serializer_fwd.hpp>
#include <boost/beast/http/span_body.hpp>
#include <boost/beast/http/span_body_fwd.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/beast/http/type_traits.hpp>
#include <boost/beast/http/vector_body.hpp>
#include <boost/beast/http/vector_body_fwd.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/src.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/error.hpp>
#include <boost/beast/websocket/option.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/websocket/stream_base.hpp>
#include <boost/beast/websocket/stream_fwd.hpp>
#include <boost/beast/websocket/teardown.hpp>
#include <boost/beast/zlib.hpp>
#include <boost/beast/zlib/deflate_stream.hpp>
#include <boost/beast/zlib/error.hpp>
#include <boost/beast/zlib/inflate_stream.hpp>
#include <boost/beast/zlib/zlib.hpp>

