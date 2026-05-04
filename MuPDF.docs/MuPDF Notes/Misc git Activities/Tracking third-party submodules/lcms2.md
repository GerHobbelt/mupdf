# lcms2

There's only one thing to watch for: *we* are based off a fork called `lcms2mt` (lcms2 for multithreaded applications) and to make the entire API thread-safe, the *thread context* is schlepped around everywhere as the first argument in (almost) every API: `cmsContext ContextID`.

When you merge in changes from `lcms2` mainline, you'll get collisions (merge conflicts), which are often easiest resolved by picking the incoming alternative and then re-adding "`cmsContext ContextID, `" or "`ContextID, `" as necessary. 

When in doubt, pull the merged code through the compiler and the missing `ContextID` parameters will quickly show up as fatal compiler errors. Simple to fix.
