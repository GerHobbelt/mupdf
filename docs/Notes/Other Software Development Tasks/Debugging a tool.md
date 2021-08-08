# Debugging a tool

Pick the application by **Set as Active Project** in the IDE.

Hit F5 key to (build and) run in the debugger.

### Pro Tip: Release builds can be debugged...

... but reckon with the effects of program *optimizations*: your breakpoints, which you may have inherited from your Debug Build days, *may not work*: any inlined or otherwise severely optimized code will invalidate the relevant breakpoints, including the *inability* to set a breakpoint on any *call* to such inlined functions!

YMMV in Release Builds, therefor.

For a proper *debugging* experience, use Debug Builds. Okay, they are a tad slower, but you can't have it all!
