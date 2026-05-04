# When you want to clean up / remove local (tracking) branches in bulk

You want to (temporarily) remove a lot of local branches so your git commit tree becomes manageable again for, say, a long merge sequence from mainline, then here's the command to accomplish that:

```
git branch -a | grep  'master\|main' -v | sed -e 's/remotes\///' | xargs git branch -d -r
```

The above example selects all local branches (including all remote tracking branches!) which do not include the name `master` or `main` and then preps those (by removing their `remotes/` prefix) for `branch -d`.

The key bit that I could not find on the Net (StackOverflow et al) is the `-r` parameter: without it you only get git errors, no matter what you attempt!

The `-r` in `git branch -d -r`  is required to remove remote tracking branches as per [the git documentation](https://git-scm.com/docs/git-branch#_examples).

`