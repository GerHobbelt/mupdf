# Why was this set up the way it is?

Partly due to history, part intentional by design.

It all started with us picking `mupdf` as the library we were going to use to help us with *any and all* PDF work: page rendering, text and metadata extraction, annotation extraction and PDF(+annotations) (re)generation.

Besides, our main (highest priority) development platform is MSWindows (Win10/64) with Microsoft Developer Studio (was MSVC2019, now MSVC2022).

`mupdf` already had a decent project/solution organization, where all projects and their bundling MSVC solutions are located in `mupdf/platforms/win32/`: we liked that choice and stuck with it. Now that we have added various other libraries and tools (from the realm of C & C++ codebases), the number of `.vcxproj` files in there has grown significantly, but the same principles remain.

BTW: for Qiqqa (and other tools) wee make a distinction re programming language(s) used: this may sound counter-intuitive (organization by *technology* rather than the intuitively more sensible organization by *function/purpose*), but long experience has shown us that programming language boundaries are a real problem, despite stuff like P/Invoke (.NET), JNI, FFI, ...: our biggest portability and version migration hurdles have been severely impacted by mixed-programming-language executables (Qiqqa, for instance, requires several platform-specific and/or commercial libraries (as DLLs) **and trying to get rid of those is a major hassle**).

So separation-by-technology is useful that way:

1. it keeps us aware of these cross-language issues.
2. it makes it easier to visualize where the inter-process communication boundaries will be.

Yes, it is highly advised to have those various components, written in different languages, communicate using IPC rather than direct access + marshalling (P/Invoke, FFI, ...) as using, f.e., *sockets* is much more flexible and way less prone to build-/run-time failures when one component runs ahead of some others, development/maintenance-wise. Which has happened a zillion times in Open Source (and elsewhere). *The first half of the Qiqqa issue tracker is littered with the crap we have to deal with thanks to tightly bound components like that!*

As a corollary of this, we decided to collect all C & C++ work (which is very much involved with PDF processing or at least buzzing through that area anyway) in `mupdf/platforms/win32`, i.e. in the `mupdf` extended clone/fork. Later on I decided to move the SQL/database work in here too, as that relies on the SQLite codebase and given our new designs (BLAKE3 hash, Specialized 64-bit timestamp, etc.) it made sense to drop the .NET wrapper for that one and go with the original C source instead. 

We even considered a backend component where these functionalities (database management and PDF management) would've been merged, but I doubt that's going to happen as there's too much risk of memory leaks and other types of failure when processing PDFs incoming straight from The Wild Web: better to have a bit of separation between our critical (metadata) database and the component that's chewing through all the PDF crap we may feed it.

Meanwhile, all those components will be C/C++ based (with some *embedded scripting* assistance thrown in for adaptability) and hence must reside in `mupdf/platforms/win32` for MSVC/Win at least.


