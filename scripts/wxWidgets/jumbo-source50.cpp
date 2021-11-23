

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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
