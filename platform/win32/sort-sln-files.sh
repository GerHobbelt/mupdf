#! /bin/bash
#
#

if [ "$1" != "-q" ] ; then

	for f in $( find . -maxdepth 1 -type f -name '*.sln' | grep -v failed-ideas ) ; do
		./add-vcxproj-dependencies-to-sln.sh $f
	done

fi

for f in *.sln ; do 
	node ./sort-sln-file.js   $f 
	./msvc_sln_cleaner.exe    $f
	node ./tweak-sln-file.js  $f
done
