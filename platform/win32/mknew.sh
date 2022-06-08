#! /bin/bash

mknewproj() {
  echo "mknewproj: $1"
  libname=$( echo lib$1 | sed -e 's/^liblib/lib/' -e 's/lib$//i' )
  cp -n libcpuid.vcxproj $libname.vcxproj
  node ./patch-vcxproj.js $libname.vcxproj
  node ./refill-vcxproj.js $libname.vcxproj
  ( node ./mk_project_line_for_sln.js $libname.vcxproj ) >> m-dev-list.sln
}

mknewAPPproj() {
  echo "mknewAPPproj: $1"
  libname=$1
  cp -n mutool.vcxproj $libname.vcxproj
  node ./patch-vcxproj.js $libname.vcxproj
  node ./refill-vcxproj.js $libname.vcxproj
  ( node ./mk_project_line_for_sln.js $libname.vcxproj ) >> m-dev-list.sln
}



mylist=$( 
cat <<EOT

fast_pfor
multiverso
lda-3-variants
gibbs-lda
portable-memory-mapping
libmio
libvrb
fmtlog
libcnl
libucl
libquill
NanoLog
libwil
libzopfli
libwildmatch
libsqlfs
sqlean
sqlite-stats
SQLiteHistograms
sqlite-fts5-snowball
sqlite_wrapper
velocypack
libdeflate
libCRCpp
ms_cpp_client_telemetry
krabsETW
tracelogging-for-ETW
typesense
Extensible-Storage-Engine
pthreadpool
lrucache11
CTPL-Thread-Pool
ThreadPool
portable_concurrency-std-future
YACLib
promise-hpp
taolog
opentelemetry-cpp
TraceETW
Sealighter
Windows10EtwEvents
SilkETW
microsoft-performance-toolkit-sdk
pelikan
oppat
compact_enc_det
tink
highwayhash
kgraph
nanoflann
hnswlib
nmslib
annoy
libngt-ann
libsptag
CTCWordBeamSearch
faiss
xgboost
pisa
nsync
randen
zfp-compressed-arrays
random
wyhash
xnnpack
merror
kahypar
flat_hash_map
pagerank
graphit
libgrape-lite
concurrencpp
lerc
stx-error-handling
result-cpp
delegate
robin-map
hopscotch-map

EOT
)

myapplist=$( 
cat <<EOT

UIforETW
EtwExplorer

EOT
)

for f in  $mylist  ; do
  echo mknewproj $f
done

for f in  $myapplist  ; do
  mknewAPPproj $f
done

