
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/basic_parser.hpp>
#include <boost/json/basic_parser_impl.hpp>
#include <boost/json/debug_printers.hpp>
#include <boost/json/error.hpp>
#include <boost/json/fwd.hpp>
#include <boost/json/is_deallocate_trivial.hpp>
#include <boost/json/kind.hpp>
#include <boost/json/monotonic_resource.hpp>
#include <boost/json/null_resource.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/parse_into.hpp>
#include <boost/json/parse_options.hpp>
#include <boost/json/parser.hpp>
#include <boost/json/pilfer.hpp>
#include <boost/json/result_for.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/serialize_options.hpp>
#include <boost/json/serializer.hpp>
#include <boost/json/set_pointer_options.hpp>
#include <boost/json/src.hpp>
#include <boost/json/static_resource.hpp>
#include <boost/json/storage_ptr.hpp>
#include <boost/json/stream_parser.hpp>
#include <boost/json/string.hpp>
#include <boost/json/string_view.hpp>
#include <boost/json/value.hpp>
#include <boost/json/value_from.hpp>
#include <boost/json/value_ref.hpp>
#include <boost/json/value_stack.hpp>
#include <boost/json/value_to.hpp>
#include <boost/json/visit.hpp>

