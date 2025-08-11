
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/config.hpp>
#include <boost/config/abi/borland_prefix.hpp>
#include <boost/config/abi/borland_suffix.hpp>
#include <boost/config/abi/msvc_prefix.hpp>
#include <boost/config/abi/msvc_suffix.hpp>
#include <boost/config/abi_prefix.hpp>
#include <boost/config/abi_suffix.hpp>
#include <boost/config/assert_cxx03.hpp>
#include <boost/config/assert_cxx11.hpp>
#include <boost/config/assert_cxx14.hpp>
#include <boost/config/assert_cxx17.hpp>
#include <boost/config/assert_cxx20.hpp>
#include <boost/config/assert_cxx23.hpp>
#include <boost/config/assert_cxx98.hpp>
#include <boost/config/auto_link.hpp>
#include <boost/config/compiler/borland.hpp>
#include <boost/config/compiler/clang.hpp>
#include <boost/config/compiler/codegear.hpp>
#include <boost/config/compiler/comeau.hpp>
#include <boost/config/compiler/common_edg.hpp>
#include <boost/config/compiler/compaq_cxx.hpp>
#include <boost/config/compiler/cray.hpp>
#include <boost/config/compiler/diab.hpp>
#include <boost/config/compiler/digitalmars.hpp>
#include <boost/config/compiler/gcc.hpp>
#include <boost/config/compiler/gcc_xml.hpp>
#include <boost/config/compiler/greenhills.hpp>
#include <boost/config/compiler/hp_acc.hpp>
#include <boost/config/compiler/intel.hpp>
#include <boost/config/compiler/kai.hpp>
#include <boost/config/compiler/metrowerks.hpp>
#include <boost/config/compiler/mpw.hpp>
#include <boost/config/compiler/nvcc.hpp>
#include <boost/config/compiler/pathscale.hpp>
#include <boost/config/compiler/pgi.hpp>
#include <boost/config/compiler/sgi_mipspro.hpp>
#include <boost/config/compiler/sunpro_cc.hpp>
#include <boost/config/compiler/vacpp.hpp>
#include <boost/config/compiler/visualc.hpp>
#include <boost/config/compiler/xlcpp.hpp>
#include <boost/config/compiler/xlcpp_zos.hpp>
#include <boost/config/header_deprecated.hpp>
#include <boost/config/helper_macros.hpp>
#include <boost/config/no_tr1/cmath.hpp>
#include <boost/config/no_tr1/complex.hpp>
#include <boost/config/no_tr1/functional.hpp>
#include <boost/config/no_tr1/memory.hpp>
#include <boost/config/no_tr1/utility.hpp>
#include <boost/config/pragma_message.hpp>
#include <boost/config/requires_threads.hpp>
#include <boost/config/stdlib/dinkumware.hpp>
#include <boost/config/stdlib/libcomo.hpp>
#include <boost/config/stdlib/libcpp.hpp>
#include <boost/config/stdlib/libstdcpp3.hpp>
#include <boost/config/stdlib/modena.hpp>
#include <boost/config/stdlib/msl.hpp>
#include <boost/config/stdlib/roguewave.hpp>
#include <boost/config/stdlib/sgi.hpp>
#include <boost/config/stdlib/stlport.hpp>
#include <boost/config/stdlib/vacpp.hpp>
#include <boost/config/stdlib/xlcpp_zos.hpp>
#include <boost/config/user.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/config/workaround.hpp>
#include <boost/cstdint.hpp>
#include <boost/cxx11_char_types.hpp>
#include <boost/limits.hpp>

