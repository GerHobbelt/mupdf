#! /bin/bash
#
# 1. Using m-dev-list.sln as the base/driver, update all SLN files.
# 2. inspect the m-dev-*.sln files and see which projects in m-dev-list are not listed in any of them (excluding the failed-ideas and may-matter sln files for now)
# 3. inspect the failed-ideas and may-matter sln files and see which projects in m-dev-list are not listed in either of them; also report projects which show up in *both*.
#

node ./sync-sln-files.js 1 m-dev-list.sln  $( find . -maxdepth 1 -type f -name '*.sln' )

# mode 2: anything listed in failed-ideas does not need to show up in any of the m-dev-*.sln files any more!
node ./sync-sln-files.js 2 m-dev-list.sln  $( find . -maxdepth 1 -type f -name 'm-dev-*.sln' | grep -v may-matter )

node ./sync-sln-files.js 3 m-dev-list.sln  *failed-ideas*.sln *may-matter*.sln
