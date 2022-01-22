

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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


//------------------------------------------------------------------------
// cmdproc.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cmdproc.cpp"


//------------------------------------------------------------------------
// cmndata.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/cmndata.cpp"


//------------------------------------------------------------------------
// collheaderctrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/collheaderctrlg.cpp"


//------------------------------------------------------------------------
// collpane.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/collpane.cpp"
#endif


//------------------------------------------------------------------------
// collpaneg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/collpaneg.cpp"


//------------------------------------------------------------------------
// colordlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/colordlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/colordlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/colordlg.cpp"
#endif


//------------------------------------------------------------------------
// colour.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/colour.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/colour.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/colour.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/colour.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/colour.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/colour.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/colour.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/colour.cpp"
#endif


//------------------------------------------------------------------------
// colourcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/colourcmn.cpp"


//------------------------------------------------------------------------
// colourdata.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/colourdata.cpp"


//------------------------------------------------------------------------
// colrdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/colrdlgg.cpp"


//------------------------------------------------------------------------
// combo.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/combo.cpp"
#endif


//------------------------------------------------------------------------
// combobox.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/combobox.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/combobox.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/combobox.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/combobox.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/combobox.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/combobox.cpp"
#endif


//------------------------------------------------------------------------
// combobox_native.cpp

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/combobox_native.cpp"
#endif


//------------------------------------------------------------------------
// combobox_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/combobox_osx.cpp"
#endif


//------------------------------------------------------------------------
// combocmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/combocmn.cpp"
