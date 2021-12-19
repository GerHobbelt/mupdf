

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// overlay.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/overlay.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/overlay.cpp"
#endif


//------------------------------------------------------------------------
// overlaycmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/overlaycmn.cpp"


//------------------------------------------------------------------------
// ownerdrw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ownerdrw.cpp"
#endif


//------------------------------------------------------------------------
// ownerdrwcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ownerdrwcmn.cpp"


//------------------------------------------------------------------------
// palette.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/palette.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/palette.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/palette.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/palette.cpp"
#endif


//------------------------------------------------------------------------
// paletteg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/paletteg.cpp"


//------------------------------------------------------------------------
// panelcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/panelcmn.cpp"


//------------------------------------------------------------------------
// pango_x.cpp

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/pango_x.cpp"
#endif


//------------------------------------------------------------------------
// paper.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/paper.cpp"


//------------------------------------------------------------------------
// pen.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/pen.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/pen.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/pen.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/pen.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/pen.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/pen.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/pen.cpp"
#endif


//------------------------------------------------------------------------
// persist.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/persist.cpp"


//------------------------------------------------------------------------
// pickerbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/pickerbase.cpp"


//------------------------------------------------------------------------
// platinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/platinfo.cpp"


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


//------------------------------------------------------------------------
// property.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/propgrid/property.cpp"


//------------------------------------------------------------------------
// propgrid.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/propgrid/propgrid.cpp"


//------------------------------------------------------------------------
// propgridiface.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/propgrid/propgridiface.cpp"


//------------------------------------------------------------------------
// propgridpagestate.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/propgrid/propgridpagestate.cpp"
