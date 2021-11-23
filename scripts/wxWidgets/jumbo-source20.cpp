

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



//------------------------------------------------------------------------
// ffile.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/ffile.cpp"


//------------------------------------------------------------------------
// file.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/file.cpp"


//------------------------------------------------------------------------
// fileback.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fileback.cpp"


//------------------------------------------------------------------------
// fileconf.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/fileconf.cpp"


//------------------------------------------------------------------------
// filectrl.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/filectrl.cpp"
#endif


//------------------------------------------------------------------------
// filectrlcmn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filectrlcmn.cpp"


//------------------------------------------------------------------------
// filectrlg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/filectrlg.cpp"


//------------------------------------------------------------------------
// filedlg.cpp

#if defined(__WXGTK20__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk/filedlg.cpp"
#endif

#if defined(__WXGTK__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/gtk1/filedlg.cpp"
#endif

#if defined(__WXMOTIF__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/motif/filedlg.cpp"
#endif

#if defined(__WXMSW__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/msw/filedlg.cpp"
#endif

#if defined(__WXQT__)
#  include "../../thirdparty/owemdjee/wxWidgets/src/qt/filedlg.cpp"
#endif


//------------------------------------------------------------------------
// filedlgg.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/generic/filedlgg.cpp"


//------------------------------------------------------------------------
// filefn.cpp

#include "../../thirdparty/owemdjee/wxWidgets/src/common/filefn.cpp"
