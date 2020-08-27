#! /bin/bash
#

cd $( dirname $0 )
cd ..

f=$( basename $0 .sh )

echo "executing $f.py"
python scripts/$f.py -b 02
echo "done."
