
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/chain.hpp>
#include <boost/iostreams/char_traits.hpp>
#include <boost/iostreams/checked_operations.hpp>
#include <boost/iostreams/close.hpp>
#include <boost/iostreams/code_converter.hpp>
#include <boost/iostreams/combine.hpp>
#include <boost/iostreams/compose.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/constants.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/filter/aggregate.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/counter.hpp>
#include <boost/iostreams/filter/grep.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/line.hpp>
#include <boost/iostreams/filter/lzma.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/iostreams/filter/regex.hpp>
#include <boost/iostreams/filter/stdio.hpp>
#include <boost/iostreams/filter/symmetric.hpp>
#include <boost/iostreams/filter/test.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/flush.hpp>
#include <boost/iostreams/get.hpp>
#include <boost/iostreams/imbue.hpp>
#include <boost/iostreams/input_sequence.hpp>
#include <boost/iostreams/invert.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/operations_fwd.hpp>
#include <boost/iostreams/optimal_buffer_size.hpp>
#include <boost/iostreams/output_sequence.hpp>
#include <boost/iostreams/pipeline.hpp>
#include <boost/iostreams/positioning.hpp>
#include <boost/iostreams/put.hpp>
#include <boost/iostreams/putback.hpp>
#include <boost/iostreams/read.hpp>
#include <boost/iostreams/restrict.hpp>
#include <boost/iostreams/seek.hpp>
#include <boost/iostreams/skip.hpp>
#include <boost/iostreams/slice.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/traits.hpp>
#include <boost/iostreams/traits_fwd.hpp>
#include <boost/iostreams/write.hpp>

