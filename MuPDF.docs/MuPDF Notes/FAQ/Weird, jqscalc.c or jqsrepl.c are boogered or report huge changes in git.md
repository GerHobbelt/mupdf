# Weird: `jqscalc.c` and/or `jqsrepl.c` are boogered or report huge changes in git

One such observation is `jqscalc.c` reporting several 1000 (thousands!) of **removed lines** and only very few added, at the same time.

Or `jqsrepl.c` compiles fine, but running the repl test application shows it's completely b00gered.


## Remedy

MSVC2019 isn't always doing the right thing with build scripts.

Both `jqscalc.c` and `jqsrepl.c` are generated C files, containing the *compiled P-code for the corresponding QuickJS JavaScript scripts*! 

When `qjsc` is (re-)compiled, chances are that the P-codes have changed, if ever so slightly, and the JS sources should be *re*-compiled to ensure they match the `jqs` interpreter's expectations.

However, this doesn't always happen. You can **force recompilation** by nuking the `.c` files like this:

```
cd platform/win32
rm ../../scripts/libQuickJS/*.c
```

... and build the MSVC Solution once again.

You should now observe that `jqsc` is used to recompile the original JavaScript sources.

You should also observe that, while several thousand deleted lines are reported by git for the commit diff of `jqscalc.c`, the same commit diff should also report several *thousand* lines have been *added* at the same time, thus keeping `jqscalc.c` in the same size ballpark as before.


