#! /bin/bash


if [ -z "$1" ] ; then
	ARG="3"
else
	ARG="$1"
fi

if [ -z "$2" ] ; then
	FILTER=.
else
	FILTER=$2
fi


mknewproj() {
	libname=$( echo lib$1 | sed -E -e 's/^liblib/lib/' -e 's/-?lib$//i' )
	if ! test -f $1.vcxproj -o -f $libname.vcxproj ; then
		echo "mknewproj: $libname"
		cp -n libcpuid.vcxproj $libname.vcxproj
		node ./patch-vcxproj.js $libname.vcxproj
		./update-vcxproj.sh $libname.vcxproj
		node ./refill-vcxproj.js $libname.vcxproj
	fi
}

mknewAPPproj() {
	libname=$1
	if ! test -f $libname.vcxproj ; then
		echo "mknewAPPproj: $libname"
		cp -n mutool.vcxproj $libname.vcxproj
		node ./patch-vcxproj.js $libname.vcxproj
		./update-vcxproj.sh $libname.vcxproj
		node ./refill-vcxproj.js $libname.vcxproj
	fi
}

delnewproj() {
	libname=$1
	echo "delnewproj: $libname"
	rm ${libname}.vcxproj*   ${libname}_tests.vcxproj*   ${libname}_examples.vcxproj*   2> /dev/null
	libname=$( echo lib$1 | sed -e 's/^liblib/lib/' -e 's/-?lib$//i' )
	rm ${libname}.vcxproj*   ${libname}_tests.vcxproj*   ${libname}_examples.vcxproj*   2> /dev/null
}

delnewproj2() {
	libname=$1
	echo "delnewproj: $libname"
	rm ${libname}.vcxproj*   ${libname}_tests.vcxproj*   ${libname}_examples.vcxproj*   2> /dev/null
}


