

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD



//------------------------------------------------------------------------
// translation.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/translation.cpp"


//------------------------------------------------------------------------
// treebase.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/treebase.cpp"


//------------------------------------------------------------------------
// treebkg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/treebkg.cpp"


//------------------------------------------------------------------------
// treectlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/treectlg.cpp"


//------------------------------------------------------------------------
// treectrl.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/treectrl.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/treectrl.cpp"
#endif


//------------------------------------------------------------------------
// treeentry_gtk.c

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/treeentry_gtk.c"
#endif


//------------------------------------------------------------------------
// treelist.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/treelist.cpp"


//------------------------------------------------------------------------
// txtstrm.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/txtstrm.cpp"


//------------------------------------------------------------------------
// uiaction.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/uiaction.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/uiaction.cpp"
#endif


//------------------------------------------------------------------------
// uiaction_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/uiaction_osx.cpp"
#endif


//------------------------------------------------------------------------
// uiactioncmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/uiactioncmn.cpp"


//------------------------------------------------------------------------
// uiactionx11.cpp

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/uiactionx11.cpp"
#endif


//------------------------------------------------------------------------
// uilocale.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/uilocale.cpp"

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/uilocale.cpp"
#endif

#if !defined(__WINDOWS__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/unix/uilocale.cpp"
#endif


//------------------------------------------------------------------------
// unichar.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/unichar.cpp"


//------------------------------------------------------------------------
// uri.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/uri.cpp"


//------------------------------------------------------------------------
// url.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/url.cpp"


//------------------------------------------------------------------------
// urlmsw.cpp

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/urlmsw.cpp"
#endif


//------------------------------------------------------------------------
// ustring.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ustring.cpp"


//------------------------------------------------------------------------
// utils.cpp

#if defined(__WXDFB__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/dfb/utils.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/utils.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/utils.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/utils.cpp"
#endif

#if defined(__WXX11__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/x11/utils.cpp"
#endif


//------------------------------------------------------------------------
// utils_osx.cpp

#if defined(__WXMAC__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/osx/utils_osx.cpp"
#endif
