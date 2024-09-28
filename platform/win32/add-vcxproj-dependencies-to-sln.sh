#! /bin/bash
#
# parameter: the SLN file to process
#

if ! test -f "$1" ; then
	echo "Error. Please specify a SLN file as argument to the script."
	exit 1
fi

SLN="$1"

# collect the set of vcxproj in the SLN:
echo "" > vcxproj-file-list.lst
for f in $( grep -w "vcxproj" "$SLN" | tr '\\ \t"\r' "\n" | grep -w "vcxproj\$" ) ; do
	# help us ignore the .NET vcxproj files which have names with spaces in them: those don't matter for this dependencies collecting work...
	if test -f "$f" ; then
		echo "$f" >> vcxproj-file-list.lst
	fi
done


# custom conditions: don't inspect the __build_target* projects when we're not processing the mupdf or dev-list sln files:
if [[ $SLN =~ "mupdf" ]] ; then
	true
elif [[ $SLN =~ "m-dev-list" ]] ; then
	true
elif [[ $SLN =~ "m-single" ]] ; then
	true
else
	echo "###################### IGNORING __build_target* projects... ##################"
	cat vcxproj-file-list.lst | grep -v __build_target > tmp.lst
	cat vcxproj-file-list.lst | grep __build_target_current_work >> tmp.lst
	cat tmp.lst | sort | uniq > vcxproj-file-list.lst 
	rm tmp.lst
fi


echo "" > vcxproj-file-list-extras.lst
for f in $( cat vcxproj-file-list.lst | sort | uniq ) ; do
	echo "PROCESSING::: $f"
	#grep "[.]vcxproj[\"]" $f
	for g in $( grep "[.]vcxproj[\"]" "$f" | tr '\\ \t"\r' "\n" | grep -w "vcxproj\$" ) ; do
		if test -f "$g" ; then
			if ! grep "^$g\$" vcxproj-file-list.lst &>/dev/null ; then
				echo "+++++ $g"
				echo "$g" >> vcxproj-file-list-extras.lst
			fi
		fi
	done
done

# now we have all initial dependencies in   vcxproj-file-list-extras.lst
# expand that list until there's no change any more...
echo "############## Following the dependencies... ####################"

while true ; do
	cat vcxproj-file-list.lst vcxproj-file-list-extras.lst > tmp.lst
	cat tmp.lst | sort | uniq > vcxproj-file-list.lst 
	rm tmp.lst

	cat vcxproj-file-list-extras.lst | sort | uniq > tmp.lst
	cp tmp.lst   vcxproj-file-list-extras.lst
	rm tmp.lst

	LINECOUNT=$( cat vcxproj-file-list-extras.lst | wc -l )
	for f in $( cat vcxproj-file-list-extras.lst ) ; do
		echo "PROCESSING::: $f"
		#grep "[.]vcxproj[\"]" $f
		for g in $( grep "[.]vcxproj[\"]" "$f" | tr '\\ \t"\r' "\n" | grep -w "vcxproj\$" ) ; do
			if test -f "$g" ; then
				if ! grep "^$g\$" vcxproj-file-list.lst &>/dev/null ; then
					if ! grep "^$g\$" vcxproj-file-list-extras.lst &>/dev/null ; then
						echo "+++++ $g"
						echo "$g" >> vcxproj-file-list-extras.lst
					fi
				fi
			fi
		done
	done
	NEWLINECOUNT=$( cat vcxproj-file-list-extras.lst | wc -l )
	echo "LINECOUNT:: $LINECOUNT / $NEWLINECOUNT"

	if test $LINECOUNT -eq $NEWLINECOUNT ; then
		break
	fi
	echo "############## Another round... ####################"
done

# now add all missing vcxproj to the sln...
echo "############## Updating the SLN with the dependencies discovered... ####################"
for f in $( cat vcxproj-file-list.lst ) ; do 
	LINECOUNT=$( grep -w $f $SLN | wc -l )
	echo "$LINECOUNT :: $f"
	if test 1 != $LINECOUNT ; then 
		echo "ADDING:::: $f" 
		node ./mk_project_line_for_sln.js $f >> $SLN 
	fi 
done


