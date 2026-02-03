
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/decimal.hpp>
#include <boost/decimal/cfenv.hpp>
#include <boost/decimal/cfloat.hpp>
#include <boost/decimal/charconv.hpp>
#include <boost/decimal/cmath.hpp>
#include <boost/decimal/cstdio.hpp>
#include <boost/decimal/cstdlib.hpp>
#include <boost/decimal/decimal128_t.hpp>
#include <boost/decimal/decimal32_t.hpp>
#include <boost/decimal/decimal64_t.hpp>
#include <boost/decimal/decimal_fast128_t.hpp>
#include <boost/decimal/decimal_fast32_t.hpp>
#include <boost/decimal/decimal_fast64_t.hpp>
#include <boost/decimal/fmt_format.hpp>
#include <boost/decimal/format.hpp>
#include <boost/decimal/fwd.hpp>
#include <boost/decimal/hash.hpp>
#include <boost/decimal/iostream.hpp>
#include <boost/decimal/literals.hpp>
#include <boost/decimal/numbers.hpp>
#include <boost/decimal/string.hpp>
#include <boost/decimal/uint128_t.hpp>

