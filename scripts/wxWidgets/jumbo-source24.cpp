

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// reparent.cpp

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/reparent.cpp"
#endif


//------------------------------------------------------------------------
// rgncmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/rgncmn.cpp"


//------------------------------------------------------------------------
// richmsgdlg.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/richmsgdlg.cpp"
#endif


//------------------------------------------------------------------------
// richmsgdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/richmsgdlgg.cpp"


//------------------------------------------------------------------------
// richtooltip.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/richtooltip.cpp"
#endif


//------------------------------------------------------------------------
// richtooltipcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/richtooltipcmn.cpp"


//------------------------------------------------------------------------
// richtooltipg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/richtooltipg.cpp"


//------------------------------------------------------------------------
// rowheightcache.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/rowheightcache.cpp"


//------------------------------------------------------------------------
// safearray.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/safearray.cpp"
#endif


//------------------------------------------------------------------------
// sashwin.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/sashwin.cpp"


//------------------------------------------------------------------------
// sckaddr.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sckaddr.cpp"


//------------------------------------------------------------------------
// sckfile.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sckfile.cpp"


//------------------------------------------------------------------------
// sckipc.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sckipc.cpp"


//------------------------------------------------------------------------
// sckstrm.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sckstrm.cpp"


//------------------------------------------------------------------------
// scrarrow.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/scrarrow.cpp"
#endif


//------------------------------------------------------------------------
// scrlwing.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/scrlwing.cpp"


//------------------------------------------------------------------------
// scrolbar.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/scrolbar.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/scrolbar.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/scrolbar.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/scrolbar.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/scrolbar.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/scrolbar.cpp"
#endif


//------------------------------------------------------------------------
// scrolbar_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/scrolbar_osx.cpp"
#endif


//------------------------------------------------------------------------
// scrolbarcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/scrolbarcmn.cpp"


//------------------------------------------------------------------------
// scrolwin.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/scrolwin.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/scrolwin.cpp"
#endif
