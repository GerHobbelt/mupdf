

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// brush.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/brush.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/brush.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/brush.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/brush.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/brush.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/brush.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/brush.cpp"
#endif


//------------------------------------------------------------------------
// btncmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/btncmn.cpp"


//------------------------------------------------------------------------
// busyinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/busyinfo.cpp"


//------------------------------------------------------------------------
// button.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/button.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/button.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/button.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/button.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/button.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/button.cpp"
#endif


//------------------------------------------------------------------------
// button_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/button_osx.cpp"
#endif


//------------------------------------------------------------------------
// buttonbar.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/buttonbar.cpp"


//------------------------------------------------------------------------
// cairo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cairo.cpp"


//------------------------------------------------------------------------
// calctrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/calctrl.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/calctrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/calctrl.cpp"
#endif


//------------------------------------------------------------------------
// calctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/calctrlcmn.cpp"


//------------------------------------------------------------------------
// calctrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/calctrlg.cpp"
