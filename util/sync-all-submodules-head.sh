#! /bin/bash
#
# - Assumes entire dev tree on drive Z:
# - Assumes GerHobbelt git scripts are available at Z:/tools/
#

pushd $( dirname "$0" )
pwd

cd ../thirdparty/

for f in * ; do 
	if test "$f" != "owemdjee" ; then  
		# only process real sub-repo's as otherwise we will *nuke* the main repo and that would be ... bad.  ;-)
		if test -f  $f/.git ; then 
			echo $f 
			pushd $f
			/z/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/sync-module.sh
			popd 
		fi 
	fi 
done

cd ../thirdparty/owemdjee/

for f in * ; do 
	# only process real sub-repo's as otherwise we will *nuke* the main repo and that would be ... bad.  ;-)
	if test -f  $f/.git ; then 
		echo $f 
		pushd $f
		/z/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/sync-module.sh
		popd 
	fi 
done

popd
