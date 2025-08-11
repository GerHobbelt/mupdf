
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/polygon/gmp_override.hpp>
#include <boost/polygon/gtl.hpp>
#include <boost/polygon/interval_concept.hpp>
#include <boost/polygon/interval_data.hpp>
#include <boost/polygon/interval_traits.hpp>
#include <boost/polygon/isotropy.hpp>
#include <boost/polygon/point_concept.hpp>
#include <boost/polygon/point_data.hpp>
#include <boost/polygon/point_traits.hpp>
#include <boost/polygon/polygon.hpp>
#include <boost/polygon/polygon_45_data.hpp>
#include <boost/polygon/polygon_45_set_concept.hpp>
#include <boost/polygon/polygon_45_set_data.hpp>
#include <boost/polygon/polygon_45_set_traits.hpp>
#include <boost/polygon/polygon_45_with_holes_data.hpp>
#include <boost/polygon/polygon_90_data.hpp>
#include <boost/polygon/polygon_90_set_concept.hpp>
#include <boost/polygon/polygon_90_set_data.hpp>
#include <boost/polygon/polygon_90_set_traits.hpp>
#include <boost/polygon/polygon_90_with_holes_data.hpp>
#include <boost/polygon/polygon_data.hpp>
#include <boost/polygon/polygon_set_concept.hpp>
#include <boost/polygon/polygon_set_data.hpp>
#include <boost/polygon/polygon_set_traits.hpp>
#include <boost/polygon/polygon_traits.hpp>
#include <boost/polygon/polygon_with_holes_data.hpp>
#include <boost/polygon/rectangle_concept.hpp>
#include <boost/polygon/rectangle_data.hpp>
#include <boost/polygon/rectangle_traits.hpp>
#include <boost/polygon/segment_concept.hpp>
#include <boost/polygon/segment_data.hpp>
#include <boost/polygon/segment_traits.hpp>
#include <boost/polygon/segment_utils.hpp>
#include <boost/polygon/transform.hpp>
#include <boost/polygon/voronoi.hpp>
#include <boost/polygon/voronoi_builder.hpp>
#include <boost/polygon/voronoi_diagram.hpp>
#include <boost/polygon/voronoi_geometry_type.hpp>

