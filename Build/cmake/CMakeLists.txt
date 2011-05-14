# Epiar CMake main build file
cmake_minimum_required (VERSION 2.6)
project (Epiar)

# Set's Epiar version information
set(EPIAR_VERSION_MAJOR 0)
set(EPIAR_VERSION_MINOR 5)
set(EPIAR_VERSION_PATCH 0)
set(EPIAR_VERSION "${EPIAR_VERSION_MAJOR}.${EPIAR_VERSION_MINOR}.${EPIAR_VERSION_PATCH}")

# Directory where all cmake scripts are
set (SCRIPT_DIR "${PROJECT_SOURCE_DIR}/CMakeScripts")
# Directory for custom Modules
set(CMAKE_MODULE_PATH "${SCRIPT_DIR}/CustomModules")

find_path(Epiar_SRC_DIR includes.h PATHS "../../Source")
find_path(Epiar_OUT_DIR Source/includes.h PATHS "../../.")

# Include external functions
include ("${SCRIPT_DIR}/IncludeFunctions.txt")
# Include default options
include ("${SCRIPT_DIR}/EpiarDefaultOptions.txt")
# Include Epiar Sources
include ("${SCRIPT_DIR}/EpiarSourceFiles.txt")
# Include flags (compiler/linker flags, required defines)
include ("${SCRIPT_DIR}/EpiarFlags.txt")
# Include thirdparty
include ("${SCRIPT_DIR}/CheckThirdParty.txt")
# Install directives
include ("${SCRIPT_DIR}/EpiarInstall.txt")

# Epiar includes
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Epiar_SRC_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(EpiarBIN ${EpiarLIBS})

if(WIN32)
	# On windows, we want to use a script called Epiar.cmd for console mode
	set_target_properties(EpiarBIN PROPERTIES
		OUTPUT_NAME	EpiarExe)
else(WIN32)
	set_target_properties(EpiarBIN PROPERTIES
		OUTPUT_NAME	Epiar)
endif(WIN32)


set_target_properties(EpiarBIN PROPERTIES
	DEBUG_OUTPUT_NAME Epiar_debug
	RUNTIME_OUTPUT_DIRECTORY ${Epiar_OUT_DIR})

# Include tests
include ("${SCRIPT_DIR}/EpiarTests.txt")

# Footer
include ("${SCRIPT_DIR}/EpiarFooter.txt")

# vim:nospell
