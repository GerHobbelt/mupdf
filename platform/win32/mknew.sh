#! /bin/bash


if [ -z "$1" ] ; then
	ARG="3"
else
	ARG="$1"
fi

if [ -z "$2" ] ; then
	FILTER=.
else
	# WARNING: this is used as a regex, so reckon with special behaviour for $^.*+?[]
	#
	# The brutal fix we employ here is to nuke everything at or after the first regex operator
	#
	#echo "$2" | sed -E -e 's/[ \t\r\n.*+\[\]\(\)?].*$//'    -- somehow brafs on the ] in the [..] set no matter what I try for escapes   :-S
	FILTER=$( echo "$2" | sed -E -e 'sX[.*+?()\[].*$XX' )
fi


mknewproj() {
	libname=$( echo lib$1 | sed -E -e 's/^liblib/lib/' -e 's/-?lib$//i' )
	if ! test -f $1.vcxproj ; then
		if ! test -f $libname.vcxproj ; then
			echo "mknewproj: $libname"
			cp -n libcpuid.vcxproj $libname.vcxproj
			node ./patch-vcxproj.js $libname.vcxproj
			./update-vcxproj.sh $libname.vcxproj
			./refill-vcxproj.sh $libname.vcxproj
		fi
	fi
}

mknewAPPproj() {
	libname=$1
	if ! test -f $libname.vcxproj ; then
		echo "mknewAPPproj: $libname"
		cp -n mutool.vcxproj $libname.vcxproj
		node ./patch-vcxproj.js $libname.vcxproj
		./update-vcxproj.sh $libname.vcxproj
		./refill-vcxproj.sh $libname.vcxproj
	fi
}

mknewMISCproj() {
	libname=$1
	if ! test -f $libname.vcxproj ; then
		echo "mknewMISCproj: $libname"
		cp -n collection-0.vcxproj $libname.vcxproj
		node ./patch-vcxproj.js $libname.vcxproj
		./update-vcxproj.sh $libname.vcxproj
		./refill-vcxproj.sh $libname.vcxproj
	fi
}

delnewproj() {
	libname=$1
	echo "delnewproj: $libname"
	rm ${libname}.vcxproj*   ${libname}_tests.vcxproj*   ${libname}_examples.vcxproj*   ${libname}_demos.vcxproj*   2> /dev/null
	libname=$( echo lib$1 | sed -e 's/^liblib/lib/' -e 's/-?lib$//i' )
	rm ${libname}.vcxproj*   ${libname}_tests.vcxproj*   ${libname}_examples.vcxproj*   ${libname}_demos.vcxproj*   2> /dev/null
}

delnewproj2() {
	libname=$1
	echo "delnewproj: $libname"
	rm ${libname}.vcxproj*   ${libname}_tests.vcxproj*   ${libname}_examples.vcxproj*   2> /dev/null
}


# Anything that would, should or could (partly) serve as a library from our monolithic build perspective:

