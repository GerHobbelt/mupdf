

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// imagxpm.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/imagxpm.cpp"


//------------------------------------------------------------------------
// infobar.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/infobar.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/infobar.cpp"
#endif


//------------------------------------------------------------------------
// iniconf.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/iniconf.cpp"
#endif


//------------------------------------------------------------------------
// init.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/init.cpp"


//------------------------------------------------------------------------
// inpcons.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/inpcons.cpp"
#endif


//------------------------------------------------------------------------
// inphand.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/inphand.cpp"
#endif


//------------------------------------------------------------------------
// intl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/intl.cpp"


//------------------------------------------------------------------------
// ipcbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ipcbase.cpp"


//------------------------------------------------------------------------
// joystick.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/joystick.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/joystick.cpp"
#endif


//------------------------------------------------------------------------
// languageinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/languageinfo.cpp"


//------------------------------------------------------------------------
// layout.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/layout.cpp"


//------------------------------------------------------------------------
// laywin.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/laywin.cpp"


//------------------------------------------------------------------------
// lboxcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/lboxcmn.cpp"


//------------------------------------------------------------------------
// list.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/list.cpp"


//------------------------------------------------------------------------
// listbkg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/listbkg.cpp"


//------------------------------------------------------------------------
// listbox.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/listbox.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/listbox.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/listbox.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/listbox.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/listbox.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/listbox.cpp"
#endif


//------------------------------------------------------------------------
// listbox_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/listbox_osx.cpp"
#endif


//------------------------------------------------------------------------
// listctrl.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/listctrl.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/listctrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/listctrl.cpp"
#endif


//------------------------------------------------------------------------
// listctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/listctrlcmn.cpp"


//------------------------------------------------------------------------
// log.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/log.cpp"


//------------------------------------------------------------------------
// logg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/logg.cpp"


//------------------------------------------------------------------------
// longlong.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/longlong.cpp"


//------------------------------------------------------------------------
// lzmastream.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/lzmastream.cpp"


//------------------------------------------------------------------------
// main.cpp

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/main.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/main.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/main.cpp"
#endif


//------------------------------------------------------------------------
// markupparser.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/markupparser.cpp"


//------------------------------------------------------------------------
// markuptext.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/markuptext.cpp"


//------------------------------------------------------------------------
// mask.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/mask.cpp"


//------------------------------------------------------------------------
// matrix.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/matrix.cpp"


//------------------------------------------------------------------------
// mdi.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/mdi.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/mdi.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mdi.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/mdi.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/mdi.cpp"
#endif


//------------------------------------------------------------------------
// mdig.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/mdig.cpp"


//------------------------------------------------------------------------
// mediactrl.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/mediactrl.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/mediactrl.cpp"
#endif


//------------------------------------------------------------------------
// mediactrl_am.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mediactrl_am.cpp"
#endif


//------------------------------------------------------------------------
// mediactrl_gstplayer.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/mediactrl_gstplayer.cpp"
#endif


//------------------------------------------------------------------------
// mediactrl_qt.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mediactrl_qt.cpp"
#endif


//------------------------------------------------------------------------
// mediactrl_wmp10.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/mediactrl_wmp10.cpp"
#endif


//------------------------------------------------------------------------
// mediactrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/mediactrlcmn.cpp"


//------------------------------------------------------------------------
// memory.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/memory.cpp"


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
