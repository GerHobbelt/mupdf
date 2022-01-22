

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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
