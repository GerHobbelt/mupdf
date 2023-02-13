

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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

#if defined(__UWP__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/uwp/menu.cpp"
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

#if defined(__UWP__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/uwp/menuitem.cpp"
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
