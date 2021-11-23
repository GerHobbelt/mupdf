

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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
// nonownedwnd_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/nonownedwnd_osx.cpp"
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


//------------------------------------------------------------------------
// notebook_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/notebook_osx.cpp"
#endif


//------------------------------------------------------------------------
// notifmsg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/notifmsg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/notifmsg.cpp"
#endif


//------------------------------------------------------------------------
// notifmsgcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/notifmsgcmn.cpp"


//------------------------------------------------------------------------
// notifmsgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/notifmsgg.cpp"


//------------------------------------------------------------------------
// notifmsgrt.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/rt/notifmsgrt.cpp"
#endif


//------------------------------------------------------------------------
// numdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/numdlgg.cpp"


//------------------------------------------------------------------------
// numformatter.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/numformatter.cpp"
