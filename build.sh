#!/bin/sh

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-./tinyrpc-build}
# BUILD_TYPE=${BUILD_TYPE:-Release}
BUILD_TYPE=${BUILD_TYPE:-Debug}
INSTALL_DIR=${INSTALL_DIR:-../${BUILD_TYPE}-install}
BUILD_NO_EXAMPLES=${BUILD_NO_EXAMPLES:-0}
STUB_FORMATTER="echo"

# rm -rf $BUILD_DIR \
        mkdir -p $BUILD_DIR/$BUILD_TYPE \
        && cd $BUILD_DIR/$BUILD_TYPE \
        && cmake \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
            -DCMAKE_BUILD_NO_EXAMPLES=$BUILD_NO_EXAMPLES \
            -DCMAKE_STUB_FORMATTER=$STUB_FORMATTER \
            $SOURCE_DIR \
        && make $*