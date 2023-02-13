

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// scrthumb.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/scrthumb.cpp"
#endif


//------------------------------------------------------------------------
// secretstore.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/secretstore.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/secretstore.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/secretstore.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/secretstore.cpp"
#endif


//------------------------------------------------------------------------
// selectdispatcher.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/selectdispatcher.cpp"


//------------------------------------------------------------------------
// selstore.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/selstore.cpp"


//------------------------------------------------------------------------
// settcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/settcmn.cpp"


//------------------------------------------------------------------------
// settings.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/settings.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/settings.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/settings.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/settings.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/settings.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/settings.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/settings.cpp"
#endif


//------------------------------------------------------------------------
// settingsuniv.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/settingsuniv.cpp"
#endif


//------------------------------------------------------------------------
// sizer.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sizer.cpp"


//------------------------------------------------------------------------
// slider.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/slider.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/slider.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/slider.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/slider.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/slider.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/slider.cpp"
#endif


//------------------------------------------------------------------------
// slider_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/slider_osx.cpp"
#endif


//------------------------------------------------------------------------
// slidercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/slidercmn.cpp"


//------------------------------------------------------------------------
// snglinst.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/snglinst.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/snglinst.cpp"
#endif


//------------------------------------------------------------------------
// socket.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/socket.cpp"


//------------------------------------------------------------------------
// socketiohandler.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/socketiohandler.cpp"


//------------------------------------------------------------------------
// sockgtk.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/sockgtk.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/sockgtk.cpp"
#endif


//------------------------------------------------------------------------
// sockmot.cpp

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/sockmot.cpp"
#endif


//------------------------------------------------------------------------
// sockmsw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/sockmsw.cpp"
#endif


//------------------------------------------------------------------------
// sockosx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/sockosx.cpp"
#endif


//------------------------------------------------------------------------
// sockqt.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/sockqt.cpp"
#endif


//------------------------------------------------------------------------
// sockunix.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/sockunix.cpp"
#endif
