

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// fmapbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fmapbase.cpp"


//------------------------------------------------------------------------
// font.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/font.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/font.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/font.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/font.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/font.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/font.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/font.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/font.cpp"
#endif


//------------------------------------------------------------------------
// fontcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fontcmn.cpp"


//------------------------------------------------------------------------
// fontdata.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fontdata.cpp"


//------------------------------------------------------------------------
// fontdlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/fontdlg.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/fontdlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/fontdlg.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/fontdlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/fontdlg.cpp"
#endif


//------------------------------------------------------------------------
// fontdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/fontdlgg.cpp"


//------------------------------------------------------------------------
// fontenum.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/fontenum.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/fontenum.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/fontenum.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/fontenum.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/fontenum.cpp"
#endif


//------------------------------------------------------------------------
// fontenumcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fontenumcmn.cpp"


//------------------------------------------------------------------------
// fontmap.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fontmap.cpp"


//------------------------------------------------------------------------
// fontmgr.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/fontmgr.cpp"
#endif
