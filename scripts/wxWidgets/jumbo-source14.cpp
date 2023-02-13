

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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
// glcanvas_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/glcanvas_osx.cpp"
#endif


//------------------------------------------------------------------------
// glcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/glcmn.cpp"


//------------------------------------------------------------------------
// glegl.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/glegl.cpp"
#endif


//------------------------------------------------------------------------
// glx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/glx11.cpp"
#endif


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
// grid.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/grid.cpp"


//------------------------------------------------------------------------
// gridcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gridcmn.cpp"


//------------------------------------------------------------------------
// gridctrl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/gridctrl.cpp"


//------------------------------------------------------------------------
// grideditors.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/grideditors.cpp"


//------------------------------------------------------------------------
// gridsel.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/gridsel.cpp"


//------------------------------------------------------------------------
// gtk.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/themes/gtk.cpp"
#endif


//------------------------------------------------------------------------
// gvfs.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/gnome/gvfs.cpp"
#endif


//------------------------------------------------------------------------
// hash.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/hash.cpp"


//------------------------------------------------------------------------
// hashmap.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/hashmap.cpp"


//------------------------------------------------------------------------
// headercolcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/headercolcmn.cpp"


//------------------------------------------------------------------------
// headerctrl.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/headerctrl.cpp"
#endif
