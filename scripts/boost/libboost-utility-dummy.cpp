
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/call_traits.hpp>
#include <boost/compressed_pair.hpp>
#include <boost/operators.hpp>
#include <boost/operators_v1.hpp>
#include <boost/utility.hpp>
#include <boost/utility/base_from_member.hpp>
#include <boost/utility/binary.hpp>
#include <boost/utility/compare_pointees.hpp>
#include <boost/utility/identity_type.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/utility/string_ref_fwd.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/utility/string_view_fwd.hpp>
#include <boost/utility/typed_in_place_factory.hpp>
#include <boost/utility/value_init.hpp>

