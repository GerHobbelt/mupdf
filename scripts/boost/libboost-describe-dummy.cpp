
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/describe.hpp>
#include <boost/describe/bases.hpp>
#include <boost/describe/class.hpp>
#include <boost/describe/descriptor_by_name.hpp>
#include <boost/describe/descriptor_by_pointer.hpp>
#include <boost/describe/enum.hpp>
#include <boost/describe/enum_from_string.hpp>
#include <boost/describe/enum_to_string.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/describe/modifier_description.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/describe/operators.hpp>

