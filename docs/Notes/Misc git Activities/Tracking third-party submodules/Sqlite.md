# SQLite: `thirdparty/owemdjee/sqlite`

Of course we track the master branch, but we also track the WAL2 development branch as we have included that feature in our own mainline.

Several other **experimental** tracking branches exist, but the important ones, that should all tag along (i.e. point to the same commit unless something important happens to their particular feature):

- master
- wal2
- database-list-enhancement

These branches are deprecated:

- wal1
- cli-extension

Also note that we use **[[zlib-ng]]** throughout, hence you will see our tracking branch differences-to-original-mainline include source code edits to migrate from vanilla zlib to zlib-ng (which we track in the `thirdparty/zlib` submodule, incidentally).

Another set of diffs that'll jump up when you diff the source tree against the original are thee 'const correctness' patches applied to all code that's driven from any `main(argc, argv)` in there: our '*monolithic build*' replaces those `main()` entry points with uniquely named functions, which can then be called by our monolithic application(s), e.g. `mutool`.



## Additional tasks when tracking = updating git

When you've updated the repo when tracking, you'll have to run

```
make amalgamation
```

again to regenerate the amalgamated source files.

Then diff the root directory against the `../sqlite-amalgamation`, transfer the changes and check them in in that repo as well: *that* is the SQLite repository we actually use to produce the SQLite library binary on our platforms!

