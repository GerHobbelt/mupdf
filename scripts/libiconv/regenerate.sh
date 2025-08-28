#! /bin/bash
#
# derived from https://stackoverflow.com/questions/59817502/setting-nmake-makefile-directory/59824258#59824258
# but that approach didn't fly for us from within Visual Studio.
#
# Furthermore, it turns out that Utility Projects (https://stackoverflow.com/questions/48720991/what-is-utility-configuration-type-in-visual-studio /
# https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa733962(v=vs.60)?redirectedfrom=MSDN ) only execute their
# 'Pre-Build' action ONCE and there's no clear way to re-run that action/command on the next BUILD, so that type of project turned out to be pretty
# useless for us.
# Instead, we use a "Makefile" Project Type ( https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa733947(v=vs.60) )
# where the Visual Studio command line is:
#
#     pushd "$(ProjectDir)" && bash -c "../../scripts/libiconv/regenerate.sh" & popd
#
# as, of course (*sigh*), `bash` does NOT cope with the Win32/MSDOS path BACKSLASHES in $(ProjectDir), but we still do not want to be bound to using
# nmake, so instead we chose to run GNU Make through a BASH shell script, as invoked by the Build/Rebuild/Clean target commands in the
# MAKEFILE Project Type Properties.
#

# be very verbose to help us diagnose what's going down when this bugger executes...
set -vx

BASEDIR=$( dirname $( realpath "$0" ) )

# first: update the tool executables where we'll expect them: in /platform/win32/

pushd ${BASEDIR}/../../platform/win32/

# *this* command does NOT show its output in the Visual Studio Output pane...
#
#     pwd
#
# but this way it does... (apparently stdout is NOT captured ?properly? in Visual Studio Output pane. hm...)
pwd                        1>&2 
# ... as `echo` *does* work:
echo $( pwd )

SRCDIR=bin/Release-Unicode-64bit-x64/

echo "copy/update ${SRCDIR}/*.exe to /platform/win32/"
cp -v -u ${SRCDIR}/*.exe ./

GPERF=$( realpath ./gperf.exe )
ICONV_TOOLS=$( realpath ./libiconv_tools.exe )

popd 

pushd ${BASEDIR}/../../thirdparty/owemdjee/libiconv/

make -f ${BASEDIR}/Makefile   regenerate    GPERF=${GPERF} ICONV_TOOLS=${ICONV_TOOLS} BASEDIR=${BASEDIR}

popd
