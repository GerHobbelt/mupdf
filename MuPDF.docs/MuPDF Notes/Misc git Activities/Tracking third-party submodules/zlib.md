# zlib: `thirdparty/zlib`

Originally, MuPDF's `thirdparty/zlib` submodule tracked the original `zlib` repository and we continued to do so too, for a while.

However, we switched to tracking `zlib-ng` instead and the master branch reflects this.

The list of monitored remotes still includes the original `zlib` repo, but our focus is on `zlib-ng` and we do adapt any other submodule in this collection that's using old `zlib` to use `zlib-ng` instead: after all, the interface changes are *very minimal*: mostly those are changing any zlib API call to prefixing it with `zng` instead of merely `z`, so the adaptation cost is almost negligible, while we benefit from more bleeding edge development activity at `zlib-ng`, compared to `zlib`.

> For those that don't like living on the edge, `zlib` is one of the most stable source code libraries out there, but, while I may moan and complain at times, I *do like living on the edge* a little more than rock-steady conservativeness in software dev.

