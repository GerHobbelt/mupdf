@echo on

echo "######## Copying Template files ########"
echo "ProjectDir=%1"
echo "OutDirFullPath=%2"

pushd .

rem robocopy $(ProjectDir)..\..\thirdparty\owemdjee\wxFormBuilder\output\  $(OutDirFullPath) *.* /S /DCOPY:DAT /W:0 /R:0

IF NOT EXIST "%1" goto fail
IF NOT EXIST "%2" goto fail

cd "%2"

robocopy "%1/../../thirdparty/owemdjee/wxFormBuilder/output/" ./ *.* /S /DCOPY:DAT /W:0 /R:0
robocopy "%1/../../thirdparty/owemdjee/wxFormBuilder/resources/" ./resources/ *.* /S /DCOPY:DAT /W:0 /R:0

if not exist plugins (
	mkdir plugins
)

for %%f in ( additional common containers forms layout ) do (
	echo "=== Copying plugins/%%f/lib%%f.dll"
	if not exist plugins\%%f (
		mkdir plugins\%%f
	)
	copy /Y wxFormBuilderPlugin-%%f.dll plugins\%%f\lib%%f.dll
)

goto ende

:fail

echo "!!!!!!!!!!!! MISSING or NON-EXISTING path(s) specified !!!!!!!!!!!!!!"

:ende

echo "######## wxFormBuilder template files: set up END ########"

popd
