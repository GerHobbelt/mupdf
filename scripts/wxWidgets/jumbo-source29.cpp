

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// taskbarcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/taskbarcmn.cpp"


//------------------------------------------------------------------------
// taskbarx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/taskbarx11.cpp"
#endif


//------------------------------------------------------------------------
// tbarbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/tbarbase.cpp"


//------------------------------------------------------------------------
// textbuf.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/textbuf.cpp"


//------------------------------------------------------------------------
// textcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/textcmn.cpp"


//------------------------------------------------------------------------
// textctrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/textctrl.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/textctrl.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/textctrl.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/textctrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/textctrl.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/textctrl.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/textctrl.cpp"
#endif


//------------------------------------------------------------------------
// textctrl_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/textctrl_osx.cpp"
#endif


//------------------------------------------------------------------------
// textdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/textdlgg.cpp"


//------------------------------------------------------------------------
// textentry.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/textentry.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/textentry.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/textentry.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/textentry.cpp"
#endif


//------------------------------------------------------------------------
// textentry_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/textentry_osx.cpp"
#endif


//------------------------------------------------------------------------
// textentrycmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/textentrycmn.cpp"


//------------------------------------------------------------------------
// textfile.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/textfile.cpp"


//------------------------------------------------------------------------
// textmeasure.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/textmeasure.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/textmeasure.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/textmeasure.cpp"
#endif


//------------------------------------------------------------------------
// textmeasurecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/textmeasurecmn.cpp"


//------------------------------------------------------------------------
// tglbtn.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/tglbtn.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/tglbtn.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/tglbtn.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/tglbtn.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/tglbtn.cpp"
#endif


//------------------------------------------------------------------------
// tglbtn_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/tglbtn_osx.cpp"
#endif


//------------------------------------------------------------------------
// theme.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/theme.cpp"
#endif


//------------------------------------------------------------------------
// thread.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/thread.cpp"
#endif


//------------------------------------------------------------------------
// threadinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/threadinfo.cpp"


//------------------------------------------------------------------------
// threadno.cpp

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/threadno.cpp"
#endif
