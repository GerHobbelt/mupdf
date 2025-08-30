#! /bin/bash
#
# generate one .vcxproj for each Detours' sample application
#

for f in $( find ../../thirdparty/owemdjee/Detours/samples/ -maxdepth 1 -mindepth 1 -type d  | xargs -n1 basename ) ; do
	cp -n Detours_sample_static.vcxproj  Detours_sample_$f.vcxproj 
done
