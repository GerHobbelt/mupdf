#! /bin/bash
#
# refill [vcxproj] [path-to-source-tree]
# 
# Re-initialize the vcxproj MSVC project file with the files in the source tree.
# 
# Uses the already present JavaScript scripts for initializing and filling / updating
# a vcxproj project file.
# 

NAME=$( echo $1 | sed -E -e 's/\.[^.]+$/.vcxproj/' )
echo "Project Name = $NAME"

if ! test -f "$NAME" ; then
	echo "Project $NAME does not exist (yet?). Please correct this."
	exit 3
fi


if test -z "$2" ; then
	# see if we can *infer* the source directory from the project name
	SRCDIR=$( node $0/../refill-vcxproj.js $NAME )
	if test -z "$SRCDIR" ; then
		# check if a .spec for this project exists: if so, assume that one has a proper 'directories:' and/or 'sources:' entry!
		SPECFILE=$( echo $NAME | sed -e 's/\.vcxproj/.spec/' )
		if ! test -f "$SPECFILE" ; then
			echo "Cannot infer source/submodule directory for $NAME. Please either specify as second command-line parameter or provide a suitable SPEC file."
			exit 2
		#else
		#	echo "NOTE: could not infer source/submodule directory for $1, but SPEC file is available: assuming that one will provide."
		fi
	fi
else
	SRCDIR=$2
fi

node ./patch-vcxproj.js $NAME tweak
node ./add-sources-to-vcxproj.js $NAME $SRCDIR
node ./update-vcxproj.js $NAME
