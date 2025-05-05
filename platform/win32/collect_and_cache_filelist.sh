#! /bin/bash
#
# collect a list of all source files so we can grep them fast or find a certain file: 
# this is much faster than using `find`.
#

pushd "$( dirname "$0" )"

find ../../ -type f -a ! -ipath '*/tmp/*' -a ! -ipath '*/.git/*' > filelist.lst &

popd
