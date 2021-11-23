
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// gdiplus.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gdiplus.cpp"
#endif

//------------------------------------------------------------------------
// geometry.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/geometry.cpp"

//------------------------------------------------------------------------
// gifdecod.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/gifdecod.cpp"

//------------------------------------------------------------------------
// glcanvas.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/glcanvas.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/glcanvas.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/glcanvas.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/glcanvas.cpp"
#endif
#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/glcanvas.cpp"
#endif

//------------------------------------------------------------------------
// glcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/glcmn.cpp"

//------------------------------------------------------------------------
// graphcmn.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/graphcmn.cpp"

//------------------------------------------------------------------------
// graphicc.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/graphicc.cpp"

//------------------------------------------------------------------------
// graphics.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/graphics.cpp"
#endif
#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/graphics.cpp"
#endif
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/graphics.cpp"
#endif

//------------------------------------------------------------------------
// graphicsd2d.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/graphicsd2d.cpp"
#endif

//------------------------------------------------------------------------
// grid.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/grid.cpp"
