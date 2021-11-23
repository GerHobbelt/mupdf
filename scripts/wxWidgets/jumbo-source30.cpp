
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// mask.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/mask.cpp"

//------------------------------------------------------------------------
// matrix.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/matrix.cpp"

//------------------------------------------------------------------------
// mdi.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/mdi.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/mdi.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mdi.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/mdi.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/mdi.cpp"
#endif

//------------------------------------------------------------------------
// mdig.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/mdig.cpp"

//------------------------------------------------------------------------
// mediactrl.cpp
 
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/mediactrl.cpp"
#endif
#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/mediactrl.cpp"
#endif

//------------------------------------------------------------------------
// mediactrl_am.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mediactrl_am.cpp"
#endif

//------------------------------------------------------------------------
// mediactrl_qt.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mediactrl_qt.cpp"
#endif

//------------------------------------------------------------------------
// mediactrl_wmp10.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mediactrl_wmp10.cpp"
#endif

//------------------------------------------------------------------------
// mediactrlcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/mediactrlcmn.cpp"

//------------------------------------------------------------------------
// memory.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/memory.cpp"
