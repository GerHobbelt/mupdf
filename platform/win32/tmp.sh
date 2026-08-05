#! /bin/bash

cat > /tmp/projects-to-do.lst <<EOT

str-buf-stream-fmt-noheap
uclibc-ng
uClibc
cmark-gfm
typed_fixed_point_and_fractional_arith



EOT

# csvutils
# bibutils 
# buffered_tee
# the-yabbering-politician 
# habla-el-babbla
# splitmerge 




for f in $( cat /tmp/projects-to-do.lst ) ; do	
    ECHO $f
	if test -f $f.vcxproj ; then
	  touch $f.spec
	fi
	if test -f lib$f.vcxproj ; then
	  touch lib$f.spec
	fi

	if ! test -f $f-examples.vcxproj ; then
	  cp --no-clobber libcli11-examples.vcxproj $f-examples.vcxproj
	  touch $f-examples.spec
	fi
	if ! test -f $f-tests.vcxproj ; then
	  cp --no-clobber libyaml-tests.vcxproj $f-tests.vcxproj
	  touch $f-tests.spec
	fi
	
	./mknew.sh 1234 $f
done
