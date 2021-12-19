

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// encconv.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/encconv.cpp"


//------------------------------------------------------------------------
// enhmeta.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/enhmeta.cpp"
#endif


//------------------------------------------------------------------------
// epolldispatcher.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/epolldispatcher.cpp"
#endif


//------------------------------------------------------------------------
// event.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/event.cpp"


//------------------------------------------------------------------------
// evtloop.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/evtloop.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/evtloop.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/evtloop.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/evtloop.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/evtloop.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/evtloop.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/evtloop.cpp"
#endif


//------------------------------------------------------------------------
// evtloop_cf.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/evtloop_cf.cpp"
#endif


//------------------------------------------------------------------------
// evtloopcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/evtloopcmn.cpp"


//------------------------------------------------------------------------
// evtloopconsole.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/evtloopconsole.cpp"
#endif


//------------------------------------------------------------------------
// evtloopunix.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/evtloopunix.cpp"
#endif


//------------------------------------------------------------------------
// extended.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/extended.cpp"


//------------------------------------------------------------------------
// fddlgcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fddlgcmn.cpp"


//------------------------------------------------------------------------
// fdiodispatcher.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fdiodispatcher.cpp"


//------------------------------------------------------------------------
// fdiounix.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/fdiounix.cpp"
#endif


//------------------------------------------------------------------------
// fdrepdlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/fdrepdlg.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/fdrepdlg.cpp"
#endif


//------------------------------------------------------------------------
// ffile.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ffile.cpp"


//------------------------------------------------------------------------
// file.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/file.cpp"


//------------------------------------------------------------------------
// fileback.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fileback.cpp"


//------------------------------------------------------------------------
// fileconf.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fileconf.cpp"


//------------------------------------------------------------------------
// filectrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/filectrl.cpp"
#endif


//------------------------------------------------------------------------
// filectrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filectrlcmn.cpp"


//------------------------------------------------------------------------
// filectrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/filectrlg.cpp"


//------------------------------------------------------------------------
// filedlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/filedlg.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/filedlg.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/filedlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/filedlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/filedlg.cpp"
#endif


//------------------------------------------------------------------------
// filedlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/filedlgg.cpp"


//------------------------------------------------------------------------
// filefn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filefn.cpp"


//------------------------------------------------------------------------
// filehistory.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/filehistory.cpp"
#endif


//------------------------------------------------------------------------
// filehistorycmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filehistorycmn.cpp"


//------------------------------------------------------------------------
// filename.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filename.cpp"


//------------------------------------------------------------------------
// filepicker.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/filepicker.cpp"
#endif


//------------------------------------------------------------------------
// filepickercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filepickercmn.cpp"


//------------------------------------------------------------------------
// filepickerg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/filepickerg.cpp"


//------------------------------------------------------------------------
// filesys.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filesys.cpp"


//------------------------------------------------------------------------
// filtall.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filtall.cpp"


//------------------------------------------------------------------------
// filtfind.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filtfind.cpp"


//------------------------------------------------------------------------
// fldlgcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fldlgcmn.cpp"


//------------------------------------------------------------------------
// floatpane.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/aui/floatpane.cpp"


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
