
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/tr1/array.hpp>
#include <boost/tr1/cmath.hpp>
#include <boost/tr1/complex.hpp>
#include <boost/tr1/functional.hpp>
#include <boost/tr1/memory.hpp>
#include <boost/tr1/random.hpp>
#include <boost/tr1/regex.hpp>
#include <boost/tr1/tr1/bcc32/array.h>
#include <boost/tr1/tr1/bcc32/random.h>
#include <boost/tr1/tr1/bcc32/regex.h>
#include <boost/tr1/tr1/bcc32/tuple.h>
#include <boost/tr1/tr1/bcc32/type_tra.h>
#include <boost/tr1/tr1/bcc32/unordere.h>
#include <boost/tr1/tuple.hpp>
#include <boost/tr1/type_traits.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/unordered_set.hpp>
#include <boost/tr1/utility.hpp>

