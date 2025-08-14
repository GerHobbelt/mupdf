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

if [ -z $STAGE ] || [ $STAGE -eq 1 ] ; then
	node ./sync-sln-files.js 1 m-dev-list.sln  $( find . -maxdepth 1 -type f -name '*.sln' )
fi

# mode 2: anything listed in failed-ideas or obnoxious-to-be-evaluated does not need to show up in any of the m-dev-*.sln files any more!
if [ -z $STAGE ] || [ $STAGE -eq 2 ] ; then
	node ./sync-sln-files.js 2 m-dev-list.sln  $( find . -maxdepth 1 -type f -name '*.sln' )
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
	for f in $( find . -maxdepth 1 -type f -name '*.sln' | grep -v -E -e 'failed-ideas|obnoxious-to-be-evaluated' ) ; do
		./add-vcxproj-dependencies-to-sln.sh $f
	done
fi

# mode 5: clean up / sort project files listed in each Solution (SLN)
if [ -z $STAGE ] || [ $STAGE -eq 5 ] ; then
	for f in *.sln ; do 
		node ./sort-sln-file.js  $f 
		./msvc_sln_cleaner.exe $f 
		node ./tweak-sln-file.js  $f
	done
fi
