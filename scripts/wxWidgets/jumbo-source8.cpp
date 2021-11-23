

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// mousemanager.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/mousemanager.cpp"


//------------------------------------------------------------------------
// msgdlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/msgdlg.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/msgdlg.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/msgdlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/msgdlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/msgdlg.cpp"
#endif


//------------------------------------------------------------------------
// msgdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/msgdlgg.cpp"


//------------------------------------------------------------------------
// msgout.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/msgout.cpp"


//------------------------------------------------------------------------
// mstream.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/mstream.cpp"


//------------------------------------------------------------------------
// nativdlg.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/nativdlg.cpp"
#endif


//------------------------------------------------------------------------
// nativewin.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/nativewin.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/nativewin.cpp"
#endif


//------------------------------------------------------------------------
// nbkbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/nbkbase.cpp"


//------------------------------------------------------------------------
// net.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/net.cpp"
#endif


//------------------------------------------------------------------------
// nonownedwnd.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/nonownedwnd.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/nonownedwnd.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/nonownedwnd.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/nonownedwnd.cpp"
#endif


//------------------------------------------------------------------------
// nonownedwnd_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/nonownedwnd_osx.cpp"
#endif


//------------------------------------------------------------------------
// notebook.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/notebook.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/notebook.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/notebook.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/notebook.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/notebook.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/notebook.cpp"
#endif


//------------------------------------------------------------------------
// notebook_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/notebook_osx.cpp"
#endif


//------------------------------------------------------------------------
// notifmsg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/notifmsg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/notifmsg.cpp"
#endif


//------------------------------------------------------------------------
// notifmsgcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/notifmsgcmn.cpp"


//------------------------------------------------------------------------
// notifmsgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/notifmsgg.cpp"


//------------------------------------------------------------------------
// numdlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/numdlgg.cpp"


//------------------------------------------------------------------------
// numformatter.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/numformatter.cpp"


//------------------------------------------------------------------------
// object.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/object.cpp"


//------------------------------------------------------------------------
// odcombo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/odcombo.cpp"


//------------------------------------------------------------------------
// odcombocmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/odcombocmn.cpp"


//------------------------------------------------------------------------
// oleutils.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ole/oleutils.cpp"
#endif


//------------------------------------------------------------------------
// overlay.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/overlay.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/overlay.cpp"
#endif


//------------------------------------------------------------------------
// overlaycmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/overlaycmn.cpp"


//------------------------------------------------------------------------
// ownerdrw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/ownerdrw.cpp"
#endif


//------------------------------------------------------------------------
// ownerdrwcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ownerdrwcmn.cpp"


//------------------------------------------------------------------------
// palette.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/palette.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/palette.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/palette.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/palette.cpp"
#endif


//------------------------------------------------------------------------
// paletteg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/paletteg.cpp"


//------------------------------------------------------------------------
// panelcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/panelcmn.cpp"


//------------------------------------------------------------------------
// pango_x.cpp

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/pango_x.cpp"
#endif


//------------------------------------------------------------------------
// paper.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/paper.cpp"


//------------------------------------------------------------------------
// pen.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/pen.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/pen.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/pen.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/pen.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/pen.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/pen.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/pen.cpp"
#endif


//------------------------------------------------------------------------
// persist.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/persist.cpp"


//------------------------------------------------------------------------
// pickerbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/pickerbase.cpp"


//------------------------------------------------------------------------
// platinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/platinfo.cpp"


//------------------------------------------------------------------------
// popupcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/popupcmn.cpp"


//------------------------------------------------------------------------
// popupwin.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/popupwin.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/popupwin.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/popupwin.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/popupwin.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/popupwin.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/popupwin.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/popupwin.cpp"
#endif


//------------------------------------------------------------------------
// power.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/power.cpp"
#endif


//------------------------------------------------------------------------
// powercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/powercmn.cpp"


//------------------------------------------------------------------------
// preferencescmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/preferencescmn.cpp"


//------------------------------------------------------------------------
// preferencesg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/preferencesg.cpp"


//------------------------------------------------------------------------
// print.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/print.cpp"
#endif


//------------------------------------------------------------------------
// printdlg.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/printdlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/printdlg.cpp"
#endif


//------------------------------------------------------------------------
// printdlg_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/printdlg_osx.cpp"
#endif


//------------------------------------------------------------------------
// printmac.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/printmac.cpp"
#endif


//------------------------------------------------------------------------
// printps.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/printps.cpp"


//------------------------------------------------------------------------
// printqt.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/printqt.cpp"
#endif


//------------------------------------------------------------------------
// printwin.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/printwin.cpp"
#endif


//------------------------------------------------------------------------
// private.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/private.cpp"
#endif


//------------------------------------------------------------------------
// prntbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/prntbase.cpp"
