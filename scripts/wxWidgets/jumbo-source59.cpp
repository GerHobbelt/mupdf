

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// utilsgtk.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/utilsgtk.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/utilsgtk.cpp"
#endif


//------------------------------------------------------------------------
// utilsgui.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/utilsgui.cpp"
#endif


//------------------------------------------------------------------------
// utilsres.cpp

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/utilsres.cpp"
#endif


//------------------------------------------------------------------------
// utilsrt.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/rt/utilsrt.cpp"
#endif


//------------------------------------------------------------------------
// utilsunx.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/utilsunx.cpp"
#endif


//------------------------------------------------------------------------
// utilswin.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/utilswin.cpp"
#endif


//------------------------------------------------------------------------
// utilsx.cpp

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/utilsx.cpp"
#endif


//------------------------------------------------------------------------
// utilsx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/utilsx11.cpp"
#endif


//------------------------------------------------------------------------
// uuid.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/uuid.cpp"
#endif


//------------------------------------------------------------------------
// uxtheme.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/uxtheme.cpp"
#endif
