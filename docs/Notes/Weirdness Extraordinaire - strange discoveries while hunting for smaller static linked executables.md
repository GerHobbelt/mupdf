# Weirdness Extraordinaire :: strange discoveries while hunting for smaller static linked executables

## When /LTCG (Link-Time Code Generation) actually fixed a weird not-really-a-cycle dependency

This was found while looking what we could / should do to make statically linked binaries **smaller**: we started with `wxw-samples-console` as that one *should*, at least in principle[^1], produce a rather small sort of "hello world" kind of binary.

Turns out that one clocked in at a whopping 50+MByte once the linker was done!

We added flags to the project to have the linker produce a detailed MAP file and it was **huge**. It included embedded fonts, a zillion wxWidgets GUI class members and a heck of a lot `fz_blahblahblah` function references, which is your MuPDF library being linked in for a (very) large, and unnecessary, part.

Usually this stuff happens when you've got some wicked "check and init all the supported libraries" startup code somewhere, but an initial scan through the 70K+ MAP references didn't reveal anything obviously suspicious -- of course, there's a 70K sized forest to go through, so chances are you'll miss it, but anyway: this called for a different approach.

AFAICT there are no tools or flags to tell (or second-guess) a linker to produce a full-fledged dependency map/dump, so you can traverse that dependency graph to find out **why in God's name** some embedded fonts made it into this executable, for example. The *hard* part is coming up with an obscure and insensible enough reference and then somehow (sweat & tears & manual searches, a.k.a. *effort* required) dig up where the naughty boy is located that caused that part of the bloat. 

With 50-odd Megabytes you can bet there'll be several naughty boys, but let's start with finding **one**, shall we?

After a few duds (too many dependents at some point, so a bad pick, try another...) we settled on `pdf_document_handler` because `wxw-samples-console` *definitely* wasn't going to allude to doing some PDF document I/O work.

First off, `pdf_document_handler` is a `struct`, a *global*, and (very) old bad experiences surfaced: we **do** have all projects set up to compile every *function* to its own, independent *segment*, so a linker *should* have an easy time to discard the useless ones, even if they shared a source file once. But *globals*, particularly ones with *function references* stored in them, sometimes have been nasty to me, so a quick hack is to wrap that one in its own little getter function (still a `static`, but now `pdf_document_handler` would be a variable *internal* to that getter function) and patching the codebase at the few spots where it is referenced to properly use the new getter function.

Result: no change. Linker still produces a humongous binary and my new `__get_pdf_document_handler` nicely features in the updated MAP file. Dang!

### A new little trick...

Manually going through the code, a.k.a. code review, for dependents, is no fun. We need a new trick.

What would the linker say if we simply **removed** that new getter function from the codebase entirely? A little `#if 0 ... [[endif]]` surrounding it and let's see who the link phase will report as *needing* this bugger.

Turns out `fz_open_accelerated_document_with_stream` is reported as the one needing it. Next, we `#if 0 ... [[endif]]`  that one too, and while we're at it and did dependent analysis just before, we also kill `fz_register_document_handlers` the same way.

Long-ish story short: turns out that once we arrived at `fz_new_xhtml_document_from_document` and nuked that one as well, together with the others along the way, the linker stopped complaining and delivered a fresh executable!

*WTF*?!?!

Yup, code inspection reveals that absolutely *nobody* is using `fz_new_xhtml_document_from_document` so that's okay, but **why** did the linker previously decide to include it in the first place?! And only (I didn't mention this before) once we edit the `wxw-samplees-console` demo code back to have at least *one* wxWidgets-related statement in there -- you get a clean executable when you do `printf("hello world\n")` but *Heaven forbid* if you use so much as a `wxPrintf` or `wxString` to accomplish the same feat -- while having all the old `wxw-samples-console` code still `#if 0 ... [[endif]]` disabled.

Is there some glitch that's helped along by a dependency cycle in the MuPDF library there? What about all the other stuff being included by the linker for no good reason what-so-ever?!

I couldn't find a dependency cycle there... `pdf_*` calls are picked up by generic `fz_*` calls and those *do not* wrap back into the `pdf_*` ones. At least I couldn't find none of that.

So it's time for waving the dead chicken and do some *voodoo*.

