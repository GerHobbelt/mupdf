@echo on

echo "######## Copying Test Cases ########"
echo "ProjectDir=%1"
echo "OutDirFullPath=%2"

IF NOT EXIST "%1" goto fail
IF NOT EXIST "%2" goto fail

cd %2

rem robocopy  /E "%1/../../thirdparty/owemdjee/libxml2/test/" "%2/test/" *

echo "Unlinking existing links, if any exist..."
rmdir test
rem when these paths were COPIES instead of SYMLINKS/HARDLINKS, then they still exist
rem and a `rm -rf` is in order:
rmdir /S test

echo "Create hardlinks..."
mklink /H test    "%1/../../thirdparty/owemdjee/libxml2/test/" 

IF EXIST "test" goto ende

echo "Create Junctions instead (hardlinking apparently failed)..."
mklink /J test    "%1/../../thirdparty/owemdjee/libxml2/test/"

IF EXIST "test" goto ende

echo "!!!!!!!!!!!! FAILED to create symbolic/hardlink to TEST directory !!!!!!!!!!!!!!"

goto ende

:fail

echo "!!!!!!!!!!!! MISSING or NON-EXISTING path(s) specified !!!!!!!!!!!!!!"

:ende

echo "######## LibXML2 Test Cases: set up done END ########"
