#!/bin/bash

## Without starting from scratch, the relevant XCFLAGS arguments (disabling/enabling embedded fonts) are ignored
## A better solution may be possible
rm -rf ../../build

source /home/user/Documents/dev/emsdk/emsdk_env.sh

echo Building generated files:
make -j4 -C ../.. generate

## There are 2 versions of this build--
## 		One build with all major features enabled (only features disabled are 3rd-party dependencies [e.g. OCR] and misc fonts [e.g. emojis])
## 		One build with virtually all features disabled (basically only good for extracting text content from PDFs)
## The former build is ~5MB while the latter build is ~2.8MB.

## Note: The vast majority of these options do not have a meaningful impact on file size.
## Disabling fonts decreases the final .wasm significantly--from 5.0MB to 2.9MB. 
## All of the other options combined decrease the final .wasm file from 2.9MB to 2.8MB.

## Disable non-PDF formats
## FZ_ENABLE_XPS=0
## FZ_ENABLE_SVG=0
## FZ_ENABLE_CBZ=0
## FZ_ENABLE_IMG=0
## FZ_ENABLE_HTML=0
## FZ_ENABLE_EPUB=0

## Disable document writers
## FZ_ENABLE_OCR_OUTPUT=0
## FZ_ENABLE_DOCX_OUTPUT=0
## FZ_ENABLE_ODT_OUTPUT=0

## Disable ICC color profiles
## FZ_ENABLE_ICC=0

## Disable JavaScript
## FZ_ENABLE_JS=0

## Disable fonts
## TOFU
## TOFU_CJK

## The option NO_CJK reduces size considerably, but does so by eliminating cmap values, so may have unintended consequences.  This option is not currently included. 


echo Building library:
make -j4 -C ../.. \
	OS=wasm2 build=release \
	XCFLAGS="-DTOFU -DTOFU_CJK -DFZ_ENABLE_SVG=0 -DFZ_ENABLE_HTML=0 -DFZ_ENABLE_EPUB=0 -DFZ_ENABLE_JS=0 -DFZ_ENABLE_ICC=0 -DFZ_ENABLE_XPS=0 -DFZ_ENABLE_CBZ=0 -DFZ_ENABLE_IMG=0 -DFZ_ENABLE_OCR_OUTPUT=0 -DFZ_ENABLE_DOCX_OUTPUT=0 -DFZ_ENABLE_ODT_OUTPUT=0" \
	libs

echo
echo Linking WebAssembly:
emcc -Wall -Os -g1 -o libmupdf.js \
	-s WASM=1 \
	-s VERBOSE=0 \
	-s ASSERTIONS=1 \
	-s ABORTING_MALLOC=0 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s MAXIMUM_MEMORY=4GB \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
	-s EXPORTED_FUNCTIONS='["_malloc","_free"]' \
	-s FORCE_FILESYSTEM \
	-I ../../include \
	--pre-js wrap.js \
	wrap.c \
	../../build/wasm2/release/libmupdf.a \
	../../build/wasm2/release/libmupdf-third.a

sed -i 's/var FS = {/export var FS = {/g' libmupdf.js
sed -i 's/var Module = typeof/export var Module = typeof/g' libmupdf.js

# Edit so that paths work with Node.js
sed -i 's/function getBinarySync(file) {/function relToAbsPath(fileName) {\nconst url = new URL(fileName, import.meta.url);\n\treturn url.protocol == "file:" ? url.host + url.pathname : url.href;\n}\nwasmBinaryFile = relToAbsPath(".\/libmupdf.wasm");\nfunction getBinarySync(file) {/g' libmupdf.js

echo Done.
