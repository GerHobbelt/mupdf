#! /bin/bash
#
# generate one .vcxproj for each wxWidgets' sample application
#

for f in $( find ../../thirdparty/owemdjee/wxWidgets/samples/ -maxdepth 1 -mindepth 1 -type d  | xargs -n1 basename ) ; do
	cp -n wxw-samples-menu.vcxproj  wxw-samples-$f.vcxproj 
done

for f in $( find ../../thirdparty/owemdjee/wxWidgets/demos/ -maxdepth 1 -mindepth 1 -type d  | xargs -n1 basename ) ; do
	cp -n wxw-demos-life.vcxproj  wxw-demos-$f.vcxproj 
done
