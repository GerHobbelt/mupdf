
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//------------------------------------------------------------------------
// apptraits.cpp
 
#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/apptraits.cpp"
#endif
#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/apptraits.cpp"
#endif

//------------------------------------------------------------------------
// arcall.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/arcall.cpp"

//------------------------------------------------------------------------
// arcfind.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/arcfind.cpp"

//------------------------------------------------------------------------
// archive.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/archive.cpp"

//------------------------------------------------------------------------
// arrstr.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/arrstr.cpp"

//------------------------------------------------------------------------
// artgtk.cpp
 
#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/artgtk.cpp"
#endif
#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/artgtk.cpp"
#endif

//------------------------------------------------------------------------
// artmsw.cpp
 
#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/artmsw.cpp"
#endif

//------------------------------------------------------------------------
// artprov.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/artprov.cpp"

//------------------------------------------------------------------------
// artstd.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/artstd.cpp"

//------------------------------------------------------------------------
// arttango.cpp
 
#include "../../thirdparty/owemdjee/wxWidgets/src/common/arttango.cpp"
