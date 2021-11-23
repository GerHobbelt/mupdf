

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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
// init.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/init.cpp"


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
