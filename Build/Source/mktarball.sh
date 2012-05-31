#!/bin/bash

# This script performs common operations needed to transform a git repository
# into the source tarball for distribution.
#
# This is meant to be run from the project root.

# First, clean up
make clean

# Extract version number from Source/version.h
VERSION=$(cat Source/version.h | grep EPIAR_VERSION_FULL | cut -d'"' -f2)

# Make the directory that we will later tarball
mkdir "epiar-$VERSION"

# Copy everything into it - we'll filter out what we don't need later
cp -R * "epiar-$VERSION"
rm "epiar-$VERSION/autom4te.cache"

