#!/usr/bin/env bash
mkdir -p Epiar.app/Contents/MacOS
cp Epiar Epiar.app/Contents/MacOS
cp -r Resources Epiar.app/Contents/
cp Info.plist Epiar.app/Contents/
cp Resources/Graphics/Epiar_Icon.icns Epiar.app/Contents/Resources/
rm -rf Epiar.app/Contents/Resources/Graphics/*.xcf
