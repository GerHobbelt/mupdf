#! /bin/bash
#
# 1. Using m-dev-list.sln as the base/driver, update all SLN files.
# 2. inspect the m-dev-*.sln files and see which projects in m-dev-list are not listed in any of them (excluding the failed-ideas and may-matter sln files for now)
# 3. inspect the failed-ideas, obnoxious-to-be-evaluated and may-matter sln files and see which projects in m-dev-list are not listed in either of them; also report projects which show up in *more than 1 of them*.
# 4. all 'obnoxious' projects that are still of interest should end up in the 'misc' sln so we can work on them from there, when we decide to address their issues.
#

if [ -n "$1" ] ; then
	STAGE=$((0 + $1))
else
	STAGE=
fi

if [ -z "$2" ] ; then
	FILTER=.
else
	# WARNING: this is used as a regex, so reckon with special behaviour for $^.*+?[]
	#
	# The brutal fix we employ here is to nuke everything at or after the first regex operator
	#
	#echo "$2" | sed -E -e 's/[ \t\r\n.*+\[\]\(\)?].*$//'    -- somehow barfs on the ] in the [..] set no matter what I try for escapes   :-S
	FILTER=$( echo "$2" | sed -E -e 'sX[.*+?()\[].*$XX' -e 's/-?lib$//i' )
fi

if [ "$FILTER" = "." ] ; then
	WILDCARD="*"
else
	WILDCARD="*$FILTER*"
fi


# debugging:
if true ; then
	echo "FILTER: $FILTER"
	echo "WILDCARD: $WILDCARD"
	echo "STAGE: $STAGE"
fi


# mode 2: anything listed in failed-ideas or obnoxious-to-be-evaluated does not need to show up in any of the m-dev-*.sln files any more!
if [ -z $STAGE ] || [ $STAGE -eq 2 ] ; then
	node ./sync-sln-files.js 2 m-dev-list.sln  $( find . -maxdepth 1 -type f -name '*.sln' )  $( find . -maxdepth 1 -type f -name '__build*.vcxproj' )
fi

# mode 3: synchronize all projects' UUIDs everywhere.
if [ -z $STAGE ] || [ $STAGE -eq 3 ] ; then
	# cope with node error: Argument list too long
	cat > __tmp_sln_vcproj_list__ <<EOF
m-dev-list.sln  

$( find . -maxdepth 1 -type f -name '*.sln' )  
$( find . -maxdepth 1 -type f -name '*.vcxproj' )  
$( find . -maxdepth 1 -type f -name '*.csproj' )  
$( find . -maxdepth 1 -type f -name '*.vcxitems' )

EOF

	node ./sync-sln-files.js 3 __tmp_sln_vcproj_list__
fi

# mode 4: update SLN dependencies, i.e. make sure all projects referenced in any other project included in a Solution (SLN) is indeed present in the Solution.
if [ -z $STAGE ] || [ $STAGE -eq 4 ] ; then
	for f in $( find . -maxdepth 1 -type f -name '*.sln' | grep -v -E -e 'failed-ideas|obnoxious-to-be-evaluated' | grep -i -e "$FILTER" ) ; do
		./add-vcxproj-dependencies-to-sln.sh $f
	done
fi

# mode 1: copy the same Win32/Win64 debug/release build list chunk from m-dev-list to all the others: Sync Solutions (SLN)
if [ -z $STAGE ] || [ $STAGE -eq 1 ] ; then
	node ./sync-sln-files.js 1 m-dev-list.sln  $( find . -maxdepth 1 -type f -name '*.sln' | grep -i -e "$FILTER" )
fi

# mode 5: add all project files to the overview solution!
if [ -z $STAGE ] || [ $STAGE -eq 5 ] ; then
	# we do this brute-force by simply adding ALL projects to that solution again; the next
	# load by Visual Studio will clean up the .sln file for us.
	echo "augment MSVC solution 'm-dev-list.sln' by adding all known projects..."
	for f in $( ls *.vcxproj | tr -d '\r' ) ; do
		LINECOUNT=$( grep -w $f m-dev-list.sln | wc -l )
		echo "$LINECOUNT :: $f"
		if test $LINECOUNT -lt 1 ; then 
			echo "ADDING:::: $f" 
			node ./mk_project_line_for_sln.js $f >> m-dev-list.sln 
		fi 
	done
fi

# mode 6: clean up / sort project files listed in each Solution (SLN)
if [ -z $STAGE ] || [ $STAGE -eq 6 ] ; then
	for f in $( ls *.sln | tr -d '\r' | grep -i -e "$FILTER" ) ; do 
		node ./sort-sln-file.js  $f 
		./msvc_sln_cleaner.exe $f 
		node ./tweak-sln-file.js  $f
	done
fi
