
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// mousemanager.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/mousemanager.cpp"

//------------------------------------------------------------------------
// msgdlg.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/msgdlg.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/msgdlg.cpp"
#endif
#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/msgdlg.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/msgdlg.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/msgdlg.cpp"
#endif

//------------------------------------------------------------------------
// msgdlgg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/msgdlgg.cpp"

//------------------------------------------------------------------------
// msgout.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/msgout.cpp"

//------------------------------------------------------------------------
// mstream.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/mstream.cpp"

//------------------------------------------------------------------------
// nativdlg.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/nativdlg.cpp"
#endif

//------------------------------------------------------------------------
// nativewin.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/nativewin.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/nativewin.cpp"
#endif

//------------------------------------------------------------------------
// nbkbase.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/nbkbase.cpp"

//------------------------------------------------------------------------
// nonownedwnd.cpp
 
#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/nonownedwnd.cpp"
#endif
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/nonownedwnd.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/nonownedwnd.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/nonownedwnd.cpp"
#endif

//------------------------------------------------------------------------
// notebook.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/notebook.cpp"
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/notebook.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/notebook.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/notebook.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/notebook.cpp"
#endif
#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/notebook.cpp"
#endif
