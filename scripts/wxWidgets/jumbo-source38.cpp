
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// region.cpp
 
#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/region.cpp"
#endif
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/region.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/region.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/region.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/region.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/region.cpp"
#endif
#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/region.cpp"
#endif

//------------------------------------------------------------------------
// regiong.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/regiong.cpp"

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
