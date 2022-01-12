# When all your libraries build, but your executables or MuPDFLib.dll fail with missing references

The MSVC2019 projects have been set up to use a common *monolithic* DLL: `MuPDFLib.dll`.

When that one fails to build, (almost) all executables won't link/build either, as they depend on this DLL (which carries all our libraries' code).

Visual Studio is sometimes a little confused and hasn't properly updated the *export file* that goes with that DLL. 

## Remedy

Nuke the DEF file:

```
cd platform/win32
rm *.def
```

...  and rebuild.

You should see MSVC invoking the Python script `gen_libmupdf.dll.py` as part of (re)building the MuPDFLib project. This script scans the various library header files and extracts the API functions which should be exported in the DLL.

When this script fails to produce the proper set of exports, then LibMUPDF.dll won't be able to link (reporting missing/unknown references).


> **Extra**: as we're compiling a mixed bunch of C and C++ code, be very alert when inspecting such error reports:
> 
> `extern "C"` functions will report with an `_` underscore prefix, while C++ exports will report their *mangled* function name.

