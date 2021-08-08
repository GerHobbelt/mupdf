# Fetching the repository and its submodules

```
git pull --all
git submodule update --init --recursive
```

OR:

This assumes you have the git assist scripts installed in your `$PATH` and set up the aliases accordingly:
- use `gpp -L` to grab any recent changes from github and elsewhere & push your latest changes. This is the faster version of `gpp -f` and only looks at *recently active git remotes*.
- Of course, you can use `git pull` and `git push --all` as well, but I tend to use those scripts as I work with repo's and forks where I like to monitor third-party activity as part of the pull/push process: `gpp -L` & friends help me answer the question: "has anybody else been busy lately?" too.
- `gpp -f` is for when you have time or wish to update *everything*: I start this in a separate `bash` console and just let it run while I continue working. `gpp -f` can take a long time if you have registered many remotes and/or have a deep submodule repository tree (such as my Qiqqa and MuPDF git trees).
