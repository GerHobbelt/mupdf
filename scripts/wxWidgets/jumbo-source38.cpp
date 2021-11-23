

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// object.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/object.cpp"


//------------------------------------------------------------------------
// odcombo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/odcombo.cpp"


//------------------------------------------------------------------------
// odcombocmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/odcombocmn.cpp"


//------------------------------------------------------------------------
// oleutils.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/oleutils.cpp"
#endif


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
