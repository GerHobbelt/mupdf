#! /bin/bash
#
# refill [vcxproj] [path-to-source-tree]
# 
# Re-initialize the vcxproj MSVC project file with the files in the source tree.
# 
# Uses the already present JavaScript scripts for initializing and filling / updating
# a vcxproj project file.
# 

node ./patch-vcxproj.js $1 tweak
node ./add-sources-to-vcxproj.js $1 $2
node ./update-vcxproj.js $1
