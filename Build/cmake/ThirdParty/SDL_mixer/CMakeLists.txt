# SDL Image CMake build file
cmake_minimum_required (VERSION 2.6)
project(SDLMixer)

if (NOT SDLMIXER_SOURCE_NAME)
	set (SDLMIXER_SOURCE_NAME "SDL_mixer-1.2.11" CACHE STRING "SDL_mixer source name")
endif (NOT SDLMIXER_SOURCE_NAME)

set(SDLMixer_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${SDLMIXER_SOURCE_NAME}")
set(SDLMIXER_INCLUDE_DIR "${SDLMixer_SRC_FOLDER}" CACHE STRING "SDL_mixer include directory" FORCE)
set(SDLMIXER_LIBRARY SDL_mixer CACHE STRING "SDL_mixer libraries" FORCE)
set(sdlmixer_src
	"${SDLMixer_SRC_FOLDER}/dynamic_flac.c"
	"${SDLMixer_SRC_FOLDER}/dynamic_flac.h"
	"${SDLMixer_SRC_FOLDER}/dynamic_mod.c"
	"${SDLMixer_SRC_FOLDER}/dynamic_mod.h"
	"${SDLMixer_SRC_FOLDER}/dynamic_mp3.c"
	"${SDLMixer_SRC_FOLDER}/dynamic_mp3.h"
	"${SDLMixer_SRC_FOLDER}/dynamic_ogg.c"
	"${SDLMixer_SRC_FOLDER}/dynamic_ogg.h"
	"${SDLMixer_SRC_FOLDER}/effect_position.c"
	"${SDLMixer_SRC_FOLDER}/effect_stereoreverse.c"
	"${SDLMixer_SRC_FOLDER}/effects_internal.c"
	"${SDLMixer_SRC_FOLDER}/effects_internal.h"
	"${SDLMixer_SRC_FOLDER}/load_aiff.c"
	"${SDLMixer_SRC_FOLDER}/load_aiff.h"
	"${SDLMixer_SRC_FOLDER}/load_ogg.c"
	"${SDLMixer_SRC_FOLDER}/load_ogg.h"
	"${SDLMixer_SRC_FOLDER}/load_voc.c"
	"${SDLMixer_SRC_FOLDER}/load_voc.h"
	"${SDLMixer_SRC_FOLDER}/mixer.c"
	"${SDLMixer_SRC_FOLDER}/music.c"
	"${SDLMixer_SRC_FOLDER}/music_cmd.c"
	"${SDLMixer_SRC_FOLDER}/music_cmd.h"
	"${SDLMixer_SRC_FOLDER}/music_flac.c"
	"${SDLMixer_SRC_FOLDER}/music_flac.h"
	"${SDLMixer_SRC_FOLDER}/music_mad.c"
	"${SDLMixer_SRC_FOLDER}/music_mad.h"
	"${SDLMixer_SRC_FOLDER}/music_mod.c"
	"${SDLMixer_SRC_FOLDER}/music_mod.h"
	"${SDLMixer_SRC_FOLDER}/music_ogg.c"
	"${SDLMixer_SRC_FOLDER}/music_ogg.h"
	"${SDLMixer_SRC_FOLDER}/SDL_mixer.h"
	"${SDLMixer_SRC_FOLDER}/wavestream.c"
	"${SDLMixer_SRC_FOLDER}/wavestream.h"
)

add_library(SDL_mixer STATIC ${sdlmixer_src})
if (NOT SDLMIXER_STATICLIB_OUTPUT_PATH)
	set (SDLMIXER_STATICLIB_OUTPUT_PATH "." CACHE STRING "SDL_mixer static lib output directory")
endif (NOT SDLMIXER_STATICLIB_OUTPUT_PATH)
set_target_properties (${SDLMIXER_LIBRARY} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${SDLMIXER_STATICLIB_OUTPUT_PATH}")

if (TARGET ${SDL_LIBRARY_MAIN})
	add_dependencies(${SDLMIXER_LIBRARY} ${SDL_LIBRARY_MAIN})
endif (TARGET ${SDL_LIBRARY_MAIN})
include_directories(${SDL_INCLUDE_DIR})
include_directories(${SDLMIXER_EXTERNAL_INC})

target_link_libraries(${SDLMIXER_LIBRARY} ${SDL_LIBRARY}
	${VORBIS_LIBRARIES}
	${MIKMOD_LIBRARIES})

# Some defines
if (WIN32)
	set(sdlmixer_compile_def
		WIN32
		_WINDOWS
		#WAV_MUSIC
		MOD_MUSIC
		#MOD_DYNAMIC="mikmod.dll"
		#MID_MUSIC
		#USE_TIMIDITY_MIDI
		#USE_NATIVE_MIDI
		OGG_MUSIC
		#OGG_DYNAMIC="libvorbisfile-3.dll"
		#MP3_MUSIC
		#MP3_DYNAMIC="smpeg.dll"
		)
		# Additonal Flags not used
		# LOAD_BMP LOAD_GIF LOAD_JPG LOAD_JPG_DYNAMIC="jpeg.dll" LOAD_LBM LOAD_PCX
	if (MSVC)
		set(sdlmixer_compile_def ${sdlmixer_compile_def}
			_CRT_SECURE_NO_WARNINGS)

	endif (MSVC)
endif (WIN32)
set_target_properties(${SDLMIXER_LIBRARY} PROPERTIES
	COMPILE_DEFINITIONS "${sdlmixer_compile_def}")

# vim:ft=cmake
