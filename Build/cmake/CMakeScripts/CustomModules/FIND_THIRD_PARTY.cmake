# Wrapper for Find_Package function
# This function requires the PARSE_ARGUMENTS macro
# Required Variables to be defined
#	None
# Required Arguments
#	1. Package Name
# Options
#	1. REQUIRED - define this to halt CMAKE progress on failed find

macro (FIND_THIRD_PARTY)
	# Parse Arguments
	PARSE_ARGUMENTS(FND "FOUNDVAR" "REQUIRED" ${ARGV})
	list(GET FND_DEFAULT_ARGS 0 FND_NAME)

	find_package(${FND_NAME})
	# Sometimes modules use a different found variable than the file name
	if (FND_FOUNDVAR)
		set(FND_PKG_FOUND ${${FND_FOUNDVAR}})
	else (FND_FOUNDVAR)
		string(TOUPPER ${FND_NAME} FND_UNAME)
		set(FND_PKG_FOUND ${${FND_UNAME}_FOUND})
	endif (FND_FOUNDVAR)

	# Halt cmake if required package is not found
	if (NOT FND_PKG_FOUND AND FND_REQUIRED)
		message(FATAL_ERROR "Unable to find ${FND_NAME}: FindPackage returned ${FND_PKG_FOUND}")
	endif(NOT FND_PKG_FOUND AND FND_REQUIRED)

	# Mark SDL variable as advanced
	if ( ${FND_NAME} STREQUAL SDL )
		mark_as_advanced(SDL_INCLUDE_DIR)
		mark_as_advanced(SDL_LIBRARY)
		mark_as_advanced(SDLMAIN_LIBRARY)
		mark_as_advanced(SDL_LIBRARY_TEMP)
	# Mark SDL_image variable as advanced
	elseif ( ${FND_NAME} STREQUAL SDL_image )
		mark_as_advanced(SDLIMAGE_INCLUDE_DIR)
		mark_as_advanced(SDLIMAGE_LIBRARY)
	# Mark SDL_mixer variable as advanced
	elseif ( ${FND_NAME} STREQUAL SDL_mixer )
		mark_as_advanced(SDLMIXER_INCLUDE_DIR)
		mark_as_advanced(SDLMIXER_LIBRARY)
	# Mark PhysFS variable as advanced
	elseif ( ${FND_NAME} STREQUAL PhysFS )
		mark_as_advanced(PHYSFS_INCLUDE_DIR)
		mark_as_advanced(PHYSFS_LIBRARY)
	endif ( ${FND_NAME} STREQUAL SDL )
endmacro (FIND_THIRD_PARTY)

# vim:ft=cmake
