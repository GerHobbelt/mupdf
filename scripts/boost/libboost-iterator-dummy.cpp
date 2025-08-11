
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/function_output_iterator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/indirect_reference.hpp>
#include <boost/iterator/advance.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/distance.hpp>
#include <boost/iterator/enable_if_convertible.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/function_input_iterator.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/iterator/generator_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/interoperable.hpp>
#include <boost/iterator/is_iterator.hpp>
#include <boost/iterator/is_lvalue_iterator.hpp>
#include <boost/iterator/is_readable_iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_archetypes.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/min_category.hpp>
#include <boost/iterator/minimum_category.hpp>
#include <boost/iterator/new_iterator_tests.hpp>
#include <boost/iterator/permutation_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/iterator/shared_container_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/next_prior.hpp>
#include <boost/pointee.hpp>
#include <boost/shared_container_iterator.hpp>

