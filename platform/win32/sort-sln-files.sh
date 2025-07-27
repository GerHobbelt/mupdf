#! /bin/bash
#
#

for f in *.sln ; do 
	node ./sort-sln-file.js   $f 
	./msvc_sln_cleaner.exe    $f
	node ./tweak-sln-file.js  $f
done
