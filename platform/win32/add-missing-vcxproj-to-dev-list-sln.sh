#! /bin/bash

for f in *.vcxproj ; do 
	LINECOUNT=$( grep -w $f m-dev-list.sln | wc -l )
	echo "$LINECOUNT :: $f"
	if test 1 != $LINECOUNT ; then 
		echo "ADDING:::: $f" 
		node ./mk_project_line_for_sln.js $f >> m-dev-list.sln 
	fi 
done
