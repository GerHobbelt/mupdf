

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// appcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/appcmn.cpp"


//------------------------------------------------------------------------
// appprogress.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/appprogress.cpp"
#endif


//------------------------------------------------------------------------
// apptraits.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/apptraits.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/apptraits.cpp"
#endif


//------------------------------------------------------------------------
// appunix.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/appunix.cpp"
#endif


//------------------------------------------------------------------------
// arcall.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/arcall.cpp"


//------------------------------------------------------------------------
// arcfind.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/arcfind.cpp"


//------------------------------------------------------------------------
// archive.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/archive.cpp"


//------------------------------------------------------------------------
// arrstr.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/arrstr.cpp"


//------------------------------------------------------------------------
// artgtk.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/artgtk.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/artgtk.cpp"
#endif


//------------------------------------------------------------------------
// artmac.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/artmac.cpp"
#endif


//------------------------------------------------------------------------
// artmsw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/artmsw.cpp"
#endif


//------------------------------------------------------------------------
// artprov.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/artprov.cpp"


//------------------------------------------------------------------------
// artstd.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/artstd.cpp"


//------------------------------------------------------------------------
// arttango.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/arttango.cpp"


//------------------------------------------------------------------------
// assertdlg_gtk.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/assertdlg_gtk.cpp"
#endif


//------------------------------------------------------------------------
// auibar.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/aui/auibar.cpp"


//------------------------------------------------------------------------
// auibook.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/aui/auibook.cpp"


//------------------------------------------------------------------------
// automtn.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/automtn.cpp"
#endif


//------------------------------------------------------------------------
// bannerwindow.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/bannerwindow.cpp"


//------------------------------------------------------------------------
// barartmsw.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/aui/barartmsw.cpp"
