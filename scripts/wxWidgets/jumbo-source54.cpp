

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// textmeasurecmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/textmeasurecmn.cpp"


//------------------------------------------------------------------------
// tglbtn.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/tglbtn.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/tglbtn.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/tglbtn.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/tglbtn.cpp"
#endif

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/tglbtn.cpp"
#endif


//------------------------------------------------------------------------
// tglbtn_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/tglbtn_osx.cpp"
#endif


//------------------------------------------------------------------------
// theme.cpp

#if !defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/univ/theme.cpp"
#endif


//------------------------------------------------------------------------
// thread.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/thread.cpp"
#endif


//------------------------------------------------------------------------
// threadinfo.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/threadinfo.cpp"


//------------------------------------------------------------------------
// threadno.cpp

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/threadno.cpp"
#endif


//------------------------------------------------------------------------
// threadpsx.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/threadpsx.cpp"
#endif


//------------------------------------------------------------------------
// threadsgi.cpp

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/threadsgi.cpp"
#endif


//------------------------------------------------------------------------
// time.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/time.cpp"
