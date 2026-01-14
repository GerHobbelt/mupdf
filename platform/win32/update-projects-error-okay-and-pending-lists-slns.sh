#! /bin/bash
#
# scans the /bin/ linker output directories to discover which projects did build to completion and which didn't
# and places the successful and failing ones in seaprate SLN solutions for further work.
#

# collect the set of vcxproj in the main SLN and dump them in 'pending':
if true ; then
	echo "" >> m-dev-list-projects-pending.sln
	grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942" -A1 m-dev-list.sln >> m-dev-list-projects-pending.sln
fi

for f in $( find ./bin -type f \( -name '*.exe' -o -name '*.lib' \) -print ) ; do 
	echo "$f" 
	x=$( basename $( basename "$f" .lib ) .exe )
	#echo "$x"
	grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*\"${x}\"" -A1  m-dev-list.sln >> m-dev-list-projects-okay.sln
done

# every project that's in the okay SLN must be removed from the erroring and pending sets:
for f in $( grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942"  m-dev-list-projects-okay.sln | cut -d ' ' -f 4 | tr -d '",' ) ; do
	if test -f "$f" ; then
		echo "Removing okay project $f from the erroring + pending SLN sets..."
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-erroring.sln 
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-pending.sln 
	fi
done

./add-vcxproj-dependencies-to-sln.sh 			m-dev-list-projects-okay.sln

for f in  m-dev-list-projects-okay.sln  m-dev-list-projects-erroring.sln  m-dev-list-projects-pending.sln  ; do 
	node ./sort-sln-file.js   $f 
	./msvc_sln_cleaner.exe    $f
	node ./tweak-sln-file.js  $f
done
