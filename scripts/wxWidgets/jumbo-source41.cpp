
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// slider.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/slider.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/slider.cpp"
#endif
#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/slider.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/slider.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/slider.cpp"
#endif
#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/slider.cpp"
#endif

//------------------------------------------------------------------------
// slidercmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/slidercmn.cpp"

//------------------------------------------------------------------------
// snglinst.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/snglinst.cpp"
#endif
#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/snglinst.cpp"
#endif

//------------------------------------------------------------------------
// socket.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/socket.cpp"

//------------------------------------------------------------------------
// socketiohandler.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/socketiohandler.cpp"

//------------------------------------------------------------------------
// sockgtk.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/sockgtk.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/sockgtk.cpp"
#endif

//------------------------------------------------------------------------
// sockmot.cpp
 
#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/sockmot.cpp"
#endif

//------------------------------------------------------------------------
// sockmsw.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/sockmsw.cpp"
#endif

//------------------------------------------------------------------------
// sockqt.cpp
 
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/sockqt.cpp"
#endif

//------------------------------------------------------------------------
// sound.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/sound.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/sound.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/sound.cpp"
#endif
#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/sound.cpp"
#endif
