
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/sort/block_indirect_sort/blk_detail/backbone.hpp>
#include <boost/sort/block_indirect_sort/blk_detail/block.hpp>
#include <boost/sort/block_indirect_sort/blk_detail/constants.hpp>
#include <boost/sort/block_indirect_sort/blk_detail/merge_blocks.hpp>
#include <boost/sort/block_indirect_sort/blk_detail/move_blocks.hpp>
#include <boost/sort/block_indirect_sort/blk_detail/parallel_sort.hpp>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include <boost/sort/common/deque_cnc.hpp>
#include <boost/sort/common/file_vector.hpp>
#include <boost/sort/common/indirect.hpp>
#include <boost/sort/common/int_array.hpp>
#include <boost/sort/common/merge_block.hpp>
#include <boost/sort/common/merge_four.hpp>
#include <boost/sort/common/merge_vector.hpp>
#include <boost/sort/common/pivot.hpp>
#include <boost/sort/common/range.hpp>
#include <boost/sort/common/rearrange.hpp>
#include <boost/sort/common/scheduler.hpp>
#include <boost/sort/common/sort_basic.hpp>
#include <boost/sort/common/spinlock.hpp>
#include <boost/sort/common/stack_cnc.hpp>
#include <boost/sort/common/time_measure.hpp>
#include <boost/sort/common/util/algorithm.hpp>
#include <boost/sort/common/util/atomic.hpp>
#include <boost/sort/common/util/circular_buffer.hpp>
#include <boost/sort/common/util/insert.hpp>
#include <boost/sort/common/util/merge.hpp>
#include <boost/sort/common/util/search.hpp>
#include <boost/sort/common/util/traits.hpp>
#include <boost/sort/flat_stable_sort/flat_stable_sort.hpp>
#include <boost/sort/insert_sort/insert_sort.hpp>
#include <boost/sort/parallel_stable_sort/parallel_stable_sort.hpp>
#include <boost/sort/pdqsort/pdqsort.hpp>
#include <boost/sort/sample_sort/sample_sort.hpp>
#include <boost/sort/sort.hpp>
#include <boost/sort/spinsort/spinsort.hpp>
#include <boost/sort/spreadsort/float_sort.hpp>
#include <boost/sort/spreadsort/integer_sort.hpp>
#include <boost/sort/spreadsort/spreadsort.hpp>
#include <boost/sort/spreadsort/string_sort.hpp>

