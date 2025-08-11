
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/vmd/array.hpp>
#include <boost/vmd/array/to_seq.hpp>
#include <boost/vmd/array/to_tuple.hpp>
#include <boost/vmd/assert.hpp>
#include <boost/vmd/assert_is_array.hpp>
#include <boost/vmd/assert_is_empty.hpp>
#include <boost/vmd/assert_is_identifier.hpp>
#include <boost/vmd/assert_is_list.hpp>
#include <boost/vmd/assert_is_number.hpp>
#include <boost/vmd/assert_is_seq.hpp>
#include <boost/vmd/assert_is_tuple.hpp>
#include <boost/vmd/assert_is_type.hpp>
#include <boost/vmd/elem.hpp>
#include <boost/vmd/empty.hpp>
#include <boost/vmd/enum.hpp>
#include <boost/vmd/equal.hpp>
#include <boost/vmd/get_type.hpp>
#include <boost/vmd/identity.hpp>
#include <boost/vmd/is_array.hpp>
#include <boost/vmd/is_empty.hpp>
#include <boost/vmd/is_empty_array.hpp>
#include <boost/vmd/is_empty_list.hpp>
#include <boost/vmd/is_general_identifier.hpp>
#include <boost/vmd/is_identifier.hpp>
#include <boost/vmd/is_list.hpp>
#include <boost/vmd/is_multi.hpp>
#include <boost/vmd/is_number.hpp>
#include <boost/vmd/is_parens_empty.hpp>
#include <boost/vmd/is_seq.hpp>
#include <boost/vmd/is_tuple.hpp>
#include <boost/vmd/is_type.hpp>
#include <boost/vmd/is_unary.hpp>
#include <boost/vmd/list.hpp>
#include <boost/vmd/list/to_seq.hpp>
#include <boost/vmd/list/to_tuple.hpp>
#include <boost/vmd/not_equal.hpp>
#include <boost/vmd/seq.hpp>
#include <boost/vmd/seq/is_vmd_seq.hpp>
#include <boost/vmd/seq/pop_back.hpp>
#include <boost/vmd/seq/pop_front.hpp>
#include <boost/vmd/seq/push_back.hpp>
#include <boost/vmd/seq/push_front.hpp>
#include <boost/vmd/seq/remove.hpp>
#include <boost/vmd/seq/size.hpp>
#include <boost/vmd/seq/to_array.hpp>
#include <boost/vmd/seq/to_list.hpp>
#include <boost/vmd/seq/to_tuple.hpp>
#include <boost/vmd/size.hpp>
#include <boost/vmd/to_array.hpp>
#include <boost/vmd/to_list.hpp>
#include <boost/vmd/to_seq.hpp>
#include <boost/vmd/to_tuple.hpp>
#include <boost/vmd/tuple.hpp>
#include <boost/vmd/tuple/is_vmd_tuple.hpp>
#include <boost/vmd/tuple/pop_back.hpp>
#include <boost/vmd/tuple/pop_front.hpp>
#include <boost/vmd/tuple/push_back.hpp>
#include <boost/vmd/tuple/push_front.hpp>
#include <boost/vmd/tuple/remove.hpp>
#include <boost/vmd/tuple/size.hpp>
#include <boost/vmd/tuple/to_array.hpp>
#include <boost/vmd/tuple/to_list.hpp>
#include <boost/vmd/tuple/to_seq.hpp>
#include <boost/vmd/vmd.hpp>

