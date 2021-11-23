

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// spinctlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/spinctlg.cpp"


//------------------------------------------------------------------------
// spinctrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/spinctrl.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/spinctrl.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/spinctrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/spinctrl.cpp"
#endif


//------------------------------------------------------------------------
// spinctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/spinctrlcmn.cpp"


//------------------------------------------------------------------------
// splash.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/splash.cpp"


//------------------------------------------------------------------------
// splitter.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/splitter.cpp"


//------------------------------------------------------------------------
// srchcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/srchcmn.cpp"


//------------------------------------------------------------------------
// srchctlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/srchctlg.cpp"


//------------------------------------------------------------------------
// srchctrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/srchctrl.cpp"
#endif


//------------------------------------------------------------------------
// srchctrl_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/srchctrl_osx.cpp"
#endif


//------------------------------------------------------------------------
// sstream.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sstream.cpp"
