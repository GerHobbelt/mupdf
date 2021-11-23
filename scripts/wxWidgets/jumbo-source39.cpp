

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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
