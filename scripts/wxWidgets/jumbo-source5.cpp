

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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


//------------------------------------------------------------------------
// dnd_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/dnd_osx.cpp"
#endif


//------------------------------------------------------------------------
// dndcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dndcmn.cpp"


//------------------------------------------------------------------------
// dobjcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dobjcmn.cpp"


//------------------------------------------------------------------------
// dockart.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/aui/dockart.cpp"


//------------------------------------------------------------------------
// docmdi.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/docmdi.cpp"


//------------------------------------------------------------------------
// docview.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/docview.cpp"


//------------------------------------------------------------------------
// dpycmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dpycmn.cpp"


//------------------------------------------------------------------------
// dragimag.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dragimag.cpp"
#endif


//------------------------------------------------------------------------
// dragimgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dragimgg.cpp"


//------------------------------------------------------------------------
// dropsrc.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/dropsrc.cpp"
#endif


//------------------------------------------------------------------------
// droptgt.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/droptgt.cpp"
#endif


//------------------------------------------------------------------------
// dseldlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dseldlg.cpp"


//------------------------------------------------------------------------
// dvrenderer.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dvrenderer.cpp"
#endif


//------------------------------------------------------------------------
// dvrenderers.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dvrenderers.cpp"
#endif


//------------------------------------------------------------------------
// dynlib.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dynlib.cpp"


//------------------------------------------------------------------------
// dynload.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dynload.cpp"


//------------------------------------------------------------------------
// editlbox.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/editlbox.cpp"


//------------------------------------------------------------------------
// editors.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/propgrid/editors.cpp"


//------------------------------------------------------------------------
// effects.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/effects.cpp"


//------------------------------------------------------------------------
// eggtrayicon.c

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/eggtrayicon.c"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/eggtrayicon.c"
#endif
