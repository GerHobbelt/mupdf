

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// statbmp_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/statbmp_osx.cpp"
#endif


//------------------------------------------------------------------------
// statbmpcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/statbmpcmn.cpp"


//------------------------------------------------------------------------
// statbmpg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/statbmpg.cpp"


//------------------------------------------------------------------------
// statbox.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/statbox.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/statbox.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/statbox.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/statbox.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/statbox.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/statbox.cpp"
#endif


//------------------------------------------------------------------------
// statbox_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/statbox_osx.cpp"
#endif


//------------------------------------------------------------------------
// statboxcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/statboxcmn.cpp"


//------------------------------------------------------------------------
// statbrma.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/statbrma.cpp"
#endif


//------------------------------------------------------------------------
// statline.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/statline.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/statline.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/statline.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/statline.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/statline.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/statline.cpp"
#endif


//------------------------------------------------------------------------
// statline_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/statline_osx.cpp"
#endif


//------------------------------------------------------------------------
// statlinecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/statlinecmn.cpp"


//------------------------------------------------------------------------
// stattext.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/stattext.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/stattext.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/stattext.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/stattext.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/stattext.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/stattext.cpp"
#endif


//------------------------------------------------------------------------
// stattext_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/stattext_osx.cpp"
#endif


//------------------------------------------------------------------------
// stattextcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/stattextcmn.cpp"


//------------------------------------------------------------------------
// stattextg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/stattextg.cpp"


//------------------------------------------------------------------------
// statusbar.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/statusbar.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/statusbar.cpp"
#endif


//------------------------------------------------------------------------
// statusbr.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/statusbr.cpp"

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/statusbr.cpp"
#endif


//------------------------------------------------------------------------
// stdpaths.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/stdpaths.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/stdpaths.cpp"
#endif


//------------------------------------------------------------------------
// stdpbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/stdpbase.cpp"


//------------------------------------------------------------------------
// stdrend.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/stdrend.cpp"
#endif


//------------------------------------------------------------------------
// stdstream.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/stdstream.cpp"
