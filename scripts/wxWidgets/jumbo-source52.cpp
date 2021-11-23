

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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


//------------------------------------------------------------------------
// taskbarbutton.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/taskbarbutton.cpp"
#endif


//------------------------------------------------------------------------
// taskbarcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/taskbarcmn.cpp"


//------------------------------------------------------------------------
// taskbarx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/taskbarx11.cpp"
#endif


//------------------------------------------------------------------------
// tbarbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/tbarbase.cpp"