mylist=$(
grep -v '#' <<EOT

A-MNS_TemplateMatching
ADE-graph-management
ApprovalTestsCpp
BBHash
BCF-cuckoo-index
BLAKE3
BaseMatrixOps
Bi-Sent2Vec
BitFunnel
BlingFire
BoxFort
libCHM
CImg
CLBlast
CLBlast-database
CLTune
CRFpp
CRFsuite-extended
CRoaring
CTCWordBeamSearch
CTPL-Thread-Pool
CacheLib
Capture2Text
Clipper2
ColorSpace
Criterion
CryptSync
CxImage
Cysboard
DBoW2
DCF-cuckoo-index
DGM-CRF
ECMA262
Extensible-Storage-Engine
FFmpeg
FIt-SNE
GMM-HMM-kMeans
GMMreg
GoldFish-CBOR
HDiffPatch
ITK
Imath
Ipopt
JabRef-Browser-Extension
JabRef-abbreviations
JamSpell
LDCF-hash
LLhttp-parser
LSWMS
LightGBM
LightLDA
Lightning.NET
MITIE-nlp
MNN
MariGold.OpenXHTML
NSIS-OBSInstallerUtils
NSISDotNetChecker
NSISFileCheck
NSISMultiUser
NanoLog
NiuTrans.NMT
OfficeIMO
Open-XML-SDK
OpenBLAS
OpenCL-CTS
OpenCL-Headers
OpenCL-SDK
OpenColorIO
OpenEXR
OpenFST
OpenFST-utils
OpenImageIO

PGM-index
PaddlePaddle
PhotonLibOS
Pillow
PlatformFolders
QCBOR
QuickJS
QuickJS-C++-Wrapper
QuickJS-C++-Wrapper2
RE-flex
RapidFuzz
RuntimeCompiledCPlusPlus
RxCpp
SQLiteCpp
SQLiteHistograms
ScriptX
Sealighter
ShapeCrawler
Sherwood
Signals
SilkETW
Sophus
StarSpace
SymSpell
TelemetrySourcerer
ThreadPool
TraceETW
URI-Encode-C
URL-Detector
VQMT
VSNASM
Verify
VisualScriptEngine
Win32_read_directory_changes
Win32_read_directory_changes_IOCP
WinHttpPAL
Windows10EtwEvents
Xoshiro-cpp
YACLib
ZLMediaKit
abseil-cpp
adaptiveqf
annoy
arangodb
argparse
armadillo
arrayfire
asio
asyncplusplus
asynqro
autodiff
b2xtranslator
basez
bhtsne--Barnes-Hut-t-SNE
bibtool
bibutils
binlog
bitsery
blaze
bloom
bolt
boost
boost-url
boringssl
breakpad
brotli
libblosc
caches
caffe
cairo
cairo-demos
capture-thread
catboost
cctz
ccv-nnc
cdc-file-transfer
cereal
ceres-solver
circlehash
civetweb
clBLAS
cld1-language-detect
cld2-language-detect
cld3-language-detect
cli11
clipp
cmph-hasher
color-util
colormap-shaders
comdb2-bdb
compact_enc_det
completesearch
concurrencpp
concurrentqueue
coost

cpp-httplib
cpp-ipc
cpp-netlib
cpp-terminal
cpp_rest_sdk
cppdap
cppflow
cppjieba
cpptoml
cppzmq
cpuinfo
cpython
cqf
cr
crc32c
createprocess-windows
crfsuite
crow

csv-parser
csvquote
csync2
ctsa
cuckoo-index
cuckoofilter
curl
curl-impersonate
cxxopts
cxxtest
datamash
date
datetimepp
dateutils
debugbreak
decision-tree
delegate
density
densityxx
diffutils
dirent
###### djvulibre -- already exists as libdjvu
dlfcn-win32
dlib
docxBox
doh
doxa
dtl-diff-template-library
dtoa-benchmark
dynet
easyexif
easyloggingpp
ecal
edit-distance
efsw
emphf-hash
enca
ensmallen
eventpp
exiv2
expected-lite
exprtk
extract
eytzinger
faiss
farmhash
farver-OKlab
fast-lzma2
fastBPE
fastPRNG
fastText
fast_float
fast_pfor
fastapprox
fastfilter_cpp
fastgron
fatal
faup
fftw3
fifo_map
file
filesystem
flat.hpp
flat_hash_map
flip
fluent-bit
fmem
fmemopen_windows
fmt
fmtlog
folly
fontconfig
freeglut
freetype
fribidi
friso
frozen
g3log
gbenchmark
gcem
gdbm
getopt
gettext
gflags
gibbs-lda
glib2
glob
glog
gmic
gmic-community
gmt
google-diff-match-patch
googletest
graph-coloring
graphit
groonga
gtn
guetzli
gumbo-libxml
gumbo-parser
gumbo-query
h5cpp
harbour-core
harfbuzz
hedley
highway
highwayhash
hikyuu
hmm-scalable
hmm-stoch
hnswlib
hocr-fileformat
hocr-spec
hocr-tools
honggfuzz
hopscotch-map
hsluv-c
##############.NET######## html2openxml
htmlstreamparser
http-parser
hunspell
hunspell-hyphen
hyperscan
hypertextcpp
iODBC
iceberghashtable
iceoryx
id3-tagparser
ifopt
imagedistance
indicators
infoware
ion-c
iresearch
itpp
ivf-hnsw
jasper
jbig2dec
jemalloc
jerryscript
jpeg
jpeg-xl
jpeginfo
jq
json
jsoncons
jtc
kahypar
kalman-cpp
kfr
kgraph
klib
koan
krabsETW
lapack
lcms2
lda
lda-3-variants
lda-bigartm
leptonica
lerc
lexbor
libCRCpp
libCZMQ
########.NET####### libWebP.Net
lib_nas_lockfile
libaco
libalg
libaom
libarchive
libass
libassert
libavif
libbf
libbloom
libbloomfilters
libboost-context
libboost-coroutine
libboost-fiber
libboost-filesystem
libboost-iostreams
libboost-locale
libboost-log
libboost-math
libboost-mpi
libboost-mpl
libboost-program_options
libboost-serialization
libboost-stacktrace
libboost-thread
libboost-wave
libboost_examples
libboost_tools
libbtree
libcbor
libchaos
libchardet
libclip
libclipboard
libcmime
libcnl
libconfig
libcopp
libcppjieba
libcpr
libcpuid
libcsp
libcsv2
libcuckoo
libcyaml
libde265
libdeflate
libdi-dependency-injection
libdiagnostics
libdip
libdist
libdivide
libdivsufsort
libdjvu
libdtm
libeigen
libeternaltimestamp
libevent
libevt
libexpat
libfann
libffi
libfolia
libfort
libfyaml
libgateY
libgd
libgif
libgrape-lite
libharry
libheif
libheif-alt
libicns
libiconv
libics
libidn2
libimagequant
libinsane
libjpeg-turbo
libjxl
liblinear
libmdbx
libmetalink
libmio
libmlpp
libmobi
libngt-ann
libnop
libocca
libpillowfight
libpinyin
libpmemobj-cpp
libpng
libpopcnt
libprecog
libpsd
libpsl
libq
libqrencode
libraqm
librs232
librsync
libscanf
libshmcache
libsigcplusplus
libsiridb
libsl3
libsmile
libsptag
libsql
libsqlfs
libstb
libstemmer
libsvm
libtextcat
libtiff
libtuv
libucl
libunifex
libusb
libuv
libvcopy
libvips
libvrb
libwarc
libwebp
libwebp
libwebsocketpp
libwebsockets
libwil
libwildmatch
libxcam
libxml2
libxo
libxslt
libyaml
libyaml-examples
libzip
libzmq
libzopfli
ligra-graph
line_detector
linenoise
lizard
lmdb
lmdb-safe
lmdb-store
lmdb.spreads.net
lmdbxx
lnav
localmemcache
loguru
lol
lolremez
lrucache11
lz4
lzham_codec
mace
magic_enum
magsac
mammut
manticore-columnar
manticore-plugins
marian
mathtoolbox
matplotplusplus
mcmc
memory
merror
mesh-allocator
messagebox-windows
mht-rip
microsoft-performance-toolkit-sdk
midas
mimalloc
mimetic
minifb
mipp
mlpack
mmc
mmkv
monolith
morton_filter
mozjpeg
ms_cpp_client_telemetry
mujs
multiverso
mxnet
mydumper
mysql-connector-cpp
n2-kNN
nameof
nanodbc
nanoflann
nanomsg-nng
nanosvg
nativefiledialog-extended
ncnn
netpbm
neutralinoJS
neutralinoJS-CLI
nghttp3
ngtcp2
nmslib
notcurses
npoi
nsis-nscurl
nsis-stdutils
nsync
nuspell
ocreval
oidn-OpenImageDenoise
online-hnsw
onnxruntime
opencv
opencv_contrib
opencv_extra
openjpeg
openpbs
openssl
opentelemetry-cpp
openvino
oppat
optim
osqp
osqp-cpp
osqp-eigen
osquery
otl
pHash
pagerank
palanteer
palmtree
pango
papis-zotero
pcg-c-random
pcg-cpp-random
pcm
pcre
pdf2htmlEX
pdfgrep
pdiff
pecos
pelikan
pevents
phash-gpl
phf-hash
photino.native
picoc
picohttpparser
pinyin
pipes
pisa
pixman
plf_nanotimer
pmdk
pmdk-tests
pmemkv
pmemkv-bench
podofo
poisson_blend
polatory
portable-memory-mapping
portable-snippets
portable_concurrency-std-future
prio_queue
probminhash
promise-hpp
protobuf
proxygen
prvhash
psd_sdk
pthread-win32
pthread-win32-EH
pthread-win32-JMP
pthread-win32-jumbo
pthreadpool
pybind11
pyclustering
pylene
pytorch_cpp_demo
qlever
qs_parse
quill-logging
randen
random
random-forest
rapidJSON
rapidyaml
rclone
re2
reckless
recycle
refl-cpp
replxx
restc-cpp
result-cpp
resumable-assert
robin-hood-hashing
robin-map
rotate_detection
rsync
safestringlib
salieri
scantailor
scintilla
scws-chinese-word-segmentation
sdhash
sdsl-lite
semimap
sent2vec
sentence-tokenizer
sentencepiece
sequential-line-search
shadesmar
sharedhashfile
shmdata
shoco
sile-typesetter
simd-imgproc
simdjson
sioyek
slot_map
smhasher
snap
snappy
snmalloc
snowball
sparsehash
spdlog
spdlog_setup
spdlog_sqlite_sink
spline
splinter
splitmerge
spookyhash
spy-build-sysinfo
sqawk
sqlcipher
sqlean
sqleet
sqlite
sqlite-amalgamation
sqlite-fts5-snowball
sqlite-stats
sqlite3-compression-encryption-vfs
sqlite3pp
sqlite_fts_tokenizer_chinese_simple
sqlite_wrapper
sqlite_zstd_vfs
sqliteodbc
sqlpp11
squash
ssdeep
ssimulacra2
stan
stan-math
stateline
stats
statsite
stdext-path
stringi
strtk
stx-error-handling
subprocess
subprocess-cpp
subprocess_h
sumatrapdf
swig
tabulate
taglib
taolog
taskflow
tcp_pubsub
tcpshm
telegram-bot-api
telegram-td
tensorflow
tensorflow-docs
tensorflow-io
tensorflow-text
tensorstore
termcolor
tesseract
tesseract-gImgRdrGui
tesslinesplit
textflowcpp
thread-pool
thread-pool-c
thread-pool-cpp
thrift
thunderSVM
ticpp
libtidyhtml
tink
tinn
tiny-process-library
tinycbor
tinycolormap
tinydir
tinyexpr
tinygettext
tlx
tlx-btree
tmap
toml11
tracelogging-for-ETW
transwarp
tre
treelite
tsf
tvision
twain_library
txiki.js
typesense
uchardet
ucto
uctodata
ucx
ugrep
unblending
unicode-cldr
universal-numbers
unixODBC
unpaper
unqlite
upscaledb
upskirt
url
url-parser
userver
utfcpp
variadic_table
velocypack
vmem
vmemcache
vtm
vxl
waifu2x-ncnn-vulkan
warc2text
warp-ctc
warpLDA
websocket-sharp
webview
wget
wget2
wil-Win32-Interface-Library
win-iconv
win32-dpi
winflexbison
word2vec
word2vec-GloVe
wxCharts
wxCurl
wxDatabase
wxExamples
wxFormBuilder
wxMEdit
wxPDFView
wxPdfDocument
wxSQLite3
wxVisualScriptEngine
wxWebViewChromium
wxWidgets
wyhash
xgboost
xlnt
xml-pugixml
xnnpack
xor-and-binary-fuse-filter
xpdf
xsg
xsimd
xsldbg
xtensor
xtensor-blas
xtensor-io
xtl
xxHash
yaml-cpp
yaml-test-suite
yara-pattern-matcher
yasl
yggdrasil-decision-forests
you-token-to-me
yyjson
zfp-compressed-arrays
zlibxtensor
zpp_bits
zstd
zsv
zsync2
zxing-cpp

ArborX
CppNumericalSolvers
RNGSobol
RRD
RRPN
cvmatio
dbscan
figtree
fxt
genieclust
glfw
gram_savitzky_golay
hdbscan
hdbscan-cpp
libchopshop
mecab
mitlm
nlopt
numero
paramonte
pg_similarity
pisa_formatter
radon-tf
rgf
scilab
somoclu

minizip-ng
IconFontCppHeaders
blitsort
quadsort
fluxsort
crumsort
binary_search
rotate
sist2
libunibreak
SipHash
reproc
vmaf 
openlibm

PEGTL
doctest
fpng
jsoncpp
libparameters
mosquitto
qoi
safeclib
tao-json
u8_to_std_string
vizzu

FastGlobbing
GQ-gumbo-css-selectors
GSL
avro
cachelot
chibi-scheme
chromium-snss-parse
guile
itcl
jimtcl
libhog
libparameters
lru_cache
ngrams-weighted
opentelemetry-cpp-contrib
poco
sqlplot-tools
stlcache
tcl
tclap
tclclockmod
glyph_name
libgnurx
duckdb
newlisp

AFLplusplus
PaddleClas
PaddleDetection
PaddleNLP
PaddleOCR
Paddle-Lite
VTK
agg
cfgpath
gegl
gencolormap
gnuplot
gnuplot-palettes
gnuplot_i
leptonica-hillshade
libgnurx
libmodbus
libpathutils
libpipeline
libserialport
linecook
modbus-esp8266
nano-signal-slot
newlisp
owl
pikchr
radamsa
sbcl
tee-win32
tray
utf8proc
wcwidth9

cryptest
cryptlib
gperf
libCHM
libXMP-Toolkit
libatlas
libblosc
libbtree
libcxxtest_catch2_2_gtest
libenkiTaskScheduler
libfamilia
libhashmapbtree
libicu
libicu_tests
libicu_tools
libjansson
libmarl
libmarl_tests
libmegamime
libpromise
librobust-bibtex
libsvgcharter
libtbb
libtoml
libtorch
libuint128
libuint128_tests
libupskirt
manticore
otl-ml
png-pmt-tools-lib
uberlogger

mrpt
pyglass
DiskANN
flann
falconn
ffht
flinng
lshbox
liblbfgs
CurvatureFilter
aho_corasick
aho-corasick
libahocorasick
libirwls
kiwi

detect-character-encoding
EigenRand
rapidxml
LBFGS-Lite
uno-solver
MicroPather
spdmon
miniscript
libkra
cjose
liboauth2
fnmatch
text-wrangler

EOT
)

