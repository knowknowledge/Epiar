#!/usr/bin/env bash

# To be run from the Epiar source base directory. Generates a
# Epiar.app directory.
mkdir -p Epiar.app/Contents/MacOS
cp ./Epiar Epiar.app/Contents/MacOS
cp -r Resources Epiar.app/Contents/
cp Build/OS\ X/Info.plist Epiar.app/Contents/
cp Build/OS\ X/Epiar_Icon.icns Epiar.app/Contents/Resources/
rm -rf Epiar.app/Contents/Resources/Graphics/*.xcf

