

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



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
