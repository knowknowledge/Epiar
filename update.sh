#! /bin/sh

usage() {
	echo "USAGE: update.sh MAJOR MINOR MICRO"
}

if [ "$1" = "-h" ]
then
	echo "USAGE"	
	exit 0
fi

EPIAR_VERSION_OLD=`awk '$2 == "EPIAR_VERSION_FULL" {print $3}' Source/version.h | tr -d '"'`

MAJOR=$1
MINOR=$2
MICRO=$3

EPIAR_VERSION_NEW="$MAJOR.$MINOR.$MICRO"

echo "Old Version: $EPIAR_VERSION_FULL"
echo "New Version: $MAJOR.$MINOR.$MICRO"

echo "Updating the version number in version.h"
cat Source/version.h | \
	sed "s/EPIAR_VERSION_MAJOR.*$/EPIAR_VERSION_MAJOR $MAJOR/" | \
	sed "s/EPIAR_VERSION_MINOR.*$/EPIAR_VERSION_MINOR $MINOR/" | \
	sed "s/EPIAR_VERSION_MICRO.*$/EPIAR_VERSION_MICRO $MICRO/" | \
	sed "s/EPIAR_VERSION_FULL.*$/EPIAR_VERSION_FULL \"$EPIAR_VERSION_NEW\"/" \
	> Source/newversion.h
mv Source/newversion.h Source/version.h

echo "Updating the version number in the README"
sed "s/Version .*/Version $EPIAR_VERSION_NEW/" < README > README.new
mv README.new README

echo "Update the version Number in the Info.plist to $EPIAR_VERSION_NEW."
defaults write `pwd`/Build/OS\ X/Info CFBundleShortVersionString "$EPIAR_VERSION_NEW"
defaults read `pwd`/Build/OS\ X/Info

echo "TODO: Update all of the .xml files to $EPIAR_VERSION_NEW."
