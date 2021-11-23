
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// taskbarbutton.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/taskbarbutton.cpp"
#endif

//------------------------------------------------------------------------
// taskbarcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/taskbarcmn.cpp"

//------------------------------------------------------------------------
// tbarbase.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/tbarbase.cpp"

//------------------------------------------------------------------------
// textbuf.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/textbuf.cpp"

//------------------------------------------------------------------------
// textcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/textcmn.cpp"

//------------------------------------------------------------------------
// textctrl.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/textctrl.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/textctrl.cpp"
#endif
#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/textctrl.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/textctrl.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/textctrl.cpp"
#endif
#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/textctrl.cpp"
#endif
#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/textctrl.cpp"
#endif

//------------------------------------------------------------------------
// textdlgg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/textdlgg.cpp"

//------------------------------------------------------------------------
// textentry.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/textentry.cpp"
#endif
#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/textentry.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/textentry.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/textentry.cpp"
#endif

//------------------------------------------------------------------------
// textentrycmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/textentrycmn.cpp"

//------------------------------------------------------------------------
// textfile.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/textfile.cpp"
