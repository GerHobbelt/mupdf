

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// gauge.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/gauge.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/gauge.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/gauge.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gauge.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/gauge.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/gauge.cpp"
#endif


//------------------------------------------------------------------------
// gauge_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/gauge_osx.cpp"
#endif


//------------------------------------------------------------------------
// gaugecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gaugecmn.cpp"


//------------------------------------------------------------------------
// gbsizer.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gbsizer.cpp"


//------------------------------------------------------------------------
// gdicmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gdicmn.cpp"


//------------------------------------------------------------------------
// gdiimage.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gdiimage.cpp"
#endif


//------------------------------------------------------------------------
// gdiobj.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gdiobj.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/gdiobj.cpp"
#endif


//------------------------------------------------------------------------
// gdiplus.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gdiplus.cpp"
#endif


//------------------------------------------------------------------------
// geometry.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/geometry.cpp"


//------------------------------------------------------------------------
// gifdecod.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gifdecod.cpp"