# Anything that will end up as an application, even when regarded from our monolithic buil perspective

myapplist=$(
grep -v '#' <<EOT

pikchr
capnproto
loki
netdata
tempo
EtwExplorer
FreeFileSync
GraphicsMagick
IdGenerator
ImageMagick
UIforETW
bbe
bebop
calibre
cpplocate
ex-leptonica-freetype-harfbuzz
filecopyex3
fswatch
grok-jpeg2000
horsejs
jbig2enc
lzbench
metalink-cli
metalink-mini-downloader
nsis
ogdf
olena
omaha
p7zip
qpdf
siridb-server
sumatrapdf
thrift
vcopy
visible-url-detector

fontview
vcpkg

tidyhtml
gnuplot
pikchr
radamsa
tray

cryptest
gperf
manticore
png-pmt-tools
text-wrangler

EOT
)

# Anything else, that's not really code we'll be incorporating in the form of a library or application. Data, documentation, etc.:

misclist=$(
grep -v '#' <<EOT

calibre
curl-www
everything-curl
hunspell-dictionaries
ipa-dict
podofo
pyclustering
tensorflow-docs

wordfreq
wordfrequency

archive-hocr-tools

LeptonicaDocsSite
UnofficialLeptDocs

libprecog-data
libprecog-manuals

mupdf_explored

langdata_LSTM
tessconfigs
tessdata
tessdata_best
tessdata_contrib
tessdata_fast
tessdoc
tesseract_api_docs
tesseract_docs
tesseract_langdata
tesseract_ocr_test
tesstrain
unicode-cldr-data
unicode-icu-data
unicode-icu-demos

citation-abbreviations
citation-journals
citation-styles
citeproc-js
hunspell-dictionaries
ipa-dict
many-stop-words
preprocess-corpuses
stopwords

doxygen-awesome-css

javascript-serialization-benchmark

1D-RGB-color-gradient
2D-color-gradient-or-Procedural-texture
awesome-c
hsluv-color-gradient

EOT
)

