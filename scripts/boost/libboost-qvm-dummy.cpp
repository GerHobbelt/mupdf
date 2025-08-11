
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/qvm.hpp>
#include <boost/qvm/all.hpp>
#include <boost/qvm/assert.hpp>
#include <boost/qvm/config.hpp>
#include <boost/qvm/deduce_mat.hpp>
#include <boost/qvm/deduce_quat.hpp>
#include <boost/qvm/deduce_scalar.hpp>
#include <boost/qvm/deduce_vec.hpp>
#include <boost/qvm/enable_if.hpp>
#include <boost/qvm/error.hpp>
#include <boost/qvm/is_scalar.hpp>
#include <boost/qvm/lite.hpp>
#include <boost/qvm/map.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/map_mat_vec.hpp>
#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/mat.hpp>
#include <boost/qvm/mat_access.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/mat_operations2.hpp>
#include <boost/qvm/mat_operations3.hpp>
#include <boost/qvm/mat_operations4.hpp>
#include <boost/qvm/mat_traits.hpp>
#include <boost/qvm/mat_traits_array.hpp>
#include <boost/qvm/mat_traits_defaults.hpp>
#include <boost/qvm/math.hpp>
#include <boost/qvm/operations.hpp>
#include <boost/qvm/quat.hpp>
#include <boost/qvm/quat_access.hpp>
#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/quat_traits.hpp>
#include <boost/qvm/quat_traits_array.hpp>
#include <boost/qvm/quat_traits_defaults.hpp>
#include <boost/qvm/quat_vec_operations.hpp>
#include <boost/qvm/scalar_traits.hpp>
#include <boost/qvm/static_assert.hpp>
#include <boost/qvm/swizzle.hpp>
#include <boost/qvm/swizzle2.hpp>
#include <boost/qvm/swizzle3.hpp>
#include <boost/qvm/swizzle4.hpp>
#include <boost/qvm/throw_exception.hpp>
#include <boost/qvm/to_string.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/vec_access.hpp>
#include <boost/qvm/vec_mat_operations.hpp>
#include <boost/qvm/vec_mat_operations2.hpp>
#include <boost/qvm/vec_mat_operations3.hpp>
#include <boost/qvm/vec_mat_operations4.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/vec_operations2.hpp>
#include <boost/qvm/vec_operations3.hpp>
#include <boost/qvm/vec_operations4.hpp>
#include <boost/qvm/vec_register.hpp>
#include <boost/qvm/vec_register2.hpp>
#include <boost/qvm/vec_register3.hpp>
#include <boost/qvm/vec_register4.hpp>
#include <boost/qvm/vec_traits.hpp>
#include <boost/qvm/vec_traits_array.hpp>
#include <boost/qvm/vec_traits_defaults.hpp>
#include <boost/qvm/vec_traits_gnuc.hpp>
#include <boost/qvm_lite.hpp>

