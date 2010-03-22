# SDL Image CMake build file
cmake_minimum_required (VERSION 2.6)
project(SDLImage)

if (NOT SDLIMAGE_SOURCE_NAME)
	set (SDLIMAGE_SOURCE_NAME "SDL_image-1.2.10" CACHE STRING "SDL_image source name")
endif (NOT SDLIMAGE_SOURCE_NAME)

set(SDLImage_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${SDLIMAGE_SOURCE_NAME}")
set(SDLIMAGE_INCLUDE_DIR "${SDLImage_SRC_FOLDER}" CACHE STRING "SDL_image include directory" FORCE)
set(SDLIMAGE_LIBRARY SDL_image CACHE STRING "SDL_image libraries" FORCE)
set(sdlimage_src
${SDLImage_SRC_FOLDER}/IMG.c
${SDLImage_SRC_FOLDER}/IMG_bmp.c
${SDLImage_SRC_FOLDER}/IMG_gif.c
${SDLImage_SRC_FOLDER}/IMG_jpg.c
${SDLImage_SRC_FOLDER}/IMG_lbm.c
${SDLImage_SRC_FOLDER}/IMG_pcx.c
${SDLImage_SRC_FOLDER}/IMG_png.c
${SDLImage_SRC_FOLDER}/IMG_pnm.c
${SDLImage_SRC_FOLDER}/IMG_tga.c
${SDLImage_SRC_FOLDER}/IMG_tif.c
${SDLImage_SRC_FOLDER}/IMG_xcf.c
${SDLImage_SRC_FOLDER}/IMG_xpm.c
${SDLImage_SRC_FOLDER}/IMG_xv.c
${SDLImage_SRC_FOLDER}/IMG_xxx.c
)

add_library(SDL_image STATIC ${sdlimage_src})
if (NOT SDLIMAGE_STATICLIB_OUTPUT_PATH)
	set (SDLIMAGE_STATICLIB_OUTPUT_PATH "." CACHE STRING "SDL_image static lib output directory")
endif (NOT SDLIMAGE_STATICLIB_OUTPUT_PATH)
set_target_properties (${SDLIMAGE_LIBRARY} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${SDLIMAGE_STATICLIB_OUTPUT_PATH}")

if (NOT ZLIB_INCLUDE_DIR)
	message( SEND_ERROR "Please set ZLIB_INCLUDE_DIR to the correct path")
endif (NOT ZLIB_INCLUDE_DIR)
if (NOT PNG_INCLUDE_DIR)
	message( SEND_ERROR "Please set PNG_INCLUDE_DIR to the correct path")
endif (NOT PNG_INCLUDE_DIR)
if (NOT SDL_INCLUDE_DIR)
	message( SEND_ERROR "Please set SDL_INCLUDE_DIR to the correct path")
endif (NOT SDL_INCLUDE_DIR)
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${SDL_INCLUDE_DIR})

if (TARGET ${SDL_LIBRARY_MAIN})
	add_dependencies(${SDLIMAGE_LIBRARY} ${SDL_LIBRARY_MAIN})
endif (TARGET ${SDL_LIBRARY_MAIN})
if (TARGET "${PNG_LIBRARIES}")
	add_dependencies(${SDLIMAGE_LIBRARY} ${PNG_LIBRARIES})
endif(TARGET "${PNG_LIBRARIES}")
target_link_libraries(${SDLIMAGE_LIBRARY} ${SDL_LIBRARY} ${PNG_LIBRARIES})
# Some defines
if (WIN32)
	set(sdlimage_compile_def
		WIN32 _WINDOWS LOAD_PNG)
		# Additonal Flags not used
		# LOAD_BMP LOAD_GIF LOAD_JPG LOAD_JPG_DYNAMIC="jpeg.dll" LOAD_LBM LOAD_PCX
	if (MSVC)
		set(sdlimage_compile_def ${sdlimage_compile_def}
			_CRT_SECURE_NO_WARNINGS)

	endif (MSVC)
endif (WIN32)
set_target_properties(${SDLIMAGE_LIBRARY} PROPERTIES
	COMPILE_DEFINITIONS "${sdlimage_compile_def}")

# vim:ft=cmake
