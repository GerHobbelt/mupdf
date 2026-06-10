#! /bin/bash

cat > /tmp/projects-to-do.lst <<EOT

bitfield_array
data_seek_cache
scales_chords_modes
enhanced_linn_sequencer
fixed_space_std_allocator
adsr_curve_tweens
flash_wear_leveling_management
specialty_histograms
str-buf-stream-fmt-noheap

doctest

cmake-cpp
cmake-init
cpp-project
cpptemplate
cleanCppProject
cmake_examples
cmake_template
googletest-ci-cpp-template
ModernCppStarter
modern-cpp-template

libdiagnostics
libparameters
libpathutils
libchiaroscuramente
libchopshop
chewing_text_cud
text-wrangler
worde_butcher
libhashish
politician
libunisani



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
