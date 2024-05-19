#! /bin/bash
#
#
#

rm tmp    2>/dev/null
rm -f tmp2   2>/dev/null

echo "get the submodule list.."
grep 'path =' ../../thirdparty/owemdjee/.gitmodules | sed -E -e 's/path =//' -e 's/^\s+//g' > tmp

echo "filter the submodule list.."
for f in $( cat tmp | tr -d '\r' ) ; do 
	#echo "testing: $f ..."
	#grep  -w $f ./mknew.sh | grep -v '#'
	WC=$( grep  -w $f ./mknew.sh | grep -v '#' | wc -l )
	#echo "WC=$WC"
	if test $WC -gt 0 ; then 
		#echo "Y $f" | tee -a tmp2
		true
	else 
		echo "$f" | tee -a tmp2
	fi 
done

