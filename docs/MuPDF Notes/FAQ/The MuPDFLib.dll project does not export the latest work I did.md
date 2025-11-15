# The `MuPDFLib.dll` project doesn't export the latest work I did / `MuPDFLib.def` is out of date and does not update properly!

That's a hairy one to get right in Visual Studio -- without getting hit over the head with additional build actions and *build time* at odd moments.

*Theoretically*, Project Build Events (with file dependencies), *should* cater to our need of having a Makefile-like behaviour where the DEF file is only re-generated when one or more of the source files (and/or script generator itself!) get updated.

In Makefiles, this is tough to accomplish as well as the DEF file is dependent on a *lot* of source files, making for an *extremely wide* dependency tree then.

Visual Studio doesn't cope well with this scenario either, so we've decided to reduce the dependency set, risking using an *out of date* DEF file at times.


## The fix / the action

When you doubt the veracity of the DEF file, you can simply **delete** it, e.g.

```
cd platforms/win32/
rm *.def
```

It will be regenerated during the next build of the MuPDFLib project then.


