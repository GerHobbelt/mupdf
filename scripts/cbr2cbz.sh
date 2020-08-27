#!/bin/bash
if [ "$#" -ne 2 ]; then
	echo usage: cbr2cbz input.cbr output.cbz
	exit 1
fi
TMP=$(mktemp -d /tmp/cbr2cbz.XXXX)
unar -D -o $TMP "$1"
OUT=$(readlink -f "$2")
cd $TMP && zip -0 -X -r "$OUT" .
rm -rf $TMP
