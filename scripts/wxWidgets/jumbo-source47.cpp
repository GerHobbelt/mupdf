
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// textmeasure.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/textmeasure.cpp"
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/textmeasure.cpp"
#endif
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/textmeasure.cpp"
#endif

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
// threadsgi.cpp
 
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/threadsgi.cpp"
#endif

//------------------------------------------------------------------------
// time.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/time.cpp"

//------------------------------------------------------------------------
// timectrl.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/timectrl.cpp"
#endif

//------------------------------------------------------------------------
// timectrlg.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/generic/timectrlg.cpp"
