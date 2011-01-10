#!/usr/bin/env bash
EPIAR_VERSION_FULL=`awk '$2 == "EPIAR_VERSION_FULL" {print $3}' Source/version.h | tr -d '"'`
echo "Update the version Number in the Info.plist to $EPIAR_VERSION_FULL."
defaults write `pwd`/Info CFBundleShortVersionString "$EPIAR_VERSION_FULL"
defaults read `pwd`/Info

DMG_LOC=`pwd`/Epiar$EPIAR_VERSION_FULL/
APP_LOC=$DMG_LOC/Epiar.app/
echo "Creating Epiar.app Package at $APP_LOC"
mkdir -p $APP_LOC/Contents/MacOS
cp Epiar $APP_LOC/Contents/MacOS
cp -r Resources $APP_LOC/Contents/
cp Info.plist $APP_LOC/Contents/
cp Resources/Icons/Epiar_Icon.icns $APP_LOC/Contents/Resources/

# The Blueprint files are used for creating the Art.
echo "Removing blueprint files."
rm -rf $APP_LOC/Contents/Resources/Blueprints

echo "Adding README and LICENSE files."
cp AUTHORS $DMG_LOC
cp COPYING $DMG_LOC
cp LICENSE* $DMG_LOC
cp README $DMG_LOC

# Package Epiar into a .dmg 
echo "Creating Epiar dmg..."
hdiutil create Epiar$EPIAR_VERSION_FULL.dmg -srcfolder $DMG_LOC