# projects which use names different from their actual storage directory; see also update-cvxproj.js script's mapping table:

deletelist=$(
grep -v '#' <<EOT

pdfium
poppler
CHM-lib
OptimizationTemplateLibrary
XMP-Toolkit-SDK
bibtex-robust-decoder
cef-pdf
c-blosc2
cpp-btree
cryptopp
cxxtest_catch_2_gtest
drogon
enkiTS-TaskScheduler
google-marl
gperf-hash
h2o-server
json-jansson
lda-Familia
libQuickJSpp
libQuickJSpp2
libchm_io
libdjvu_io
libdjvulibre
libdtldiff
libgdiff
libhdiff
liboiio
libquill
tidy-html5
manticoresearch
math-atlas
mime-mega
oneTBB
parallel-hashmap
pmt-png-tools
promise-cpp
pytorch
svg-charter
tomlpp
uberlog
uint128_t
unicode-icu
upskirt-markdown
zotero
zotero-better-bibtex
zotero-bib
zotero-build
zotero-connectors
zotero-google-docs-integration
zotero-libreoffice-integration
zotero-scholar-citations
zotero-shortdoi
zotero-standalone-build
zotero-translate
zotero-translation-server
zotero-translators
zotero-web-library
zotero-word-for-windows-integration
zotero-zotfile

EOT
)

