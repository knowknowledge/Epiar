# Lua51 CMake build file
cmake_minimum_required (VERSION 2.6)
project (lua51)

if (NOT LUA_SOURCE_NAME)
	set (LUA_SOURCE_NAME "lua-5.1.4" CACHE STRING "Lua51 source name")
endif (NOT LUA_SOURCE_NAME)

set(LUA_SRC_FOLDER "${PROJECT_SOURCE_DIR}/${LUA_SOURCE_NAME}/src")
# Create variable for include and library
set(LUA_INCLUDE_DIR "${LUA_SRC_FOLDER}" CACHE STRING "Lua51 include directory" FORCE)
set(LUA_LIBRARIES lua51 CACHE STRING "Lua51 libraries" FORCE)
set(lua_src
${LUA_SRC_FOLDER}/lapi.c
${LUA_SRC_FOLDER}/lcode.c
${LUA_SRC_FOLDER}/ldebug.c
${LUA_SRC_FOLDER}/ldo.c
${LUA_SRC_FOLDER}/ldump.c
${LUA_SRC_FOLDER}/lfunc.c
${LUA_SRC_FOLDER}/lgc.c
${LUA_SRC_FOLDER}/llex.c
${LUA_SRC_FOLDER}/lmem.c
${LUA_SRC_FOLDER}/lobject.c
${LUA_SRC_FOLDER}/lopcodes.c
${LUA_SRC_FOLDER}/lparser.c
${LUA_SRC_FOLDER}/lstate.c
${LUA_SRC_FOLDER}/lstring.c
${LUA_SRC_FOLDER}/ltable.c
${LUA_SRC_FOLDER}/ltm.c
${LUA_SRC_FOLDER}/lundump.c
${LUA_SRC_FOLDER}/lvm.c
${LUA_SRC_FOLDER}/lzio.c
${LUA_SRC_FOLDER}/lauxlib.c
${LUA_SRC_FOLDER}/lbaselib.c
${LUA_SRC_FOLDER}/ldblib.c
${LUA_SRC_FOLDER}/liolib.c
${LUA_SRC_FOLDER}/lmathlib.c
${LUA_SRC_FOLDER}/loslib.c
${LUA_SRC_FOLDER}/ltablib.c
${LUA_SRC_FOLDER}/lstrlib.c
${LUA_SRC_FOLDER}/loadlib.c
${LUA_SRC_FOLDER}/linit.c
)

add_library(${LUA_LIBRARIES} STATIC ${lua_src})
if (NOT LUA_STATICLIB_OUTPUT_PATH)
	set(LUA_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"Lua51 static lib output directory")
endif (NOT LUA_STATICLIB_OUTPUT_PATH)

set_target_properties (${LUA_LIBRARIES} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${Lua51_STATICLIB_OUTPUT_PATH}")

if (WIN32)
	if (MSVC)
		set(lua_compile_def
			_CRT_SECURE_NO_DEPRECATE
			_CRT_NONSTDC_NO_DEPRECATE)
	endif (MSVC)
endif (WIN32)
set_target_properties(${LUA_LIBRARIES} PROPERTIES
	COMPILE_DEFINITIONS "${lua_compile_def}")
# vim:ft=cmake
