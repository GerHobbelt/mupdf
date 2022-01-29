

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// popupcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/popupcmn.cpp"


//------------------------------------------------------------------------
// popupwin.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/popupwin.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/popupwin.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/popupwin.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/popupwin.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/popupwin.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/popupwin.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/popupwin.cpp"
#endif


//------------------------------------------------------------------------
// power.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/power.cpp"
#endif


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


//------------------------------------------------------------------------
// private.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/private.cpp"
#endif


//------------------------------------------------------------------------
// prntbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/prntbase.cpp"


//------------------------------------------------------------------------
// prntdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/prntdlgg.cpp"


//------------------------------------------------------------------------
// process.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/process.cpp"


//------------------------------------------------------------------------
// progdlg.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/progdlg.cpp"
#endif


//------------------------------------------------------------------------
// progdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/progdlgg.cpp"


//------------------------------------------------------------------------
// propdlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/propdlg.cpp"
