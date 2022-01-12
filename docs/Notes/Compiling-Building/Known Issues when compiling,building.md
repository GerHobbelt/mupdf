# Known Issues when compiling/building

## MSVC/Visual Studio: build errors; some executables are not produced ("Failed to build")

### Solution

#### Option A

You may need to hit Ctrl+Shift+B (Build All) **twice**.

If the second time around the codebase *still doesn't complete*, you'll need to check your Visual Studio `Output` panel log more closely to see where things went wrong and look further for ways to fix the reported errors:

#### If All Else Fails: Option B (*The Nuclear Option*)

An approach that may resolve any *otherwise inexplicable build-time issues* is to *Clean* and then *Rebuild All* in Visual Studio. 

When even *that* doesn't deliver and you're pretty confident the code *should* build, you're looking at another (related) Known Issue: Visual Studio is confused enough that you need to nuke it all: run the `nuke-all-build-directories.sh` then:

1. Stop/Close Visual Studio. Make sure it is not running any more!
2.  
   ```
   cd /platform/win32/
   ./nuke-all-build-directories.sh
   ```
3. Restart Visual Studio and load the `mupdf.sln` solution as before. Build All.

> What the *nuke* script does is go way beyond Visual Studio's own 'Clean All' command: it completely obliterates all Visual Studio caches and intermediate files in the build environment (MSVC's CleanAll does **not** do this! :facepalm:), only leaving your Debug Settings intact (the command-line(s) you specified for the various tools/projects in the solution as you are debugging them.)
>
> More specifically, the script nukes the `.vs` directory (which solves al lot of those *inexplicable Visual Studio problems it seems you can't get rid of*) plus a whole lot more, including all compiler intermediate files, thus forcing a *truly clean build* in Visual Studio.


### Cause

The solution is quite large and sometimes Visual Studio doesn't get it right re order of execution around some of the Custom Build steps in the projects. 

> Whether that's a Visual Studio mistake or us not having set up the custom build steps *completely properly* is open to debate -- if you improve this behaviour, don't hesitate to pipe up in the issue tracker.

### Related Notes

- Visual Studio acts weird; doesn't want to compile the solution properly anyway.





## MSVC/Visual Studio acts weird; doesn't want to compile the solution properly anyway

### Solution

It's time to fanatically clean your rig then.

See [[#MSVC Visual Studio build errors some executables are not produced Failed to build|MSVC/Visual Studio: build errors; some executables are not produced ("Failed to build")]] --> [[#If All Else Fails Option B The Nuclear Option|Option B]].



## QuickJS REPL doesn't want to update/rebuild after edit/change

### Solution

1. Select the *Extra Libraries --> Core --> libQuickJS* project
2. Execute *Rebuild* command for this project (Right-click, then click 'Rebuild'.)
3. Build the solution.

### Cause

The JSCALC and REPL scripts are *compiled* by QuickJS in the libQuickJS project's Custom Build step. For this step to succeed, you a good (working) `mutool` binary to be present already, which won't happen until you have a succesful `mupdf.sln` build. Sounds a bit like chicken & egg and, in a way, it is. However, the build can succeeed without these JS scripts having been (re)compiled as their pre-compiled versions are provided in the git repository.

This is one of the reasons you may need to *Build Twice*: the first Build action produces a sane `mutool.exe`, then the second round will be able to recompile these QuickJS scripts to their equivalent C files and thus generate an *updated* `mutool` binary.

> More precisely: an *updated* `MuPDFLib.DLL` file. Which is the core of the `mutool` functionality.





## MSVC/Visual Studio: `mutool` et al are not (re-)built on Build (Ctrl+Shift+B) when I've edited some library's source files

### Solution

#### Option A

Rebuild. Or Rebuild All. 

#### Option B

When that doesn't fly -- and this *does happen* -- you're in The Land Of Confusion Of Visual Studio and you need to Clean All, before running Rebuild All in Visual Studio.

#### Option C (*The Nuclear Option*)

When even *that* (Option B above) doesn't work out for you, you'll need to execute the nuke script as described in the [[#MSVC Visual Studio acts weird doesn't want to compile the solution properly anyway|"MSVC/Visual Studio acts weird; doesn't want to compile the solution properly anyway"]] Known Issue.

### Cause

Unknown. Sometimes Visual Studio just doesn't do the right thing when you're working in a larger solution and you edit something that's technically a couple of levels deep in the solution/Target Project dependency tree.

It may be timing (File System Change Monitoring is great but not *magical*) or some other root cause we don't know about yet. However, we **do know** that the `nuke-all-build-directories.sh` *Nuclear Option* is what you need as a first step towards getting out of this conundrum.