Back when I started with this, I had the MSVC project(s) set up as "Compile At Link Time" because I liked the idea of maximizing the optimization opportunities back then, but at some point I ran into trouble with MSVC2019-revision-something and back-pedaled to *old skool* "compile right now and let the linker have the easy job of snip & stitch *object file segments*". Meanwhile we're several MSVC2019 releases further down the road (MSVC2022 has come out already) so this might be something to see: does the *compiler stage* screw up as badly as the *linker stage* when fed a complex bunch of goods?

So I did something that I usually *do not do*: **only the `libmupdf` project has now been configured to Compile At Link Time** while I've kept all the others the same -- usually I go through all related projects and make sure their settings match *exactly*, an old habit learned from painful experience. But it's *voodoo time* so let's see what gives: it can't get worse than this anyway.

Lo and behold: MSVC trundles on and even pops a warning near the end telling me:

> libmupdf.lib(jmemcust.obj) : MSIL .netmodule or module compiled with /GL found; restarting link with /LTCG; add /LTCG to the link command line to improve linker performance

and then runs the sample application without a hitch! Wow! Quick! Let's check the MAP file!

The MAP file shows us that that entire set of nastiness has disappeared: no more `fz_new_xhtml_document_from_document`, `fz_open_accelerated_document`, `fz_open_document_with_stream`, `fz_open_accelerated_document_with_stream`[^2], etc.etc., `__get_pdf_document_handler`! Great!

Let's revert all the hackery, thanks to `git reset --hard` or your TortoiseGit equivalent, and let's see if the `/LTCG` mode for `libmupdf` delivers some cleanliness in `wxw-samples-console`. Hitting `[F5]` for build&run and prayer wheels start spinning...

... Yes!!1! Slashed 4MByte off the resulting binary! That sure isn't compiler peephole optimizations, but being the alternative for **linker oddities**. At least for `libmupdf`. Of course, `libtiff` and other image formats still feature heavily in the latest MAP file but heck, if the linker can't see through the wxWidgets forest than *those* obviously *are needed*.

So let's see if we can accomplish the same positive result for wxWidgets by having that monolithic project also do it the `/LTCG` way. Of course, that would be `wxWidgets.vcxproj` in our case, as that one is for the statically linked goodies. Like `wxw-samples-console` right now. Here's some "*Whole Program Optimization (/GL)*" (MSVC Compiler setting) for you, baby! 

> We flipped that bit for `wxScintilla` and `wxCharts` too for good measure.

> This has developer round-trip time consequences though: previously, the build process would max out my laptop's cores as multiple source files would be compiled in parallel.
> 
> That still happens, but with `/GL` this now has become a much *faster* task, so the core-maxing-out phase of the entire build is shorter, while the *single core linker phase* has much more work to do, thanks to `/LTCG` happening there. 
> 
> So you win some, you loose some (more)? I guess this means I won't be flipping the bit on all them projects...

Drat! That didn't change much. (The binary even got a wee little *bigger* this time.) Apparently wxWidgets is so intertwined (like MFC and all those other GUI frameworks out there) that this little fiddling doesn't help to get rid of useless material, once you start using any of it.

Guess that means I'll have to produce specially **carved subsets**[^3] in separate library projects if I'm serious about minimal binary size (for quickest possible start times!) -- the only application of the bunch that would benefit from this extra attention would be the Qiqqa Launcher/Monitor, I suppose. Alas.



## MSVC: *Shared Source Library* next

To make that happen, we finally have reason to use MSVC2019's "Shared Source" project type: let's see what we can accomplish when we drop the entire wxWidgets codebase into such a one and then tweak it with some judicious preprocessor `#define`s in the end project's configuration (`wxw_samples_console` in this case).




[^1]: because a wxWidgets *console* application would sensibly **not** contain any of the GUI code/classes at least, eh?

[^2]: Another wicked bit I ran into: `fz_open_accelerated_document_with_stream` didn't feature in the linker MAP file **at any time**, but `fz_open_document_with_stream` *did*. Huh, is that the compiler deciding it can be *inlined* anyway, even while at that moment it cannot tell that is the only spot it's used, as it's a global function and part of the published API. Odd...

[^3]: **carved subsets** would mean setting a different set of `#define`s for wxWidgets so as to discard any unneeded dependencies at pre-processor/compile-time, as I'm loath to let go of my monolithic build approach otherwise, as it was done for a reason.
