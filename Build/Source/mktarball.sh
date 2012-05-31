#!/bin/bash

# This script performs common operations needed to transform a git repository
# into the source tarball for distribution.
#
# This is meant to be run from the project root.

# Turn on Bash's extended pattern matching
shopt -s extglob

# First, clean up
make clean

# Extract version number from Source/version.h
VERSION=$(cat Source/version.h | grep EPIAR_VERSION_FULL | cut -d'"' -f2)

# Make the directory that we will later tarball
mkdir "epiar-$VERSION"

# Copy everything into it - we'll filter out what we don't need later
cp -R !(epiar-$VERSION) epiar-$VERSION
rm -rf "epiar-$VERSION/autom4te.cache"
rm -rf "epiar-$VERSION/Doxygen"
rm epiar-$VERSION/{config.log,config.status,config.h,Makefile,Makefile.in,stamp-h1,missing,depcomp,config.sub,config.guess,install-sh}

# Generate the tarball and clean up our mess
tar --bzip2 -cf epiar-$VERSION.tar.bz2 epiar-$VERSION
rm -rf epiar-$VERSION

