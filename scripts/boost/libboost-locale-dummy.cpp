
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/locale.hpp>
#include <boost/locale/boundary.hpp>
#include <boost/locale/boundary/boundary_point.hpp>
#include <boost/locale/boundary/facets.hpp>
#include <boost/locale/boundary/index.hpp>
#include <boost/locale/boundary/segment.hpp>
#include <boost/locale/boundary/types.hpp>
#include <boost/locale/collator.hpp>
#include <boost/locale/config.hpp>
#include <boost/locale/date_time.hpp>
#include <boost/locale/date_time_facet.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/locale/encoding_errors.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/locale/format.hpp>
#include <boost/locale/formatting.hpp>
#include <boost/locale/generator.hpp>
#include <boost/locale/generic_codecvt.hpp>
#include <boost/locale/gnu_gettext.hpp>
#include <boost/locale/hold_ptr.hpp>
#include <boost/locale/info.hpp>
#include <boost/locale/localization_backend.hpp>
#include <boost/locale/message.hpp>
#include <boost/locale/time_zone.hpp>
#include <boost/locale/utf.hpp>
#include <boost/locale/utf8_codecvt.hpp>
#include <boost/locale/util.hpp>
#include <boost/locale/util/locale_data.hpp>
#include <boost/locale/util/string.hpp>

