# Libpng CMake build file
cmake_minimum_required (VERSION 2.6)
project (LIBPNG)

if (NOT PNG_SOURCE_NAME)
	set (PNG_SOURCE_NAME "libpng-1.4.0" CACHE STRING "PNG source name")
endif (NOT PNG_SOURCE_NAME)

set(LIBPNG_SRC_DIR "${PROJECT_SOURCE_DIR}/${PNG_SOURCE_NAME}")
set(PNG_INCLUDE_DIR "${LIBPNG_SRC_DIR}" CACHE STRING "PNG include directory" FORCE)
set(PNG_LIBRARIES libpng CACHE STRING "PNG libraries" FORCE)
set (libpng_src
	${LIBPNG_SRC_DIR}/png.c
	${LIBPNG_SRC_DIR}/pngerror.c
	${LIBPNG_SRC_DIR}/pngget.c
	${LIBPNG_SRC_DIR}/pngmem.c
	${LIBPNG_SRC_DIR}/pngpread.c
	${LIBPNG_SRC_DIR}/pngread.c
	${LIBPNG_SRC_DIR}/pngrio.c
	${LIBPNG_SRC_DIR}/pngrtran.c
	${LIBPNG_SRC_DIR}/pngrutil.c
	${LIBPNG_SRC_DIR}/pngset.c
	${LIBPNG_SRC_DIR}/pngtest.c
	${LIBPNG_SRC_DIR}/pngtrans.c
	${LIBPNG_SRC_DIR}/pngwio.c
	${LIBPNG_SRC_DIR}/pngwrite.c
	${LIBPNG_SRC_DIR}/pngwtran.c
	${LIBPNG_SRC_DIR}/pngwutil.c
)
add_library(libpng STATIC ${libpng_src})
if (NOT PNG_STATICLIB_OUTPUT_PATH)
	set (PNG_STATICLIB_OUTPUT_PATH "." CACHE STRING "PNG static lib output directory")
endif (NOT PNG_STATICLIB_OUTPUT_PATH)

if (TARGET ${ZLIB_LIBRARIES})
	add_dependencies(${PNG_LIBRARIES} ${ZLIB_LIBRARIES})
endif (TARGET ${ZLIB_LIBRARIES})

if (WIN32)
	if (MSVC)
	set_target_properties(libpng PROPERTIES
		COMPILE_DEFINITIONS "WIN32;_DEBUG;DEBUG;PNG_NO_MMX_CODE;_CRT_SECURE_NO_WARNINGS")
	endif (MSVC)
endif(WIN32)

set_target_properties(libpng PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${PNG_STATICLIB_OUTPUT_PATH})

# Assume ZLIB was already searched for
if (NOT ZLIB_INCLUDE_DIR)
	message(SEND_ERROR "Please define ZLIB_INCLUDE_DIR and ZLIB_LIBRARIES to appropriate files.")
endif (NOT ZLIB_INCLUDE_DIR)
include_directories(${ZLIB_INCLUDE_DIR})
# If we're building zlib from source
if (NOT ZLIB_FOUND)
	add_dependencies(libpng ${ZLIB_LIBRARIES})
endif (NOT ZLIB_FOUND)

target_link_libraries( ${PNG_LIBRARIES}
	${ZLIB_LIBRARIES}
	)

