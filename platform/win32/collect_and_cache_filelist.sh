#! /bin/bash
#
# collect a list of all source files so we can grep them fast o find a certain file: 
# this is much faster than using `find`.
#

find ../../ -type f -a ! -ipath '*/tmp/*' > filelist.lst
