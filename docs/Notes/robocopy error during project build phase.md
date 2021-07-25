# `robocopy` error reported during project build phase

That's a bit of a nasty one. 
Turns out `robocopy` returns a non-zero *exit code* when it (successfully!) copied one or more files, e.g. tesseract data files. This, in turn, leads MSVC (MSBuild) to believe that the command *failed* -- which is **not true**: check the `Output` tab in the IDE to observe the actual behaviour.

Nevertheless, I have not been able to truly *fix* this, so the work-around is to **build twice**: 
- the first time around, `robocopy` will copy the tesseract data files to the binaries' directory so they are present at a a location that's easy to reach from your binaries when testing/debugging. This build process will *fail* due to the `robocopy` copy/update session, that's part of the build.
- the *second* time around, the `robocopy` copy/update command will have **nothing to copy** and thus report 0 (SUCCESS) as an exit code, allowing Visual Studio to complete the build process. This time the remaining parts are compiled and you should have something useful at the end.

