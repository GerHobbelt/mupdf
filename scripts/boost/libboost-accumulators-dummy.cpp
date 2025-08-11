
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/accumulators_fwd.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/accumulator_concept.hpp>
#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/framework/accumulators/droppable_accumulator.hpp>
#include <boost/accumulators/framework/accumulators/external_accumulator.hpp>
#include <boost/accumulators/framework/accumulators/reference_accumulator.hpp>
#include <boost/accumulators/framework/accumulators/value_accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/framework/external.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/features.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/weight.hpp>
#include <boost/accumulators/framework/parameters/weights.hpp>
#include <boost/accumulators/numeric/functional.hpp>
#include <boost/accumulators/numeric/functional/complex.hpp>
#include <boost/accumulators/numeric/functional/valarray.hpp>
#include <boost/accumulators/numeric/functional/vector.hpp>
#include <boost/accumulators/numeric/functional_fwd.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/density.hpp>
#include <boost/accumulators/statistics/error_of.hpp>
#include <boost/accumulators/statistics/error_of_mean.hpp>
#include <boost/accumulators/statistics/extended_p_square.hpp>
#include <boost/accumulators/statistics/extended_p_square_quantile.hpp>
#include <boost/accumulators/statistics/kurtosis.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/p_square_cumul_dist.hpp>
#include <boost/accumulators/statistics/p_square_cumulative_distribution.hpp>
#include <boost/accumulators/statistics/p_square_quantile.hpp>
#include <boost/accumulators/statistics/parameters/quantile_probability.hpp>
#include <boost/accumulators/statistics/peaks_over_threshold.hpp>
#include <boost/accumulators/statistics/pot_quantile.hpp>
#include <boost/accumulators/statistics/pot_tail_mean.hpp>
#include <boost/accumulators/statistics/rolling_count.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/rolling_moment.hpp>
#include <boost/accumulators/statistics/rolling_sum.hpp>
#include <boost/accumulators/statistics/rolling_variance.hpp>
#include <boost/accumulators/statistics/rolling_window.hpp>
#include <boost/accumulators/statistics/skewness.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <boost/accumulators/statistics/tail.hpp>
#include <boost/accumulators/statistics/tail_mean.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>
#include <boost/accumulators/statistics/tail_variate.hpp>
#include <boost/accumulators/statistics/tail_variate_means.hpp>
#include <boost/accumulators/statistics/times2_iterator.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>
#include <boost/accumulators/statistics/weighted_covariance.hpp>
#include <boost/accumulators/statistics/weighted_density.hpp>
#include <boost/accumulators/statistics/weighted_extended_p_square.hpp>
#include <boost/accumulators/statistics/weighted_kurtosis.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_median.hpp>
#include <boost/accumulators/statistics/weighted_moment.hpp>
#include <boost/accumulators/statistics/weighted_p_square_cumul_dist.hpp>
#include <boost/accumulators/statistics/weighted_p_square_cumulative_distribution.hpp>
#include <boost/accumulators/statistics/weighted_p_square_quantile.hpp>
#include <boost/accumulators/statistics/weighted_peaks_over_threshold.hpp>
#include <boost/accumulators/statistics/weighted_skewness.hpp>
#include <boost/accumulators/statistics/weighted_sum.hpp>
#include <boost/accumulators/statistics/weighted_sum_kahan.hpp>
#include <boost/accumulators/statistics/weighted_tail_mean.hpp>
#include <boost/accumulators/statistics/weighted_tail_quantile.hpp>
#include <boost/accumulators/statistics/weighted_tail_variate_means.hpp>
#include <boost/accumulators/statistics/weighted_variance.hpp>
#include <boost/accumulators/statistics/with_error.hpp>
#include <boost/accumulators/statistics_fwd.hpp>

