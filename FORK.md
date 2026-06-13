# Fork notes

This is a fork of [ArtifexSoftware/mupdf](https://github.com/ArtifexSoftware/mupdf). It carries a small set of text-extraction and damaged-PDF robustness fixes that upstream does not currently ship.

The `fork` branch is based on the upstream tag **1.27.2** and contains a small set of extra commits.

## What's different from upstream

| File | Change |
|---|---|
| `source/fitz/encodings.c`, `include/mupdf/fitz/font.h` | New `fz_unicode_from_numeric_glyph_name` helper that decodes Distiller 3.x `C<n>` glyph names — exposed as an opt-in stext fallback, never applied unconditionally |
| `source/fitz/stext-device.c`, `include/mupdf/fitz/structured-text.h` | New `use-glyph-name-for-unknown-unicode` stext option (`FZ_STEXT_USE_GLYPH_NAME_FOR_UNKNOWN_UNICODE`) that applies the `C<n>` decode when a glyph's normal unicode lookup has already failed |
| `source/pdf/pdf-op-run.c` | (1) Guard against excessively deep acyclic Form XObject nesting in the page interpreter; (2) memoize marked-content MCID lookups so pathological tagged PDFs don't degrade to O(n²) per page |
| `source/pdf/pdf-repair.c` | (1) Bound the `endstream` scan so a corrupt stream with a missing terminator cannot swallow the objects that follow it; (2) rewind and resync — instead of aborting or skipping ahead — when a runaway dictionary/array parse consumes objects past its boundary |
| `source/pdf/pdf-page.c` | Tolerate dangling/non-page kids in the page tree, and rebuild the page map if a mid-walk repair drops it, so a truncated PDF still yields the pages it does have |
| `source/pdf/pdf-type3.c`, `source/fitz/font.c`, `include/mupdf/fitz/glyph-cache.h` | Deduplicate Type 3 `CharProcs`: load each distinct glyph stream once and share its display list across the many character codes an `/Encoding` maps onto it |
| `FORK.md` | This file |

## Why the glyph-name option exists

Older Acrobat Distiller (3.x) embeds CFF/Type 1C fonts whose glyph names are of the form `C<n>` (e.g. `C57`, `C98`, `C108`) — where `<n>` is the Unicode codepoint in decimal. The fonts have no ToUnicode CMap, so MuPDF's `fz_unicode_from_glyph_name` (`source/fitz/encodings.c`) falls through to `FZ_REPLACEMENT_CHARACTER` and text extraction returns `U+FFFD` for every glyph.

Both PDF.js (`src/core/fonts.js`) and Poppler (`poppler/GfxFont.cc parseNumericName`) handle this glyph naming convention. PyMuPDF works around it by enabling `FZ_STEXT_USE_CID_FOR_UNKNOWN_UNICODE` (an upstream MuPDF flag) — but for this corpus the CID/GID fallbacks are a no-op, because neither the character code nor the glyph index equals the codepoint; the only carrier of the codepoint is the glyph **name**.

Decoding `C<n>` is a heuristic guess: other producers use `C<n>` as an arbitrary glyph-index name, where the decode yields a wrong-but-plausible character (e.g. `±` → `6`) that is impossible to detect downstream. Applying it unconditionally would therefore silently corrupt those files and destroy the `U+FFFD` signal callers rely on to detect unmapped text and route to OCR.

So the decode is **opt-in**, mirroring MuPDF's own `use-cid-for-unknown-unicode` / `use-gid-for-unknown-unicode` mechanism:

- `fz_unicode_from_glyph_name` keeps upstream behaviour (no `C<n>` branch), so default extraction still emits a detectable `U+FFFD`.
- `fz_unicode_from_numeric_glyph_name` (`source/fitz/encodings.c`) decodes `C<n>` and is called by the stext device only when `FZ_STEXT_USE_GLYPH_NAME_FOR_UNKNOWN_UNICODE` is set and the normal unicode lookup has already returned `U+FFFD`. The glyph name is read back from the font via `fz_get_glyph_name`. It is checked before the CID and GID fallbacks (it is the most specific signal).

This lets a caller extract once with the option off to detect a genuinely unmapped text layer, then re-extract just those pages with the option on to recover born-digital text instead of OCRing — keeping both the recovery and the detection signal.

Affected corpus: older academic PDFs from Distiller 3.x (Elsevier, Wiley, etc., circa 2000–2003) with `AdvTimes*`, `AdvPi*`, `AdvP*` fonts.

Verify with `mutool run` and a small script file (NB: `mutool run` takes a
script path, not an inline `-e` snippet; and `mutool draw -O` is the *spots*
option and does **not** parse stext flags):

```sh
cat > /tmp/check.js <<'JS'
var p = Document.openDocument(scriptArgs[0]).loadPage(0);
print(p.toStructuredText("preserve-whitespace").asText().slice(0, 80));
print(p.toStructuredText("preserve-whitespace,use-glyph-name-for-unknown-unicode").asText().slice(0, 80));
JS
mutool run /tmp/check.js sample.pdf
```

The first line is `U+FFFD` runs; the second is readable English. The fork
regression runner (`make fork-regression-test`) asserts this automatically.

## WASM Form XObject nesting guard

MuPDF 1.27.2's PDF run processor detects cyclic Form XObject recursion, but it does not cap long acyclic Form XObject chains. A real pdfTeX 1.40.25 pdf contains figure XObjects with deeply nested transparency-group forms. Native MuPDF recovers with `exception stack overflow!` warnings and keeps rendering/extracting, but the WASM build can exhaust/corrupt the linear-memory stack and trap with `RuntimeError: memory access out of bounds`.

The local patch adds an explicit Form XObject nesting cap in `source/pdf/pdf-op-run.c:pdf_run_xobject`. Once the cap is reached, the interpreter warns and skips that nested XObject instead of recursing further. This matches MuPDF's existing behavior of tolerating bad or excessive page content where possible, and prevents a single page from killing the cached WASM instance.

## Marked-content MCID lookup memoization

`pdf_lookup_mcid_in_mcids` (`source/pdf/pdf-op-run.c`) resolves a marked-content `MCID` to its structure-tree element. Its O(1) fast path assumes the per-page `ParentTree` array is indexed by MCID; when that misses it falls back to a linear scan of every element, resolving an indirect object for every entry of each element's `/K` array.

Some producers (observed: Foxit PhantomPDF Printer 9.7.1) write **document-cumulative** MCID values into page content streams instead of the spec's per-page 0-based values. The MCID then always exceeds the per-page array length, so the fast path misses on *every* `BDC`/`EMC`/text operator. On a page with hundreds of marked-content operators and a structure element carrying a large aggregated `/K` array (e.g. an `/S /Link` element with thousands of entries), the recovery scan reruns per operator and the page degrades to O(operators × elements × K-length) — tens of seconds per page, affecting every code path that interprets the page (text extraction, rendering, OCR detection).

The local patch adds a lazily-built per-page index (`build_mcid_index` in `pdf-op-run.c`) mapping every integer MCID value to its structure element. The recovery path consults the index instead of rescanning, turning each lookup into O(1) amortized. It is a pure memoization of the existing recovery scan: first element in array order still wins, misses still return `NULL`, so extraction output is unchanged.

## Repair: bounded `endstream` scan

When MuPDF rebuilds the xref of a damaged PDF, `pdf_repair_obj` (`source/pdf/pdf-repair.c`) locates each stream's end. For a stream whose dictionary declares an explicit `/Length` it first checks whether `endstream` sits at that offset; if not, it falls back to scanning the file byte-by-byte for the next `endstream` keyword.

That fallback scan is unbounded. A corrupt or truncated stream whose `endstream` keyword is missing entirely (observed: truncated/zero-padded academic PDFs where a large image XObject's terminator was lost) makes the scan run on to the *next* stream's `endstream` — often hundreds of kilobytes later. Every `N G obj` definition in between is then consumed as stream body and never registered in the rebuilt xref. The page tree and most page objects vanish, and the whole document extracts as zero pages. Poppler recovers these files because its reconstruction does not let a stream hide subsequent objects.

The local patch bounds the fallback scan to `PDF_REPAIR_ENDSTREAM_SLACK` (2 KiB) bytes past the declared `/Length`. If `endstream` is not found within that window the declared `/Length` is trusted and the outer scan resyncs on the following objects. This only affects streams that are already malformed (a well-formed stream's `endstream` is found by the exact-offset check and never reaches the scan), so well-formed PDFs are unaffected.

## Repair: rewind on a runaway dictionary/array parse

`pdf_parse_dict` and `pdf_parse_array` (`source/pdf/pdf-parse.c`) do not stop at object boundaries — an unclosed `[` or `<<`, or an unterminated hex string, makes them consume tokens forward (across `N G obj` / `endobj` / stream bodies) until they find a terminator or hit EOF. During an xref rebuild that is destructive: the repair scan parses two kinds of dictionary, and either can run away on a damaged file:

- **A bare `<<`** (not preceded by `N G obj`), treated by the scan as a possible file trailer. Observed: a PDF formed by concatenating two documents where the seam falls inside the first trailer's `/ID` array, leaving an unterminated hex string. `pdf_parse_dict` chases the missing `>` for ~140 KB, swallowing the second document's catalog and page tree.
- **An object's own dictionary**, parsed by `pdf_repair_obj`. Observed: truncated academic PDFs (CNKI `ReaderEx`) whose final object — a `ToUnicode` CMap — is cut mid-array; the unclosed array runs to EOF, consuming every object after it, including the `/Root` catalog and `/Pages` tree.

In both cases upstream then either aborts the whole repair (`roots->len == 0` — no `/Root` seen yet) or `continue`s from wherever the failed parse stopped, skipping every object the runaway consumed. The document extracts as zero pages even though Poppler reconstructs it.

The local patch makes a failed/runaway dictionary parse **rewind and resync** instead:

- The object scan no longer gives up on `roots->len == 0` alone. As long as one object has been recovered the rebuilt xref is kept — `pdf_repair_trailer` later scans the recovered objects for a `/Type /Catalog`, so a `/Root` past the truncation point is not needed.
- When a bare-`<<` trailer parse fails, or advances the file pointer past `PDF_REPAIR_TRAILER_MAX` (16 KiB — far more than any real trailer), the scan rewinds to just past the `<<` and resyncs on the `N G obj` definitions that follow.
- When an object's dictionary fails to parse, `pdf_repair_obj` rewinds to the object's content start and returns, so the outer scan re-finds the swallowed objects rather than recording the runaway extent or aborting. The broken object is *not* recorded (signalled to the caller via a new `brokenp` out-param) — so in an incrementally-updated PDF a truncated trailing copy cannot overwrite an earlier valid definition of the same object number. This rewind/resync path is gated on `brokenp`: callers that pass `NULL` (the progressive/hint object readers in `pdf-xref.c`, which have no resync loop) keep the original behavior — rethrow a truncated object at EOF, swallow any other broken dictionary as empty — so they are never left stranded on the same bytes.

Repair only walks already-damaged files, and a well-formed dictionary parse never fails or runs long, so well-formed PDFs are unaffected. The result matches Poppler, which reconstructs all three observed files.

## Page tree: tolerate dangling kids and mid-walk repair

`pdf_load_page_tree_imp` / `pdf_load_page_tree_internal` (`source/pdf/pdf-page.c`) build the page map by walking `/Root/Pages`. Upstream, the walk throws `non-page object in page tree` the moment it meets a kid that is neither a `/Page` nor a `/Pages` node, which aborts the whole document. A truncated PDF routinely has page-tree kids that point at objects missing from the file (they resolve to `null`), so a document with, say, 7 of 24 pages still present extracts as zero pages even though the 7 are intact.

A second problem compounds it: resolving such a dangling kid can trigger document repair *in the middle of the walk*, and repair drops the partially-built page maps (`pdf_drop_page_tree_internal`) from under the running walk.

The local patch makes the page-tree walk tolerant:

- A kid that is neither a page nor a page-tree node is skipped with a warning instead of aborting the walk; `pdf_load_page_tree_internal` then shrinks `/Root/Pages/Count` to the number of pages actually found, so the count and the map agree.
- A kid that omits `/Type` is classified structurally (`/Kids` → internal node, `/MediaBox` → leaf page), matching the existing tolerance of the slow-lookup path.
- If a repair runs mid-walk and drops the maps, the walk detects the dropped maps, abandons cleanly (no NULL dereference), and `pdf_load_page_tree_internal` rebuilds from scratch — repair runs at most once, so the rebuild walks a stable, fully repaired tree.

The result matches native `mutool`'s and Poppler's behavior of extracting the pages a damaged document does have instead of failing the whole file.

## Type 3 font: deduplicate shared CharProcs

A Type 3 font defines each glyph as a PDF content stream listed in its
`/CharProcs` dictionary, and its `/Encoding /Differences` array maps character
codes to those glyphs by name. Nothing stops an `/Encoding` from mapping many
codes onto the *same* `CharProcs` stream.

`pdf_load_type3_font` (`source/pdf/pdf-type3.c`) loaded a glyph per *encoded
code*: it called `pdf_load_stream` once for every code, so a font that maps 200
codes onto one glyph allocated 200 identical `fz_buffer`s. `pdf_load_type3_glyphs`
then ran `fz_prepare_t3_glyph` once per code, building 200 identical display
lists. Every Type 3 `fz_font` is pinned for the document's lifetime
(`doc->type3_fonts`, for `fz_decouple_type3_font`), so none of that memory is
reclaimable until the document closes.

Some producers (observed: `pdftk-java` + iText output of scanned CJK documents)
emit thousands of such Type 3 fonts, each with a handful of real glyphs spread
over a wide encoding. The duplication multiplies memory ~50–100× — a 33 MB / 403
page document expanded the MuPDF heap past 3 GB and, in the WASM build, hit the
2 GB ceiling and failed extraction with `realloc failed`.

The local patch deduplicates by `CharProcs` stream object number:

- `pdf_load_type3_font` loads each distinct stream once and shares the
  `fz_buffer` (via `fz_keep_buffer`) for every other code mapped to it.
- `pdf_load_type3_glyphs` detects codes that share a `t3procs` buffer and aliases
  the already-prepared glyph — `fz_alias_t3_glyph` (new, `source/fitz/font.c`,
  declared in `include/mupdf/fitz/glyph-cache.h`) shares the display list (via
  `fz_keep_display_list`) and copies the device flags and glyph bbox.

Codes that resolve to the same `CharProcs` stream are by definition the same
glyph program, so the shared display list, flags and bbox are identical to what
per-code preparation produced; extraction and rendering output are unchanged.
Per-code advance widths (`t3widths`) are still loaded individually. The fix is
refcount-safe: `fz_drop_font` drops all 256 `t3procs`/`t3lists` entries, and the
shared buffers/display lists are released when their last reference goes.

## Building the WebAssembly module

The build infrastructure lives in `platform/wasm/`. See [`platform/wasm/BUILDING.md`](platform/wasm/BUILDING.md) for full details. Quick start:

```sh
# Install Emscripten 4.0.8 at /opt/emsdk (or set the EMSDK env var)
cd platform/wasm
npm install
bash tools/build.sh
```

Build artifacts land in `platform/wasm/dist/`:

- `mupdf-wasm.js` — the factory (an ES module; rename to `.mjs` if your bundler needs that extension)
- `mupdf-wasm.wasm` — the WebAssembly binary

The factory and the `.wasm` are a matched pair, both emitted together by `emcc`. **Do not mix files from different builds** — newer emcc versions minify wasm import module names, which causes a "import object field 'env' is not an Object" error if the two halves disagree.

### macOS build note

`tools/build.sh` calls `nproc`, which is Linux-only. On macOS, supply a shim before running:

```sh
mkdir -p ~/.local/bin && cat > ~/.local/bin/nproc <<'EOF'
#!/bin/sh
exec sysctl -n hw.ncpu
EOF
chmod +x ~/.local/bin/nproc
export PATH="$HOME/.local/bin:$PATH"
```

## Verifying the fix

A one-page Distiller 3.x sample is the canonical regression check. The option
must be **off by default** (detectable `U+FFFD`) and **recover when on**:

```js
import * as fs from "node:fs"

globalThis["$libmupdf_wasm_Module"] = {
  wasmBinary: fs.readFileSync("./platform/wasm/dist/mupdf-wasm.wasm"),
}
const mupdf = await import("./platform/wasm/dist/mupdf.js")
const doc = mupdf.Document.openDocument(
  fs.readFileSync("sample.pdf"),
  "application/pdf",
)
const page = doc.loadPage(0)
// Off: still U+FFFD runs (so the unmapped text layer stays detectable).
console.log(page.toStructuredText("preserve-whitespace").asText())
// On: readable English.
console.log(
  page.toStructuredText("preserve-whitespace,use-glyph-name-for-unknown-unicode").asText()
)
```

With the option on the output is readable English starting with the article body ("Management of risks, uncertainties and opportunities on projects…") and containing "International Journal of Project Management 19 (2001) 89±101". With the option off — and on a fully unpatched build with any options — the same page returns runs of `U+FFFD`.

## Keeping in sync with upstream

The `fork` branch contains the patches listed above plus this `FORK.md` on top of upstream tag `1.27.2`. To rebase onto a newer MuPDF release, cherry-pick the fork commits onto the new tag:

```sh
git remote add upstream https://github.com/ArtifexSoftware/mupdf
git fetch upstream --tags
git checkout -b fork-<new-tag> <new-tag>
git cherry-pick <fork commit shas...>
# then bump the version references in FORK.md to <new-tag> and amend
```

## Pushing to GitHub

This fork has not yet been pushed. When you do:

1. Create an empty `<you>/mupdf` repo on GitHub.
2. From this repo's root:
   ```sh
   git remote rename origin upstream
   git remote add origin git@github.com:<you>/mupdf.git
   git push -u origin fork
   ```
