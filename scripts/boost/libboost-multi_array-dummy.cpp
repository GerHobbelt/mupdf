
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/multi_array.hpp>
#include <boost/multi_array/algorithm.hpp>
#include <boost/multi_array/base.hpp>
#include <boost/multi_array/collection_concept.hpp>
#include <boost/multi_array/concept_checks.hpp>
#include <boost/multi_array/copy_array.hpp>
#include <boost/multi_array/extent_gen.hpp>
#include <boost/multi_array/extent_range.hpp>
#include <boost/multi_array/index_gen.hpp>
#include <boost/multi_array/index_range.hpp>
#include <boost/multi_array/iterator.hpp>
#include <boost/multi_array/multi_array_ref.hpp>
#include <boost/multi_array/range_list.hpp>
#include <boost/multi_array/storage_order.hpp>
#include <boost/multi_array/subarray.hpp>
#include <boost/multi_array/types.hpp>
#include <boost/multi_array/view.hpp>

