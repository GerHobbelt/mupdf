

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// imagtiff.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/imagtiff.cpp"


//------------------------------------------------------------------------
// imagxpm.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/imagxpm.cpp"


//------------------------------------------------------------------------
// infobar.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/infobar.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/infobar.cpp"
#endif


//------------------------------------------------------------------------
// iniconf.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/iniconf.cpp"
#endif


//------------------------------------------------------------------------
// inpcons.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/inpcons.cpp"
#endif


//------------------------------------------------------------------------
// inphand.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/inphand.cpp"
#endif


//------------------------------------------------------------------------
// intl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/intl.cpp"


//------------------------------------------------------------------------
// ipcbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ipcbase.cpp"


//------------------------------------------------------------------------
// joystick.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/joystick.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/joystick.cpp"
#endif


//------------------------------------------------------------------------
// languageinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/languageinfo.cpp"


//------------------------------------------------------------------------
// layout.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/layout.cpp"


//------------------------------------------------------------------------
// laywin.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/laywin.cpp"


//------------------------------------------------------------------------
// lboxcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/lboxcmn.cpp"


//------------------------------------------------------------------------
// list.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/list.cpp"


//------------------------------------------------------------------------
// listbkg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/listbkg.cpp"


//------------------------------------------------------------------------
// listbox.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/listbox.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/listbox.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/listbox.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/listbox.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/listbox.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/listbox.cpp"
#endif


//------------------------------------------------------------------------
// listbox_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/listbox_osx.cpp"
#endif


//------------------------------------------------------------------------
// listctrl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/listctrl.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/listctrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/listctrl.cpp"
#endif


//------------------------------------------------------------------------
// listctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/listctrlcmn.cpp"


//------------------------------------------------------------------------
// log.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/log.cpp"
