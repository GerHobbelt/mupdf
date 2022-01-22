

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// dcpsg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dcpsg.cpp"


//------------------------------------------------------------------------
// dcscreen.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/dcscreen.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dcscreen.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/dcscreen.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/dcscreen.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dcscreen.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/dcscreen.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dcscreen.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/dcscreen.cpp"
#endif


//------------------------------------------------------------------------
// dcsvg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dcsvg.cpp"


//------------------------------------------------------------------------
// dde.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dde.cpp"
#endif


//------------------------------------------------------------------------
// debughlp.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/debughlp.cpp"
#endif


//------------------------------------------------------------------------
// debugrpt.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/debugrpt.cpp"


//------------------------------------------------------------------------
// defs.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/defs.cpp"
#endif


//------------------------------------------------------------------------
// demo.c

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/xmcombo/demo.c"
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
