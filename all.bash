#!/bin/bash

set -e

cd src

BUILD_TYPE="Release"
SOURCE=`pwd`

cd /tmp
mkdir -p build-vismatrix
cd build-vismatrix/
cmake -S $SOURCE \
    -D CMAKE_BUILD_TYPE=${BUILD_TYPE}

make
