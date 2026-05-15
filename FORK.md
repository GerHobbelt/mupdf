# Fork notes

This is a fork of [ArtifexSoftware/mupdf](https://github.com/ArtifexSoftware/mupdf). It exists to ship a small fix to MuPDF's glyph-name decoder that upstream does not currently carry.

The `fork` branch is based on the upstream tag **1.27.2** and contains a small set of extra commits.

## What's different from upstream

| File | Change |
|---|---|
| `source/fitz/encodings.c` | One extra branch in `fz_unicode_from_glyph_name` to handle Distiller 3.x `C<n>` glyph names |
| `source/pdf/pdf-op-run.c` | (1) Guard against excessively deep acyclic Form XObject nesting in the page interpreter; (2) memoize marked-content MCID lookups so pathological tagged PDFs don't degrade to O(n²) per page |
| `FORK.md` | This file |

That's the entire delta. The glyph-name patch is 5 added lines; see the commit `Recognise Distiller 3.x C<n> glyph names in fz_unicode_from_glyph_name` for full context.

## Why the patch exists

Older Acrobat Distiller (3.x) embeds CFF/Type 1C fonts whose glyph names are of the form `C<n>` (e.g. `C57`, `C98`, `C108`) — where `<n>` is the Unicode codepoint in decimal. The fonts have no ToUnicode CMap, so MuPDF's `fz_unicode_from_glyph_name` (`source/fitz/encodings.c`) falls through to `FZ_REPLACEMENT_CHARACTER` and text extraction returns `U+FFFD` for every glyph.

Both PDF.js (`src/core/fonts.js`) and Poppler (`poppler/GfxFont.cc parseNumericName`) handle this glyph naming convention. PyMuPDF works around it by enabling `FZ_STEXT_USE_CID_FOR_UNKNOWN_UNICODE` (an upstream MuPDF flag), but does not carry a C-source patch.

The patch adds a conservative `C<n>` branch alongside the existing `a<n>` one, before the `FZ_REPLACEMENT_CHARACTER` fallback. AGL lookup still runs first, so real AGL names like `C` aren't affected.

Affected corpus: older academic PDFs from Distiller 3.x (Elsevier, Wiley, etc., circa 2000–2003) with `AdvTimes*`, `AdvPi*`, `AdvP*` fonts.

## WASM Form XObject nesting guard

MuPDF 1.27.2's PDF run processor detects cyclic Form XObject recursion, but it does not cap long acyclic Form XObject chains. A real pdfTeX 1.40.25 pdf contains figure XObjects with deeply nested transparency-group forms. Native MuPDF recovers with `exception stack overflow!` warnings and keeps rendering/extracting, but the WASM build can exhaust/corrupt the linear-memory stack and trap with `RuntimeError: memory access out of bounds`.

The local patch adds an explicit Form XObject nesting cap in `source/pdf/pdf-op-run.c:pdf_run_xobject`. Once the cap is reached, the interpreter warns and skips that nested XObject instead of recursing further. This matches MuPDF's existing behavior of tolerating bad or excessive page content where possible, and prevents a single page from killing the cached WASM instance.

## Marked-content MCID lookup memoization

`pdf_lookup_mcid_in_mcids` (`source/pdf/pdf-op-run.c`) resolves a marked-content `MCID` to its structure-tree element. Its O(1) fast path assumes the per-page `ParentTree` array is indexed by MCID; when that misses it falls back to a linear scan of every element, resolving an indirect object for every entry of each element's `/K` array.

Some producers (observed: Foxit PhantomPDF Printer 9.7.1) write **document-cumulative** MCID values into page content streams instead of the spec's per-page 0-based values. The MCID then always exceeds the per-page array length, so the fast path misses on *every* `BDC`/`EMC`/text operator. On a page with hundreds of marked-content operators and a structure element carrying a large aggregated `/K` array (e.g. an `/S /Link` element with thousands of entries), the recovery scan reruns per operator and the page degrades to O(operators × elements × K-length) — tens of seconds per page, affecting every code path that interprets the page (text extraction, rendering, OCR detection).

The local patch adds a lazily-built per-page index (`build_mcid_index` in `pdf-op-run.c`) mapping every integer MCID value to its structure element. The recovery path consults the index instead of rescanning, turning each lookup into O(1) amortized. It is a pure memoization of the existing recovery scan: first element in array order still wins, misses still return `NULL`, so extraction output is unchanged.

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

A one-page Distiller 3.x sample is the canonical regression check. On the patched build:

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
const text = doc.loadPage(0).toStructuredText("preserve-whitespace").asText()
console.log(text)
```

The output should be readable English starting with the article body ("Management of risks, uncertainties and opportunities on projects…") and containing "International Journal of Project Management 19 (2001) 89±101". On an unpatched build the same page returns 3438 glyphs of `U+FFFD`.

## Keeping in sync with upstream

The `fork` branch contains the encodings patch plus this `FORK.md` on top of upstream tag `1.27.2`. To rebase onto a newer MuPDF release:

```sh
git remote add upstream https://github.com/ArtifexSoftware/mupdf
git fetch upstream --tags
git checkout -b fork-<new-tag> <new-tag>
git cherry-pick <encodings.c patch sha> <FORK.md sha>
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
