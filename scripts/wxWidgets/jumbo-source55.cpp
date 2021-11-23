

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// timectrl.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/timectrl.cpp"
#endif


//------------------------------------------------------------------------
// timectrl_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/timectrl_osx.cpp"
#endif


//------------------------------------------------------------------------
// timectrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/timectrlg.cpp"


//------------------------------------------------------------------------
// timer.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/timer.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/timer.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/timer.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/timer.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/timer.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/timer.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/timer.cpp"
#endif


//------------------------------------------------------------------------
// timercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/timercmn.cpp"


//------------------------------------------------------------------------
// timerimpl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/timerimpl.cpp"


//------------------------------------------------------------------------
// timerunx.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/timerunx.cpp"
#endif


//------------------------------------------------------------------------
// tipdlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/tipdlg.cpp"


//------------------------------------------------------------------------
// tipwin.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/tipwin.cpp"


//------------------------------------------------------------------------
// tokenzr.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/tokenzr.cpp"
