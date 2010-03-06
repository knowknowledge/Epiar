# OGG CMake build file
cmake_minimum_required(VERSION 2.6)
project(OGG)

if (NOT OGG_SOURCE_NAME)
	set(OGG_SOURCE_NAME libogg-1.1.4)
endif (NOT OGG_SOURCE_NAME)

set(OGG_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${OGG_SOURCE_NAME}")
# Create variables for include and library
set(OGG_INCLUDE_DIR ${OGG_SRC_FOLDER}/include CACHE STRING
	"OGG include directory" FORCE)
set(OGG_LIBRARIES OGG CACHE STRING
	"OGG libraries" FORCE)
set(OGG_src
	${OGG_SRC_FOLDER}/src/bitwise.c
	${OGG_SRC_FOLDER}/src/framing.c
	${OGG_INCLUDE_DIR}/ogg/ogg.h
	${OGG_INCLUDE_DIR}/ogg/os_types.h
	${OGG_SRC_FOLDER}/win32/ogg.def
	)

include_directories(${OGG_INCLUDE_DIR})

add_library(${OGG_LIBRARIES} STATIC ${OGG_src})
set_target_properties(${OGG_LIBRARIES} PROPERTIES
	COMPILE_DEFINITIONS "WIN32;_DEBUG;_WINDOWS;_USRDLL;LIBOGG_EXPORTS")
if (NOT OGG_STATICLIB_OUTPUT_PATH)
	set(OGG_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"OGG static lib output directory")
endif (NOT OGG_STATICLIB_OUTPUT_PATH)
set_target_properties(${OGG_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${OGG_STATICLIB_OUTPUT_PATH}")

