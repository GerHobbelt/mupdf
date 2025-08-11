
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/align.hpp>
#include <boost/align/align.hpp>
#include <boost/align/align_down.hpp>
#include <boost/align/align_up.hpp>
#include <boost/align/aligned_alloc.hpp>
#include <boost/align/aligned_allocator.hpp>
#include <boost/align/aligned_allocator_adaptor.hpp>
#include <boost/align/aligned_allocator_adaptor_forward.hpp>
#include <boost/align/aligned_allocator_forward.hpp>
#include <boost/align/aligned_delete.hpp>
#include <boost/align/aligned_delete_forward.hpp>
#include <boost/align/alignment_of.hpp>
#include <boost/align/alignment_of_forward.hpp>
#include <boost/align/assume_aligned.hpp>
#include <boost/align/is_aligned.hpp>

