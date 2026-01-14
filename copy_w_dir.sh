#! /bin/bash

SRC="$1"
DSTDIR="$( dirname "$1" )"
TARGETDIR="$2"
if test -z "$TARGETDIR" ; then
	TARGETDIR=$( dirname "$0" )/thirdparty/owemdjee/libhashish/tmp
fi

echo ""
#echo "----------------------------------------------------------"
echo "SRC = $SRC"
#echo "DSTDIR = $DSTDIR"
#echo "TARGETDIR = $TARGETDIR"
if test -n "$TARGETDIR/$DSTDIR" ; then
	mkdir -p "$TARGETDIR/$DSTDIR"
fi

cp --no-clobber "$1"    "$TARGETDIR/$DSTDIR/"
