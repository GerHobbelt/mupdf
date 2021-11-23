
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// dialog.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dialog.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/dialog.cpp"
#endif
#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/dialog.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dialog.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dialog.cpp"
#endif
#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/dialog.cpp"
#endif

//------------------------------------------------------------------------
// dialup.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dialup.cpp"
#endif
#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/dialup.cpp"
#endif

//------------------------------------------------------------------------
// dib.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dib.cpp"
#endif

//------------------------------------------------------------------------
// dir.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dir.cpp"
#endif
#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/dir.cpp"
#endif

//------------------------------------------------------------------------
// dircmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/dircmn.cpp"

//------------------------------------------------------------------------
// dirctrlcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/dirctrlcmn.cpp"

//------------------------------------------------------------------------
// dirctrlg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dirctrlg.cpp"

//------------------------------------------------------------------------
// dirdlg.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dirdlg.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dirdlg.cpp"
#endif

//------------------------------------------------------------------------
// dirdlgg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dirdlgg.cpp"

//------------------------------------------------------------------------
// display.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/display.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/display.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/display.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/display.cpp"
#endif
