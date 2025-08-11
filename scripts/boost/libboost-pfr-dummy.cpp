
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/pfr.hpp>
#include <boost/pfr/config.hpp>
#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <boost/pfr/functions_for.hpp>
#include <boost/pfr/functors.hpp>
#include <boost/pfr/io.hpp>
#include <boost/pfr/io_fields.hpp>
#include <boost/pfr/ops.hpp>
#include <boost/pfr/ops_fields.hpp>
#include <boost/pfr/traits.hpp>
#include <boost/pfr/traits_fwd.hpp>
#include <boost/pfr/tuple_size.hpp>

