@echo on

echo "######## Copying Template files ########"
echo "ProjectDir=%1"
echo "OutDirFullPath=%2"

rem robocopy $(ProjectDir)..\..\thirdparty\owemdjee\wxFormBuilder\output\  $(OutDirFullPath) *.* /S /DCOPY:DAT /W:0 /R:0

IF NOT EXIST "%1" goto fail
IF NOT EXIST "%2" goto fail

cd "%2"

robocopy "%1/../../thirdparty/owemdjee/wxFormBuilder/output/" ./ *.* /S /DCOPY:DAT /W:0 /R:0

goto ende

:fail

echo "!!!!!!!!!!!! MISSING or NON-EXISTING path(s) specified !!!!!!!!!!!!!!"

:ende

echo "######## wxFormBuilder template files: set up END ########"
