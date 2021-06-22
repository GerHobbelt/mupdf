
# For developers

Currently supported: MSVC 2019 (Microsoft Visual Studio 2019) :: `/platform/win32/mupdf.sln`


## Misc. tasks

- make sure node and python are in your `$PATH` as both are invoked in Build Steps in the various Visual Studio projects loaded by `mupdf.sln`

- use `single.sln` as a scratch solution when working on new libraries or specific parts of the collection when you do not want to load (and call) the entire bunch but only a very limited subset. Add & remove projects from this solution as you see fit: it will be in flux as development focus moves around...

- `CONTRIBUTORS.md` can be easily regenerated from git by using the `mk_git_contributors_doc.sh` shell script; that one is available in the repo: https://github.com/GerHobbelt/developer-utility-commands


