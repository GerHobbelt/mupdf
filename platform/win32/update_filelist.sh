#! /bin/bash

pushd "$( dirname "$0" )"

find ../.. -type f > filelist.lst  &

popd
