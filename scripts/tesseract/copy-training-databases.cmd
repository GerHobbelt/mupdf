@echo on

echo "######## Copying Training Databases ########"
echo "ProjectDir=%1"
echo "OutDirFullPath=%2"

rem robocopy /S /XO /XJF /R:0 ..\..\thirdparty\tessdata_best\ Debug-64\tessdata\ *.* /XF .git*

IF NOT EXIST "%1" goto fail
IF NOT EXIST "%2" goto fail

cd %2

echo "Unlinking existing links, if any exist..."
rd tessdata
rd tessdata_best
rd tessdata_fast
rd tessdata_regular

echo "Create hardlinks..."
rem robocopy /S /XO /XJF /R:0 /NP %1\..\..\thirdparty\tessdata_best\ %2\tessdata_best\ *.* /XF .git*
mklink /H tessdata_best    %1\..\..\thirdparty\tessdata_best\ 
mklink /H tessdata_fast    %1\..\..\thirdparty\tessdata_fast\ 
mklink /H tessdata_regular %1\..\..\thirdparty\tessdata\ 
mklink /H tessdata         %1\..\..\thirdparty\tessdata_fast\ 

IF EXIST "tessdata" IF EXIST "tessdata_regular" IF EXIST "tessdata_fast" IF EXIST "tessdata_best" goto ende

echo "Create Junctions instead (hardlinking apparently failed)..."
mklink /J tessdata_best    %1\..\..\thirdparty\tessdata_best\ 
mklink /J tessdata_fast    %1\..\..\thirdparty\tessdata_fast\ 
mklink /J tessdata_regular %1\..\..\thirdparty\tessdata\ 
mklink /J tessdata         %1\..\..\thirdparty\tessdata_fast\ 

goto ende

:fail

echo "!!!!!!!!!!!! MISSING or NON-EXISTING path(s) specified !!!!!!!!!!!!!!"

:ende

echo "######## Tesseract Training Databases: Symlinks set up action END ########"
