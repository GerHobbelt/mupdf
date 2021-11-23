

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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


//------------------------------------------------------------------------
// displayx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/displayx11.cpp"
#endif


//------------------------------------------------------------------------
// dlgcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dlgcmn.cpp"


//------------------------------------------------------------------------
// dlmsw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dlmsw.cpp"
#endif


//------------------------------------------------------------------------
// dlunix.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/dlunix.cpp"
#endif


//------------------------------------------------------------------------
// dnd.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dnd.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/dnd.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/dnd.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dnd.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/dnd.cpp"
#endif
