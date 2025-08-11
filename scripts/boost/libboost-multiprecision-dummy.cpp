
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/multiprecision/complex128.hpp>
#include <boost/multiprecision/complex_adaptor.hpp>
#include <boost/multiprecision/concepts/mp_number_archetypes.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_bin_float/io.hpp>
#include <boost/multiprecision/cpp_bin_float/transcendental.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_fabs.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_floor.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_fma.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_fpclassify.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_frexp.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isinf.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isnan.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_ldexp.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_limits.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_log.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_sqrt.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_constants.hpp>
#include <boost/multiprecision/cpp_double_fp.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_int/add.hpp>
#include <boost/multiprecision/cpp_int/add_unsigned.hpp>
#include <boost/multiprecision/cpp_int/bitwise.hpp>
#include <boost/multiprecision/cpp_int/checked.hpp>
#include <boost/multiprecision/cpp_int/comparison.hpp>
#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>
#include <boost/multiprecision/cpp_int/divide.hpp>
#include <boost/multiprecision/cpp_int/import_export.hpp>
#include <boost/multiprecision/cpp_int/intel_intrinsics.hpp>
#include <boost/multiprecision/cpp_int/limits.hpp>
#include <boost/multiprecision/cpp_int/literals.hpp>
#include <boost/multiprecision/cpp_int/misc.hpp>
#include <boost/multiprecision/cpp_int/multiply.hpp>
#include <boost/multiprecision/cpp_int/serialize.hpp>
#include <boost/multiprecision/cpp_int/value_pack.hpp>
#include <boost/multiprecision/debug_adaptor.hpp>
#include <boost/multiprecision/eigen.hpp>
#include <boost/multiprecision/float128.hpp>
#include <boost/multiprecision/fwd.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/integer.hpp>
#include <boost/multiprecision/logged_adaptor.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/multiprecision/mpc.hpp>
#include <boost/multiprecision/mpfi.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/random.hpp>
#include <boost/multiprecision/rational_adaptor.hpp>
#include <boost/multiprecision/tommath.hpp>
#include <boost/multiprecision/traits/extract_exponent_type.hpp>
#include <boost/multiprecision/traits/is_backend.hpp>
#include <boost/multiprecision/traits/is_byte_container.hpp>
#include <boost/multiprecision/traits/is_complex.hpp>
#include <boost/multiprecision/traits/is_convertible_arithmetic.hpp>
#include <boost/multiprecision/traits/is_variable_precision.hpp>
#include <boost/multiprecision/traits/max_digits10.hpp>
#include <boost/multiprecision/traits/std_integer_traits.hpp>
#include <boost/multiprecision/traits/transcendental_reduction_type.hpp>

