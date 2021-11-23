

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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
// mediactrl_gstplayer.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/mediactrl_gstplayer.cpp"
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


//------------------------------------------------------------------------
// menu.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/menu.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/menu.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/menu.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/menu.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/menu.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/menu.cpp"
#endif


//------------------------------------------------------------------------
// menu_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/menu_osx.cpp"
#endif


//------------------------------------------------------------------------
// menucmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/menucmn.cpp"
