
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/apply_permutation.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/algorithm/cxx11/copy_n.hpp>
#include <boost/algorithm/cxx11/find_if_not.hpp>
#include <boost/algorithm/cxx11/iota.hpp>
#include <boost/algorithm/cxx11/is_partitioned.hpp>
#include <boost/algorithm/cxx11/is_permutation.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/cxx11/one_of.hpp>
#include <boost/algorithm/cxx11/partition_copy.hpp>
#include <boost/algorithm/cxx11/partition_point.hpp>
#include <boost/algorithm/cxx14/equal.hpp>
#include <boost/algorithm/cxx14/is_permutation.hpp>
#include <boost/algorithm/cxx14/mismatch.hpp>
#include <boost/algorithm/cxx17/exclusive_scan.hpp>
#include <boost/algorithm/cxx17/for_each_n.hpp>
#include <boost/algorithm/cxx17/inclusive_scan.hpp>
#include <boost/algorithm/cxx17/reduce.hpp>
#include <boost/algorithm/cxx17/transform_exclusive_scan.hpp>
#include <boost/algorithm/cxx17/transform_inclusive_scan.hpp>
#include <boost/algorithm/cxx17/transform_reduce.hpp>
#include <boost/algorithm/find_backward.hpp>
#include <boost/algorithm/find_not.hpp>
#include <boost/algorithm/gather.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/is_clamped.hpp>
#include <boost/algorithm/is_palindrome.hpp>
#include <boost/algorithm/is_partitioned_until.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/algorithm/searching/boyer_moore.hpp>
#include <boost/algorithm/searching/boyer_moore_horspool.hpp>
#include <boost/algorithm/searching/knuth_morris_pratt.hpp>
#include <boost/algorithm/sort_subrange.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/concept.hpp>
#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/predicate_facade.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/sequence_traits.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/std/list_traits.hpp>
#include <boost/algorithm/string/std/rope_traits.hpp>
#include <boost/algorithm/string/std/slist_traits.hpp>
#include <boost/algorithm/string/std/string_traits.hpp>
#include <boost/algorithm/string/std_containers_traits.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/yes_no_type.hpp>
#include <boost/algorithm/string_regex.hpp>

