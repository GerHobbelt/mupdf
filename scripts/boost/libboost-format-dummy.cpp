
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/format.hpp>
#include <boost/format/alt_sstream.hpp>
#include <boost/format/alt_sstream_impl.hpp>
#include <boost/format/exceptions.hpp>
#include <boost/format/feed_args.hpp>
#include <boost/format/format_class.hpp>
#include <boost/format/format_fwd.hpp>
#include <boost/format/format_implementation.hpp>
#include <boost/format/free_funcs.hpp>
#include <boost/format/group.hpp>
#include <boost/format/internals.hpp>
#include <boost/format/internals_fwd.hpp>
#include <boost/format/parsing.hpp>

