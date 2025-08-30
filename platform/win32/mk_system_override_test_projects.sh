#! /bin/bash
#
# generate one .vcxproj for each system-override's test application
#

for f in $( find ../../source/system_override/ -type f -name 'test_*.c*'  | xargs -n1 basename | sed -E -e 's/[.]c[^.]*$//' ) ; do
	echo $f
	cp -n libsystem_override_test_cpp.vcxproj  libsystem_override_$f.vcxproj 
	cp -n libsystem_override_test_cpp.spec     libsystem_override_$f.spec
done
