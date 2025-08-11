
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/histogram.hpp>
#include <boost/histogram/accumulators.hpp>
#include <boost/histogram/accumulators/collector.hpp>
#include <boost/histogram/accumulators/count.hpp>
#include <boost/histogram/accumulators/fraction.hpp>
#include <boost/histogram/accumulators/is_thread_safe.hpp>
#include <boost/histogram/accumulators/mean.hpp>
#include <boost/histogram/accumulators/ostream.hpp>
#include <boost/histogram/accumulators/sum.hpp>
#include <boost/histogram/accumulators/weighted_mean.hpp>
#include <boost/histogram/accumulators/weighted_sum.hpp>
#include <boost/histogram/algorithm.hpp>
#include <boost/histogram/algorithm/empty.hpp>
#include <boost/histogram/algorithm/project.hpp>
#include <boost/histogram/algorithm/reduce.hpp>
#include <boost/histogram/algorithm/sum.hpp>
#include <boost/histogram/axis.hpp>
#include <boost/histogram/axis/boolean.hpp>
#include <boost/histogram/axis/category.hpp>
#include <boost/histogram/axis/integer.hpp>
#include <boost/histogram/axis/interval_view.hpp>
#include <boost/histogram/axis/iterator.hpp>
#include <boost/histogram/axis/metadata_base.hpp>
#include <boost/histogram/axis/option.hpp>
#include <boost/histogram/axis/ostream.hpp>
#include <boost/histogram/axis/polymorphic_bin.hpp>
#include <boost/histogram/axis/regular.hpp>
#include <boost/histogram/axis/traits.hpp>
#include <boost/histogram/axis/variable.hpp>
#include <boost/histogram/axis/variant.hpp>
#include <boost/histogram/fwd.hpp>
#include <boost/histogram/histogram.hpp>
#include <boost/histogram/indexed.hpp>
#include <boost/histogram/literals.hpp>
#include <boost/histogram/make_histogram.hpp>
#include <boost/histogram/make_profile.hpp>
#include <boost/histogram/multi_index.hpp>
#include <boost/histogram/ostream.hpp>
#include <boost/histogram/sample.hpp>
#include <boost/histogram/serialization.hpp>
#include <boost/histogram/storage_adaptor.hpp>
#include <boost/histogram/unlimited_storage.hpp>
#include <boost/histogram/unsafe_access.hpp>
#include <boost/histogram/utility/binomial_proportion_interval.hpp>
#include <boost/histogram/utility/clopper_pearson_interval.hpp>
#include <boost/histogram/utility/jeffreys_interval.hpp>
#include <boost/histogram/utility/wald_interval.hpp>
#include <boost/histogram/utility/wilson_interval.hpp>
#include <boost/histogram/weight.hpp>

