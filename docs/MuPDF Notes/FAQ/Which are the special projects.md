# Which are the special projects?

That, of course, depends on what you consider *special*.

From a '*exactly like the others, with 4 build targets (debug/release, 32/64-bit)*' perspective, the special ones are:

- `bin2coff`: a utility which is used in Custom Build commands in other projects
- `javaviewer` & `javaviewerlib`: those are for interfacing the mupdf core code with Java (JNI). **We're not using those.**
- `libfonts`: a resource library which is a bunch of fonts being prepped for inclusion in the final binary code so `mupdf` will have some fonts available to render PDF pages under all circumstances. A.k.a. the default font collection.
- `libresources`: another resources library which contains other materials which must be included in the final PDF-rendering `mupdf` library.
- `mu-office-lib` & `mu-office-test`: interfacing with MuOffice. Provided by Artifex with the original mupdf source code. **We're not using those.**
- `qiqqa-mutools-packager`: the packager utility project, which is *experimental* and intended to produce separate install & *update* installers for the mupdf-et-al tools only. 

  > Ergo: an attempt to produce installers for *part* of the total Qiqqa rig. 
  > 
  > The idea was to have installers for every part so those can be upgraded *more or less independently*.
  > Nice idea; a bit tough to deliver on when the components themselves are not finished yet...