deletelist2=$(
grep -v '#' <<EOT

libYACLib
libnmslib
libhnswlib
libsafestringlib
libCacheLib

EOT
)


if false ; then
	echo "--------------------------------------------------"
	echo "mylist=$mylist"
	echo "--------------------------------------------------"
	echo "myapplist=$myapplist"
	echo "--------------------------------------------------"
	echo "misclist=$misclist"
	echo "--------------------------------------------------"
	echo "deletelist=$deletelist"
	echo "--------------------------------------------------"
	echo "deletelist=$deletelist2"
	echo "--------------------------------------------------"
fi


# debugging:
if false ; then
	echo "FILTER: $FILTER"
	echo "ARG: $ARG"
fi


if [[ "$ARG" =~ [1] ]] ; then
	for f in  $mylist  ; do
		if [[ $f =~ $FILTER ]] ; then
			mknewproj $f
		fi
	done
fi

if [[ "$ARG" =~ [2] ]] ; then
	for f in  $myapplist  ; do
		if [[ $f =~ $FILTER ]] ; then
			mknewAPPproj $f
		fi
	done

	for f in  $misclist  ; do
		if [[ $f =~ $FILTER ]] ; then
			mknewMISCproj $f
		fi
	done
fi

if [[ "$ARG" =~ [12] ]] ; then
	# and only now do we add all those generated project files to the overview solution!
	# we do this brute-force by simply adding ALL projects to that solution again; the next
	# load by Visual Studio will clean up the .sln file for us.
	echo "augment MSVC solution 'm-dev-list.sln' by adding all known projects..."
	(
		for f in *.vcxproj ; do
			if [[ $f =~ $FILTER ]] ; then
				node ./mk_project_line_for_sln.js $f
			fi
		done
	) >> m-dev-list.sln
fi

if [[ "$ARG" =~ [3] ]] ; then
	for f in *.vcxproj ; do
		if [[ $f =~ $FILTER ]] ; then
			echo "Updating $f..."
			./update-vcxproj.sh $f
		fi
	done
fi

if [[ "$ARG" =~ [4] ]] ; then
	for f in *.vcxproj ; do
		if [[ $f =~ $FILTER ]] ; then
			echo "Refilling $f..."
			./refill-vcxproj.sh $f
		fi
	done
fi

if [[ "$ARG" =~ [0] ]] ; then
	for f in  $deletelist  ; do
	  delnewproj $f
	done
	for f in  $deletelist2  ; do
	  delnewproj2 $f
	done
fi

