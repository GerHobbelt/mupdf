#! /bin/bash

cat > tmp <<EOF

politician

EOF

for f in $( cat tmp | tr -d '\r' ) ; do 
	if ! test -z "$f" ; then
		echo $f 
		./mknew.sh 1234 $f
	fi
done
