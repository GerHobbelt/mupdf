# When you start with this stuff

Start with loading the Visual Studio `mupdf.sln` solution.

In the top toolbar, select the build target: 'Debug' and 'x64'. This tells MSVC you want a 'debug build' 64-bit set of binaries. These should work on all relatively modern hardware.

> You'll need to do some more work if you want to produce binaries for *old* hardware, which does not yet support the AVX2 processor commands!

Build by hitting Control+Shift+B.

That takes a long time; monitor progress in the bottom section of the IDE in the 'Output' tab.

### Tip

When the build produces weird errors, see if your issue sounds like any of the [[Known Issues when compiling,building]]. Also do [[DEVELOPERS-README#Developer FAQ|check our FAQ]].