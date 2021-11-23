

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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


//------------------------------------------------------------------------
// stackwalk.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/stackwalk.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/stackwalk.cpp"
#endif


//------------------------------------------------------------------------
// statbar.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/statbar.cpp"


//------------------------------------------------------------------------
// statbmp.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/statbmp.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/statbmp.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/statbmp.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/statbmp.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/statbmp.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/statbmp.cpp"
#endif


//------------------------------------------------------------------------
// statbmp_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/statbmp_osx.cpp"
#endif


//------------------------------------------------------------------------
// statbmpcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/statbmpcmn.cpp"


//------------------------------------------------------------------------
// statbmpg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/statbmpg.cpp"


//------------------------------------------------------------------------
// statbox.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/statbox.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/statbox.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/statbox.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/statbox.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/statbox.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/statbox.cpp"
#endif


//------------------------------------------------------------------------
// statbox_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/statbox_osx.cpp"
#endif


//------------------------------------------------------------------------
// statboxcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/statboxcmn.cpp"


//------------------------------------------------------------------------
// statbrma.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/carbon/statbrma.cpp"
#endif


//------------------------------------------------------------------------
// statline.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/statline.cpp"

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/statline.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/statline.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/statline.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/statline.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/statline.cpp"
#endif


//------------------------------------------------------------------------
// statline_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/statline_osx.cpp"
#endif


//------------------------------------------------------------------------
// statlinecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/statlinecmn.cpp"


//------------------------------------------------------------------------
// stattext.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/stattext.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/stattext.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/stattext.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/stattext.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/stattext.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/stattext.cpp"
#endif


//------------------------------------------------------------------------
// stattext_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/stattext_osx.cpp"
#endif


//------------------------------------------------------------------------
// stattextcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/stattextcmn.cpp"


//------------------------------------------------------------------------
// stattextg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/stattextg.cpp"


//------------------------------------------------------------------------
// statusbar.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/statusbar.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/statusbar.cpp"
#endif


//------------------------------------------------------------------------
// statusbr.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/statusbr.cpp"

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/statusbr.cpp"
#endif


//------------------------------------------------------------------------
// stdpaths.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/stdpaths.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/stdpaths.cpp"
#endif


//------------------------------------------------------------------------
// stdpbase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/stdpbase.cpp"


//------------------------------------------------------------------------
// stdrend.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/stdrend.cpp"
#endif
