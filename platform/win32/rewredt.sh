#! /bin/bash
#
#    git filter-branch --tree-filter $( pwd )/platform/win32/rewredt.sh master..x
#
# produces its own copy of the source files in the repo in a separate .git-rewrite/t/ directory:
# we need to do our cleanup there, instead of going to our original source tree.
#

echo "----------"
pwd
find -size +40M ! -name '*.exe' ! -name '*.sql' ! -name '*.lst' -delete -print
