Building Epiar with CMake Guide

Note: It is recommended you have the following directory structure for this
build:

* Parent directory
	+ Epiar
		- Doxygen
		- Resources
		- Source
		- (etc)...
	+ EpiarBuild
		- CMake
		- include
		- lib
		- Thirdparty
		- (etc)...

Of course you can have any arbitrary directory structure you want, but you have
to modify `Epiar_OUT_DIR` ( Needs to point to Epiar base directory) and
`Epiar_SRC_DIR` ( Needs to point to Epiar/Source).


Windows
=========================

1) CMake installation
-------------------------
If you don't have cmake already, get the installer or zip file from
<http://www.cmake.org/cmake/resources/software.html>, install/extract it.
And make sure the "bin" subdirectory is in the path.


2) Running CMake
-------------------------
NOTE: Please do NOT modify the "Where is the source code:" path, this should
point to the EpiarBuild folder, and not the actual source code path.

Open up a command prompt with your compiler environment and run
CMake.cmd.  Click on "Configure" to populate the default variables.

ALTERNATIVELY: you can run `cmake` from the command prompt, doing so will require
you to manually specify all the variables from the command line, see the CMake
help for more information.

3) External dependencies
-------------------------
NOTE: It is recommended you build the external dependencies from source if using
visual studio so that you can do debug builds of the libraries. (External
dependencies should be automatically downloaded for you if you do not have them)

If you already have the external dependencies such as `SDL`,
`SDL_image`, `Lua`, etc installed, you can uncheck build from source and run
"Configure" again to search for the existing include and libraries on your
system.

If you don't have the external dependencies. Check the `"THIRDPARTY_DOWNLOAD"`
checkbox and CMake will automatically download the create the proper files to
build the 3rd party libraries. Currently these are statically linked to your
executable.

4) Generating makefiles/project
-------------------------
Click Generate to generate the build files. By default, the files are generated
in the "build" subdirectory, but this can be changed by modifying the "Where to
build the binaries:" path.

You can then use the makefiles/project as you normally would.

Linux
=========================

1) CMake installation
-------------------------
You should always prefer CMake in your package manager, this will make it much
easier to keep it up to date.  The only reason you should not want it is if you
do not want to install QT, in that case the standalone version will have QT
statically linked in.

If CMake 2.6 is not in your package manager (or you prefer the independent
build), you can download it from
<http://www.cmake.org/cmake/resources/software.html>, it has no external
dependencies.  Install/extract it to somewhere, and add the "bin" subdirectory
to your path.

2) Running CMake
-------------------------
Then run CMake.sh.  Click on "Configure" to populate the default variables.

3) External dependencies
-------------------------
Recommended: Since most distros supply prebuilt libraries in the package manager, it is best
that you get the development files from your package manager and not build from
source. (Also, building from source is not supported in linux as the most common
distros will provided the development files we use, and using the native
makefiles on linux is not much of a hassle with the `make install` command).

4) Generating makefiles/project
-------------------------
Click Generate will then generate the makefiles in the "build" subdirectory
(by default).  This can be changed in the GUI if desired.

Now you can build the file in the "build" subdirectory.

 vim:ft=mkd
