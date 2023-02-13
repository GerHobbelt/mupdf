

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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


//------------------------------------------------------------------------
// fontmgrcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fontmgrcmn.cpp"


//------------------------------------------------------------------------
// fontpicker.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/fontpicker.cpp"
#endif


//------------------------------------------------------------------------
// fontpickercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fontpickercmn.cpp"


//------------------------------------------------------------------------
// fontpickerg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/fontpickerg.cpp"


//------------------------------------------------------------------------
// fontutil.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/fontutil.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/fontutil.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/fontutil.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/fontutil.cpp"
#endif


//------------------------------------------------------------------------
// fontutilcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fontutilcmn.cpp"


//------------------------------------------------------------------------
// frame.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/frame.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/frame.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/frame.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/frame.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/frame.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/frame.cpp"
#endif

#if defined(__UWP__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/uwp/frame.cpp"
#endif


//------------------------------------------------------------------------
// framecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/framecmn.cpp"


//------------------------------------------------------------------------
// framemanager.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/aui/framemanager.cpp"


//------------------------------------------------------------------------
// framuniv.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/framuniv.cpp"
#endif
