#!/bin/bash

LIST=$(echo resources/to_unicode/* | sort)

for f in $LIST
do
	b=$(basename $f)
	echo "#include \"to_unicode/$b.h\""
	python3 scripts/cmapdump.py > source/pdf/to_unicode/$b.h $f
done


echo "static pdf_cmap *to_unicode_table[] = {"
for f in $LIST
do
	b=$(basename $f)
	c=$(echo $b | tr - _)
	echo "&cmap_$c,"
done
echo "};"
