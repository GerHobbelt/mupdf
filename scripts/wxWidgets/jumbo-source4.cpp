

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// buttonbar.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/buttonbar.cpp"


//------------------------------------------------------------------------
// cairo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cairo.cpp"


//------------------------------------------------------------------------
// calctrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/calctrl.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/calctrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/calctrl.cpp"
#endif


//------------------------------------------------------------------------
// calctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/calctrlcmn.cpp"


//------------------------------------------------------------------------
// calctrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/calctrlg.cpp"


//------------------------------------------------------------------------
// caret.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/caret.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/caret.cpp"
#endif


//------------------------------------------------------------------------
// cfstring.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/cfstring.cpp"
#endif


//------------------------------------------------------------------------
// checkbox.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/checkbox.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/checkbox.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/checkbox.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/checkbox.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/checkbox.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/checkbox.cpp"
#endif


//------------------------------------------------------------------------
// checkbox_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/checkbox_osx.cpp"
#endif


//------------------------------------------------------------------------
// checkboxcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/checkboxcmn.cpp"


//------------------------------------------------------------------------
// checklst.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/checklst.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/checklst.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/checklst.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/checklst.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/checklst.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/checklst.cpp"
#endif


//------------------------------------------------------------------------
// checklst_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/checklst_osx.cpp"
#endif


//------------------------------------------------------------------------
// checklstcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/checklstcmn.cpp"


//------------------------------------------------------------------------
// chm.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/html/chm.cpp"


//------------------------------------------------------------------------
// choicbkg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/choicbkg.cpp"


//------------------------------------------------------------------------
// choiccmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/choiccmn.cpp"


//------------------------------------------------------------------------
// choicdgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/choicdgg.cpp"


//------------------------------------------------------------------------
// choice_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/choice_osx.cpp"
#endif


//------------------------------------------------------------------------
// clipbrd.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/clipbrd.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/clipbrd.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/clipbrd.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/clipbrd.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/clipbrd.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/clipbrd.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/clipbrd.cpp"
#endif


//------------------------------------------------------------------------
// clipcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/clipcmn.cpp"
