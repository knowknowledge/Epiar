#!/usr/bin/env bash
DLLS="iconv.dll SDL.dll SDL_image.dll SDL_mixer.dll zlib1.dll libvorbis-0.dll libxml2-2.dll libtiff-3.dll libstdc++-6.dll libpng15-15.dll libphysfs.dll libogg-0.dll libjpeg-62.dll libfreetype-6.dll libftgl-2.dll libgcc_s_sjlj-1.dll"

# To be run from the Epiar source base directory. Generates an
# epiar-win32 directory.
make clean
mingw32-configure
mingw32-make
mkdir -p epiar-win32
cp ./epiar.exe epiar-win32/
cp -r Resources epiar-win32/
cp README NEWS LICENSE* PACKAGING AUTHORS ChangeLog epiar-win32/
rm -f epiar-win32/Resources/Graphics/*.xcf
rm -f epiar-win32/Resources/Definitions/saved-games.xml

# The following assumes Fedora 16-based MinGW. If this becomes a
# problem (i.e. other MinGW developers), we can expand this
for f in $DLLS; do cp /usr/i686-w64-mingw32/sys-root/mingw/bin/$f epiar-win32; done

