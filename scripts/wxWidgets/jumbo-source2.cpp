

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// anidecod.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/anidecod.cpp"


//------------------------------------------------------------------------
// animate.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/animate.cpp"
#endif


//------------------------------------------------------------------------
// animatecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/animatecmn.cpp"


//------------------------------------------------------------------------
// animateg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/animateg.cpp"


//------------------------------------------------------------------------
// any.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/any.cpp"


//------------------------------------------------------------------------
// anybutton.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/anybutton.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/anybutton.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/anybutton.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/anybutton.cpp"
#endif


//------------------------------------------------------------------------
// anybutton_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/anybutton_osx.cpp"
#endif


//------------------------------------------------------------------------
// app.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/app.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/app.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/app.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/app.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/app.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/app.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/app.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/app.cpp"
#endif


//------------------------------------------------------------------------
// appbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/appbase.cpp"


//------------------------------------------------------------------------
// appcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/appcmn.cpp"
