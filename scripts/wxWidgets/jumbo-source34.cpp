
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// overlay.cpp
 
#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/overlay.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/overlay.cpp"
#endif

//------------------------------------------------------------------------
// overlaycmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/overlaycmn.cpp"

//------------------------------------------------------------------------
// ownerdrw.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ownerdrw.cpp"
#endif

//------------------------------------------------------------------------
// ownerdrwcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/ownerdrwcmn.cpp"

//------------------------------------------------------------------------
// palette.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/palette.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/palette.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/palette.cpp"
#endif
#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/palette.cpp"
#endif

//------------------------------------------------------------------------
// paletteg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/paletteg.cpp"

//------------------------------------------------------------------------
// panelcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/panelcmn.cpp"

//------------------------------------------------------------------------
// pango_x.cpp
 
#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/pango_x.cpp"
#endif

//------------------------------------------------------------------------
// paper.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/paper.cpp"

//------------------------------------------------------------------------
// pen.cpp
 
#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/pen.cpp"
#endif
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/pen.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/pen.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/pen.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/pen.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/pen.cpp"
#endif
#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/pen.cpp"
#endif
