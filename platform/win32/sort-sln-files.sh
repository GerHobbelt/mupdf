#! /bin/bash
#
#

if [ "$1" != "-q" ] ; then
	ADD_DEPS=true
else
	shift
	ADD_DEPS=false
fi

if [ -n "$1" ] ; then
	SLN_SET="$1"
else
	SLN_SET='*.sln'
fi
echo "SLN_SET: $SLN_SET"
echo "ADD_DEPS: $ADD_DEPS"


if $ADD_DEPS ; then
	echo X
	for f in $( find . -maxdepth 1 -type f -name "$SLN_SET" | grep -v failed-ideas ) ; do
		echo "SLN: $f"
		./add-vcxproj-dependencies-to-sln.sh $f
	done
fi

for f in $SLN_SET ; do 
	echo "SLN: $f"
	node ./sort-sln-file.js   $f 
	./msvc_sln_cleaner.exe    $f
	node ./tweak-sln-file.js  $f
done
