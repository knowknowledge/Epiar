# SDL CMake build file
cmake_minimum_required (VERSION 2.6)
project (SDL)

if (NOT SDL_SOURCE_NAME)
	set (SDL_SOURCE_NAME "SDL-1.2.14" CACHE STRING "SDL source name")
endif (NOT SDL_SOURCE_NAME)

set(SDL_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${SDL_SOURCE_NAME}/src")
set(SDL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/${SDL_SOURCE_NAME}/include" CACHE STRING "SDL include directory" FORCE)
set(SDL_LIBRARY_BULK SDL)
set(SDL_LIBRARY_MAIN SDLmain)

unset(SDL_LIBRARY CACHE)
if (WIN32)
	# If using mingw, need to prepend mingw32 lib
	if (MINGW)
		set(SDL_LIBRARY mingw32)
	endif (MINGW)
endif (WIN32)
set(SDL_LIBRARY ${SDL_LIBRARY} ${SDL_LIBRARY_BULK} ${SDL_LIBRARY_MAIN} CACHE STRING "SDL libraries" FORCE)

set(sdl_src
${SDL_SRC_FOLDER}/SDL.c
${SDL_SRC_FOLDER}/SDL_error.c
${SDL_SRC_FOLDER}/SDL_fatal.c
${SDL_SRC_FOLDER}/audio/SDL_audio.c
${SDL_SRC_FOLDER}/audio/SDL_audiocvt.c
${SDL_SRC_FOLDER}/audio/SDL_mixer.c
${SDL_SRC_FOLDER}/audio/SDL_mixer_MMX_VC.c
${SDL_SRC_FOLDER}/audio/SDL_wave.c
${SDL_SRC_FOLDER}/audio/disk/SDL_diskaudio.c
${SDL_SRC_FOLDER}/audio/dummy/SDL_dummyaudio.c
${SDL_SRC_FOLDER}/audio/windib/SDL_dibaudio.c
${SDL_SRC_FOLDER}/audio/windx5/SDL_dx5audio.c
${SDL_SRC_FOLDER}/cdrom/SDL_cdrom.c
${SDL_SRC_FOLDER}/cdrom/win32/SDL_syscdrom.c
${SDL_SRC_FOLDER}/cpuinfo/SDL_cpuinfo.c
${SDL_SRC_FOLDER}/events/SDL_active.c
${SDL_SRC_FOLDER}/events/SDL_events.c
${SDL_SRC_FOLDER}/events/SDL_expose.c
${SDL_SRC_FOLDER}/events/SDL_keyboard.c
${SDL_SRC_FOLDER}/events/SDL_mouse.c
${SDL_SRC_FOLDER}/events/SDL_quit.c
${SDL_SRC_FOLDER}/events/SDL_resize.c
${SDL_SRC_FOLDER}/file/SDL_rwops.c
${SDL_SRC_FOLDER}/joystick/SDL_joystick.c
${SDL_SRC_FOLDER}/joystick/win32/SDL_mmjoystick.c
${SDL_SRC_FOLDER}/loadso/win32/SDL_sysloadso.c
${SDL_SRC_FOLDER}/stdlib/SDL_getenv.c
${SDL_SRC_FOLDER}/stdlib/SDL_iconv.c
${SDL_SRC_FOLDER}/stdlib/SDL_malloc.c
${SDL_SRC_FOLDER}/stdlib/SDL_qsort.c
${SDL_SRC_FOLDER}/stdlib/SDL_stdlib.c
${SDL_SRC_FOLDER}/stdlib/SDL_string.c
${SDL_SRC_FOLDER}/thread/SDL_thread.c
${SDL_SRC_FOLDER}/thread/generic/SDL_syscond.c
${SDL_SRC_FOLDER}/thread/win32/SDL_sysmutex.c
${SDL_SRC_FOLDER}/thread/win32/SDL_syssem.c
${SDL_SRC_FOLDER}/thread/win32/SDL_systhread.c
${SDL_SRC_FOLDER}/timer/SDL_timer.c
${SDL_SRC_FOLDER}/timer/win32/SDL_systimer.c
${SDL_SRC_FOLDER}/video/SDL_RLEaccel.c
${SDL_SRC_FOLDER}/video/SDL_blit.c
${SDL_SRC_FOLDER}/video/SDL_blit_0.c
${SDL_SRC_FOLDER}/video/SDL_blit_1.c
${SDL_SRC_FOLDER}/video/SDL_blit_A.c
${SDL_SRC_FOLDER}/video/SDL_blit_N.c
${SDL_SRC_FOLDER}/video/SDL_bmp.c
${SDL_SRC_FOLDER}/video/SDL_cursor.c
${SDL_SRC_FOLDER}/video/SDL_gamma.c
${SDL_SRC_FOLDER}/video/SDL_pixels.c
${SDL_SRC_FOLDER}/video/SDL_stretch.c
${SDL_SRC_FOLDER}/video/SDL_surface.c
${SDL_SRC_FOLDER}/video/SDL_video.c
${SDL_SRC_FOLDER}/video/SDL_yuv.c
${SDL_SRC_FOLDER}/video/SDL_yuv_mmx.c
${SDL_SRC_FOLDER}/video/SDL_yuv_sw.c
${SDL_SRC_FOLDER}/video/dummy/SDL_nullevents.c
${SDL_SRC_FOLDER}/video/dummy/SDL_nullmouse.c
${SDL_SRC_FOLDER}/video/dummy/SDL_nullvideo.c
${SDL_SRC_FOLDER}/video/wincommon/SDL_sysevents.c
${SDL_SRC_FOLDER}/video/wincommon/SDL_sysmouse.c
${SDL_SRC_FOLDER}/video/wincommon/SDL_syswm.c
${SDL_SRC_FOLDER}/video/wincommon/SDL_wingl.c
${SDL_SRC_FOLDER}/video/windib/SDL_dibevents.c
${SDL_SRC_FOLDER}/video/windib/SDL_dibvideo.c
${SDL_SRC_FOLDER}/video/windx5/SDL_dx5events.c
${SDL_SRC_FOLDER}/video/windx5/SDL_dx5video.c
${SDL_SRC_FOLDER}/video/windx5/SDL_dx5yuv.c
)
set(sdl_main_src
	${SDL_SRC_FOLDER}/main/win32/SDL_win32_main.c
)

# Creates the libraries
include_directories(${SDL_INCLUDE_DIR})
add_library(${SDL_LIBRARY_BULK} STATIC ${sdl_src})
add_library(${SDL_LIBRARY_MAIN} STATIC ${sdl_main_src})
if (NOT SDL_STATICLIB_OUTPUT_PATH)
	set(SDL_STATICLIB_OUTPUT_PATH "." CACHE STRING "SDL static lib output directory")
endif (NOT SDL_STATICLIB_OUTPUT_PATH)

set_target_properties (${SDL_LIBRARY_BULK} ${SDL_LIBRARY_MAIN} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${SDL_STATICLIB_OUTPUT_PATH}")

add_dependencies(${SDL_LIBRARY_MAIN} ${SDL_LIBRARY_BULK})
# Some defines
if (WIN32)
	set(sdl_compile_def
		WIN32
		_WINDOWS
		ENABLE_DIRECTX
		NO_STDIO_REDIRECT
		)
	if (MSVC)
		set(sdl_compile_def ${sdl_compile_def}
			_CRT_SECURE_NO_WARNINGS)
	elseif (WATCOM)
		set(sdl_compile_def ${sdl_compile_def}
			_WIN32_WINNT=0x0400)
	endif (MSVC)
	target_link_libraries(${SDL_LIBRARY_BULK} winmm dxguid)
endif (WIN32)
set_target_properties(${SDL_LIBRARY_BULK} ${SDL_LIBRARY_MAIN} PROPERTIES
	COMPILE_DEFINITIONS "${sdl_compile_def}")

# vim:ft=cmake
