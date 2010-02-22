# LuaJIT CMake build file
cmake_minimum_required (VERSION 2.6)
project (luajit)

if (NOT LUAJIT_SOURCE_NAME)
	set (LUAJIT_SOURCE_NAME "LuaJIT-1.1.5" CACHE STRING "LuaJIT source name")
endif (NOT LUAJIT_SOURCE_NAME)

set(LUAJIT_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${LUAJIT_SOURCE_NAME}/src")
# Create variable for include and library
set(LUAJIT_INCLUDE_DIR "${LUAJIT_SRC_FOLDER}" CACHE STRING "LuaJIT include directory" FORCE)
set(LUAJIT_LIBRARIES LUAJIT CACHE STRING "LuaJIT libraries" FORCE)
set(lua_src
	${LUAJIT_SRC_FOLDER}/lapi.c
	${LUAJIT_SRC_FOLDER}/lauxlib.c
	${LUAJIT_SRC_FOLDER}/lbaselib.c
	${LUAJIT_SRC_FOLDER}/lcoco.c
	${LUAJIT_SRC_FOLDER}/lcode.c
	${LUAJIT_SRC_FOLDER}/ldblib.c
	${LUAJIT_SRC_FOLDER}/ldebug.c
	${LUAJIT_SRC_FOLDER}/ldo.c
	${LUAJIT_SRC_FOLDER}/ldump.c
	${LUAJIT_SRC_FOLDER}/lfunc.c
	${LUAJIT_SRC_FOLDER}/lgc.c
	${LUAJIT_SRC_FOLDER}/linit.c
	${LUAJIT_SRC_FOLDER}/liolib.c
	${LUAJIT_SRC_FOLDER}/ljitlib.c
	${LUAJIT_SRC_FOLDER}/ljit_backend.c
	${LUAJIT_SRC_FOLDER}/ljit_core.c
	${LUAJIT_SRC_FOLDER}/ljit_dasm.c
	${LUAJIT_SRC_FOLDER}/ljit_mem.c
	${LUAJIT_SRC_FOLDER}/llex.c
	${LUAJIT_SRC_FOLDER}/lmathlib.c
	${LUAJIT_SRC_FOLDER}/lmem.c
	${LUAJIT_SRC_FOLDER}/loadlib.c
	${LUAJIT_SRC_FOLDER}/lobject.c
	${LUAJIT_SRC_FOLDER}/lopcodes.c
	${LUAJIT_SRC_FOLDER}/loslib.c
	${LUAJIT_SRC_FOLDER}/lparser.c
	${LUAJIT_SRC_FOLDER}/lstate.c
	${LUAJIT_SRC_FOLDER}/lstring.c
	${LUAJIT_SRC_FOLDER}/lstrlib.c
	${LUAJIT_SRC_FOLDER}/ltable.c
	${LUAJIT_SRC_FOLDER}/ltablib.c
	${LUAJIT_SRC_FOLDER}/ltm.c
	${LUAJIT_SRC_FOLDER}/lundump.c
	${LUAJIT_SRC_FOLDER}/lvm.c
	${LUAJIT_SRC_FOLDER}/lzio.c
)

include_directories("${PROJECT_SOURCE_DIR}/${LUAJIT_SOURCE_NAME}/dynasm")
add_library(${LUAJIT_LIBRARIES} STATIC ${lua_src})
if (NOT LUAJIT_STATICLIB_OUTPUT_PATH)
	set(LUAJIT_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"LuaJIT static lib output directory")
endif (NOT LUAJIT_STATICLIB_OUTPUT_PATH)

set_target_properties (${LUAJIT_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${LUAJIT_STATICLIB_OUTPUT_PATH}")

if (WIN32)
	if (MSVC)
		set(lua_compile_def
			_CRT_SECURE_NO_DEPRECATE
			_CRT_NONSTDC_NO_DEPRECATE)
	endif (MSVC)
endif (WIN32)
set_target_properties(${LUAJIT_LIBRARIES} PROPERTIES
	COMPILE_DEFINITIONS "${lua_compile_def}")
# vim:ft=cmake
