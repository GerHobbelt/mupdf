
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


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
