# Vorbis CMake build file
cmake_minimum_required(VERSION 2.6)
project(VORBIS)

if (NOT VORBIS_SOURCE_NAME)
	set(VORBIS_SOURCE_NAME libvorbis-1.2.3)
endif (NOT VORBIS_SOURCE_NAME)

set(VORBIS_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${VORBIS_SOURCE_NAME}")
# Create variables for include and library
set(VORBIS_INCLUDE_DIR ${VORBIS_SRC_FOLDER}/include CACHE STRING
	"VORBIS include directory" FORCE)
# Vorbis is actually composed of 2 libraries
set(VORBIS_LIBRARY VORBIS)
set(VORBIS_FILE_LIBRARY VORBIS_FILE)
set(VORBIS_LIBRARIES ${VORBIS_LIBRARY} ${VORBIS_FILE_LIBRARY} CACHE STRING
	"VORBIS libraries" FORCE)
set(VORBIS_src
	${VORBIS_SRC_FOLDER}/lib/analysis.c
	${VORBIS_SRC_FOLDER}/lib/bitrate.c
	${VORBIS_SRC_FOLDER}/lib/block.c
	${VORBIS_SRC_FOLDER}/lib/codebook.c
	${VORBIS_SRC_FOLDER}/lib/envelope.c
	${VORBIS_SRC_FOLDER}/lib/floor0.c
	${VORBIS_SRC_FOLDER}/lib/floor1.c
	${VORBIS_SRC_FOLDER}/lib/info.c
	${VORBIS_SRC_FOLDER}/lib/lookup.c
	${VORBIS_SRC_FOLDER}/lib/lpc.c
	${VORBIS_SRC_FOLDER}/lib/lsp.c
	${VORBIS_SRC_FOLDER}/lib/mapping0.c
	${VORBIS_SRC_FOLDER}/lib/mdct.c
	${VORBIS_SRC_FOLDER}/lib/psy.c
	${VORBIS_SRC_FOLDER}/lib/registry.c
	${VORBIS_SRC_FOLDER}/lib/res0.c
	${VORBIS_SRC_FOLDER}/lib/sharedbook.c
	${VORBIS_SRC_FOLDER}/lib/smallft.c
	${VORBIS_SRC_FOLDER}/lib/synthesis.c
	${VORBIS_SRC_FOLDER}/win32/vorbis.def
	${VORBIS_SRC_FOLDER}/lib/vorbisenc.c
	${VORBIS_SRC_FOLDER}/lib/window.c
	${VORBIS_SRC_FOLDER}/lib/backends.h
	${VORBIS_SRC_FOLDER}/lib/bitrate.h
	${VORBIS_SRC_FOLDER}/lib/codebook.h
	${VORBIS_SRC_FOLDER}/include/vorbis/codec.h
	${VORBIS_SRC_FOLDER}/lib/codec_internal.h
	${VORBIS_SRC_FOLDER}/lib/envelope.h
	${VORBIS_SRC_FOLDER}/lib/modes/floor_all.h
	${VORBIS_SRC_FOLDER}/lib/books/floor/floor_books.h
	${VORBIS_SRC_FOLDER}/lib/highlevel.h
	${VORBIS_SRC_FOLDER}/lib/lookup.h
	${VORBIS_SRC_FOLDER}/lib/lookup_data.h
	${VORBIS_SRC_FOLDER}/lib/lpc.h
	${VORBIS_SRC_FOLDER}/lib/lsp.h
	${VORBIS_SRC_FOLDER}/lib/masking.h
	${VORBIS_SRC_FOLDER}/lib/mdct.h
	${VORBIS_SRC_FOLDER}/lib/misc.h
	${VORBIS_SRC_FOLDER}/lib/os.h
	${VORBIS_SRC_FOLDER}/lib/psy.h
	${VORBIS_SRC_FOLDER}/lib/modes/psych_11.h
	${VORBIS_SRC_FOLDER}/lib/modes/psych_16.h
	${VORBIS_SRC_FOLDER}/lib/modes/psych_44.h
	${VORBIS_SRC_FOLDER}/lib/modes/psych_8.h
	${VORBIS_SRC_FOLDER}/lib/registry.h
	${VORBIS_SRC_FOLDER}/lib/books/coupled/res_books_stereo.h
	${VORBIS_SRC_FOLDER}/lib/books/uncoupled/res_books_uncoupled.h
	${VORBIS_SRC_FOLDER}/lib/modes/residue_16.h
	${VORBIS_SRC_FOLDER}/lib/modes/residue_44.h
	${VORBIS_SRC_FOLDER}/lib/modes/residue_44u.h
	${VORBIS_SRC_FOLDER}/lib/modes/residue_8.h
	${VORBIS_SRC_FOLDER}/lib/scales.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_11.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_16.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_22.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_32.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_44.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_44u.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_8.h
	${VORBIS_SRC_FOLDER}/lib/modes/setup_X.h
	${VORBIS_SRC_FOLDER}/lib/smallft.h
	${VORBIS_SRC_FOLDER}/include/vorbis/vorbisenc.h
	${VORBIS_SRC_FOLDER}/include/vorbis/vorbisfile.h
	${VORBIS_SRC_FOLDER}/lib/window.h
	)
set(VORBIS_FILE_src
	${VORBIS_SRC_FOLDER}/lib/vorbisfile.c
	${VORBIS_SRC_FOLDER}/include/vorbis/vorbisfile.h
	)


if ( OGG_INCLUDE_DIR )
	include_directories(${OGG_INCLUDE_DIR})
else ( OGG_INCLUDE_DIR )
	message(SEND_ERROR "OGG include directory not defined!")
endif ( OGG_INCLUDE_DIR )
include_directories(${VORBIS_INCLUDE_DIR})

add_library(${VORBIS_LIBRARY} STATIC ${VORBIS_src})
add_library(${VORBIS_FILE_LIBRARY} STATIC ${VORBIS_FILE_src})
set_target_properties(${VORBIS_LIBRARY} PROPERTIES
	COMPILE_DEFINITIONS "WIN32;_WINDOWS;_USRDLL;LIBVORBIS_EXPORTS;_CRT_SECURE_NO_WARNINGS"
	COMPILE_FLAGS "/wd4244 /wd4305")
set_target_properties(${VORBIS_FILE_LIBRARY} PROPERTIES
	COMPILE_DEFINITIONS "WIN32;_WINDOWS;_USRDLL;LIBVORBISFILE_EXPORTS;_CRT_SECURE_NO_WARNINGS"
	COMPILE_FLAGS "/wd4244")
if (NOT VORBIS_STATICLIB_OUTPUT_PATH)
	set(VORBIS_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"VORBIS static lib output directory")
endif (NOT VORBIS_STATICLIB_OUTPUT_PATH)
set_target_properties(${VORBIS_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${VORBIS_STATICLIB_OUTPUT_PATH}")

if ( OGG_LIBRARIES )
	target_link_libraries(${VORBIS_LIBRARY}
		${OGG_LIBRARIES})
else ( OGG_LIBRARIES )
	message( SEND_ERROR "OGG libraries not defined!")
endif ( OGG_LIBRARIES )
