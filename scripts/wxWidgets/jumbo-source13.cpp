

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// framuniv.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/framuniv.cpp"
#endif


//------------------------------------------------------------------------
// fs_arc.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fs_arc.cpp"


//------------------------------------------------------------------------
// fs_filter.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fs_filter.cpp"


//------------------------------------------------------------------------
// fs_inet.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fs_inet.cpp"


//------------------------------------------------------------------------
// fs_mem.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fs_mem.cpp"


//------------------------------------------------------------------------
// fswatcher.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/fswatcher.cpp"
#endif


//------------------------------------------------------------------------
// fswatcher_fsevents.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/fswatcher_fsevents.cpp"
#endif


//------------------------------------------------------------------------
// fswatcher_inotify.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/fswatcher_inotify.cpp"
#endif


//------------------------------------------------------------------------
// fswatcher_kqueue.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/fswatcher_kqueue.cpp"
#endif


//------------------------------------------------------------------------
// fswatchercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fswatchercmn.cpp"


//------------------------------------------------------------------------
// fswatcherg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/fswatcherg.cpp"


//------------------------------------------------------------------------
// ftp.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ftp.cpp"


//------------------------------------------------------------------------
// gauge.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/gauge.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/gauge.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/gauge.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gauge.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/gauge.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/gauge.cpp"
#endif


//------------------------------------------------------------------------
// gauge_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/gauge_osx.cpp"
#endif


//------------------------------------------------------------------------
// gaugecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gaugecmn.cpp"


//------------------------------------------------------------------------
// gbsizer.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gbsizer.cpp"


//------------------------------------------------------------------------
// gdicmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/gdicmn.cpp"


//------------------------------------------------------------------------
// gdiimage.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gdiimage.cpp"
#endif


//------------------------------------------------------------------------
// gdiobj.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gdiobj.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/gdiobj.cpp"
#endif


//------------------------------------------------------------------------
// gdiplus.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/gdiplus.cpp"
#endif
