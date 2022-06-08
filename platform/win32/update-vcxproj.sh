#! /bin/bash
#
# update [vcxproj]
# 
# Update the vcxproj MSVC project file settings.
# 
# Uses the already present JavaScript scripts for updating
# a vcxproj project file.
# 

#node ./patch-vcxproj.js $1 tweak
node ./update-vcxproj.js $1
