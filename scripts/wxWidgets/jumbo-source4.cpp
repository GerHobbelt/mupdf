

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// dirctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dirctrlcmn.cpp"


//------------------------------------------------------------------------
// dirctrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dirctrlg.cpp"


//------------------------------------------------------------------------
// dirdlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dirdlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dirdlg.cpp"
#endif


//------------------------------------------------------------------------
// dirdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dirdlgg.cpp"


//------------------------------------------------------------------------
// display.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/display.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/display.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/display.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/display.cpp"
#endif


//------------------------------------------------------------------------
// displayx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/displayx11.cpp"
#endif


//------------------------------------------------------------------------
// dlgcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dlgcmn.cpp"


//------------------------------------------------------------------------
// dlmsw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dlmsw.cpp"
#endif


//------------------------------------------------------------------------
// dlunix.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/dlunix.cpp"
#endif


//------------------------------------------------------------------------
// dnd.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/dnd.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/dnd.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/dnd.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dnd.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/dnd.cpp"
#endif


//------------------------------------------------------------------------
// dnd_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/dnd_osx.cpp"
#endif


//------------------------------------------------------------------------
// dndcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dndcmn.cpp"


//------------------------------------------------------------------------
// dobjcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dobjcmn.cpp"


//------------------------------------------------------------------------
// docmdi.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/docmdi.cpp"


//------------------------------------------------------------------------
// docview.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/docview.cpp"


//------------------------------------------------------------------------
// dpycmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dpycmn.cpp"


//------------------------------------------------------------------------
// dragimag.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/dragimag.cpp"
#endif


//------------------------------------------------------------------------
// dragimgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/dragimgg.cpp"


//------------------------------------------------------------------------
// dropsrc.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/dropsrc.cpp"
#endif


//------------------------------------------------------------------------
// droptgt.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/droptgt.cpp"
#endif


//------------------------------------------------------------------------
// dseldlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dseldlg.cpp"


//------------------------------------------------------------------------
// dvrenderer.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dvrenderer.cpp"
#endif


//------------------------------------------------------------------------
// dvrenderers.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/dvrenderers.cpp"
#endif


//------------------------------------------------------------------------
// dynlib.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dynlib.cpp"


//------------------------------------------------------------------------
// dynload.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/dynload.cpp"


//------------------------------------------------------------------------
// editlbox.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/editlbox.cpp"


//------------------------------------------------------------------------
// effects.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/effects.cpp"


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
