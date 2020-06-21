#! /bin/bash
#

cd $( dirname $0 )
cd ..

f=$( basename $0 .sh )

echo "executing $f.py"
python3 scripts/$f.py -b 02
echo "done."
