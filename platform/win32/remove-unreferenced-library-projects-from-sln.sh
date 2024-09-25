#! /bin/bash
#
# removes any libXYZ project from the solutionif it is not referenced as a depoendency by ANY project in this directory.
#

grep 'lib.*vcxproj' mupdf.sln | tr '"' '\n' | grep vcxproj | sed -e 's/[.]vcxproj//' > libprojnames.lst

for f in $( cat  libprojnames.lst | tr -d '\r' ) ; do
	echo "CHECKING: $f"
	REFCOUNT=$( grep "ProjectReference.*$f" *.vcxproj | wc -l )
	echo "REFCOUNT = $REFCOUNT"
	if test $REFCOUNT = 0 ; then
		echo "REMOVING $f..."
		sed -i -e "/Project.*, \"$f[.]vcxproj\", /d" $1
	fi
done
