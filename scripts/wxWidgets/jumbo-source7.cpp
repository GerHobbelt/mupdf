

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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
