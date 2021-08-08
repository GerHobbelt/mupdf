# Tools to help me add a bunch of source files to an existing project

```
cd platforms/win32/
node ./add-sources-to-vcxproj.js libsample.vcxproj ../../thirdparty/owemdjee/libsample/
```

adds all C and C++ source and header files in the `../../thirdparty/owemdjee/libsample/` directory tree to the `libsample.vcxproj` project file.

The script also edits (or creates, if it doesn't exist yet!) the matching `libsample.vcxproj.filters` file to place the files in a nice folder structure in the Visual Studio project.

Add the project to your solution, or if you already have done that previously and have the solution loaded, then MSVC will request you to *Reload* the project to observe the changes.

Delete any files in the project you don't want afterwards, e.g. google fuzzer directories and such, *iff* you don't want that test code in your project. (Keep the googletest / gmock / fuzz codes in a separate project: you can setup that project using this same script and then delete the lib sources there, while keeping the test codes.)

I find this scripted + manual postprocessing (*deleting*) much faster and less strenuous than recursively going through a dirtree and adding files in Visual Studio via the usual UI means.