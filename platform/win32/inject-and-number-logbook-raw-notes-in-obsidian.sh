#! /bin/bash

EXEPATH=$( realpath $( dirname $0 ) )
pushd $EXEPATH

cd "../../../docs-src/Notes/Progress in Development/Testing & Evaluating/Collected RAW logbook notes - PDF bulktest + mutool_ex PDF + URL tests/"

pwd
ls -lr

for f in *.md ; do
	node $EXEPATH/inject-and-number-processor.js "$( realpath "$f" )"
done	
	
popd
