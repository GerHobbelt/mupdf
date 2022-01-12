# When you wish to bring new libraries into the fold

Use `m-single.sln` to have a lean Visual Studio solution where you can iron out the initial grave bugs and screaming compiler errors (such as header files that cannot be located, due to missing include paths, etc.)

When you have a new lib / codebase, we'll assume you've added it as a submodule to `thirdparty/owemdjee/` in its own directory.

Let's call that new library of yours `foobar`.

Then it's assumed to sit, as a *git submodule*, in `thirdparty/owemdjee/foobar/`.

## The fastest way to create a matching/compliant MSVC project file

The fastest way to create a MSVC project file which matches the ever-so-subtle compiler and linker settings of the other libs in this compound system is to *copy another one*, e.g.:

```
cp -n libcrow.vcxproj libfoobar.vcxproj
cp -n libcrow.vcxproj.filters libfoobar.vcxproj.filters
```

> Note that we copy the `.filters` file too so you'll get that nice folder-like hierarchy of 'Sources Files', 'Header Files', etc. in your new project too!

Now we go and **patch** the new project file to ensure it's named properly: `libfoobar`:

```
node ./patch-vcxproj.js libfoobar.vcxproj libfoobar
```

This does three things:

1. It renames the project in that project file.
2. It cleans the project file, removing all previously registered source and header files.
3. *Cleaning includes removing all project-specific include paths, so you'll have to add those later!*

Thus you end up with a **clean** project file, which you can already 'Add' to your MSVC Solution.

### Quickly adding all existing source files to the Project

You can quickly add all existing source & include files (C and C++) to the project file, and have them automatically placed in the appropriate section too, thanks to the `.filters` file, which will be patched alongside:

```
node ./add-sources-to-vcxproj.js libfoobar.vcxproj ../../thirdparty/owemdjee/foobar/
```

This will add all C & C++ files it can find in there (and the README!) to the project file.

Couldn't be quicker!

## Done

The resulting project file can be loaded in DevStudio and edited there; you can easily select and remove the few source files you don't want in the project.
