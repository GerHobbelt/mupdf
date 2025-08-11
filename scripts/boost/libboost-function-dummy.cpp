
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/function.hpp>
#include <boost/function/function0.hpp>
#include <boost/function/function1.hpp>
#include <boost/function/function10.hpp>
#include <boost/function/function2.hpp>
#include <boost/function/function3.hpp>
#include <boost/function/function4.hpp>
#include <boost/function/function5.hpp>
#include <boost/function/function6.hpp>
#include <boost/function/function7.hpp>
#include <boost/function/function8.hpp>
#include <boost/function/function9.hpp>
#include <boost/function/function_base.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/function/function_template.hpp>
#include <boost/function/function_typeof.hpp>
#include <boost/function_equal.hpp>

