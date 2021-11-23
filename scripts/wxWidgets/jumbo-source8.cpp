

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// choicdgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/choicdgg.cpp"


//------------------------------------------------------------------------
// choice.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/choice.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/choice.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/choice.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/choice.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/choice.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/choice.cpp"
#endif


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


//------------------------------------------------------------------------
// clntdata.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/clntdata.cpp"


//------------------------------------------------------------------------
// clrpicker.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/clrpicker.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/clrpicker.cpp"
#endif


//------------------------------------------------------------------------
// clrpickercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/clrpickercmn.cpp"


//------------------------------------------------------------------------
// clrpickerg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/clrpickerg.cpp"


//------------------------------------------------------------------------
// cmdline.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cmdline.cpp"
