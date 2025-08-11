
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/hash2/blake2.hpp>
#include <boost/hash2/digest.hpp>
#include <boost/hash2/endian.hpp>
#include <boost/hash2/flavor.hpp>
#include <boost/hash2/fnv1a.hpp>
#include <boost/hash2/get_integral_result.hpp>
#include <boost/hash2/has_constant_size.hpp>
#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/hash_append_fwd.hpp>
#include <boost/hash2/hmac.hpp>
#include <boost/hash2/is_contiguously_hashable.hpp>
#include <boost/hash2/is_endian_independent.hpp>
#include <boost/hash2/is_trivially_equality_comparable.hpp>
#include <boost/hash2/legacy/murmur3.hpp>
#include <boost/hash2/legacy/spooky2.hpp>
#include <boost/hash2/md5.hpp>
#include <boost/hash2/ripemd.hpp>
#include <boost/hash2/sha1.hpp>
#include <boost/hash2/sha2.hpp>
#include <boost/hash2/sha3.hpp>
#include <boost/hash2/siphash.hpp>
#include <boost/hash2/xxh3.hpp>
#include <boost/hash2/xxhash.hpp>

