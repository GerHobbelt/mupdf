# Static vs Dynamic libraries :: MSVC projects and general findings

## Monolithic MuPDFLib notes
Default is to build a monolithic DLL for this bunch, which includes the MuPDF, tesseract and many image format libraries.

### TODO
At the time of this writing, we also bundle tool and test code in these and export those `xyz_lib_named_tool_main()` functions. We need to separate those out into separate projects and a separate monolithic tool and monolithic test/samples/demos DLL to ensure our production DLL (MuPDFLib itself) will be minimal sized.

### The MSVC projects

`MuPDFLib` is the default (**DLL**) project.

`MuPDFLib_Static` is a copy which produces a monolithic *static library* -- of course, that one can safely contain all those tool/demo/sample/test functions as well so it's only `MuPDFLib` itself which may need cleaning as per the TODO above.

> ## I've got an *Idea*!
> 
> As we use a DEF file for the DLL exports, it may suffice to just strip off those test/demo/tool APIs from the DEF file in the `MuPDFLib.py`  utility script (which is a heavily augmented derivative of the original MuPDF one, by the way) and there's no need to produce yet more library projects, just so we can have a reduced-size DLL monolithic library here!
> 
> Alternatively, we could add `pcre2` to the `MuPDFLib` / `MuPDFLib_Static` monolith and let the DEF file generator (`MuPDFLib.py`) take care of the proper PCRE2 exports in the DLL case: PCRE2 is a C library, so this should work out well too!



## wxWidgets and friends

wxWidgets is built as a monolithic **static library** by default: this happens in the `wxWidgets` project. 

wxWidgets has a couple of dependencies, which are included as static libraries in that monolith, f.e. `pcre2`, as those are only used by the wxWidgets library thus far (and we don't plan on using them elsewhere in our codebase yet, either).

The monolithic **DLL**  version of wxWidgets comes from the `wxWidgets_DLL` project. That one is, of course, dependent on the MuPDFLib **DLL** for some generic services, including support for regular image formats such as PNG, as the MuPDFLib library/DLL includes those libraries and we don't want any duplication.


### Summary

`wxWidgets` is the default (**static library**) project.

`wxWidgets_DLL` is a copy which produces a monolithic **DLL**.


### Notes

Note that the `libpcre2` project has been specifically set to produce a **static build** for that library (by defining `PCRE2_STATIC` in the compiler/preprocessor settings) as wee found that leaving out that define would still produce workable code, yet any statically-linked *executable*  would *export thee entire PCRE2 library alongside* which is never our intent.

> This was discovered when we started hunting down the cause(s) for the relative large `.exe` file sizes for some of the wxWidgets samples. Tweaking the projects (such as `wxw-samples-minimal`) to also produce a MAP file during the link phase uncovered this.



### wxCharts

wxCharts is built as yet another **DLL** or static library:

`wxCharts` is the default (**static library**) project.

`wxCharts_DLL` is a copy which produces a monolithic **DLL**.



## When building executables: which dependencies should my MSVC project have?

When you wish to produce a *statically linked* executable, use `MuPDFLib_Static` (or `MuPDFLib` if you want to load that part as a shared DLL when your executable has some heavy usage of that codebase anyway). Where needed, add `wxWidgets` and `wxCharts` as dependencies and your linker should sort you out.

Alternatively, when you wish to produce a small-size executable which re-uses the DLLs' codebases, then add `MuPDFLib`, `wxCharts_DLL` and `wxWidgets_DLL` as your dependencies.

**The Bottom Line**: you can pick either of the `MuPDFLib...` projects as dependency, but always stick to either the static or dynamic versions of `wxWidgets` and `wxCharts`.



## Weirdness Extraordinaire

See [[Weirdness Extraordinaire - strange discoveries while hunting for smaller static linked executables]].