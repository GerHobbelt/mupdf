
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/checked_delete.hpp>
#include <boost/core/addressof.hpp>
#include <boost/core/alignof.hpp>
#include <boost/core/alloc_construct.hpp>
#include <boost/core/allocator_access.hpp>
#include <boost/core/allocator_traits.hpp>
#include <boost/core/bit.hpp>
#include <boost/core/checked_delete.hpp>
#include <boost/core/cmath.hpp>
#include <boost/core/data.hpp>
#include <boost/core/default_allocator.hpp>
#include <boost/core/demangle.hpp>
#include <boost/core/empty_value.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/core/exchange.hpp>
#include <boost/core/explicit_operator_bool.hpp>
#include <boost/core/fclose_deleter.hpp>
#include <boost/core/first_scalar.hpp>
#include <boost/core/functor.hpp>
#include <boost/core/identity.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/core/invoke_swap.hpp>
#include <boost/core/is_same.hpp>
#include <boost/core/launder.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/core/make_span.hpp>
#include <boost/core/max_align.hpp>
#include <boost/core/memory_resource.hpp>
#include <boost/core/no_exceptions_support.hpp>
#include <boost/core/noinit_adaptor.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/core/nvp.hpp>
#include <boost/core/pointer_in_range.hpp>
#include <boost/core/pointer_traits.hpp>
#include <boost/core/quick_exit.hpp>
#include <boost/core/ref.hpp>
#include <boost/core/scoped_enum.hpp>
#include <boost/core/serialization.hpp>
#include <boost/core/size.hpp>
#include <boost/core/snprintf.hpp>
#include <boost/core/span.hpp>
#include <boost/core/swap.hpp>
#include <boost/core/type_name.hpp>
#include <boost/core/typeinfo.hpp>
#include <boost/core/uncaught_exceptions.hpp>
#include <boost/core/underlying_type.hpp>
#include <boost/core/use_default.hpp>
#include <boost/core/verbose_terminate_handler.hpp>
#include <boost/core/yield_primitives.hpp>
#include <boost/get_pointer.hpp>
#include <boost/iterator.hpp>
#include <boost/non_type.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <boost/swap.hpp>
#include <boost/type.hpp>
#include <boost/utility/addressof.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/explicit_operator_bool.hpp>
#include <boost/utility/swap.hpp>
#include <boost/visit_each.hpp>

