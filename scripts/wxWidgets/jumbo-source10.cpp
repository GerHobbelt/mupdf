

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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
// editors.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/propgrid/editors.cpp"


//------------------------------------------------------------------------
// eggtrayicon.c

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/eggtrayicon.c"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/eggtrayicon.c"
#endif


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
