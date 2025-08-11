
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/integer.hpp>
#include <boost/integer/common_factor.hpp>
#include <boost/integer/common_factor_ct.hpp>
#include <boost/integer/common_factor_rt.hpp>
#include <boost/integer/extended_euclidean.hpp>
#include <boost/integer/integer_log2.hpp>
#include <boost/integer/integer_mask.hpp>
#include <boost/integer/mod_inverse.hpp>
#include <boost/integer/static_log2.hpp>
#include <boost/integer/static_min_max.hpp>
#include <boost/integer_fwd.hpp>
#include <boost/integer_traits.hpp>

