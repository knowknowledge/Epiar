# Created by: DuMuT6p
# Last modified: June/23/2012

# Instructions:
# 1. Open epiar`s main directory(e.g. C:\epiar).
# 2. Make sure the following files are in the directory:
# Epiar.exe, AUTHORS, ChangeLog, COPYING, CREDITS.GNU-FreeType, INSTALL, LICENSE, LICENSE.bitstream_fonts,
# LICENSE.visitor_fonts, NEWS, README, the Resources directory.
# 3. Copy there this script(epiar-installer.nsi) and the NSIS_Resources directory. 
# 4. Copy there the following libraries: 
# libfreetype-6.dll, libftgl-2.dll, libgcc_s_sjlj-1.dll, libiconv-2.dll, libjpeg-7.dll, libogg-0.dll, 
# libphysfs-1-0-1.dll, libpng14-14.dll, libstdc++-6.dll, libtiff-3.dll, libvorbis-0.dll, libvorbisfile-3.dll, 
# libxml2-2.dll, SDL.dll, SDL.original.dll, SDL_image.dll, SDL_mixer.dll, zlib1.dll.
# 5. Compile the script 

# multiuser settings
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_COMMANDLINE

# includes
!include MultiUser.nsh
!include MUI2.nsh

# window name
name "Epiar 0.5.1"

# define installer name
outFile "epiar-0.5.1.exe"
 
# set install directory
InstallDir "C:\Program Files\Epiar"

# verify user privileges before everything else
Function .onInit
  !insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd

# interface Settings

# Abort warning
!define MUI_ABORTWARNING
!define MUI_ABORTWARNING_TEXT "Do you really want to cancel epiar installer?"
!define MUI_ABORTWARNING_CANCEL_DEFAULT

# icons
!define MUI_ICON "NSIS_Resources\NSIS_MUI_ICON.ico"
!define MUI_UNICON "NSIS_Resources\NSIS_MUI_UNIICON.ico"

# page header
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "NSIS_Resources\NSIS_BrandImage.bmp"

# installer/uninstaller welcome/finish page
!define MUI_WELCOMEFINISHPAGE_BITMAP "NSIS_Resources\NSIS_Welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "NSIS_Resources\NSIS_Welcome.bmp"

# installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE LICENSE
!insertmacro MULTIUSER_PAGE_INSTALLMODE
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN $INSTDIR\Epiar.exe
!define MUI_FINISHPAGE_RUN_TEXT "Run Epiar now"
!define MUI_FINISHPAGE_LINK "View Epiar website"
!define MUI_FINISHPAGE_LINK_LOCATION http://epiar.net/
!insertmacro MUI_PAGE_FINISH

# uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Bulgarian"
 
# default section start
section
 
# define output path
setOutPath $INSTDIR

# write registry keys
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Epiar" \
                 "DisplayName" "Epiar"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Epiar" \
                 "UninstallString" "$\"$INSTDIR\epiar-uninstall.exe$\""

# create the shortcuts in the start menu programs directory
createDirectory "$SMPROGRAMS\Epiar"
createShortCut "$SMPROGRAMS\Epiar\Epiar.lnk" "$INSTDIR\Epiar.exe"
createShortCut "$SMPROGRAMS\Epiar\UninstallEpiar.lnk" "$INSTDIR\epiar-uninstall.exe"
createShortCut "$SMPROGRAMS\Epiar\README.lnk" "$INSTDIR\README"
createShortCut "$DESKTOP\Epiar.lnk" "$INSTDIR\Epiar.exe"

# specify files to go in output path
file Epiar.exe
file AUTHORS
file ChangeLog
file LICENSE
file LICENSE.bitstream_fonts
file LICENSE.music
file LICENSE.other_fonts
file LICENSE.visitor_fonts
file NEWS
file PACKAGING
file README

setOutPath $INSTDIR
file iconv.dll
file libfreetype-6.dll
file libftgl-2.dll
file libgcc_s_sjlj-1.dll
file libjpeg-62.dll
file libogg-0.dll
file libphysfs.dll
file libpng15-15.dll
file libstdc++-6.dll
file libtiff-3.dll
file libvorbis-0.dll
file libxml2-2.dll
file SDL.dll
file SDL.dll
file SDL_image.dll
file SDL_mixer.dll
file zlib1.dll

setOutPath $INSTDIR\Resources\Animations
file Resources\Animations\*
setOutPath $INSTDIR\Resources\Art
file Resources\Art\*
setOutPath $INSTDIR\Resources\Audio\Effects
file Resources\Audio\Effects\*
setOutPath $INSTDIR\Resources\Audio\Engines
file Resources\Audio\Engines\*
setOutPath $INSTDIR\Resources\Audio\Interface
file Resources\Audio\Interface\*
setOutPath $INSTDIR\Resources\Audio\Music
file Resources\Audio\Music\*
setOutPath $INSTDIR\Resources\Audio\Weapons
file Resources\Audio\Weapons\*
setOutPath $INSTDIR\Resources\Blueprints
file Resources\Blueprints\*
setOutPath $INSTDIR\Resources\Definitions
file Resources\Definitions\*
setOutPath $INSTDIR\Resources\Fonts
file Resources\Fonts\*
setOutPath $INSTDIR\Resources\Graphics
file Resources\Graphics\*
setOutPath $INSTDIR\Resources\Icons
file Resources\Icons\*
setOutPath $INSTDIR\Resources\Scripts
file Resources\Scripts\*
setOutPath $INSTDIR\Resources\Simulation\default
file Resources\Simulation\default\*
setOutPath $INSTDIR\Resources\Skin
file Resources\Skin\*

setOutPath $INSTDIR

# define uninstaller name
writeUninstaller $INSTDIR\epiar-uninstall.exe
 
# default section end
sectionEnd
 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
section "Uninstall"
 
# Always delete uninstaller first
delete $INSTDIR\epiar-uninstall.exe

# delete registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Epiar"

# delete shortcut
delete "$DESKTOP\Epiar.lnk"
 
# delete the install directory and ALL files in it
RMDir /r $INSTDIR
RMDir /r $SMPROGRAMS\Epiar
 
sectionEnd