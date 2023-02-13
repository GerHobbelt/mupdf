

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// utilscmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/utilscmn.cpp"


//------------------------------------------------------------------------
// utilsexc.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/utilsexc.cpp"
#endif


//------------------------------------------------------------------------
// utilsexc_cf.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/utilsexc_cf.cpp"
#endif


//------------------------------------------------------------------------
// utilsgtk.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/utilsgtk.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/utilsgtk.cpp"
#endif


//------------------------------------------------------------------------
// utilsgui.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/utilsgui.cpp"
#endif


//------------------------------------------------------------------------
// utilsres.cpp

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/utilsres.cpp"
#endif


//------------------------------------------------------------------------
// utilsrt.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/rt/utilsrt.cpp"
#endif


//------------------------------------------------------------------------
// utilsunx.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/utilsunx.cpp"
#endif


//------------------------------------------------------------------------
// utilswin.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/utilswin.cpp"
#endif


//------------------------------------------------------------------------
// utilsx.cpp

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/utilsx.cpp"
#endif


//------------------------------------------------------------------------
// utilsx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/utilsx11.cpp"
#endif


//------------------------------------------------------------------------
// uuid.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/uuid.cpp"
#endif


//------------------------------------------------------------------------
// uxtheme.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/uxtheme.cpp"
#endif


//------------------------------------------------------------------------
// valgen.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/valgen.cpp"


//------------------------------------------------------------------------
// validate.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/validate.cpp"


//------------------------------------------------------------------------
// valnum.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/valnum.cpp"


//------------------------------------------------------------------------
// valtext.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/valtext.cpp"


//------------------------------------------------------------------------
// variant.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/variant.cpp"


//------------------------------------------------------------------------
// vlbox.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/vlbox.cpp"


//------------------------------------------------------------------------
// volume.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/volume.cpp"
#endif
