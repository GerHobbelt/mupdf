

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// aboutdlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/aboutdlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/aboutdlg.cpp"
#endif


//------------------------------------------------------------------------
// aboutdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/aboutdlgg.cpp"


//------------------------------------------------------------------------
// accel.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/accel.cpp"

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/accel.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/accel.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/accel.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/accel.cpp"
#endif


//------------------------------------------------------------------------
// accelcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/accelcmn.cpp"


//------------------------------------------------------------------------
// access.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/access.cpp"
#endif


//------------------------------------------------------------------------
// accesscmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/accesscmn.cpp"


//------------------------------------------------------------------------
// activex.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/activex.cpp"
#endif


//------------------------------------------------------------------------
// activityindicator.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/activityindicator.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/activityindicator.cpp"
#endif


//------------------------------------------------------------------------
// addremovectrl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/addremovectrl.cpp"


//------------------------------------------------------------------------
// advprops.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/propgrid/advprops.cpp"


//------------------------------------------------------------------------
// affinematrix2d.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/affinematrix2d.cpp"


//------------------------------------------------------------------------
// anidecod.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/anidecod.cpp"


//------------------------------------------------------------------------
// animate.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/animate.cpp"
#endif


//------------------------------------------------------------------------
// animatecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/animatecmn.cpp"


//------------------------------------------------------------------------
// animateg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/animateg.cpp"


//------------------------------------------------------------------------
// any.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/any.cpp"


//------------------------------------------------------------------------
// anybutton.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/anybutton.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/anybutton.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/anybutton.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/anybutton.cpp"
#endif


//------------------------------------------------------------------------
// anybutton_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/anybutton_osx.cpp"
#endif


//------------------------------------------------------------------------
// app.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/app.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/app.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/app.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/app.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/app.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/app.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/app.cpp"
#endif

#if defined(__UWP__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/uwp/app.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/app.cpp"
#endif


//------------------------------------------------------------------------
// appbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/appbase.cpp"
