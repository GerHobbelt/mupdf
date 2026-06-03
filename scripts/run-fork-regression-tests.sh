#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CORPUS_DIR="${1:-$ROOT/fork-regressions/data}"
MUTOOL="${MUTOOL:-$ROOT/build/debug/mutool}"

if [[ ! -x "$MUTOOL" ]]; then
	printf 'error: mutool not found or not executable: %s\n' "$MUTOOL" >&2
	printf 'hint: run `make build=debug build/debug/mutool`, or set MUTOOL=/path/to/mutool\n' >&2
	exit 1
fi

if [[ ! -d "$CORPUS_DIR" ]]; then
	printf 'error: corpus directory does not exist: %s\n' "$CORPUS_DIR" >&2
	exit 1
fi

files=()
while IFS= read -r -d '' file; do
	files+=("$file")
done < <(find "$CORPUS_DIR" -type f ! -name '.gitkeep' ! -name '.DS_Store' -print0 | sort -z)

if (( ${#files[@]} == 0 )); then
	printf 'No fork regression files found in %s\n' "$CORPUS_DIR"
	exit 0
fi

printf 'Running %d fork regression file(s) with %s\n' "${#files[@]}" "$MUTOOL"

# Smoke pass: every corpus file must extract without crashing/hanging. Several
# files are intentionally damaged PDFs (the repair fixes), so mutool legitimately
# reports recovery errors and a normal non-zero exit — that is tolerated. But a
# crash (segfault/abort/kill) is exactly what this smoke check exists to catch,
# so a signal exit (128+N) still fails the run. The goal is "does not crash",
# not "exit 0".
smoke_failed=0
for file in "${files[@]}"; do
	rel="${file#"$ROOT"/}"
	printf '==> %s\n' "$rel"
	status=0
	"$MUTOOL" draw -st "$file" || status=$?
	if (( status >= 128 )); then
		printf '   CRASH: mutool terminated by signal %d on %s\n' "$(( status - 128 ))" "$rel" >&2
		smoke_failed=1
	elif (( status != 0 )); then
		printf '   (mutool exited %d on damaged input — tolerated)\n' "$status"
	fi
done
if (( smoke_failed )); then
	printf 'error: one or more corpus files crashed during the smoke pass\n' >&2
	exit 1
fi

# Content assertion for the `use-glyph-name-for-unknown-unicode` option:
# OFF must still emit U+FFFD (so an unmapped text layer stays detectable),
# ON must recover readable text. NB: `mutool draw -O` is the spots option and
# does NOT parse stext flags, so this must go through `toStructuredText` via
# `mutool run`.
GLYPH_SAMPLE="$CORPUS_DIR/distiller-c-glyphs/sample.pdf"
if [[ -f "$GLYPH_SAMPLE" ]]; then
	printf '==> glyph-name recovery assertion (%s)\n' "${GLYPH_SAMPLE#"$ROOT"/}"
	script="$(mktemp -t glyphname.XXXXXX.js)"
	trap 'rm -f "$script"' EXIT
	cat > "$script" <<'JS'
var path = scriptArgs[0];
var page = Document.openDocument(path).loadPage(0);
var off = page.toStructuredText("preserve-whitespace").asText();
var on = page.toStructuredText("preserve-whitespace,use-glyph-name-for-unknown-unicode").asText();
var FFFD = String.fromCharCode(0xFFFD);
if (off.indexOf(FFFD) < 0)
	throw new Error("FAIL: option OFF should leave U+FFFD (unmapped layer must stay detectable)");
if (on.indexOf("Management of risks") < 0)
	throw new Error("FAIL: option ON should recover readable text");
if (on.indexOf(FFFD) >= 0)
	throw new Error("FAIL: option ON should resolve the U+FFFD runs");
print("OK: glyph-name recovery off=U+FFFD on=recovered");
JS
	"$MUTOOL" run "$script" "$GLYPH_SAMPLE"
fi
