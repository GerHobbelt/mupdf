

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// debugrpt.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/debugrpt.cpp"


//------------------------------------------------------------------------
// defs.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/defs.cpp"
#endif


//------------------------------------------------------------------------
// desktopenv.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/desktopenv.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/desktopenv.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/desktopenv.cpp"
#endif


//------------------------------------------------------------------------
// desktopenvcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/desktopenvcmn.cpp"


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
// dialog_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/dialog_osx.cpp"
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
