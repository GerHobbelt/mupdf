

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// barartuwp.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/aui/barartuwp.cpp"


//------------------------------------------------------------------------
// base64.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/base64.cpp"


//------------------------------------------------------------------------
// basemsw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/basemsw.cpp"
#endif


//------------------------------------------------------------------------
// bitmap.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/bitmap.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/bitmap.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/bitmap.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/bitmap.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/bitmap.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/bitmap.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/bitmap.cpp"
#endif


//------------------------------------------------------------------------
// bmpbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/bmpbase.cpp"


//------------------------------------------------------------------------
// bmpbndl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/bmpbndl.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/bmpbndl.cpp"
#endif


//------------------------------------------------------------------------
// bmpbtncmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/bmpbtncmn.cpp"


//------------------------------------------------------------------------
// bmpbuttn.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/bmpbuttn.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/bmpbuttn.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/bmpbuttn.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/bmpbuttn.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/bmpbuttn.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/bmpbuttn.cpp"
#endif


//------------------------------------------------------------------------
// bmpbuttn_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/bmpbuttn_osx.cpp"
#endif


//------------------------------------------------------------------------
// bmpcbox.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/bmpcbox.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/bmpcbox.cpp"
#endif


//------------------------------------------------------------------------
// bmpcboxcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/bmpcboxcmn.cpp"


//------------------------------------------------------------------------
// bmpcboxg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/bmpcboxg.cpp"


//------------------------------------------------------------------------
// bmpmotif.cpp

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/bmpmotif.cpp"
#endif


//------------------------------------------------------------------------
// bmpsvg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/bmpsvg.cpp"


//------------------------------------------------------------------------
// bookctrl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/bookctrl.cpp"


//------------------------------------------------------------------------
// brush.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/brush.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/brush.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/brush.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/brush.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/brush.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/brush.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/brush.cpp"
#endif


//------------------------------------------------------------------------
// btncmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/btncmn.cpp"


//------------------------------------------------------------------------
// busyinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/busyinfo.cpp"


//------------------------------------------------------------------------
// button.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/button.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/button.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/button.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/button.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/button.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/button.cpp"
#endif


//------------------------------------------------------------------------
// button_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/button_osx.cpp"
#endif
