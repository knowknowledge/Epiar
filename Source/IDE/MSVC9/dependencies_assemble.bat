@echo off

echo Copying header files
copy dependencies\libpng\*.h include\
copy dependencies\libsdl\include\*.h include\
copy dependencies\libsdlimage\*.h include\
copy dependencies\libz\*.h include\
copy dependencies\libxml2\include\libxml include\libxml\

copy dependencies\config.h include\
copy dependencies\SDL_config.h include\
copy dependencies\zconf.h include\
copy dependencies\xmlversion.h include\libxml\

for %%c in (Debug Release) do for %%e in (lib) do (
	echo Copying %%c %%e files
	copy dependencies\output\libpng\%%c\libpng.%%e lib\%%c\
	copy dependencies\output\libsdl\%%c\libsdl.%%e lib\%%c\
	copy dependencies\output\libsdlimage\%%c\libsdlimage.%%e lib\%%c\
	copy dependencies\output\libz\%%c\libz.%%e lib\%%c\
	copy dependencies\output\libxml2\%%c\libxml2.%%e lib\%%c\
)
