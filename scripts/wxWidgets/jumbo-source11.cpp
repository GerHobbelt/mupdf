

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// containr.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/containr.cpp"


//------------------------------------------------------------------------
// control.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/control.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/control.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/control.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/control.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/control.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/control.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/control.cpp"
#endif


//------------------------------------------------------------------------
// convauto.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/convauto.cpp"


//------------------------------------------------------------------------
// converter.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/converter.cpp"
#endif


//------------------------------------------------------------------------
// crashrpt.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/crashrpt.cpp"
#endif


//------------------------------------------------------------------------
// creddlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/creddlgg.cpp"


//------------------------------------------------------------------------
// cshelp.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cshelp.cpp"


//------------------------------------------------------------------------
// ctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ctrlcmn.cpp"


//------------------------------------------------------------------------
// ctrlrend.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/ctrlrend.cpp"
#endif


//------------------------------------------------------------------------
// ctrlsub.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ctrlsub.cpp"

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/ctrlsub.cpp"
#endif
