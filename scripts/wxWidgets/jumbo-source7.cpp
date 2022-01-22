

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// datavgen.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/datavgen.cpp"


//------------------------------------------------------------------------
// dataview.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dataview.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dataview.cpp"
#endif


//------------------------------------------------------------------------
// dataview_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/dataview_osx.cpp"
#endif


//------------------------------------------------------------------------
// datecontrols.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/datecontrols.cpp"
#endif


//------------------------------------------------------------------------
// datectlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/datectlg.cpp"


//------------------------------------------------------------------------
// datectrl.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/datectrl.cpp"
#endif


//------------------------------------------------------------------------
// datectrl_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/datectrl_osx.cpp"
#endif


//------------------------------------------------------------------------
// datetime.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/datetime.cpp"


//------------------------------------------------------------------------
// datetimectrl.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/datetimectrl.cpp"
#endif


//------------------------------------------------------------------------
// datetimectrl_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/datetimectrl_osx.cpp"
#endif


//------------------------------------------------------------------------
// datetimefmt.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/datetimefmt.cpp"


//------------------------------------------------------------------------
// datstrm.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/datstrm.cpp"


//------------------------------------------------------------------------
// dbgrptg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dbgrptg.cpp"


//------------------------------------------------------------------------
// dc.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/dc.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dc.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/dc.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/dc.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dc.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dc.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/dc.cpp"
#endif


//------------------------------------------------------------------------
// dcbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dcbase.cpp"


//------------------------------------------------------------------------
// dcbufcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dcbufcmn.cpp"


//------------------------------------------------------------------------
// dcclient.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/dcclient.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dcclient.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/dcclient.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/dcclient.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dcclient.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/dcclient.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dcclient.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/dcclient.cpp"
#endif


//------------------------------------------------------------------------
// dcgraph.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dcgraph.cpp"


//------------------------------------------------------------------------
// dcmemory.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/dcmemory.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dcmemory.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/dcmemory.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/dcmemory.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dcmemory.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/dcmemory.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dcmemory.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/dcmemory.cpp"
#endif


//------------------------------------------------------------------------
// dcprint.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dcprint.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/dcprint.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dcprint.cpp"
#endif
