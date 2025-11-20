#! /bin/bash
#
#
#

node ./redist-projects-to-solutions.js

# clean up / sort project files listed in each Solution (SLN)
for f in $( ls *.sln | tr -d '\r' | grep -i -e "$FILTER" ) ; do 
	node ./sort-sln-file.js  $f 
	./msvc_sln_cleaner.exe $f 
	node ./tweak-sln-file.js  $f
done
