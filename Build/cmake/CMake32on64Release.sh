#!/bin/bash
# This script is meant to be used for cross-compiling in chroot

# Build configuration
BUILD="Release"
DOXYGEN="OFF"
TESTS="OFF"
INTERNAL_LUA="OFF"
LUAJIT="ON"

# Cross compiling definitions
CROSSFLAGS="-DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=i686"

# Intermedia directory
IDIR="`pwd`/build32Release"

mkdir "$IDIR"
cmake -H"`pwd`" -B"$IDIR" -DCOMPILE_DOXYGEN=$DOXYGEN -DCOMPILE_TEST=$TESTS -DUSE_INTERNAL_LUA=$INTERNAL_LUA -DUSE_LUAJIT=$LUAJIT $CROSSFLAGS

# We need to define Release configuration after initial generation
cmake -H"`pwd`" -B"$IDIR" -DCMAKE_BUILD_TYPE=$BUILD
