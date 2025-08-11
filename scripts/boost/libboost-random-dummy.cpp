
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/nondet_random.hpp>
#include <boost/random.hpp>
#include <boost/random/additive_combine.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/beta_distribution.hpp>
#include <boost/random/binomial_distribution.hpp>
#include <boost/random/cauchy_distribution.hpp>
#include <boost/random/chi_squared_distribution.hpp>
#include <boost/random/discard_block.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/extreme_value_distribution.hpp>
#include <boost/random/faure.hpp>
#include <boost/random/fisher_f_distribution.hpp>
#include <boost/random/gamma_distribution.hpp>
#include <boost/random/generalized_inverse_gaussian_distribution.hpp>
#include <boost/random/generate_canonical.hpp>
#include <boost/random/geometric_distribution.hpp>
#include <boost/random/hyperexponential_distribution.hpp>
#include <boost/random/independent_bits.hpp>
#include <boost/random/inverse_gaussian_distribution.hpp>
#include <boost/random/inversive_congruential.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/laplace_distribution.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/linear_feedback_shift.hpp>
#include <boost/random/lognormal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/mixmax.hpp>
#include <boost/random/negative_binomial_distribution.hpp>
#include <boost/random/niederreiter_base2.hpp>
#include <boost/random/non_central_chi_squared_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/piecewise_constant_distribution.hpp>
#include <boost/random/piecewise_linear_distribution.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/random_number_generator.hpp>
#include <boost/random/ranlux.hpp>
#include <boost/random/seed_seq.hpp>
#include <boost/random/shuffle_order.hpp>
#include <boost/random/shuffle_output.hpp>
#include <boost/random/sobol.hpp>
#include <boost/random/splitmix64.hpp>
#include <boost/random/student_t_distribution.hpp>
#include <boost/random/subtract_with_carry.hpp>
#include <boost/random/taus88.hpp>
#include <boost/random/traits.hpp>
#include <boost/random/triangle_distribution.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/weibull_distribution.hpp>
#include <boost/random/xor_combine.hpp>
#include <boost/random/xoshiro.hpp>