mylist=$(
grep -v '#' <<EOT

A-MNS_TemplateMatching
ApprovalTestsCpp
BBHash
BCF-cuckoo-index
BLAKE3
BlingFire
BoxFort
CHM-lib
CImg
CLBlast
CRFpp
CRFsuite-extended
CRoaring
CTCWordBeamSearch
CTPL-Thread-Pool
CacheLib
Criterion
CxImage
Cysboard
DBoW2
DCF-cuckoo-index
DGM-CRF
ECMA262
Extensible-Storage-Engine
GMM-HMM-kMeans
GMMreg
GoldFish-CBOR
HDiffPatch
JabRef-Browser-Extension
JabRef-abbreviations
JamSpell
LDCF-hash
LightGBM
LightLDA
Lightning.NET
MITIE-nlp
MNN
MariGold.OpenXHTML
NanoLog
NiuTrans.NMT
OfficeIMO
Open-XML-SDK
OpenBLAS
OpenCL-CTS
OpenCL-Headers
OpenCL-SDK
OpenFST
OpenFST-utils
OptimizationTemplateLibrary
PGM-index
PaddlePaddle
PhotonLibOS
QCBOR
QuickJS
QuickJS-C++-Wrapper
QuickJS-C++-Wrapper2
RxCpp
SQLiteCpp
SQLiteHistograms
ScriptX
Sealighter
ShapeCrawler
Signals
SilkETW
StarSpace
ThreadPool
TraceETW
VQMT
Verify
VisualScriptEngine
Win32_read_directory_changes
Win32_read_directory_changes_IOCP
WinHttpPAL
Windows10EtwEvents
XMP-Toolkit-SDK
YACLib
annoy
arangodb
argparse
arrayfire
asio
asyncplusplus
asynqro
##############b2xtranslator
basez
bhtsne--Barnes-Hut-t-SNE
bibtex-robust-decoder
bibtool
bibutils
binlog
bolt
boost
boost-url
breakpad
brotli
c-blosc2
caches
caffe
catboost
cctz
ccv-nnc
cef-pdf
cereal
ceres-solver
circlehash
citation-abbreviations
citation-journals
citation-styles
citeproc-js
civetweb
clBLAS
cld2-language-detect
cli11
clipp
cmph-hasher
comdb2-bdb
compact_enc_det
completesearch
concurrencpp
concurrentqueue
coost
cpp-btree
cpp-ipc
cpp_rest_sdk
cppflow
cppjieba
cpptoml
cppzmq
cpuinfo
cpython
createprocess-windows
crfsuite
crow
cryptopp
csv-parser
csync2
cuckoo-index
cuckoofilter
cxxtest
cxxtest_catch_2_gtest
date
datetimepp
dateutils
debugbreak
delegate
diffutils
dirent
djvulibre
dlib
docxBox
dtl-diff-template-library
dtoa-benchmark
dynet
ecal
efsw
emphf-hash
enkiTS-TaskScheduler
eventpp
exiv2
expected-lite
faiss
fastBPE
fastText
fast_float
fast_pfor
fatal
file
filesystem
flat_hash_map
fmt
fmtlog
folly
friso
frozen
gbenchmark
gdbm
gettext
gflags
gibbs-lda
glib2
glob
glog
google-diff-match-patch
google-marl
googletest
gperf-hash
graphit
gtn
gumbo-libxml
gumbo-query
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
horsejs
##############.NET########html2openxml
htmlstreamparser
http-parser
hunspell
hunspell-dictionaries
hunspell-hyphen
hyperscan
hypertextcpp
iceoryx
indicators
infoware
ion-c
ipa-dict
iresearch
jasper
jemalloc
jpeg-xl
jq
json
json-jansson
jsoncons
kahypar
kgraph
krabsETW
lapack
lda
lda-3-variants
lda-Familia
lda-bigartm
lerc
libCRCpp
libCZMQ
lib_nas_lockfile
libaco
libaom
libarchive
libassert
libbf
libbloom
libcbor
libchardet
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
libcyaml
libde265
libdeflate
libdi-dependency-injection
libdist
libeigen
libeternaltimestamp
libevt
libexpat
libffi
libfolia
libfort
libfyaml
libgd
libgif
libgrape-lite
libheif
libheif-alt
libicns
libiconv
libidn2
libimagequant
libjpeg-turbo
libjxl
liblinear
libmdbx
libmio
libmlpp
libmobi
libngt-ann
libocca
libpinyin
libpsl
libq
libqrencode
libquill
librs232
librsync
libscanf
libshmcache
libsigcplusplus
libsmile
libsptag
libsqlfs
libstb
libstemmer
libsvm
libtextcat
libtuv
libucl
libunifex
libuv
libvips
libvrb
libwarc
libwebp
libwebsocketpp
libwebsockets
libwil
libwildmatch
libxml2
libxslt
libyaml
libyaml-examples
libzip
libzmq
libzopfli
ligra-graph
line_detector
linenoise
lmdb
lmdb-safe
lmdb-store
lmdb.spreads.net
lmdbxx
localmemcache
lrucache11
lz4
mace
magic_enum
mammut
manticore-columnar
manticore-plugins
manticoresearch
many-stop-words
marian
math-atlas
mcmc
memory
merror
mesh-allocator
messagebox-windows
mht-rip
microsoft-performance-toolkit-sdk
midas
mimalloc
mime-mega
mimetic
mipp
mlpack
mmc
monolith
morton_filter
ms_cpp_client_telemetry
multiverso
mxnet
mydumper
mysql-connector-cpp
nameof
nanoflann
nanomsg-nng
nanosvg
nativefiledialog-extended
ncnn
neutralinoJS
neutralinoJS-CLI
nmslib
notcurses
npoi
nsync
nuspell
ocreval
oiio
oneTBB
onnxruntime
opencv
opencv_contrib
openpbs
openssl
opentelemetry-cpp
oppat
osquery
otl
pHash
pagerank
palanteer
palmtree
papis-zotero
parallel-hashmap
pcg-c-random
pcg-cpp-random
pcm
pcre
pdf2htmlEX
pdfgrep
pdfium
pelikan
percona-server
pevents
phash-gpl
phf-hash
photino.native
picoc
picohttpparser
pinyin
pipes
pisa
plf_nanotimer
pmt-png-tools
podofo
poppler
portable-memory-mapping
portable-snippets
portable_concurrency-std-future
preprocess-corpuses
prio_queue
probminhash
promise-cpp
promise-hpp
proxygen
pthread-win32
pthreadpool
pybind11
pyclustering
pytorch
pytorch_cpp_demo
qlever
randen
random
rapidJSON
rapidyaml
rclone
re2
recycle
refl-cpp
replxx
restc-cpp
result-cpp
resumable-assert
robin-hood-hashing
robin-map
rsync
safestringlib
salieri
scantailor
scintilla
sdhash
sentence-tokenizer
sentencepiece
shadesmar
sharedhashfile
shmdata
shoco
sioyek
smhasher
snap
snmalloc
snowball
sparsehash
spdlog
spdlog_setup
spy-build-sysinfo
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
sqlpp11
ssdeep
ssimulacra2
stan
stan-math
stateline
statsite
stdext-path
stopwords
stringi
stx-error-handling
subprocess
subprocess-cpp
subprocess_h
svg-charter
swig
tabulate
taolog
taskflow
tcp_pubsub
tcpshm
tensorflow
tensorflow-docs
tensorflow-io
tensorflow-text
tesseract-gImgRdrGui
tesslinesplit
textflowcpp
thread-pool
thread-pool-c
thread-pool-cpp
thunderSVM
ticpp
tidy-html5
tink
tinn
tiny-process-library
tinycbor
tinycolormap
tinyexpr
tinygettext
tlx
tlx-btree
toml11
tomlpp
tracelogging-for-ETW
transwarp
tre
tsf
tvision
txiki.js
typesense
uberlog
uchardet
ucto
uctodata
ucx
uint128_t
unicode-cldr
unicode-icu
universal-numbers
unpaper
upscaledb
upskirt
url
userver
utfcpp
variadic_table
velocypack
warc2text
warp-ctc
warpLDA
websocket-sharp
webview
wget
wil-Win32-Interface-Library
win32-dpi
winflexbison
word2vec
word2vec-GloVe
wxCharts
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
xpdf
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
you-token-to-me
yyjson
zfp-compressed-arrays
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
zpp_bits
zstd
zsv
zsync2
zxing-cpp

ssdeep
cr
RuntimeCompiledCPlusPlus
koan
iODBC
nanodbc
unixODBC
jerryscript
wxDatabase
sqliteodbc
Xoshiro-cpp
fastPRNG
libchaos

libevent
abseil-cpp
libdivsufsort
fluent-bit
bitsery
CryptSync
taglib
harbour-core
id3-tagparser
mmkv

EOT
)

