

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// scrolwin.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/scrolwin.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/scrolwin.cpp"
#endif


//------------------------------------------------------------------------
// scrthumb.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/scrthumb.cpp"
#endif


//------------------------------------------------------------------------
// secretstore.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/secretstore.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/secretstore.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/secretstore.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/secretstore.cpp"
#endif


//------------------------------------------------------------------------
// selectdispatcher.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/selectdispatcher.cpp"


//------------------------------------------------------------------------
// selstore.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/selstore.cpp"


//------------------------------------------------------------------------
// settcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/settcmn.cpp"


//------------------------------------------------------------------------
// settings.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/settings.cpp"
#endif

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/settings.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/settings.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/settings.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/settings.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/settings.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/settings.cpp"
#endif


//------------------------------------------------------------------------
// settingsuniv.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/settingsuniv.cpp"
#endif


//------------------------------------------------------------------------
// sizer.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sizer.cpp"


//------------------------------------------------------------------------
// slider.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/slider.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/slider.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/slider.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/slider.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/slider.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/slider.cpp"
#endif


//------------------------------------------------------------------------
// slider_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/slider_osx.cpp"
#endif


//------------------------------------------------------------------------
// slidercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/slidercmn.cpp"


//------------------------------------------------------------------------
// snglinst.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/snglinst.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/snglinst.cpp"
#endif


//------------------------------------------------------------------------
// socket.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/socket.cpp"


//------------------------------------------------------------------------
// socketiohandler.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/socketiohandler.cpp"


//------------------------------------------------------------------------
// sockgtk.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/sockgtk.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/sockgtk.cpp"
#endif


//------------------------------------------------------------------------
// sockmot.cpp

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/sockmot.cpp"
#endif


//------------------------------------------------------------------------
// sockmsw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/sockmsw.cpp"
#endif


//------------------------------------------------------------------------
// sockosx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/sockosx.cpp"
#endif


//------------------------------------------------------------------------
// sockqt.cpp

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/sockqt.cpp"
#endif


//------------------------------------------------------------------------
// sockunix.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/sockunix.cpp"
#endif


//------------------------------------------------------------------------
// sound.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/sound.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/sound.cpp"
#endif

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/core/sound.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/sound.cpp"
#endif


//------------------------------------------------------------------------
// sound_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/sound_osx.cpp"
#endif


//------------------------------------------------------------------------
// sound_sdl.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/sound_sdl.cpp"
#endif


//------------------------------------------------------------------------
// spinbtncmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/spinbtncmn.cpp"


//------------------------------------------------------------------------
// spinbutt.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/spinbutt.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/spinbutt.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/spinbutt.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/spinbutt.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/spinbutt.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/spinbutt.cpp"
#endif


//------------------------------------------------------------------------
// spinbutt_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/spinbutt_osx.cpp"
#endif


//------------------------------------------------------------------------
// spinctlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/spinctlg.cpp"


//------------------------------------------------------------------------
// spinctrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/spinctrl.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/spinctrl.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/spinctrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/spinctrl.cpp"
#endif


//------------------------------------------------------------------------
// spinctrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/spinctrlcmn.cpp"


//------------------------------------------------------------------------
// splash.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/splash.cpp"


//------------------------------------------------------------------------
// splitter.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/splitter.cpp"


//------------------------------------------------------------------------
// srchcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/srchcmn.cpp"


//------------------------------------------------------------------------
// srchctlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/srchctlg.cpp"


//------------------------------------------------------------------------
// srchctrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/srchctrl.cpp"
#endif


//------------------------------------------------------------------------
// srchctrl_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/srchctrl_osx.cpp"
#endif


//------------------------------------------------------------------------
// sstream.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/sstream.cpp"
