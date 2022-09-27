# Why do we have to build everything twice?

Some parts, such as the QuickJS compiler/interpreter, are (re)generating source files which must be compiled into the final product.

It turns out that MSVC2019 has a few dependency glitches or whatnot, which results in random build failures due to these dependencies not having been scheduled properly.

So far, MSVC2022 doesn't seem to suffer from this issue.


