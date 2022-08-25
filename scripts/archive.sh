#!/bin/bash

REV=$(git describe --tags)
STEM=mupdf-$REV-source

echo git archive $STEM.tar
git archive --format=tar --prefix=$STEM/ -o $STEM.tar HEAD

rm -f submodule*.tar

AUTOCONF="*/configure */autogen.sh */aclocal.m4 */ltmain.sh */m4"

function make_submodule_archive {
	# Make tarballs for submodules, stripped of unneccessary files.
	echo git archive submodule-$1.tar
	git archive --format=tar --remote=thirdparty/$1 --prefix=$STEM/thirdparty/$1/ -o submodule-$1.tar HEAD
	tar f submodule-$1.tar --wildcards --delete $AUTOCONF 2>/dev/null
	for DIR in $2
	do
		tar f submodule-$1.tar --wildcards --delete "*/$DIR"
	done
	tar Af $STEM.tar submodule-$1.tar
	#rm -f submodule-$1.tar
}

make_submodule_archive curl "tests src scripts winbuild plan9 packages projects"
make_submodule_archive extract "test"
make_submodule_archive freeglut "android blackberry progs"
make_submodule_archive freetype "builds subprojects tests"
make_submodule_archive gumbo-parser "benchmarks examples python tests visualc"
make_submodule_archive harfbuzz "test perf util subprojects"
make_submodule_archive jbig2dec
make_submodule_archive lcms2 "Lib Projects plugins testbed utils"
make_submodule_archive leptonica "prog"
make_submodule_archive libjpeg "libjpeg/test* libjpeg/make*"
make_submodule_archive mujs
make_submodule_archive openjpeg
make_submodule_archive tesseract "java tessdata unittest src/training"
make_submodule_archive zlib "amiga contrib examples msdos nintendods old os400 qnx test watcom win32"

# Most common
echo gzip $STEM.tar
gzip -f -k -9 $STEM.tar
#pigz -f -k -11 $STEM.tar

# Smallest size
echo lzip $STEM.tar
plzip -9 -f -k $STEM.tar
