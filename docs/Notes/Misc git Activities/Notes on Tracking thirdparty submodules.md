# Notes on Tracking thirdparty submodules

### SQLite: `thirdparty/owemdjee/sqlite`

Of course we track the master branch, but we also track the WAL2 developnment branch as we have included that feature in our own mainline.

Several other **experimental** tracking branches exist, but the important ones, that should all tag along (i.e. point to the same commit unless something important happens to their particular feature):

- master
- wal2
- database-list-enhancement

These branches are deprecated:

- wal1
- cli-extension

Also note that we use **zlib-ng** throughout, hence you will see our tracking branch differences-to-original-mainline include source code edits to migrate from vanilla zlib to zlib-ng (which we track in the `thirdparty/zlib` submodule, incidentally).

Another set of diffs that'll jump up wheen you diff the source tree against the original are thee 'const correctness' patches applied to all code that's driven from any `main(argc, argv)` in there: our '*monolithic build*' replaces those `main()` entry points with uniquely named functions, which can then be called by our monolithic application(s), e.g. `mutool`.



#### Additional tasks wehn tracking = updating git

When you've updated the repo when tracking, you'll have to run

```
make amalgamation
```

again to regenerate the amalgamated source files.

Then diff the root direcotry against the `../sqlite-amalgamation`, transfer the changes and check them in in that repo as well: *that* iss the SQLite repository we actually use to produce the SQLite library binary on our platforms!



### unicode-icu : `thirdparty/owemdjee/unicode-icu`

While we track the original mainline, there's a *hitch*: the original repo includes a set of `*.jar` files which they update regularly. These files, however, are:

1. of no interest to us (as we're not interested in the Java section of `icu` anyway), and
2. these `.jar` files will, when merged into our own repo, trigger a GitHub error, requiring us to install and use `git LFS` (Large File Support), which is a hassle and not worth it.

Hence we've chosen to *deelete* those `.jar` files in our tracking master branch, which will often result in *merge conflicts* as we track the original mainline whenever they've chosen to update any of those files.

Simply resolve any `.jar` file merge conflicts by chosing to '*resolve by using HEAD*', i.e. by sticking with our own version: no `.jar` files *at all*.

> *Any other* merge conflicts should, of course, be inspected and judicisously mergeed where possible/applicable.



