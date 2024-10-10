#! /bin/bash
#
# removes any XYZ project from the solution if it is not referenced as a dependency by ANY project in this directory.
#

if ! test -f "$1" ; then
	echo "Error. Please specify a SLN file as argument to the script."
	exit 1
fi

SLN="$1"

# collect the set of vcxproj in the SLN:
echo "" > vcxproj-file-list.lst
for f in $( grep -w "vcxproj" "$SLN" | tr '\\ \t"\r' "\n" | grep -w "vcxproj\$" | grep -v '^__' ) ; do
	# help us ignore the .NET vcxproj files which have names with spaces in them: those don't matter for this dependencies collecting work...
	if test -f "$f" ; then
		echo "$f" | sed -e 's/[.]vcxproj//' >> vcxproj-file-list.lst
	fi
done


# custom conditions: 
if [[ $SLN =~ "m-dev-list-of-failed-ideas" ]] ; then
	echo "SKIPPED: m-dev-list-of-failed-ideas.sln is a special solution gathering the failed/obsoleted projects we aren't interested in any longer."
	rm vcxproj-file-list.lst
	exit 0
elif [[ $SLN =~ "m-dev-list" ]] ; then
	echo "SKIPPED: m-dev-list.sln is a special solution gathering all submodules/projects we might be interested in at any time."
	rm vcxproj-file-list.lst
	exit 0
	true
else
	cat vcxproj-file-list.lst > tmp.lst
	cat tmp.lst | sort | uniq > vcxproj-file-list.lst 
	rm tmp.lst
fi


for f in $( cat  vcxproj-file-list.lst | tr -d '\r' ) ; do
	echo "CHECKING: $f"
	REFCOUNT=$( grep "ProjectReference.*$f" *.vcxproj | wc -l )
	echo "REFCOUNT = $REFCOUNT"
	if test $REFCOUNT = 0 ; then
		echo "REMOVING $f..."
		sed -i -e "/Project.*, \"$f[.]vcxproj\", /d" "$SLN"
	fi
done
