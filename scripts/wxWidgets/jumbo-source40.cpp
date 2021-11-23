

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// powercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/powercmn.cpp"


//------------------------------------------------------------------------
// preferencescmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/preferencescmn.cpp"


//------------------------------------------------------------------------
// preferencesg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/preferencesg.cpp"


//------------------------------------------------------------------------
// print.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/print.cpp"
#endif


//------------------------------------------------------------------------
// printdlg.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/printdlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/printdlg.cpp"
#endif


//------------------------------------------------------------------------
// printdlg_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/printdlg_osx.cpp"
#endif


//------------------------------------------------------------------------
// printmac.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/printmac.cpp"
#endif


//------------------------------------------------------------------------
// printps.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/printps.cpp"


//------------------------------------------------------------------------
// printqt.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/printqt.cpp"
#endif


//------------------------------------------------------------------------
// printwin.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/printwin.cpp"
#endif
