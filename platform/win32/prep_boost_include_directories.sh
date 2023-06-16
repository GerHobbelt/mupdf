#! /bin/bash
#
# Creates the moral equivalent of symlinks on Windows / cross-platform for the boost conglomerate.
#

pushd "$( dirname "$0" )"

cd ../../scripts

echo "Ditching the old collective: some files might have been removed or renamed (moved!) in the meantime!"

rm -rf boost/include

echo "And now go and set it all up..."

mkdir -p boost/include

cd boost/include

for f in $( find ../../../thirdparty/owemdjee/boost -type f \( -name '*.h' -o -name '*.hpp' \) | grep -e '\/include\/boost\/' | sort ) ; do
	DSTFILE=$( echo $f | sed -E -e 's/^.*owemdjee//' -e 's/^.*include\///' )
	if ! test -f ./$DSTFILE ; then 
		echo "symlink: $DSTFILE --> $f"
		mkdir -p $( dirname ./$DSTFILE )
		cat > ./$DSTFILE << EOF
//
// symlink-like file is generated using the platform/win32/prep_boost_include_directories.sh shell script.
//
// ** DO NOT EDIT ** (adjust the regenerator shell script mentioned above instead!)
//

#pragma once

#include "$f"

EOF
	fi
done

echo "Done!"

popd


