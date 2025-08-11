
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/mp11.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>
#include <boost/mp11/function.hpp>
#include <boost/mp11/integer_sequence.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/lambda.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/map.hpp>
#include <boost/mp11/mpl.hpp>
#include <boost/mp11/mpl_list.hpp>
#include <boost/mp11/mpl_tuple.hpp>
#include <boost/mp11/set.hpp>
#include <boost/mp11/tuple.hpp>
#include <boost/mp11/utility.hpp>

