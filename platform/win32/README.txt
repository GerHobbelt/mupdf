This MSVC project needs the thirdparty sources to be in place.

## Notes

- For OCR to work, set the environment variable `TESSDATA_PREFIX` to point at one of the the tessdata repositories in `./thirdparty/`, e.g.:

  ```
  TESSDATA_PREFIX=$(ProjectDir)..\..\thirdparty\tessdata_best
  ```
  

## Developer Notes

Additional scripts have been provided to ensure the MSVC projects all share the same compiler/linker settings:

- `patch-vcxproj.js` :: **patch a 'cloned' `.vcxproj` project to set the project name and fresh UUID**

  This is a utitlity script to help quickly create new MSVC project files based on existing ones. Copying an existing one in this directory and patching it is faster than creating an additional MSVC project using the Visual Studio IDE in the usual way as this way we are sure the settings of the new project *already match the settings of all the other projects* so we won't have any lurking surprises re Release/Debug/DLL/Static/cdecl/etc. mismatches in there, which would otherwise only pop up once you start linking this stuff with other projects and you observe odd behaviour, including crashes.

  The way to use this script is in a 'clone a selected project and create a new one' workflow like this:

  ```
  cp murepl.vcxproj clipp_demos.vcxproj
  cp murepl.vcxproj.filters clipp_demos.vcxproj.filters
  node patch-vcxproj.js clipp_demos.vcxproj
  # now add clipp_demos.vcxproj to your solution
  ```

  **Notes**:

  - also copy the `.filters` file (as shown above) to get the folder structure in the project already set up for you. 
    Visual Studio is smart enough that on first load, it will discard the files from the original project in there.

  - `patch-vcxproj.js` will remove all the listed source files from the cloned `.vcxproj` project, delivering a clean, empty, project to be added to your Visual Studio Solution (`.sln`). Therefore you should only use this script to set up a new project; 
    use `update-vcxproj.js` (see below) to **update** existing projects without nuking their contents.

- `update-vcxproj.js` :: **patch / update the given project to the desired common MSVC project settings**

  This script goes through a project file, injects common C++ compiler and linker settings and erases existing entries in the existing Debug & Release slots to prevent confusion and/or undesirable overrides due to these pre-existing settings.
  (Important overrides are kept per build target.)

  Use this script to "homogenize" the MSVC project files: they are stripped (size reduced), common settings are now located
  in a single common XML config block (something that the IDE doesn't do for you) and the current desired settings for all mupdf/qiqqa work will be set up for you in the project: these settings are specified in the `.js` script itself.

  If may be used in general cleanup/homogenize/synchronize workflows like this (where we update **all** project files to having the same compiler, librarian and linker settings for all build targets):

  ```
  for f in *.vcxproj ; do node update-vcxproj.js ; done
  ```

Another set of (shell+NodeJS) scripts have been provided for **project area cleanup**: Visual Studio IDE's '*Clean*' and '*Rebuild*' commands sometimes just don't cut it and **you want to make sure everything is built from scratch**. That's when these come in handy:

- `nuke-all-build-directories.sh`

  **NOTE: the scripts accepts a directory as single parameter; when none is provided the current directory is assumed.**

  Go through all the development project directories, find any GCC and MSVC output directories that can be safely erased and then `rm -rf` those directories in bulk.

  Sample usage:

  ```
  # make this directory the current directory; 
  # (this is its path in my own dev environment, change it to suit yours)
  cd /z/lib/tooling/qiqqa/MuPDF/platform/win32
  # nuke anything in the /z/lib/tooling/qiqqa/MuPDF/ tree:
  ./nuke-all-build-directories.sh ../../
  ```

  **Notes**:

  - the accompanying `nuke-all-build-directories.js` script is there to analyze the directories located by the shell script, filter the results using some basic heuristics and produce the bulk erase script `nuke-all-build-directories-exec.sh`. This one *should* be cleaned up afterwards, *but* we don't do that yet for reasons of debugging/inspection. 
  
    It turns out it's easier to keep that generated script around after the fact for that rare occasion where things go pearshaped: you then have a good chance to investigate the culprit after damage done and thus an easier option to decide how much damage has been done; no uncertainty about whether a certain directory has been hit by `rm -rf` or not.

- `nuke-all-npm-node_modules-directories.sh` :: **nuke all the installed node npm packages everywhere**

  **NOTE: the scripts accepts a directory as single parameter; when none is provided the current directory is assumed.**

  **NOTE**: this (and the other nuke script) are now also available in the genric tools/utility directory / repo https://github.com/GerHobbelt/developer-utility-commands as this is more relevant for other projects.

  Go through all the development project directories, find any `npm` installed packages that can be safely erased and then `rm -rf` those `node_modules` directories in bulk.

  Sample usage:

  ```
  # make this directory the current directory; 
  # (this is its path in my own dev environment, change it to suit yours)
  cd /z/lib/tooling/qiqqa/MuPDF/platform/win32
  # nuke anything in the /z/lib/tooling/qiqqa/ tree:
  ./nuke-all-npm-node_modules-directories.sh ../../../
  ```

  **Notes**:

  - the accompanying `nuke-all-npm-package-node_modules-directories.js` script is there to analyze the directories located by the shell script, filter the results using some basic heuristics and produce the bulk erase script. 
