
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


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
// stdstream.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/stdstream.cpp"

//------------------------------------------------------------------------
// stockitem.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/stockitem.cpp"

//------------------------------------------------------------------------
// stopwatch.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/stopwatch.cpp"
