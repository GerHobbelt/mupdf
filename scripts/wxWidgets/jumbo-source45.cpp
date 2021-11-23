
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// stream.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/stream.cpp"

//------------------------------------------------------------------------
// string.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/string.cpp"

//------------------------------------------------------------------------
// stringimpl.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/stringimpl.cpp"

//------------------------------------------------------------------------
// stringops.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/stringops.cpp"

//------------------------------------------------------------------------
// strvararg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/strvararg.cpp"

//------------------------------------------------------------------------
// sysopt.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/sysopt.cpp"

//------------------------------------------------------------------------
// systhemectrl.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/systhemectrl.cpp"
#endif

//------------------------------------------------------------------------
// tabg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/tabg.cpp"

//------------------------------------------------------------------------
// tarstrm.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/tarstrm.cpp"

//------------------------------------------------------------------------
// taskbar.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/taskbar.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/taskbar.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/taskbar.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/taskbar.cpp"
#endif
