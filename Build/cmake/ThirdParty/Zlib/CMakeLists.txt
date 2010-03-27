# Zlib CMake build file
cmake_minimum_required (VERSION 2.6)
project (ZLIB)

if (NOT ZLIB_SOURCE_NAME)
	set(ZLIB_SOURCE_NAME "zlib-1.2.3" CACHE STRING "ZLIB source name")
endif (NOT ZLIB_SOURCE_NAME)

set(ZLIB_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${ZLIB_SOURCE_NAME}")
# Create variables for include and library
set(ZLIB_INCLUDE_DIR "${ZLIB_SRC_FOLDER}" CACHE STRING "ZLIB include directory" FORCE)
set(ZLIB_LIBRARIES zlib CACHE STRING "ZLIB libraries" FORCE)
set(zlib_src
${ZLIB_SRC_FOLDER}/adler32.c
${ZLIB_SRC_FOLDER}/compress.c
${ZLIB_SRC_FOLDER}/crc32.c
${ZLIB_SRC_FOLDER}/deflate.c
${ZLIB_SRC_FOLDER}/gzio.c
${ZLIB_SRC_FOLDER}/infback.c
${ZLIB_SRC_FOLDER}/inffast.c
${ZLIB_SRC_FOLDER}/inflate.c
${ZLIB_SRC_FOLDER}/inftrees.c
${ZLIB_SRC_FOLDER}/trees.c
${ZLIB_SRC_FOLDER}/uncompr.c
${ZLIB_SRC_FOLDER}/zutil.c
)

add_library(zlib STATIC ${zlib_src})
if (NOT ZLIB_STATICLIB_OUTPUT_PATH)
	set(ZLIB_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"Zlib static lib output directory")
endif (NOT ZLIB_STATICLIB_OUTPUT_PATH)

if (WIN32)
	if (MSVC)
		set_target_properties(${ZLIB_LIBRARIES} PROPERTIES
			COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
	endif (MSVC)
endif (WIN32)

set_target_properties (${ZLIB_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${ZLIB_STATICLIB_OUTPUT_PATH}")

# vim:ft=cmake
