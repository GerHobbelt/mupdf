#! /bin/bash
#
# parameter: the MSVC compiler log output, which will be used to pick up the projects which contain compiler errors.
#

if ! test -f "$1" ; then
	echo "Error. Please specify a MSVC COMPILE SESSION LOG file as argument to the script."
	exit 1
fi

LOG="$1"

# collect the set of vcxproj in the main SLN and dump them in 'pending':
if false ; then
	echo "" >> m-dev-list-projects-pending.sln
	grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942" -A1 m-dev-list.sln >> m-dev-list-projects-pending.sln
fi

# every project that's in the erroring SLN must be removed from the okay and pending sets:
for f in $( grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942"  m-dev-list-projects-erroring.sln | cut -d ' ' -f 4 | tr -d '",' ) ; do
	if test -f "$f" ; then
		echo "Removing erroring project $f from the okay + pending SLN sets..."
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-okay.sln 
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-pending.sln 
	fi
done

# every project that's in the okay SLN should not be included in the pending SLN:	
for f in $( grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942"  m-dev-list-projects-okay.sln | cut -d ' ' -f 4 | tr -d '",' ) ; do
	if test -f "$f" ; then
		echo "Removing okay project $f from the pending SLN set..."
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-pending.sln 
	fi
done

## collect the set of projects with errors from the log file and remove those from the okay and pending SLNs:
#for f in $( cat $LOG  | grep owemdjee | grep -E ': error C[0-9]+: ' | cut -d \\ -f 8 | grep -v : | sort -u ) ; do
#	echo "Marking project $f as errorring..."
#	grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f" -A1 m-dev-list.sln >> m-dev-list-projects-erroring.sln
#	sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-okay.sln 
#	sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-pending.sln 
#done

# collect the set of projects with errors from the log file and remove those from the okay and pending SLNs:
for f in $( grep -E -e 'Done building project.*"[.]$' $LOG | cut -d ' ' -f 4 | tr -d '"' | sed -E -e 's/[.]$//' | sort -u ) ; do
	if test -f "$f" ; then
		echo "Marking project $f as succeeding..."
		grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f" -A1 m-dev-list.sln >> m-dev-list-projects-okay.sln
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-erroring.sln 
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-pending.sln 
	fi
done
for f in $( grep -E -e 'Done building project.* -- FAILED' compiler.log | cut -d ' ' -f 4 | tr -d '"' | sort -u ) ; do
	if test -f "$f" ; then
		echo "Marking project $f as erroring..."
		grep -E -e "^Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f" -A1 m-dev-list.sln >> m-dev-list-projects-erroring.sln
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-okay.sln 
		sed -E -e "/Project[(].*8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*$f/d" -i m-dev-list-projects-pending.sln 
	fi
done
