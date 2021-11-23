
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// init.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/init.cpp"

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
