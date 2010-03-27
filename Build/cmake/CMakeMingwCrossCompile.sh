#!/bin/bash

# Build configuration
BUILD="Release"
DOXYGEN="OFF"
TESTS="OFF"
INTERNAL_LUA="OFF"
LUAJIT="ON"

# Cross compile flags
# For some reason, cross compiler can't detect base dir
BASEDIR=$(dirname $(dirname $(pwd)))
echo $BASEDIR
CROSS="-DCMAKE_TOOLCHAIN_FILE=`pwd`/CMakeScripts/CrossCompile/Mingw_Toolchain.cmake -DEpiar_SRC_DIR=$BASEDIR/Source -DEpiar_OUT_DIR=$BASEDIR"

# Intermedia directory
IDIR="`pwd`/buildWin"

mkdir "$IDIR"
cmake -H"`pwd`" -B"$IDIR" -DCOMPILE_DOXYGEN=$DOXYGEN -DCOMPILE_TEST=$TESTS -DUSE_INTERNAL_LUA=$INTERNAL_LUA -DUSE_LUAJIT=$LUAJIT $CROSS

# Need to set build type separately
cmake -H"`pwd`" -B"$IDIR" -DCMAKE_BUILD_TYPE=$BUILD

