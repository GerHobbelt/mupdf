#! /bin/bash
#
# the tessdata* repositories MUST NOT have symlink support in Windows as otherwise 
# the copy/link script used to duplicate those (large) directories to the MSVC build target directories
# will produce some crap (broken symlinks): git's way to provide symlinks vs. our NTFS-native hardlinks
# for those repos/directories does not mesh well together.

pushd $( dirname $0 )
cd ../../thirdparty
for f in tessd* tessc* ; do 
	pushd $f 
	git config core.symlinks false 
	git submodule foreach git config core.symlinks false 
	popd 
done

# now nuke everything that's in there:
( for f in tessd* tessc* ; do  find $f   ; done ) | grep -v '[.]git' | xargs rm

# and use git reset to recover it all, properly
for f in tessd* tessc* ; do 
	pushd $f 
	git reset --hard
	git submodule foreach git reset --hard
	popd 
done





