#! /bin/bash
#
# Find MSVC build files (object files, etc.) EVERYWHERE in the Qiqqa / MuPDF dev tree and deduce the 'root' build output directories.
# Then go and nuke those ENTIRELY to arrive at a clean, source-only system ready for a 'rebuild from scratch'.
# 
# INCLUDE all source directories in our search so we also will uncover any project-specific CMake test builds, etc.:
# the only directory we allow to survive is the final binaries output directory in MuPDF: platform/win32/bin/**/
# 

# find all build files and list them for processing, i.e. deducing 'root dev build output directory detection':
dir="$1"
if test -z "$dir" ; then
	dir=. 
fi
dir=$( realpath "$dir" )
cat <<EOT
Looking for build directories in the directory tree:

    $dir

----------------------------------------------------
EOT
P=$( dirname "$0" )
find "$dir" -type f -name '*.obj' -o -name '*.sbr' -o -name '*.pdb' -o -name '*.tlog' -o -name '*.ipch' -o -name 'Browse.VC.db' > tmp.list
node "$P/nuke-all-build-directories.js" tmp.list ./nuke-all-build-directories-exec.sh
./nuke-all-build-directories-exec.sh
rm tmp.list ./nuke-all-build-directories-exec.sh
