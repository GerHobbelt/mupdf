

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



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
