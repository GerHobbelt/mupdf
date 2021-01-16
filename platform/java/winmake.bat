@echo off
@echo Cleaning
echo bogus > example\bogus.class
del /Q example\*.class
echo bogus > src\com\artifex\mupdf\fitz\bogus.class
del /Q src\com\artifex\mupdf\fitz\*.class

@echo "Java Compiler Version:"
"%JAVA_HOME%\bin\javac.exe" -version

@echo Building Viewer
"%JAVA_HOME%\bin\javac.exe" -Xlint:deprecation -classpath src -sourcepath src example/Viewer.java

@echo Building JNI classes
"%JAVA_HOME%\bin\javac.exe" -Xlint:deprecation -sourcepath src src/com/artifex/mupdf/fitz/*.java

@echo Importing DLL (built using VS solution)
if exist ..\win32\%1\javaviewerlib.dll ( @copy ..\win32\%1\javaviewerlib.dll mupdf_java.dll /y ) else ( @echo "Cannot find ..\win32\%1\javaviewerlib.dll" )

@echo Packaging into jar (incomplete as missing manifest)
"%JAVA_HOME%\bin\jar.exe" cf mupdf-java-viewer.jar mupdf_java.dll src\com\artifex\mupdf\fitz\*.class example\*.class
