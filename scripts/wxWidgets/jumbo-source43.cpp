

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// registry.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/registry.cpp"
#endif


//------------------------------------------------------------------------
// rendcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/rendcmn.cpp"


//------------------------------------------------------------------------
// renderer.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/renderer.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/renderer.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/renderer.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/renderer.cpp"
#endif


//------------------------------------------------------------------------
// renderg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/renderg.cpp"


//------------------------------------------------------------------------
// reparent.cpp

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/reparent.cpp"
#endif


//------------------------------------------------------------------------
// rgncmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/rgncmn.cpp"


//------------------------------------------------------------------------
// richmsgdlg.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/richmsgdlg.cpp"
#endif


//------------------------------------------------------------------------
// richmsgdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/richmsgdlgg.cpp"


//------------------------------------------------------------------------
// richtooltip.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/richtooltip.cpp"
#endif


//------------------------------------------------------------------------
// richtooltipcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/richtooltipcmn.cpp"
