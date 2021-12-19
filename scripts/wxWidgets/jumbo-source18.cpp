

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// utilswin.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/utilswin.cpp"
#endif


//------------------------------------------------------------------------
// utilsx.cpp

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/utilsx.cpp"
#endif


//------------------------------------------------------------------------
// utilsx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/utilsx11.cpp"
#endif


//------------------------------------------------------------------------
// uuid.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/uuid.cpp"
#endif


//------------------------------------------------------------------------
// uxtheme.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/uxtheme.cpp"
#endif


//------------------------------------------------------------------------
// valgen.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/valgen.cpp"


//------------------------------------------------------------------------
// validate.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/validate.cpp"


//------------------------------------------------------------------------
// valnum.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/valnum.cpp"


//------------------------------------------------------------------------
// valtext.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/valtext.cpp"


//------------------------------------------------------------------------
// variant.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/variant.cpp"


//------------------------------------------------------------------------
// vlbox.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/vlbox.cpp"


//------------------------------------------------------------------------
// volume.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/volume.cpp"
#endif


//------------------------------------------------------------------------
// vscroll.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/vscroll.cpp"


//------------------------------------------------------------------------
// wakeuppipe.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/wakeuppipe.cpp"
#endif


//------------------------------------------------------------------------
// webrequest.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/webrequest.cpp"


//------------------------------------------------------------------------
// webrequest_curl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/webrequest_curl.cpp"


//------------------------------------------------------------------------
// webrequest_winhttp.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/webrequest_winhttp.cpp"
#endif


//------------------------------------------------------------------------
// webview.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/webview.cpp"


//------------------------------------------------------------------------
// webview_edge.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/webview_edge.cpp"
#endif


//------------------------------------------------------------------------
// webview_ie.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/webview_ie.cpp"
#endif


//------------------------------------------------------------------------
// webview_webkit.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/webview_webkit.cpp"
#endif


//------------------------------------------------------------------------
// webview_webkit2.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/webview_webkit2.cpp"
#endif


//------------------------------------------------------------------------
// webview_webkit2_extension.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/webview_webkit2_extension.cpp"
#endif


//------------------------------------------------------------------------
// webviewarchivehandler.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/webviewarchivehandler.cpp"


//------------------------------------------------------------------------
// webviewfshandler.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/webviewfshandler.cpp"


//------------------------------------------------------------------------
// wfstream.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/wfstream.cpp"


//------------------------------------------------------------------------
// win32.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/themes/win32.cpp"
#endif


//------------------------------------------------------------------------
// win_gtk.c

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/win_gtk.c"
#endif


//------------------------------------------------------------------------
// win_gtk.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/win_gtk.cpp"
#endif


//------------------------------------------------------------------------
// wincmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/wincmn.cpp"


//------------------------------------------------------------------------
// window.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/window.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/window.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/window.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/window.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/window.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/window.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/window.cpp"
#endif


//------------------------------------------------------------------------
// window_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/window_osx.cpp"
#endif


//------------------------------------------------------------------------
// windowid.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/windowid.cpp"


//------------------------------------------------------------------------
// winestub.c

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/winestub.c"
#endif


//------------------------------------------------------------------------
// winpars.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/html/winpars.cpp"


//------------------------------------------------------------------------
// winuniv.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/winuniv.cpp"
#endif


//------------------------------------------------------------------------
// wizard.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/wizard.cpp"
