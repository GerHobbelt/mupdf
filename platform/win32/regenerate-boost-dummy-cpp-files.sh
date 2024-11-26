#! /bin/bash

for f in *boost*.vcxproj ; do 
	echo $f 
	echo "../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp" 
	if test -f ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp ; then 
		#cat $f | grep '<ClInclude Include' | sed -E -e 's/^[^"]*"/\/\/ #include "/' -e 's/"[^"]*$/"/'
		echo "" > ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp
		cat $f | grep '<ClInclude Include' | sed -E -e 's/^[^"]*"//' -e 's/"[^"]*$//' -e 's/^.*\/include\/boost\///' -e 's/^/#include <boost\//' -e 's/$/>/' >> ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp 
		echo "" >> ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp
	else 
		echo '---' 
	fi 
done
