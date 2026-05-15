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

for file in "${files[@]}"; do
	rel="${file#"$ROOT"/}"
	printf '==> %s\n' "$rel"
	"$MUTOOL" draw -st "$file"
done
