
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/leaf.hpp>
#include <boost/leaf/common.hpp>
#include <boost/leaf/config.hpp>
#include <boost/leaf/config/tls.hpp>
#include <boost/leaf/config/tls_array.hpp>
#include <boost/leaf/config/tls_cpp11.hpp>
#include <boost/leaf/config/tls_freertos.hpp>
#include <boost/leaf/config/tls_globals.hpp>
#include <boost/leaf/context.hpp>
#include <boost/leaf/diagnostics.hpp>
#include <boost/leaf/error.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/handle_errors.hpp>
#include <boost/leaf/on_error.hpp>
#include <boost/leaf/pred.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/to_variant.hpp>

