
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/hashed_index_fwd.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/identity_fwd.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/key.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/ordered_index_fwd.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/random_access_index_fwd.hpp>
#include <boost/multi_index/ranked_index.hpp>
#include <boost/multi_index/ranked_index_fwd.hpp>
#include <boost/multi_index/safe_mode_errors.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/sequenced_index_fwd.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index_container_fwd.hpp>

