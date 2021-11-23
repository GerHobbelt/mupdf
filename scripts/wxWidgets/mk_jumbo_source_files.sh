#! /bin/bash
#

# Sample output:
# 
# //------------------------------------------------------------------------
# // bmpbndl.cpp
# 
# #include "../../thirdparty/owemdjee/wxWidgets/src/common/bmpbndl.cpp"
# #include "../../thirdparty/owemdjee/wxWidgets/src/msw/bmpbndl.cpp"
# 
# #if defined(__WXMSW__)
#     #include "wx/msw/colour.h"
# #elif defined(__WXMOTIF__)
#     #include "wx/motif/colour.h"
# #elif defined(__WXGTK20__)
#     #include "wx/gtk/colour.h"
# #elif defined(__WXGTK__)
#     #include "wx/gtk1/colour.h"
# #elif defined(__WXDFB__)
#     #include "wx/generic/colour.h"
# #elif defined(__WXX11__)
#     #include "wx/x11/colour.h"
# #elif defined(__WXMAC__)
#     #include "wx/osx/colour.h"
# #elif defined(__WXQT__)
#     #include "wx/qt/colour.h"
# #endif
# 


rm jumbo-source*.cpp




CNT=0
FILENUM=1

cat > jumbo-richtext-source.cpp <<EOF

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

EOF

for f in $(  find ../../thirdparty/owemdjee/wxWidgets/src/richtext/ -name '*.cpp' ) ; do

	g=$f
	f=$( echo $f | sed -e 's/^.*\///' )

	echo "Adding $f..."

	((CNT++))
	echo "Source #${CNT}"


	cat >> jumbo-richtext-source.cpp <<EOF  

//------------------------------------------------------------------------
// $f
 
EOF

	cat >> jumbo-richtext-source.cpp <<EOF
#include "$g"
EOF

done





CNT=0
FILENUM=1



for f in $(  find ../../thirdparty/owemdjee/wxWidgets/src/ -name '*.cpp' | grep -e '/src/\(common\|generic\|msw\|motif\|gtk\|gtk1\|x11\|qt\|dfb\)/' | grep -v 'dummy\.cpp\|strconv\.cpp' | sed -e 's/^.*\///' | sort | uniq ) ; do

	echo "Adding $f..."

	if [ $CNT -ge 10 ] ; then
		((FILENUM++))
		CNT=0
	fi
	if [ $CNT -eq 0 ] ; then
		cat > jumbo-source${FILENUM}.cpp <<EOF

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

EOF
	fi
	((CNT++))
	echo "Source #${CNT}"


	cat >> jumbo-source${FILENUM}.cpp <<EOF  

//------------------------------------------------------------------------
// $f
 
EOF

	for g in $(  find ../../thirdparty/owemdjee/wxWidgets/src/ -name "$f" ) ; do

		PLATFORM=$( echo $g | sed -e 's/^.*\/src\/\([^\/]\+\)\/.*$/\1/' )
		case "$PLATFORM" in 
		msw)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXMSW__)
#  include "$g"
#endif
EOF
			;;

		motif)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXMOTIF__)
#  include "$g"
#endif
EOF
			;;

		gtk1)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXGTK__)
#  include "$g"
#endif
EOF
			;;

		gtk)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXGTK20__)
#  include "$g"
#endif
EOF
			;;

		dfb)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXDFB__)
#  include "$g"
#endif
EOF
			;;

		x11)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXX11__)
#  include "$g"
#endif
EOF
			;;

		osx)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXMAC__)
#  include "$g"
#endif
EOF
			;;

		qt)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXQT__)
#  include "$g"
#endif
EOF
			;;

		univ)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if !defined(__WXMSW__)
#  include "$g"
#endif
EOF
			;;

		unix)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if !defined(__WINDOWS__)
#  include "$g"
#endif
EOF
			;;

		*)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#include "$g"
EOF
			;;
		esac
	done
done










