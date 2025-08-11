
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/uuid.hpp>
#include <boost/uuid/basic_name_generator.hpp>
#include <boost/uuid/basic_random_generator.hpp>
#include <boost/uuid/entropy_error.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/name_generator_md5.hpp>
#include <boost/uuid/name_generator_sha1.hpp>
#include <boost/uuid/namespaces.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/time_generator.hpp>
#include <boost/uuid/time_generator_v1.hpp>
#include <boost/uuid/time_generator_v6.hpp>
#include <boost/uuid/time_generator_v7.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_clock.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>

