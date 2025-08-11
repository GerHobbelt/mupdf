
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/container_hash/extensions.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/container_hash/hash_fwd.hpp>
#include <boost/container_hash/hash_is_avalanching.hpp>
#include <boost/container_hash/is_contiguous_range.hpp>
#include <boost/container_hash/is_described_class.hpp>
#include <boost/container_hash/is_range.hpp>
#include <boost/container_hash/is_tuple_like.hpp>
#include <boost/container_hash/is_unordered_range.hpp>
#include <boost/functional/hash.hpp>
#include <boost/functional/hash/extensions.hpp>
#include <boost/functional/hash/hash.hpp>
#include <boost/functional/hash/hash_fwd.hpp>
#include <boost/functional/hash_fwd.hpp>

