#!/bin/bash

# Build configuration
BUILD="Release"
DOXYGEN="OFF"
TESTS="OFF"
INTERNAL_LUA="OFF"
LUAJIT="OFF"

# Intermedia directory
IDIR="`pwd`/buildRelease"

mkdir "$IDIR"
cmake -H"`pwd`" -B"$IDIR" -DCMAKE_BUILD_TYPE=$BUILD -DCOMPILE_DOXYGEN=$DOXYGEN -DCOMPILE_TEST=$TESTS -DUSE_INTERNAL_LUA=$INTERNAL_LUA -DUSE_LUAJIT=$LUAJIT


