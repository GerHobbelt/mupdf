

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// fs_filter.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fs_filter.cpp"


//------------------------------------------------------------------------
// fs_inet.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fs_inet.cpp"


//------------------------------------------------------------------------
// fs_mem.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fs_mem.cpp"


//------------------------------------------------------------------------
// fswatcher.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/fswatcher.cpp"
#endif


//------------------------------------------------------------------------
// fswatcher_fsevents.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/fswatcher_fsevents.cpp"
#endif


//------------------------------------------------------------------------
// fswatcher_inotify.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/fswatcher_inotify.cpp"
#endif


//------------------------------------------------------------------------
// fswatcher_kqueue.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/fswatcher_kqueue.cpp"
#endif


//------------------------------------------------------------------------
// fswatchercmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fswatchercmn.cpp"


//------------------------------------------------------------------------
// fswatcherg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/fswatcherg.cpp"


//------------------------------------------------------------------------
// ftp.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ftp.cpp"
