# unicode-icu : `thirdparty/owemdjee/unicode-icu`

While we track the original mainline, there's a *hitch*: the original repo includes a set of `*.jar` files which they update regularly. These files, however, are:

1. of no interest to us (as we're not interested in the Java section of `icu` anyway), and
2. these `.jar` files will, when merged into our own repo, trigger a GitHub error, requiring us to install and use `git LFS` (Large File Support), which is a hassle and not worth it.

Hence we've chosen to *delete* those `.jar` files in our tracking master branch, which will often result in *merge conflicts* whenever they've chosen to update any of those files.

Simply resolve any `.jar` file merge conflicts by choosing to '*resolve by using HEAD*', i.e. by sticking with our own version: no `.jar` files *at all*.

> *Any other* merge conflicts should, of course, be inspected and judiciously merged where possible/applicable.


