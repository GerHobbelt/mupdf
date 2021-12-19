

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// menu.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/menu.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/menu.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/menu.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/menu.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/menu.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/menu.cpp"
#endif


//------------------------------------------------------------------------
// menu_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/menu_osx.cpp"
#endif


//------------------------------------------------------------------------
// menucmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/menucmn.cpp"


//------------------------------------------------------------------------
// menuitem.cpp

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/menuitem.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/menuitem.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/menuitem.cpp"
#endif


//------------------------------------------------------------------------
// menuitem_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/menuitem_osx.cpp"
#endif


//------------------------------------------------------------------------
// metafile.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/metafile.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/metafile.cpp"
#endif


//------------------------------------------------------------------------
// metal.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/themes/metal.cpp"
#endif


//------------------------------------------------------------------------
// mimecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/mimecmn.cpp"


//------------------------------------------------------------------------
// mimetype.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/mimetype.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mimetype.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/mimetype.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/mimetype.cpp"
#endif


//------------------------------------------------------------------------
// minifram.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/minifram.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/minifram.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/minifram.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/minifram.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/minifram.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/minifram.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/minifram.cpp"
#endif


//------------------------------------------------------------------------
// mnemonics.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/mnemonics.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/mnemonics.cpp"
#endif


//------------------------------------------------------------------------
// modalhook.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/modalhook.cpp"


//------------------------------------------------------------------------
// module.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/module.cpp"


//------------------------------------------------------------------------
// mono.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/themes/mono.cpp"
#endif


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
// nanox.c

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/nanox.c"
#endif


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
