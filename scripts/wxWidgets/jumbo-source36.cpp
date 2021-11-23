
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// printdlg.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/printdlg.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/printdlg.cpp"
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
