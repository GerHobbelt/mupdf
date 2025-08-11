
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/redis.hpp>
#include <boost/redis/adapter/adapt.hpp>
#include <boost/redis/adapter/any_adapter.hpp>
#include <boost/redis/adapter/ignore.hpp>
#include <boost/redis/adapter/result.hpp>
#include <boost/redis/config.hpp>
#include <boost/redis/connection.hpp>
#include <boost/redis/error.hpp>
#include <boost/redis/ignore.hpp>
#include <boost/redis/logger.hpp>
#include <boost/redis/operation.hpp>
#include <boost/redis/request.hpp>
#include <boost/redis/resp3/node.hpp>
#include <boost/redis/resp3/parser.hpp>
#include <boost/redis/resp3/serialization.hpp>
#include <boost/redis/resp3/type.hpp>
#include <boost/redis/response.hpp>
#include <boost/redis/src.hpp>
#include <boost/redis/usage.hpp>

