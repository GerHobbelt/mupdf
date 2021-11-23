

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// cmdproc.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cmdproc.cpp"


//------------------------------------------------------------------------
// cmndata.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cmndata.cpp"


//------------------------------------------------------------------------
// collheaderctrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/collheaderctrlg.cpp"


//------------------------------------------------------------------------
// collpane.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/collpane.cpp"
#endif


//------------------------------------------------------------------------
// collpaneg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/collpaneg.cpp"


//------------------------------------------------------------------------
// colordlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/colordlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/colordlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/colordlg.cpp"
#endif


//------------------------------------------------------------------------
// colour.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/colour.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/colour.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/colour.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/colour.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/colour.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/colour.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/colour.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/colour.cpp"
#endif


//------------------------------------------------------------------------
// colourcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/colourcmn.cpp"


//------------------------------------------------------------------------
// colourdata.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/colourdata.cpp"


//------------------------------------------------------------------------
// colrdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/colrdlgg.cpp"
