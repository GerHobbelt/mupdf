
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/tti/gen/has_class_gen.hpp>
#include <boost/tti/gen/has_data_gen.hpp>
#include <boost/tti/gen/has_enum_gen.hpp>
#include <boost/tti/gen/has_function_gen.hpp>
#include <boost/tti/gen/has_function_template_gen.hpp>
#include <boost/tti/gen/has_member_data_gen.hpp>
#include <boost/tti/gen/has_member_function_gen.hpp>
#include <boost/tti/gen/has_member_function_template_gen.hpp>
#include <boost/tti/gen/has_static_member_data_gen.hpp>
#include <boost/tti/gen/has_static_member_function_gen.hpp>
#include <boost/tti/gen/has_static_member_function_template_gen.hpp>
#include <boost/tti/gen/has_template_gen.hpp>
#include <boost/tti/gen/has_type_gen.hpp>
#include <boost/tti/gen/has_union_gen.hpp>
#include <boost/tti/gen/member_type_gen.hpp>
#include <boost/tti/gen/namespace_gen.hpp>
#include <boost/tti/has_class.hpp>
#include <boost/tti/has_data.hpp>
#include <boost/tti/has_enum.hpp>
#include <boost/tti/has_function.hpp>
#include <boost/tti/has_function_template.hpp>
#include <boost/tti/has_member_data.hpp>
#include <boost/tti/has_member_function.hpp>
#include <boost/tti/has_member_function_template.hpp>
#include <boost/tti/has_static_member_data.hpp>
#include <boost/tti/has_static_member_function.hpp>
#include <boost/tti/has_static_member_function_template.hpp>
#include <boost/tti/has_template.hpp>
#include <boost/tti/has_type.hpp>
#include <boost/tti/has_union.hpp>
#include <boost/tti/member_type.hpp>
#include <boost/tti/tti.hpp>

