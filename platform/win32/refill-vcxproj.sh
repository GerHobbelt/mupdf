#! /bin/bash
#
# refill [vcxproj] [path-to-source-tree]
# 
# Re-initialize the vcxproj MSVC project file with the files in the source tree.
# 
# Uses the already present JavaScript scripts for initializing and filling / updating
# a vcxproj project file.
# 

if test -z "$2" ; then
	# cheeck if a .spec for this project exists: if so, takee that one!
	SPECFILE=$( echo $1 | sed -e 's/\.vcxproj/.spec/' )
	if test -f "$SPECFILE" ; then
		SRCDIR=
	else
		# see if we can *infer* the source directory from the project name
		SRCDIR=$( node $0/../refill-vcxproj.js $1 )
		if test -z "$SRCDIR" ; then
			echo "Cannot infer source/submodule directory for $1. Please specify as second command-line parameter."
			exit 2
		fi
	fi
else
	SRCDIR=$2
fi

node ./patch-vcxproj.js $1 tweak
node ./add-sources-to-vcxproj.js $1 $SRCDIR
node ./update-vcxproj.js $1