myapplist=$(
grep -v '#' <<EOT

EtwExplorer
GraphicsMagick
ImageMagick
UIforETW
bebop
calibre
drogon
h2o-server
olena
sumatrapdf
gperf

EOT
)

# projects which use names different from their actual storage directory; see also update-cvxproj.js script's mapping table:

deletelist=$(
grep -v '#' <<EOT

cryptopp
math-atlas
c-blosc2
cpp-btree
CHM-lib
djvulibre
dtl-diff-template-library
enkiTS-TaskScheduler
lda-Familia
google-diff-match-patch
parallel-hashmap
HDiffPatch
google-marl
mime-mega
promise-cpp
QuickJS-C++-Wrapper
QuickJS-C++-Wrapper2
bibtex-robust-decoder
svg-charter
oneTBB
tomlpp
pytorch
uint128_t
upskirt-markdown
XMP-Toolkit-SDK
unicode-icu
json-jansson
cxxtest_catch_2_gtest
OptimizationTemplateLibrary
manticoresearch
uberlog
pmt-png-tools
libchm_io

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
fi

if [[ "$ARG" =~ [12] ]] ; then
	for f in  $deletelist  ; do
	  delnewproj $f
	done
	for f in  $deletelist2  ; do
	  delnewproj2 $f
	done

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
