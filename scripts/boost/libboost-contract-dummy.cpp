
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/contract.hpp>
#include <boost/contract/assert.hpp>
#include <boost/contract/base_types.hpp>
#include <boost/contract/call_if.hpp>
#include <boost/contract/check.hpp>
#include <boost/contract/constructor.hpp>
#include <boost/contract/core/access.hpp>
#include <boost/contract/core/check_macro.hpp>
#include <boost/contract/core/config.hpp>
#include <boost/contract/core/constructor_precondition.hpp>
#include <boost/contract/core/exception.hpp>
#include <boost/contract/core/specify.hpp>
#include <boost/contract/core/virtual.hpp>
#include <boost/contract/destructor.hpp>
#include <boost/contract/function.hpp>
#include <boost/contract/old.hpp>
#include <boost/contract/override.hpp>
#include <boost/contract/public_function.hpp>
#include <boost/contract_macro.hpp>

